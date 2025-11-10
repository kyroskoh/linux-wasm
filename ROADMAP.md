# Linux/Wasm Development Roadmap

This document outlines the development roadmap for Linux/Wasm, building on the completed graphics and debug support implementation.

## Completed Features ✅

- [x] **DWARF Debug Support** - Line-by-line C debugging with `-g3` flags
- [x] **Graphics Foundation** - EGL/OpenGL ES interface with WebGL backend
- [x] **Basic OpenGL Functions** - Clear, viewport, color operations
- [x] **Shader Support** - Full GLSL vertex/fragment shader pipeline (31 functions!)
- [x] **Vertex Buffers** - VBO creation, binding, and data upload
- [x] **Shader Programs** - Compilation, linking, error checking
- [x] **Attributes & Uniforms** - Full vertex attribute and uniform variable support
- [x] **Drawing Functions** - glDrawArrays and glDrawElements
- [x] **Docker Integration** - Graphics compilation in containers
- [x] **Example Programs** - Working graphics demonstrations including shaders
- [x] **Comprehensive Documentation** - Complete API and usage guides

## Short Term Goals (Next 2-4 weeks)

### Priority 1: Expand Graphics API 🎨

**Goal**: Add textures and 3D capabilities on top of shader foundation

**Tasks**:
- [x] ~~Implement shader support~~ **COMPLETE!** ✅
  - ✅ `glCreateShader`, `glShaderSource`, `glCompileShader`
  - ✅ `glCreateProgram`, `glLinkProgram`, `glUseProgram`
  - ✅ `glGetShaderiv`, `glGetProgramiv` (error checking)
  - ✅ Shader info log retrieval
  
- [x] ~~Add buffer objects~~ **COMPLETE!** ✅
  - ✅ `glGenBuffers`, `glBindBuffer`
  - ✅ `glBufferData` (VBO upload)
  - ✅ Support for VBOs (IBOs ready for use)
  
- [x] ~~Implement vertex attributes~~ **COMPLETE!** ✅
  - ✅ `glVertexAttribPointer`, `glEnableVertexAttribArray`
  - ✅ `glGetAttribLocation`, `glGetUniformLocation`
  - ✅ `glUniform1f`, `glUniform1i`, `glUniformMatrix4fv`
  
- [x] ~~Add drawing functions~~ **COMPLETE!** ✅
  - ✅ `glDrawArrays`, `glDrawElements`
  
- [x] ~~Texture support~~ **COMPLETE!** ✅
  - ✅ `glGenTextures`, `glBindTexture`, `glDeleteTextures`
  - ✅ `glTexImage2D` (data upload)
  - ✅ `glTexParameteri`, `glTexParameterf` (filtering, wrapping)
  - ✅ Texture unit management (`glActiveTexture`)
  - ✅ Sampler uniforms in shaders
  - ✅ Example: `example-texture.c` (textured quad)
  
- [x] ~~Additional uniform types~~ **COMPLETE!** ✅
  - ✅ `glUniform2f`, `glUniform3f`, `glUniform4f`
  - ✅ `glUniform2fv`, `glUniform3fv`, `glUniform4fv`
  - ✅ Object ID mapping for texture handles
  
- [x] ~~Depth testing and state management~~ **COMPLETE!** ✅
  - ✅ `glEnable`/`glDisable` (GL_DEPTH_TEST, GL_BLEND, GL_CULL_FACE)
  - ✅ Full 3D depth buffer support

**Deliverable**: ✅ 3D spinning textured cube demo → **DELIVERED!** (`example-cube.c`)

**Estimated Time**: ~~1-2 weeks~~ **COMPLETE! Finished in 1 session!** ⚡

**Benefits**:
- ✅ Enables real 3D graphics applications
- ✅ Opens door to game development
- ✅ Demonstrates full graphics pipeline
- ✅ Attracts developer interest

---

### Priority 2: C++ Support & Exception Handling 🔧

**Goal**: Enable C++ programs to run with full exception support

**Tasks**:
- [ ] Test basic C++ compilation
  - Simple hello world in C++
  - Static constructors/destructors
  - Standard library usage
  
- [ ] Implement exception handling
  - Enable `-fwasm-exceptions` in build flags
  - Test try/catch/throw
  - Verify unwinding works correctly
  - Test with musl (may need libcxx/libcxxabi)
  
- [ ] Add C++ examples
  - Exception handling demo
  - STL container usage
  - Class hierarchies and inheritance
  - Template instantiation
  
- [ ] Update build system
  - Add C++ compilation support to `compile-graphics.sh`
  - Create `compile-cpp.sh` helper script
  - Add C++ flags to `linux-wasm.sh`
  
- [ ] Documentation
  - C++ programming guide for Linux/Wasm
  - Exception handling notes
  - STL compatibility matrix
  - Known limitations

**Deliverable**: Working C++ programs with exception handling

**Estimated Time**: 1 week

**Benefits**:
- Opens entire C++ ecosystem
- Exception safety for robust programs
- Enables C++ libraries and frameworks
- Modern C++ features available

---

### Priority 3: Create Impressive Graphics Demo 🎮

**Goal**: Showcase platform capabilities with stunning demo

**Options**:

**Option A: 3D Game Demo**
- Simple FPS-style renderer
- Textured 3D environment
- Mouse look controls
- Lighting effects

**Option B: Particle System**
- Thousands of particles
- Physics simulation
- Instanced rendering
- Visual effects (trails, glow)

**Option C: Shader Showcase**
- Multiple shader effects
- Real-time shader editing
- Post-processing effects
- Performance metrics overlay

**Recommended**: Start with Option C (shader showcase) as it demonstrates:
- Full graphics pipeline
- WebGL capabilities
- Real-time performance
- Educational value

**Estimated Time**: 1 week

---

## Medium Term Goals (1-3 months)

### Priority 4: Emscripten Integration 🔗

**Goal**: Leverage Emscripten's mature OpenGL ES implementation

**Approach**:

**Phase 1: Study Emscripten (Week 1)**
- [ ] Analyze `library_webgl.js` structure
- [ ] Review `library_egl.js` implementation
- [ ] Study their memory management
- [ ] Understand context/object mapping

**Phase 2: Adapt Core Libraries (Weeks 2-3)**
- [ ] Port GL function implementations
- [ ] Adapt to Linux/Wasm worker architecture
- [ ] Integrate with existing host callbacks
- [ ] Maintain backward compatibility

**Phase 3: Advanced Features (Week 4)**
- [ ] Implement full OpenGL ES 2.0 API
- [ ] Add OpenGL ES 3.0 features (if WebGL2)
- [ ] Support for extensions
- [ ] Performance optimizations

**Phase 4: Testing & Examples (Week 4)**
- [ ] Port Emscripten test suite
- [ ] Create compatibility examples
- [ ] Performance benchmarks
- [ ] Migration guide for Emscripten apps

**Deliverable**: Full OpenGL ES 2.0/3.0 compatibility

**Benefits**:
- 300+ OpenGL functions available
- Battle-tested implementation
- Wide application compatibility
- Production-ready graphics stack

---

### Priority 5: SDL2/GLFW Port 🎯

**Goal**: Enable cross-platform graphics libraries

**Tasks**:
- [ ] Port SDL2
  - Basic window management (mapped to canvas)
  - Event handling (keyboard, mouse)
  - OpenGL context creation (use EGL backend)
  - Timer and threading support
  
- [ ] Port GLFW alternative
  - Simpler than SDL2
  - Modern OpenGL focus
  - Good for prototypes
  
- [ ] Input handling
  - Keyboard events via existing console
  - Mouse events via canvas
  - Gamepad API integration
  - Touch events (mobile)
  
- [ ] Examples
  - SDL2 hello world
  - Simple SDL2 game
  - GLFW 3D demo

**Deliverable**: Working SDL2 or GLFW on Linux/Wasm

**Benefits**:
- Enables existing SDL2/GLFW applications
- Standard API for game development
- Large ecosystem of compatible software
- Can run many open-source games

---

### Priority 6: Networking via WebSockets 🌐

**Goal**: Enable network communication through browser APIs

**Tasks**:
- [ ] Implement syscall bridge
  - `socket()` → WebSocket creation
  - `connect()` → WebSocket.connect()
  - `send()`/`recv()` → WebSocket.send/onmessage
  - `close()` → WebSocket.close()
  
- [ ] Protocol support
  - TCP-like stream over WebSocket
  - Protocol negotiation
  - Binary data handling
  - Error mapping
  
- [ ] DNS resolution
  - Map `getaddrinfo()` to browser
  - Static host mapping
  - Fallback mechanisms
  
- [ ] HTTP/HTTPS support
  - `libcurl` port or alternative
  - Fetch API backend
  - REST API clients
  
- [ ] Example applications
  - Simple HTTP client
  - WebSocket chat
  - API consumer
  - Network utilities

**Deliverable**: Working network stack with WebSocket backend

**Benefits**:
- Enables networked applications
- HTTP/REST API access
- Real-time communication
- Multi-user applications possible

---

## Long Term Goals (3-6 months)

### Priority 7: GUI Framework 🖼️

**Goal**: Provide native-feeling GUI for applications

**Option A: ImGui Port** (Recommended for tools)
- Immediate mode GUI
- Great for debug UIs, tools, editors
- Minimal state management
- Easy to integrate

**Option B: Lightweight Custom GUI**
- Built on OpenGL ES foundation
- Optimized for Wasm
- Minimal dependencies
- Full control over implementation

**Tasks**:
- [ ] Choose framework (ImGui recommended)
- [ ] Port rendering backend to OpenGL ES
- [ ] Implement input handling
- [ ] Add font rendering
- [ ] Widget library
- [ ] Theming support
- [ ] Layout system
- [ ] Example applications

**Examples**:
- Text editor
- File browser
- System monitor
- Debug console

**Deliverable**: Working GUI with example applications

---

### Priority 8: Persistent Storage 💾

**Goal**: Save state between sessions

**Tasks**:
- [ ] IndexedDB backend
  - Map filesystem operations
  - Async I/O handling
  - Transaction management
  
- [ ] Virtual filesystem
  - Mount IndexedDB as `/mnt/persistent`
  - File/directory operations
  - Metadata storage
  
- [ ] State hibernation
  - Save memory state
  - Restore full system
  - Quick boot from snapshot
  
- [ ] Sync APIs
  - Upload/download saved state
  - Cloud storage integration
  - Backup/restore functionality

**Deliverable**: Persistent storage system

**Benefits**:
- Save user data
- Persist configurations
- Game save files
- Quick boot from hibernation
- Cloud sync possible

---

### Priority 9: Package Manager 📦

**Goal**: Create ecosystem of reusable Wasm packages

**Architecture**:
```
Package Repository (GitHub Pages / CDN)
    ↓
Package Manager (wapm-like)
    ↓
Installation to /usr/local or /opt
    ↓
Integration with initramfs
```

**Tasks**:
- [ ] Package format specification
  - Manifest (dependencies, version)
  - Binary (.wasm files)
  - Documentation
  - Examples
  
- [ ] Package manager tool
  - `lwpm install <package>`
  - `lwpm search <query>`
  - `lwpm update`
  - Dependency resolution
  
- [ ] Build system integration
  - Automated package building
  - CI/CD for packages
  - Version management
  
- [ ] Package repository
  - Static site or CDN
  - Package indexing
  - Search functionality
  
- [ ] Core packages
  - Popular CLI tools
  - Libraries (crypto, compression, etc.)
  - Utilities (jq, sqlite, etc.)

**Deliverable**: Working package ecosystem

---

### Priority 10: Performance Optimization ⚡

**Goal**: Make it production-ready through optimization

**Graphics Optimization**:
- [ ] Command batching
  - Queue OpenGL calls
  - Batch submit to reduce overhead
  - Async command buffers
  
- [ ] SharedArrayBuffer for sync operations
  - Direct memory access
  - Reduce message passing
  - Atomic operations
  
- [ ] OffscreenCanvas (when supported)
  - WebGL in worker thread
  - Reduced main thread load
  - Better parallelization

**General Optimization**:
- [ ] SIMD support
  - Enable Wasm SIMD
  - Optimize hot paths
  - Vectorized operations
  
- [ ] Memory optimization
  - Reduce memory footprint
  - Lazy loading
  - Memory pooling
  
- [ ] Boot optimization
  - Parallel initialization
  - Cached compilation
  - Snapshot loading
  
- [ ] Profiling infrastructure
  - Performance metrics
  - Bottleneck identification
  - Continuous monitoring

**Deliverable**: 2-5x performance improvement

---

## Experimental / Research Ideas 🔬

### Multi-Instance & IPC
- Run multiple Linux/Wasm instances
- SharedWorker communication
- Container-like isolation
- Microservices in browser

### Development Tools
- gdb remote debugging stub
- strace for system call tracing
- Performance profiling tools
- Memory debugging utilities

### Audio Support
- Web Audio API integration
- ALSA/OSS emulation
- Audio playback and recording
- Synthesis and effects

### Advanced Graphics
- Compute shaders (WebGL2)
- Transform feedback
- Multiple render targets
- Advanced post-processing

### AI/ML Integration
- TensorFlow.js integration
- ONNX runtime
- GPU acceleration
- Model inference examples

---

## Implementation Priorities

### Immediate (Start Now)
1. ✅ Shader support for 3D rendering
2. ✅ C++ exception handling
3. ✅ Create impressive demo

### Next Month
4. Emscripten OpenGL integration
5. SDL2 port
6. Network bridge basics

### Following Months
7. GUI framework (ImGui)
8. Persistent storage
9. Package manager
10. Performance optimization

---

## Success Metrics

### Technical Metrics
- [ ] Full OpenGL ES 2.0 API coverage
- [ ] <5ms graphics call latency
- [ ] C++ exceptions working
- [ ] 50+ packages available
- [ ] Boot time <5 seconds

### Community Metrics
- [ ] 100+ GitHub stars
- [ ] 10+ contributors
- [ ] 20+ example applications
- [ ] Active Discord/forum
- [ ] Documentation site

### Demonstration Metrics
- [ ] 3D game running smoothly
- [ ] Complex C++ application working
- [ ] Network-enabled app demo
- [ ] Persistent storage demo
- [ ] Complete development workflow

---

## Resources Needed

### Documentation
- API reference sites
- Tutorial videos
- Example gallery
- Blog posts

### Infrastructure
- Package CDN
- Demo hosting
- CI/CD pipelines
- Issue tracking

### Community
- Discord server
- GitHub discussions
- Contributing guidelines
- Code of conduct

---

## Timeline Overview

```
Month 1: Expand Graphics + C++ Support
├─ Week 1: Shaders & basic 3D
├─ Week 2: Textures & buffers  
├─ Week 3: C++ exceptions
└─ Week 4: Impressive demo

Month 2: Emscripten Integration
├─ Week 1: Study Emscripten GL
├─ Week 2-3: Port core libraries
└─ Week 4: Testing & examples

Month 3: SDL2 & Networking
├─ Week 1-2: SDL2 port
└─ Week 3-4: WebSocket networking

Months 4-6: Polish & Ecosystem
├─ GUI framework
├─ Persistent storage
├─ Package manager
└─ Performance optimization
```

---

## Getting Started

### For Graphics Extension (Start Here!)
```bash
# 1. Review Emscripten's implementation
# Visit: https://github.com/emscripten-core/emscripten/blob/main/src/library_webgl.js

# 2. Add shader functions to linux-worker.js
# See: runtime/GRAPHICS.md section "Adding More OpenGL Functions"

# 3. Create spinning cube example
cp runtime/example-graphics.c runtime/example-cube3d.c
# Edit to add shaders and 3D rendering

# 4. Test and iterate
./tools/compile-graphics.sh runtime/example-cube3d.c
```

### For C++ Support
```bash
# 1. Create test program
cat > test-cpp.cpp << 'EOF'
#include <iostream>
#include <stdexcept>
int main() {
    try {
        throw std::runtime_error("Test");
    } catch (const std::exception& e) {
        std::cout << "Caught: " << e.what() << std::endl;
    }
    return 0;
}
EOF

# 2. Try compiling
$LW_INSTALL/llvm/bin/clang++ \
  --target=wasm32-unknown-unknown \
  --sysroot=$LW_INSTALL/musl \
  -fPIC -shared -fwasm-exceptions \
  -o test.wasm test-cpp.cpp

# 3. Debug any issues
# 4. Document working approach
```

---

## WebAssembly Wish List

Some features in this roadmap are limited by current WebAssembly capabilities. We've compiled a list of desired WebAssembly features that would significantly benefit Linux/Wasm:

- **MMU Support** - For memory sharing and protection
- **Thread Suspension** - For proper signal handling and debugging
- **ELF Binary Format** - For better tool compatibility
- **Shared Wasm Instances** - For resource optimization
- **Memory Address Breakpoints** - For enhanced debugging

See **[WISHLIST.md](WISHLIST.md)** for detailed explanations and impact assessments.

These features would unlock new capabilities for Linux/Wasm and the broader WebAssembly ecosystem.

---

## Contributing

This roadmap is a living document. Contributions and suggestions are welcome!

To propose changes:
1. Open an issue describing the feature
2. Discuss approach and priorities
3. Update this roadmap with consensus
4. Implement and document

---

## Notes

- Priorities may shift based on community feedback
- Some items may be easier/harder than estimated
- New opportunities may arise (WebGPU, WASI evolution, etc.)
- Focus on completing one thing well over many things partially

**Let's build something amazing!** 🚀

