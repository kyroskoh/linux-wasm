# Graphics Support for Linux/Wasm

This document describes the graphics subsystem for Linux/Wasm, which provides EGL and OpenGL ES interfaces backed by WebGL in the browser.

## Architecture

The graphics system consists of three layers:

1. **WebGL Backend (Browser)**
   - Canvas element in the HTML page
   - WebGL or WebGL2 context
   - Context registry for managing multiple EGL contexts/surfaces

2. **JavaScript Host Callbacks (Runtime)**
   - `linux.js` - Main thread graphics message handlers
   - `linux-worker.js` - Worker-side EGL/OpenGL ES host callbacks
   - Message passing between workers and main thread for graphics operations

3. **C API (User Programs)**
   - `wasm-graphics.h` - EGL and OpenGL ES interface
   - Host callback declarations
   - Helper functions for initialization

## Features

### Current Implementation

- ✅ EGL display and context management
- ✅ Basic OpenGL ES operations (clear, viewport, etc.)
- ✅ Buffer swapping (presentation)
- ✅ Canvas element with WebGL context
- ✅ Example program demonstrating usage

### Future Enhancements

These features could be added to make the graphics system more complete:

- [ ] **Full OpenGL ES 2.0/3.0 API** - Currently only basic functions are exposed. More functions can be added following the same pattern (add host callback in `linux-worker.js`, forward in `linux.js`, declare in `wasm-graphics.h`).

- [ ] **Shader Support** - Add functions like `glCreateShader`, `glShaderSource`, `glCompileShader`, `glCreateProgram`, `glLinkProgram`, etc.

- [ ] **Texture Support** - Functions for texture creation, uploading, and binding.

- [ ] **Buffer Objects** - Full support for VBOs, IBOs, etc.

- [ ] **Integration with Emscripten** - Emscripten already has extensive EGL/WebGL implementation. The current system could be extended to be compatible with Emscripten's approach, potentially reusing their libraries.

- [ ] **Kernel Framebuffer Driver** - A proper kernel driver (similar to the console driver at `patches/kernel/0010-Add-Wasm-console-support.patch`) could provide a standard Linux framebuffer interface (`/dev/fb0`) or DRM interface.

## Usage

### Basic Example

```c
#include "wasm-graphics.h"
#include <stdio.h>

int main(void) {
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    // Initialize graphics
    if (graphics_initialize(&display, &surface, &context) != 0) {
        fprintf(stderr, "Failed to initialize graphics\n");
        return 1;
    }

    // Set viewport
    glViewport(0, 0, 800, 600);

    // Clear to blue
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Present
    eglSwapBuffers(display, surface);

    return 0;
}
```

### Compilation

Compile graphics programs with the Wasm target and link against musl:

```bash
clang --target=wasm32-unknown-unknown \
  --sysroot=$LW_INSTALL/musl \
  -fPIC -shared \
  -o my-graphics-app.wasm \
  my-graphics-app.c
```

### Running

1. Copy your `.wasm` file to the initramfs or make it available in the Linux/Wasm filesystem
2. Execute it like any other program:
   ```bash
   ./my-graphics-app.wasm
   ```
3. The canvas will automatically appear when graphics are initialized

## Adding More OpenGL Functions

To add a new OpenGL function, follow these steps:

### 1. Add Host Callback in `linux-worker.js`

```javascript
wasm_gl_my_function: (arg1, arg2) => {
  port.postMessage({
    method: "graphics_gl_call",
    func_name: "myFunction",
    args: [arg1, arg2],
  });
},
```

### 2. Declare in `wasm-graphics.h`

```c
__attribute__((import_module("env"), import_name("wasm_gl_my_function")))
void wasm_gl_my_function(GLtype arg1, GLtype arg2);

#define glMyFunction(a1, a2) wasm_gl_my_function(a1, a2)
```

### 3. Use in Your Program

```c
glMyFunction(value1, value2);
```

## Integration with Emscripten

Emscripten provides a comprehensive EGL/OpenGL ES implementation with WebGL backend. To leverage Emscripten's work:

1. **Option A: Port Emscripten's Implementation**
   - Study Emscripten's `library_egl.js` and `library_webgl.js`
   - Adapt their JavaScript implementation to work with the Linux/Wasm worker architecture
   - This would provide full OpenGL ES 2.0/3.0 compatibility

2. **Option B: Hybrid Approach**
   - Use Emscripten to compile graphics-heavy libraries (like SDL2, GLFW, etc.)
   - Link these libraries with Linux/Wasm programs
   - May require some adaptation of the Emscripten runtime to work with Linux syscalls

3. **Option C: Syscall-Based Graphics**
   - Implement a kernel driver that exposes graphics through standard Linux interfaces
   - Programs use ioctl() calls to the driver
   - Driver forwards to JavaScript WebGL backend

## Performance Considerations

- **Message Passing Overhead**: Current implementation uses `postMessage` between workers and main thread. For high-performance graphics, consider using `SharedArrayBuffer` with Atomics for command buffers.

- **Synchronous vs Asynchronous**: Many graphics operations are currently fire-and-forget. For operations that return values, you may need to add synchronous waiting mechanisms.

- **Batch Operations**: Consider batching multiple OpenGL calls together to reduce message passing overhead.

## Browser Support

The graphics system requires:
- WebGL 1.0 or WebGL 2.0 support (all modern browsers)
- SharedArrayBuffer support (requires cross-origin isolation)
- Web Workers support (universal)

## Known Limitations

1. **No Direct Buffer Access**: Unlike native OpenGL, you cannot directly access GPU buffers from the Wasm memory. All data transfers go through message passing.

2. **Limited Error Handling**: The current implementation has minimal error handling. Production code should add comprehensive error checking.

3. **Single Context**: The simplified implementation assumes a single graphics context. Multiple contexts would require more sophisticated context management.

4. **No Synchronization Primitives**: WebGL doesn't expose fences or other synchronization primitives available in native OpenGL.

## References

- [WebGL Specification](https://www.khronos.org/webgl/)
- [EGL Specification](https://www.khronos.org/egl/)
- [OpenGL ES 2.0 Specification](https://www.khronos.org/opengles/)
- [Emscripten WebGL/EGL Implementation](https://github.com/emscripten-core/emscripten/tree/main/src/library_webgl.js)
- [WebAssembly Proposal: Exception Handling](https://github.com/WebAssembly/exception-handling)

## Contributing

To extend the graphics system:

1. Start by implementing commonly-used OpenGL ES functions
2. Follow the established pattern for host callbacks
3. Test with real graphics applications
4. Consider performance optimizations for high-frequency calls
5. Document any new APIs in this file

For complex applications, studying how Emscripten handles graphics will provide valuable insights into production-ready approaches.

