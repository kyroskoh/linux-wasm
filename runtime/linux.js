// SPDX-License-Identifier: GPL-2.0-only

/// Create a Linux machine and run it.
const linux = async (worker_url, vmlinux, boot_cmdline, initrd, log, console_write, graphics_ctx) => {
  /// Dict of online CPUs.
  const cpus = {};

  /// Dict of tasks.
  const tasks = {};

  /// Input buffer (from keyboard to tty).
  let input_buffer = new ArrayBuffer(0);

  const text_decoder = new TextDecoder("utf-8");
  const text_encoder = new TextEncoder();

  /// Graphics context (for WebGL/EGL support)
  const graphics = graphics_ctx || null;

  const lock_notify = (locks, lock, count) => {
    Atomics.store(locks._memory, locks[lock], 1);
    Atomics.notify(locks._memory, locks[lock], count || 1);
  };

  const lock_wait = (locks, lock) => {
    Atomics.wait(locks._memory, locks[lock], 0);
    Atomics.store(locks._memory, locks[lock], 0);
  };

  /// Callbacks from Web Workers (each one representing one task).
  const message_callbacks = {
    start_primary: (message) => {
      // CPU 0 has init_task which sits in static storage. After booting it becomes CPU 0's idle task. The runner will
      // in this special case tell us where it is so that we can register it.
      log("Starting cpu 0 with init_task " + message.init_task)
      tasks[message.init_task] = cpus[0];
    },

    start_secondary: (message) => {
      if (message.cpu <= 0) {
        throw new Error("Trying to start secondary cpu with ID <= 0");
      }

      log("Starting cpu " + message.cpu + " (" + message.idle_task + ")" +
        " with start stack " + message.start_stack);
      make_cpu(message.cpu, message.idle_task, message.start_stack);
    },

    stop_secondary: (message) => {
      if (message.cpu <= 0) {
        // If you arrive here, you probably got panic():ed with a broken stack.
        if (!confirm("Trying to stop secondary cpu with ID 0.\n\n" +
          "You probably got panic():ed with a broken stack. Continue?\n\n" +
          " (Say ok if you know what you are doing and want to catch the panic, otherwise cancel.)")) {
          throw new Error("Trying to stop secondary cpu with ID 0");
        }
      }

      if (cpus[message.cpu]) {
        log("[Main]: Stopping CPU " + message.cpu);
        cpus[message.cpu].worker.terminate();
        delete cpus[message.cpu];
      } else {
        log("[Main]: Tried to stop CPU " + message.cpu + " but it was already stopped (broken system)!");
      }
    },

    create_and_run_task: (message) => {
      // ret_from_fork will make sure the task switch finishes.
      make_task(message.prev_task, message.new_task, message.name, message.user_executable);
    },

    release_task: (message) => {
      // Stop the worker, which will stop script execution. This is safe as the task should be hanging on a lock waiting
      // to be scheduled - which never happens as dead tasks don't get ever get scheduled.
      tasks[message.dead_task].worker.terminate();

      delete tasks[message.dead_task];
    },

    serialize_tasks: (message) => {
      // next_task was previously suspended, wake it up.

      // Tell the next task where we switched from, so that it can finish the task switch.
      tasks[message.next_task].last_task[0] = message.prev_task;

      // Release the above write of last_task and wake up the task.
      lock_notify(tasks[message.next_task].locks, "serialize");
    },

    console_read: (message, worker) => {
      const memory_u8 = new Uint8Array(memory.buffer);
      const buffer = new Uint8Array(input_buffer);

      const used = buffer.slice(0, message.count);
      memory_u8.set(used, message.buffer);

      const unused = buffer.slice(message.count);
      input_buffer = unused.buffer;

      // Tell the Worker that asked for input how many bytes (perhaps 0) were actually written.
      Atomics.store(message.console_read_messenger, 0, used.length);
      Atomics.notify(message.console_read_messenger, 0, 1);
    },

    console_write: (message) => {
      console_write(message.message);
    },

    log: (message) => {
      log(message.message);
    },

    // Graphics callbacks (EGL/OpenGL ES operations)
    graphics_init: (message) => {
      if (graphics && graphics.canvas) {
        graphics.canvas.parentElement.style.display = 'block';
        log("[Graphics]: Initialized");
      }
    },

    graphics_swap_buffers: (message) => {
      // WebGL automatically swaps buffers, but we can trigger a flush here if needed
      if (graphics && graphics.gl) {
        graphics.gl.flush();
      }
    },

    graphics_gl_call: (message) => {
      // Generic OpenGL call forwarding from workers
      if (!graphics || !graphics.gl) return;
      
      try {
        const gl = graphics.gl;
        
        // Map ID to actual WebGL object if needed
        let args = message.args || [];
        if (message.is_shader_id && args.length > 0) {
          args[0] = graphics.shaders.get(args[0]);
        }
        if (message.is_program_id && args.length > 0) {
          args[0] = graphics.programs.get(args[0]);
        }
        if (message.is_buffer_id && args.length > 0) {
          args[1] = graphics.buffers.get(args[1]);
        }
        if (message.is_uniform_location && args.length > 0) {
          args[0] = graphics.uniformLocations.get(args[0]);
        }
        if (message.is_texture && args.length > 1) {
          args[1] = graphics.textures.get(args[1]);
        }
        
        const func = gl[message.func_name];
        if (typeof func === 'function') {
          let result = func.apply(gl, args);
          
          // Handle object creation - assign IDs
          if (message.func_name === 'createShader' && result) {
            const id = graphics.nextShaderId++;
            graphics.shaders.set(id, result);
            result = id;
          } else if (message.func_name === 'createProgram' && result) {
            const id = graphics.nextProgramId++;
            graphics.programs.set(id, result);
            result = id;
          }
          
          // Return result via SharedArrayBuffer if requested
          if (message.result_buffer) {
            Atomics.store(message.result_buffer, 0, result || 0);
            Atomics.notify(message.result_buffer, 0, 1);
          }
        }
      } catch (error) {
        log("[Graphics]: Error in " + message.func_name + ": " + error.message);
        if (message.result_buffer) {
          Atomics.store(message.result_buffer, 0, 0);
          Atomics.notify(message.result_buffer, 0, 1);
        }
      }
    },

    graphics_gl_shader_source: (message) => {
      if (!graphics || !graphics.gl) return;
      const shader = graphics.shaders.get(message.shader);
      if (shader) {
        graphics.gl.shaderSource(shader, message.source);
      }
    },

    graphics_gl_attach_shader: (message) => {
      if (!graphics || !graphics.gl) return;
      const program = graphics.programs.get(message.program);
      const shader = graphics.shaders.get(message.shader);
      if (program && shader) {
        graphics.gl.attachShader(program, shader);
      }
    },

    graphics_gl_get_shaderiv: (message) => {
      if (!graphics || !graphics.gl) return;
      const shader = graphics.shaders.get(message.shader);
      if (shader) {
        const result = graphics.gl.getShaderParameter(shader, message.pname);
        Atomics.store(message.result_buffer, 0, result ? 1 : 0);
        Atomics.notify(message.result_buffer, 0, 1);
      }
    },

    graphics_gl_get_shader_info_log: (message) => {
      if (!graphics || !graphics.gl) return;
      const shader = graphics.shaders.get(message.shader);
      if (shader) {
        const log = graphics.gl.getShaderInfoLog(shader) || "";
        const encoder = new TextEncoder();
        const encoded = encoder.encode(log);
        const length = Math.min(encoded.length, message.max_length - 1);
        
        for (let i = 0; i < length; i++) {
          message.result_str[i] = encoded[i];
        }
        message.result_str[length] = 0; // Null terminator
        
        Atomics.store(message.result_len, 0, length);
        Atomics.notify(message.result_len, 0, 1);
      }
    },

    graphics_gl_get_programiv: (message) => {
      if (!graphics || !graphics.gl) return;
      const program = graphics.programs.get(message.program);
      if (program) {
        const result = graphics.gl.getProgramParameter(program, message.pname);
        Atomics.store(message.result_buffer, 0, result ? 1 : 0);
        Atomics.notify(message.result_buffer, 0, 1);
      }
    },

    graphics_gl_get_program_info_log: (message) => {
      if (!graphics || !graphics.gl) return;
      const program = graphics.programs.get(message.program);
      if (program) {
        const log = graphics.gl.getProgramInfoLog(program) || "";
        const encoder = new TextEncoder();
        const encoded = encoder.encode(log);
        const length = Math.min(encoded.length, message.max_length - 1);
        
        for (let i = 0; i < length; i++) {
          message.result_str[i] = encoded[i];
        }
        message.result_str[length] = 0; // Null terminator
        
        Atomics.store(message.result_len, 0, length);
        Atomics.notify(message.result_len, 0, 1);
      }
    },

    graphics_gl_get_attrib_location: (message) => {
      if (!graphics || !graphics.gl) return;
      const program = graphics.programs.get(message.program);
      if (program) {
        const location = graphics.gl.getAttribLocation(program, message.name);
        Atomics.store(message.result_buffer, 0, location);
        Atomics.notify(message.result_buffer, 0, 1);
      }
    },

    graphics_gl_get_uniform_location: (message) => {
      if (!graphics || !graphics.gl) return;
      const program = graphics.programs.get(message.program);
      if (program) {
        const location = graphics.gl.getUniformLocation(program, message.name);
        if (location) {
          const id = graphics.nextUniformLocationId++;
          graphics.uniformLocations.set(id, location);
          Atomics.store(message.result_buffer, 0, id);
        } else {
          Atomics.store(message.result_buffer, 0, -1);
        }
        Atomics.notify(message.result_buffer, 0, 1);
      }
    },

    graphics_gl_gen_buffers: (message) => {
      if (!graphics || !graphics.gl) return;
      for (let i = 0; i < message.n; i++) {
        const buffer = graphics.gl.createBuffer();
        const id = graphics.nextBufferId++;
        graphics.buffers.set(id, buffer);
        message.result_buffer[i] = id;
      }
      Atomics.notify(new Int32Array(message.result_buffer.buffer), 0, 1);
    },

    graphics_gl_buffer_data: (message) => {
      if (!graphics || !graphics.gl) return;
      const gl = graphics.gl;
      
      if (message.data) {
        gl.bufferData(message.target, message.data, message.usage);
      } else {
        gl.bufferData(message.target, message.size, message.usage);
      }
    },

    graphics_gl_uniform_matrix4fv: (message) => {
      if (!graphics || !graphics.gl) return;
      const location = graphics.uniformLocations.get(message.location);
      if (location) {
        graphics.gl.uniformMatrix4fv(location, message.transpose, message.value);
      }
    },

    graphics_gl_uniform_fv: (message) => {
      if (!graphics || !graphics.gl) return;
      const location = graphics.uniformLocations.get(message.location);
      if (location) {
        // Call the appropriate uniformNfv function based on size
        if (message.size === 2) {
          graphics.gl.uniform2fv(location, message.value);
        } else if (message.size === 3) {
          graphics.gl.uniform3fv(location, message.value);
        } else if (message.size === 4) {
          graphics.gl.uniform4fv(location, message.value);
        }
      }
    },

    graphics_gl_gen_textures: (message) => {
      if (!graphics || !graphics.gl) return;
      
      const texture_ids = [];
      for (let i = 0; i < message.n; i++) {
        const texture = graphics.gl.createTexture();
        const id = graphics.nextTextureId++;
        graphics.textures.set(id, texture);
        texture_ids.push(id);
      }
      
      // Write texture IDs to shared result buffer
      const result_buffer = message.worker._result_buffer;
      if (result_buffer) {
        const result_i32 = new Int32Array(result_buffer);
        for (let i = 0; i < texture_ids.length; i++) {
          result_i32[1 + i] = texture_ids[i];
        }
        // Signal completion
        result_i32[0] = 1;
        Atomics.notify(result_i32, 0);
      }
    },

    graphics_gl_delete_textures: (message) => {
      if (!graphics || !graphics.gl) return;
      
      for (const texture_id of message.texture_ids) {
        const texture = graphics.textures.get(texture_id);
        if (texture) {
          graphics.gl.deleteTexture(texture);
          graphics.textures.delete(texture_id);
        }
      }
    },

    graphics_gl_tex_image_2d: (message) => {
      if (!graphics || !graphics.gl) return;
      
      if (message.data === null) {
        // Allocate empty texture
        graphics.gl.texImage2D(
          message.target,
          message.level,
          message.internalformat,
          message.width,
          message.height,
          message.border,
          message.format,
          message.type,
          null
        );
      } else {
        // Upload texture data
        const data = new Uint8Array(message.data);
        graphics.gl.texImage2D(
          message.target,
          message.level,
          message.internalformat,
          message.width,
          message.height,
          message.border,
          message.format,
          message.type,
          data
        );
      }
    },
  };

  /// Memory shared between all CPUs.
  const memory = new WebAssembly.Memory({
    initial: 30, // TODO: extract this automatically from vmlinux.
    maximum: 0x10000, // Allow the full 32-bit address space to be allocated.
    shared: true,
  });

  /**
   * Create and run one CPU in a background thread (a Web Worker).
   *
   * This will run boot code for the CPU, and then drop to run the idle task. For CPU 0 this involves booting the entire
   * system, including bringing up secondary CPUs at the end, while for secondary CPUs, this just means some
   * book-keeping before dropping into their own idle tasks.
   */
  const make_cpu = (cpu, idle_task, start_stack) => {
    const options = {
      runner_type: (cpu == 0) ? "primary_cpu" : "secondary_cpu",
      start_stack: start_stack,  // undefined for CPU 0
    };

    if (cpu == 0) {
      options.boot_cmdline = boot_cmdline;
      options.initrd = initrd;
      initrd = null;  // allow gc
    }

    // idle_task is undefined for cpu 0, we will know it first when start_primary notifies us.
    const name = "CPU " + cpu + " [boot+idle]" + (cpu != 0 ? " (" + idle_task + ")" : "");

    const runner = make_vmlinux_runner(name, options);
    cpus[cpu] = runner;
    if (cpu != 0) {
      tasks[idle_task] = runner; // For CPU 0, start_primary does this registration for us.
    }
  };

  /**
   * Create and run one task. This task has been switch_to():ed by the scheduler for the first time.
   *
   * In the beginning, all tasks are serialized and have to cooperate to schedule eachother, but after secondary CPUs
   * are brought up, they can run concurrently (and will effectively be managed by the Wasm host OS). While we are not
   * able to suspend them from JS, the host OS will do that.
   */
  const make_task = (prev_task, new_task, name, user_executable) => {
    const options = {
      runner_type: "task",
      prev_task: prev_task,
      new_task: new_task,
      user_executable: user_executable,
    };
    tasks[new_task] = make_vmlinux_runner(name + " (" + new_task + ")", options);
  };

  /// Create a runner for vmlinux. It will run in a Web Worker and execute some specified code.
  const make_vmlinux_runner = (name, options) => {
    // Note: SharedWorker does not seem to allow WebAssembly Module or Memory instances posted.
    const worker = new Worker(worker_url, { name: name });

    let locks = {
      serialize: 0,
    };
    locks._memory = new Int32Array(new SharedArrayBuffer(Object.keys(locks).length * 4));

    // Store for last task when wasm_serialize() returns in switch_to(). Needed for each task, both normal ones and each
    // CPUs idle tasks (first called init_task (PID 0), not to be confused with init (PID 1) which is a normal task).
    const last_task = new Uint32Array(new SharedArrayBuffer(4));

    worker.onerror = (error) => {
      throw error;
    };

    worker.onmessage = (message_event) => {
      const data = message_event.data;
      message_callbacks[data.method](data, worker);
    };

    worker.onmessageerror = (error) => {
      throw error;
    };

    worker.postMessage({
      ...options,
      method: "init",
      vmlinux: vmlinux,
      memory: memory,
      locks: locks,
      last_task: last_task,
      runner_name: name,
    });

    return {
      worker: worker,
      locks: locks,
      last_task: last_task,
    };
  };

  // Create the primary cpu, it will later on callback to us and we start secondaries.
  make_cpu(0);

  return {
    key_input: (data) => {
      const key_buffer = text_encoder.encode(data);  // Possibly UTF-8 (up to 16 bits).

      // Append key_buffer to the end of input_buffer.
      const old_size = input_buffer.byteLength;
      input_buffer = input_buffer.transfer(old_size + key_buffer.byteLength);
      (new Uint8Array(input_buffer)).set(key_buffer, old_size);
    }
  };
};
