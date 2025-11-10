# Graphics Quick Start Guide

## Testing the Graphics System

### 1. Build the Example Program

```bash
# After building Linux/Wasm, compile the example graphics program
cd runtime
$LW_INSTALL/llvm/bin/clang \
  --target=wasm32-unknown-unknown \
  --sysroot=$LW_INSTALL/musl \
  -fPIC -shared \
  -o example-graphics.wasm \
  example-graphics.c
```

### 2. Add to Initramfs

Copy the compiled program to your BusyBox installation before building the initramfs:

```bash
cp example-graphics.wasm $LW_INSTALL/busybox/bin/
# Then rebuild initramfs
./linux-wasm.sh build-initramfs
```

### 3. Run in Browser

1. Start the web server:
   ```bash
   cd runtime
   python3 server.py
   ```

2. Open browser to `http://localhost:8000`

3. In the Linux/Wasm terminal, run:
   ```bash
   /bin/example-graphics.wasm
   ```

4. Watch the canvas appear with animated colors!

## Creating Your Own Graphics Program

### Minimal Example

```c
#include "wasm-graphics.h"
#include <stdio.h>

int main() {
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    // Initialize
    if (graphics_initialize(&display, &surface, &context) != 0) {
        fprintf(stderr, "Graphics init failed\n");
        return 1;
    }

    // Set viewport to canvas size
    glViewport(0, 0, 800, 600);

    // Clear to red
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Show on screen
    eglSwapBuffers(display, surface);

    printf("Red screen displayed!\n");
    return 0;
}
```

### Build and Run

```bash
# Compile
clang --target=wasm32-unknown-unknown \
  --sysroot=$LW_INSTALL/musl \
  -fPIC -shared \
  -o my-graphics.wasm \
  my-graphics.c

# Copy to initramfs
cp my-graphics.wasm $LW_INSTALL/busybox/bin/
./linux-wasm.sh build-initramfs

# Run (after starting web server and loading page)
/bin/my-graphics.wasm
```

## Available Functions

### EGL Functions
- `eglGetDisplay()` - Get display handle
- `eglInitialize()` - Initialize EGL
- `eglChooseConfig()` - Select graphics configuration
- `eglCreateWindowSurface()` - Create rendering surface
- `eglCreateContext()` - Create rendering context
- `eglMakeCurrent()` - Activate context
- `eglSwapBuffers()` - Present frame

### OpenGL ES Functions (Currently Implemented)
- `glClear()` - Clear buffers
- `glClearColor()` - Set clear color
- `glViewport()` - Set viewport size

### Adding More Functions

See `runtime/GRAPHICS.md` for instructions on extending the API.

## Common Issues

### Canvas Not Appearing
- Make sure `graphics_initialize()` returns 0 (success)
- Check browser console for WebGL errors
- Verify cross-origin isolation is working

### Colors Look Wrong
- WebGL uses 0.0-1.0 range for colors, not 0-255
- Alpha channel should be 1.0 for opaque colors

### Program Crashes
- Ensure all EGL functions return successfully before using OpenGL
- Check that all pointers are valid
- Add debug printf statements to trace execution

## Next Steps

1. **Study the Example**: `runtime/example-graphics.c` shows a complete working program
2. **Read Full Docs**: `runtime/GRAPHICS.md` has comprehensive information
3. **Extend the API**: Add more OpenGL functions as needed for your application
4. **Integrate Emscripten**: Consider using Emscripten's comprehensive graphics libraries

## Performance Tips

- Batch draw calls to minimize message passing overhead
- Consider using offscreen rendering for complex scenes
- Profile with browser DevTools to identify bottlenecks

## Resources

- **EGL Reference**: https://www.khronos.org/egl/
- **OpenGL ES Reference**: https://www.khronos.org/opengles/
- **WebGL Reference**: https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API

Happy graphics programming! 🎨

