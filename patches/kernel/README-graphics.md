# Kernel Graphics Driver for Linux/Wasm

## Overview

This document describes how to implement a kernel-level graphics driver for Linux/Wasm. Currently, graphics are accessible via JavaScript host callbacks directly from user programs. A proper kernel driver would provide standard Linux graphics interfaces.

## Proposed Approaches

### Option 1: Framebuffer Driver (`/dev/fb0`)

Implement a simple framebuffer device similar to the console driver in `0010-Add-Wasm-console-support.patch`.

**Benefits:**
- Simple interface
- Many existing programs support framebuffer
- Easy to implement

**Structure:**
```c
// In arch/wasm/drivers/video/wasmfb.c

static struct fb_ops wasmfb_ops = {
    .owner = THIS_MODULE,
    .fb_read = wasmfb_read,
    .fb_write = wasmfb_write,
    .fb_setcolreg = wasmfb_setcolreg,
    .fb_fillrect = cfb_fillrect,
    .fb_copyarea = cfb_copyarea,
    .fb_imageblit = cfb_imageblit,
};

// Host callback to transfer framebuffer data to WebGL
extern void wasm_framebuffer_update(void *pixels, int width, int height, int stride);
```

**Usage from userspace:**
```c
int fd = open("/dev/fb0", O_RDWR);
struct fb_var_screeninfo vinfo;
ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);

// Memory map the framebuffer
void *fb = mmap(NULL, vinfo.xres * vinfo.yres * (vinfo.bits_per_pixel / 8),
                PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

// Draw directly to framebuffer memory
uint32_t *pixels = (uint32_t *)fb;
pixels[y * vinfo.xres + x] = color;

// Flush to WebGL
ioctl(fd, FBIOPAN_DISPLAY, &vinfo);
```

### Option 2: DRM (Direct Rendering Manager)

Implement a DRM driver for more modern graphics stack support.

**Benefits:**
- Modern interface (used by Wayland, modern X.org)
- Supports acceleration
- Better for complex graphics

**Drawbacks:**
- Much more complex to implement
- Requires DRM/KMS subsystem support

**Structure:**
```c
// In arch/wasm/drivers/gpu/drm/wasm/wasm_drm.c

static const struct drm_driver wasm_driver = {
    .driver_features = DRIVER_GEM | DRIVER_MODESET | DRIVER_ATOMIC,
    .name = "wasm-drm",
    .desc = "WebAssembly DRM Driver",
    // ... more callbacks
};

// Host callbacks
extern void wasm_drm_create_context(int context_id);
extern void wasm_drm_submit_commands(void *cmd_buffer, size_t size);
```

### Option 3: Hybrid Approach (Recommended)

Keep the current user-space approach for maximum flexibility, but add optional framebuffer support for compatibility.

**Why:**
- Current approach works well for direct EGL/OpenGL ES usage
- Adding framebuffer gives compatibility with existing Linux software
- No need for complex DRM implementation initially

## Implementation Steps for Framebuffer Driver

### 1. Create Driver Source Files

Create `arch/wasm/drivers/video/wasmfb.c` (similar to `arch/wasm/drivers/tty/hvc_wasm.c`).

### 2. Add Host Callbacks

In `arch/wasm/kernel/host_bindings.h`:

```c
// Framebuffer host callbacks
extern void wasm_fb_init(int width, int height, int bpp);
extern void wasm_fb_update(void *pixels, int x, int y, int width, int height);
extern void wasm_fb_swap_buffers(void);
```

### 3. Register Driver

In `arch/wasm/kernel/setup.c`:

```c
extern int wasmfb_init(void);

void __init wasm_init_devices(void)
{
    // ... existing device initialization
    wasmfb_init();
}
```

### 4. Configure Build System

Update `arch/wasm/Kconfig`:

```kconfig
config WASM_FRAMEBUFFER
    bool "WebAssembly framebuffer support"
    depends on FB
    select FB_CFB_FILLRECT
    select FB_CFB_COPYAREA
    select FB_CFB_IMAGEBLIT
    help
      This enables a framebuffer device (/dev/fb0) backed by
      WebGL in the browser.
```

Update `arch/wasm/drivers/video/Makefile`:

```makefile
obj-$(CONFIG_WASM_FRAMEBUFFER) += wasmfb.o
```

### 5. JavaScript Runtime Support

Add to `linux-worker.js`:

```javascript
wasm_fb_init: (width, height, bpp) => {
  // Initialize framebuffer texture in WebGL
  port.postMessage({
    method: "fb_init",
    width: width,
    height: height,
    bpp: bpp,
  });
},

wasm_fb_update: (pixels, x, y, width, height) => {
  // Transfer framebuffer data to main thread
  const data = new Uint8Array(memory.buffer, pixels, width * height * 4);
  port.postMessage({
    method: "fb_update",
    pixels: data,
    x: x,
    y: y,
    width: width,
    height: height,
  });
},
```

Add to `linux.js`:

```javascript
fb_init: (message) => {
  if (graphics && graphics.gl) {
    // Create texture for framebuffer
    const gl = graphics.gl;
    graphics.fbTexture = gl.createTexture();
    // ... setup texture
  }
},

fb_update: (message) => {
  if (graphics && graphics.gl && graphics.fbTexture) {
    // Upload pixel data to texture
    const gl = graphics.gl;
    gl.bindTexture(gl.TEXTURE_2D, graphics.fbTexture);
    gl.texSubImage2D(gl.TEXTURE_2D, 0, 
                     message.x, message.y,
                     message.width, message.height,
                     gl.RGBA, gl.UNSIGNED_BYTE, message.pixels);
    // Render texture to canvas
    // ... render quad with texture
  }
},
```

## References

- Linux Framebuffer Documentation: `Documentation/fb/` in Linux source
- Simple Framebuffer Driver: `drivers/video/fbdev/simplefb.c`
- Wasm Console Driver Example: `patches/kernel/0010-Add-Wasm-console-support.patch`
- DRM Documentation: `Documentation/gpu/` in Linux source

## Current Status

✅ User-space EGL/OpenGL ES interface (functional)
❌ Kernel framebuffer driver (not implemented, optional)
❌ DRM driver (not planned initially)

The current user-space approach is sufficient for most use cases. A kernel driver would primarily benefit programs that:
- Expect `/dev/fb0` to exist
- Use framebuffer directly without EGL/OpenGL
- Need compatibility with existing Linux graphics software

For new programs, using the EGL/OpenGL ES interface directly (via `runtime/wasm-graphics.h`) is recommended.

