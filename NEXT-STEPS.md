# Next Steps - Immediate Action Items

Quick reference for what to work on next. See [ROADMAP.md](ROADMAP.md) for the complete development plan.

## Week 1-2: Expand Graphics to 3D 🎨

### Step 1: Add Shader Support

Add to `runtime/linux-worker.js`:

```javascript
wasm_gl_create_shader: (type) => {
  // Will need synchronous return mechanism
  // Consider using SharedArrayBuffer for return values
},

wasm_gl_shader_source: (shader, count, string, length) => {
  // Read shader source from memory, send to main thread
},

wasm_gl_compile_shader: (shader) => {
  // Forward to main thread for compilation
},

// ... more shader functions
```

**Resources:**
- Emscripten's library_webgl.js: https://github.com/emscripten-core/emscripten/blob/main/src/library_webgl.js
- WebGL shader tutorial: https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API/Tutorial

### Step 2: Add Buffer Objects

```javascript
wasm_gl_gen_buffers: (n, buffers) => {
  // Generate buffer IDs, write back to memory
},

wasm_gl_bind_buffer: (target, buffer) => {
  // Track bound buffers
},

wasm_gl_buffer_data: (target, size, data, usage) => {
  // Transfer data from Wasm memory to WebGL buffer
},
```

### Step 3: Add Drawing Functions

```javascript
wasm_gl_draw_arrays: (mode, first, count) => {
  // Forward to gl.drawArrays()
},

wasm_gl_draw_elements: (mode, count, type, indices) => {
  // Forward to gl.drawElements()
},
```

### Step 4: Create Spinning Cube Demo

Create `runtime/example-cube3d.c`:

```c
#include "wasm-graphics.h"

const char* vertex_shader = 
  "attribute vec3 position;\n"
  "uniform mat4 transform;\n"
  "void main() {\n"
  "  gl_Position = transform * vec4(position, 1.0);\n"
  "}\n";

const char* fragment_shader =
  "precision mediump float;\n"
  "void main() {\n"
  "  gl_FragColor = vec4(1.0, 0.5, 0.0, 1.0);\n"
  "}\n";

// Implement cube rendering with shaders
```

**Target**: Working 3D cube by end of Week 2

---

## Week 3: C++ Exception Support 🔧

### Test 1: Basic Compilation

```bash
cat > test-hello.cpp << 'EOF'
#include <iostream>
int main() {
    std::cout << "Hello from C++!" << std::endl;
    return 0;
}
EOF

$LW_INSTALL/llvm/bin/clang++ \
  --target=wasm32-unknown-unknown \
  --sysroot=$LW_INSTALL/musl \
  -fPIC -shared \
  -o test-hello.wasm test-hello.cpp
```

### Test 2: Exceptions

```bash
cat > test-exceptions.cpp << 'EOF'
#include <iostream>
#include <stdexcept>

int main() {
    try {
        throw std::runtime_error("Test exception");
    } catch (const std::exception& e) {
        std::cout << "Caught: " << e.what() << std::endl;
    }
    return 0;
}
EOF

$LW_INSTALL/llvm/bin/clang++ \
  --target=wasm32-unknown-unknown \
  --sysroot=$LW_INSTALL/musl \
  -fPIC -shared -fwasm-exceptions \
  -o test-exceptions.wasm test-exceptions.cpp
```

### If Issues Arise

Common problems and solutions:

1. **Missing libcxx**: May need to build LLVM with libc++ runtime
2. **Exception symbols undefined**: Check that `-fwasm-exceptions` is set
3. **Runtime errors**: Browser needs exception handling support

**Document findings** in new `runtime/CPP-SUPPORT.md`

---

## Week 4: Create Impressive Demo 🎮

### Option A: Shader Showcase (Recommended)

Features:
- Multiple shader effects (water, fire, plasma)
- Real-time effect switching
- Performance counter
- FPS display

### Option B: Particle System

Features:
- 10,000+ particles
- Physics (gravity, collision)
- Mouse interaction
- Visual effects

### Option C: Simple 3D Game

Features:
- First-person camera
- Textured environment
- Simple physics
- Input handling

**Target**: Impressive demo video to showcase platform

---

## Quick Wins (Can Do Anytime)

### 1. Texture Support (2-3 hours)

```javascript
wasm_gl_gen_textures: (n, textures) => { /* ... */ },
wasm_gl_bind_texture: (target, texture) => { /* ... */ },
wasm_gl_tex_image_2d: (target, level, internalformat, width, height, 
                        border, format, type, pixels) => { /* ... */ },
```

### 2. More OpenGL Functions (1-2 hours each)

Priority list:
- `glEnable` / `glDisable` (blending, depth test)
- `glBlendFunc` / `glDepthFunc`
- `glCullFace` / `glFrontFace`
- `glLineWidth` / `glPolygonOffset`

### 3. Input Handling (3-4 hours)

Add mouse events to canvas:

```javascript
// In index.html
canvas.addEventListener('mousemove', (e) => {
  // Forward to Linux/Wasm
  os.mouse_move(e.clientX, e.clientY);
});
```

### 4. Audio Support (5-6 hours)

Basic Web Audio API integration:

```javascript
wasm_audio_play: (buffer, size, rate) => {
  const audioContext = new AudioContext();
  // Play audio from memory buffer
},
```

---

## Tools to Create

### 1. Enhanced Compile Script

`tools/compile-cpp.sh` - Specialized for C++:
```bash
#!/bin/bash
# C++ compilation with proper flags
clang++ --target=wasm32-unknown-unknown \
  --sysroot=$LW_INSTALL/musl \
  -fPIC -shared -fwasm-exceptions \
  $LW_DEBUG_CFLAGS \
  -o "$OUTPUT" "$SOURCE"
```

### 2. GL Function Generator

`tools/generate-gl-bindings.py` - Auto-generate bindings:
```python
# Parse OpenGL spec
# Generate:
#   - C header declarations
#   - JavaScript host callbacks
#   - Documentation
```

### 3. Performance Profiler

`tools/profile-graphics.js` - Measure frame times:
```javascript
// Track GL calls
// Measure message passing overhead
// Generate performance report
```

---

## Research Tasks

### 1. Emscripten GL Implementation (4-8 hours)

Study how they handle:
- Object ID management (textures, buffers, shaders)
- String passing (shader source)
- Synchronous returns from async operations
- Error handling and validation

**Deliverable**: Design doc for our implementation

### 2. SDL2 Architecture (4-6 hours)

Understand:
- Window abstraction
- Event loop
- OpenGL context creation
- Platform backends

**Deliverable**: Port feasibility assessment

### 3. WebSocket Bridge Design (3-4 hours)

Plan:
- Syscall interception points
- Socket descriptor mapping
- Binary protocol
- Error handling

**Deliverable**: Architecture diagram

---

## Documentation to Write

### 1. Shader Programming Guide

`runtime/SHADERS.md`:
- How to write shaders for Linux/Wasm
- GLSL version support
- Examples and patterns
- Debugging tips

### 2. C++ Developer Guide

`runtime/CPP-GUIDE.md`:
- Setting up C++ projects
- Exception handling
- STL usage
- Performance tips

### 3. Performance Best Practices

`runtime/PERFORMANCE.md`:
- Message passing optimization
- Graphics batching
- Memory management
- Profiling techniques

---

## Testing Checklist

### Graphics Tests
- [ ] Shaders compile and link
- [ ] Textures load and display correctly
- [ ] VBOs work with vertex attributes
- [ ] Multiple draw calls per frame
- [ ] 60 FPS with 10K triangles

### C++ Tests
- [ ] Hello world compiles
- [ ] Exceptions work (try/catch/throw)
- [ ] STL containers (vector, map, string)
- [ ] Classes and inheritance
- [ ] Templates

### Integration Tests
- [ ] Graphics + C++ combined
- [ ] Multiple .wasm programs
- [ ] Build in Docker
- [ ] Deploy to web server

---

## Success Criteria

**Week 2 Success**: 
✅ 3D spinning cube with shaders running smoothly

**Week 3 Success**:
✅ C++ programs with exceptions working

**Week 4 Success**:
✅ Impressive demo that showcases platform capabilities

**Month 1 Complete**:
✅ Full 3D rendering pipeline
✅ C++ ecosystem functional  
✅ Demo video published
✅ GitHub stars increasing

---

## Getting Help

### Resources
- **WebGL Reference**: https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API
- **OpenGL ES Spec**: https://www.khronos.org/opengles/
- **Emscripten Source**: https://github.com/emscripten-core/emscripten
- **Wasm Exceptions**: https://github.com/WebAssembly/exception-handling

### Community
- Open GitHub discussions for design questions
- Share progress in README updates
- Create blog posts for major milestones

### Debugging
- Browser DevTools for JavaScript issues
- DWARF debug info for C/C++ issues  
- Console logs liberally during development
- Test incrementally, one feature at a time

---

## Daily Workflow Suggestion

```bash
# Morning: Pick one task from above
# Focus time: 2-4 hours of implementation
# Afternoon: Test, document, commit

# Evening: 
git add .
git commit -m "feat: add shader support to OpenGL"
git push

# Update ROADMAP.md progress
# Share progress (blog, Twitter, etc.)
```

---

**Start with shaders today!** 🚀

Everything else builds on a solid 3D graphics foundation. Once you have shaders working, the rest flows naturally.

Good luck and have fun! 🎨

