# Next Steps - Immediate Action Items

Quick reference for what to work on next. See [ROADMAP.md](ROADMAP.md) for the complete development plan.

## 🎉 Recently Completed (November 10, 2025)

**Graphics System - COMPLETE!** ✅
- ✅ Full shader pipeline (GLSL vertex/fragment shaders)
- ✅ Texture support (generation, upload, sampling, filtering)
- ✅ 3D rendering with depth testing
- ✅ Uniforms (scalars, vectors, matrices)
- ✅ VBOs, EBOs, vertex attributes
- ✅ 4 working examples including spinning textured cube
- ✅ Complete documentation

**Implementation Summary:**
- 46 OpenGL ES 2.0 functions implemented
- ~700 lines of runtime code
- ~680 lines of example code
- ~2000 lines of documentation
- All in a single development session! ⚡

**See:** [TEXTURE-3D-IMPLEMENTATION.md](TEXTURE-3D-IMPLEMENTATION.md), [SHADER-IMPLEMENTATION.md](SHADER-IMPLEMENTATION.md)

---

## 🎯 Current Focus (Next 2-4 Weeks)

### Priority 1: Polish & Showcase 🎨 (3-5 days) - **IN PROGRESS**

**Goal:** Create impressive demo to showcase platform capabilities

**Tasks:**

**1. Enhanced Multi-Cube Demo** (Day 1-2) ✅ **COMPLETE!**
- [x] Create `runtime/examples/example-demo.c`
- [x] 7 rotating cubes with different colors
- [x] Smooth camera orbit
- [x] FPS counter display
- [x] Performance metrics overlay
- [x] Unique textures per cube
- [x] Color tinting system
- [x] Professional console output

**2. Record Demo Video** (Day 3)
- [ ] Capture smooth 60 FPS footage
- [ ] Show texture system
- [ ] Demonstrate shader compilation
- [ ] Highlight depth testing

**3. Documentation Polish** (Day 4-5)
- [ ] Add demo screenshots to README
- [ ] Create GIF of spinning cubes
- [ ] Update feature highlights
- [ ] Blog post about implementation

**Deliverable:** Impressive demo + marketing materials

**Success Criteria:**
- ✅ Smooth 60 FPS with 5+ cubes
- ✅ Visual appeal (colors, motion, effects)
- ✅ Professional demo video
- ✅ Attracts GitHub stars

---

### Priority 2: C++ Exception Support 🔧 (1 week)

**Goal:** Enable C++ programs with full exception support

**Tasks:**

**1. Test Basic C++ Compilation** (Day 1)
```bash
# Create test program
cat > test-hello.cpp << 'EOF'
#include <iostream>
int main() {
    std::cout << "Hello from C++!" << std::endl;
    return 0;
}
EOF

# Try compiling
$LW_INSTALL/llvm/bin/clang++ \
  --target=wasm32-unknown-unknown \
  --sysroot=$LW_INSTALL/musl \
  -fPIC -shared \
  -o test-hello.wasm test-hello.cpp

# Test in browser
./linux-wasm.sh build-initramfs
# Run: /bin/test-hello.wasm
```

**2. Test Exceptions** (Day 2-3)

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

**3. Create C++ Graphics Example** (Day 4-5)
```cpp
// runtime/examples/example-cpp-cube.cpp
#include "../wasm-graphics.h"
#include <iostream>
#include <vector>
#include <memory>

class Cube {
    std::vector<float> vertices;
    GLuint vbo;
public:
    Cube() { /* initialize */ }
    void render() { /* draw */ }
};

int main() {
    try {
        std::vector<std::unique_ptr<Cube>> cubes;
        // Create and render cubes
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
```

**4. Update Build System** (Day 6-7)
- [ ] Create `tools/compile-cpp.sh` helper script
- [ ] Add C++ examples to build system
- [ ] Test in Docker environment
- [ ] Document C++ workflow

**Common Issues & Solutions:**
1. **Missing libcxx**: Build LLVM with libc++ runtime
2. **Exception symbols undefined**: Verify `-fwasm-exceptions` flag
3. **Runtime errors**: Check browser exception handling support

**Deliverable:** Working C++ programs with exceptions + `runtime/CPP-SUPPORT.md`

**Success Criteria:**
- ✅ C++ hello world works
- ✅ Exceptions (try/catch/throw) work
- ✅ STL containers work
- ✅ C++ graphics example runs smoothly

---

### Priority 3: Expand Graphics API 🎮 (1-2 weeks)

**Goal:** Add more OpenGL functions for advanced rendering

**Phase 1: State Management** (3-4 days)

**1. Blending Functions**
```javascript
// Add to runtime/linux-worker.js
wasm_gl_blend_func: (sfactor, dfactor) => { /* ... */ }
wasm_gl_blend_equation: (mode) => { /* ... */ }
```

**2. Depth Functions**
```javascript
wasm_gl_depth_func: (func) => { /* ... */ }
wasm_gl_depth_mask: (flag) => { /* ... */ }
```

**3. Culling and Winding**
```javascript
wasm_gl_cull_face: (mode) => { /* ... */ }
wasm_gl_front_face: (mode) => { /* ... */ }
```

**Phase 2: Advanced Features** (4-5 days)

**1. Framebuffer Objects (FBO)** ⭐ High Value
```javascript
wasm_gl_gen_framebuffers: (n, framebuffers) => { /* ... */ }
wasm_gl_bind_framebuffer: (target, framebuffer) => { /* ... */ }
wasm_gl_framebuffer_texture2d: (target, attachment, textarget, texture, level) => { /* ... */ }
```

Benefits:
- Render to texture
- Post-processing effects
- Multi-pass rendering
- Shadow maps

**2. More Uniform Types**
```javascript
wasm_gl_uniform2i: (location, v0, v1) => { /* ... */ }
wasm_gl_uniform3i: (location, v0, v1, v2) => { /* ... */ }
wasm_gl_uniform4i: (location, v0, v1, v2, v3) => { /* ... */ }
```

**3. Query Functions**
```javascript
wasm_gl_get_integerv: (pname, params) => { /* ... */ }
wasm_gl_get_floatv: (pname, params) => { /* ... */ }
wasm_gl_get_string: (name) => { /* ... */ }
```

**Phase 3: Examples** (2-3 days)

- [ ] Render-to-texture example
- [ ] Post-processing (blur, bloom)
- [ ] Shadow mapping demo
- [ ] Transparent objects (blending)

**Deliverable:** Near-complete OpenGL ES 2.0 implementation

**Success Criteria:**
- ✅ 70+ OpenGL functions total
- ✅ FBO example working
- ✅ Post-processing effects demo
- ✅ Compatible with more OpenGL code

---

---

## Medium-Term Goals (Weeks 4-8)

### Priority 4: Input Handling 🖱️ (3-5 days)

**Goal:** Enable mouse and keyboard input beyond terminal

**Tasks:**

**1. Mouse Input**

Add mouse events to canvas:

```javascript
// In index.html
canvas.addEventListener('mousemove', (e) => {
  // Forward to Linux/Wasm
  os.mouse_move(e.clientX, e.clientY);
});
```

**2. Keyboard Input** (separate from terminal)
```javascript
// In index.html
window.addEventListener('keydown', (e) => {
  if (e.key === 'Escape') {
    // Send to Linux/Wasm
    os.key_press(e.key, e.keyCode);
  }
});
```

**3. Gamepad API**
```javascript
wasm_gamepad_update: () => {
  const gamepads = navigator.getGamepads();
  // Forward button states and axes
},
```

**Examples:**
- [ ] Mouse-controlled camera
- [ ] WASD movement
- [ ] Click to interact
- [ ] Gamepad test program

---

### Priority 5: Audio Support 🔊 (5-7 days)

**Goal:** Basic audio playback via Web Audio API

**Tasks:**

**1. Audio Playback**
```javascript
wasm_audio_play: (buffer, size, rate) => {
  const audioContext = new AudioContext();
  // Play audio from memory buffer
},
```

**2. Synthesis**
```javascript
wasm_audio_tone: (frequency, duration) => {
  // Generate simple tones
},
```

**3. Examples**
- [ ] Play WAV file
- [ ] Synthesize beeps
- [ ] Background music
- [ ] Sound effects

---

### Priority 6: Emscripten Integration 🔗 (2-4 weeks)

**Goal:** Leverage Emscripten's mature OpenGL implementation

See ROADMAP.md Priority 4 for detailed breakdown.

**Why:** 300+ OpenGL functions, battle-tested, production-ready

---

### Priority 7: SDL2 Port 🎯 (2-3 weeks)

**Goal:** Enable cross-platform graphics libraries

See ROADMAP.md Priority 5 for detailed breakdown.

**Why:** Enables existing SDL2 applications and games

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

## Quick Reference - What to Work On

### This Week (Priority 1)
- [ ] Enhanced demo with 5-7 cubes
- [ ] FPS counter overlay
- [ ] Demo video recording
- [ ] Screenshots and GIFs

### Next Week (Priority 2)
- [ ] C++ hello world
- [ ] Exception handling tests
- [ ] C++ graphics example
- [ ] Build system updates

### Week 3-4 (Priority 3)
- [ ] FBO support
- [ ] More uniforms and state functions
- [ ] Post-processing example
- [ ] Query functions

### Beyond (Priorities 4-7)
- Input handling (mouse, keyboard, gamepad)
- Audio support
- Emscripten integration
- SDL2 port

---

## Success Metrics

### Immediate (This Month)
- [ ] Demo video published
- [ ] GitHub stars > 50
- [ ] C++ examples working
- [ ] 70+ OpenGL functions

### Medium-Term (3 Months)
- [ ] SDL2 port working
- [ ] Input handling complete
- [ ] Audio playback working
- [ ] 100+ GitHub stars

### Long-Term (6 Months)
- [ ] Full OpenGL ES 2.0 coverage
- [ ] Package ecosystem started
- [ ] Network support via WebSockets
- [ ] 500+ GitHub stars

---

## Daily Workflow

```bash
# Morning: Pick one task from current priority
# Focus: 2-4 hours of implementation
# Test: Compile, run, verify

# Commit:
git add .
git commit -m "feat: add multi-cube demo"
git push

# Share progress:
# - Update ROADMAP.md
# - Tweet/blog about milestone
# - Screenshots/GIFs
```

---

## Resources

**Current Focus:**
- [example-cube.c](runtime/examples/example-cube.c) - Base for demo
- [TEXTURE-3D-IMPLEMENTATION.md](TEXTURE-3D-IMPLEMENTATION.md) - What's implemented
- [runtime/wasm-graphics.h](runtime/wasm-graphics.h) - API reference

**Next Phase:**
- [WebGL Reference](https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API)
- [OpenGL ES 2.0 Spec](https://www.khronos.org/opengles/)
- [Emscripten Source](https://github.com/emscripten-core/emscripten)

---

**Current Status:** 🎨 Creating impressive demo! **Graphics system complete!** ✅

Let's showcase what we've built and attract more contributors! 🚀

