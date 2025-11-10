# Graphics Development with Docker

This guide explains how to develop and compile graphics applications for Linux/Wasm using Docker.

## Quick Start

### 1. Build Docker Containers

```bash
# From the project root
docker build -t linux-wasm-base:dev ./docker/linux-wasm-base
docker build -t linux-wasm-contained:dev ./docker/linux-wasm-contained
```

### 2. Run Development Container

```bash
# With host directory mounted (recommended for development)
docker run -it --name my-linux-wasm \
  --mount type=bind,src="$(pwd)",target=/linux-wasm \
  linux-wasm-base:dev bash
```

### 3. Build Everything (Inside Container)

```bash
# Build all components including graphics examples
/linux-wasm/linux-wasm.sh all
```

This will:
- Download and build LLVM, kernel, musl, busybox
- Compile graphics examples automatically
- Create initramfs with graphics examples included

### 4. Compile Custom Graphics Programs

```bash
# Inside the Docker container

# Compile the example
./tools/compile-graphics.sh runtime/examples/example-graphics.c

# Or create and compile your own
cat > my-graphics.c << 'EOF'
#include "runtime/wasm-graphics.h"
#include <stdio.h>

int main() {
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    if (graphics_initialize(&display, &surface, &context) != 0) {
        fprintf(stderr, "Graphics init failed\n");
        return 1;
    }

    glViewport(0, 0, 800, 600);
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    eglSwapBuffers(display, surface);

    printf("Green screen displayed!\n");
    return 0;
}
EOF

./tools/compile-graphics.sh my-graphics.c
```

### 5. Rebuild Initramfs

```bash
# After compiling new graphics programs
./linux-wasm.sh build-initramfs
```

### 6. Copy Runtime to Host

```bash
# From your host machine (outside Docker)
# The compiled files are in workspace/install/

# Copy to your web server directory
cp -r workspace/install/kernel/vmlinux.wasm /path/to/webserver/
cp workspace/install/initramfs/initramfs.cpio.gz /path/to/webserver/
cp -r runtime/* /path/to/webserver/
```

## Complete Workflow Example

Here's a complete example of developing a graphics application:

### Step 1: Start Docker Container

```bash
docker run -it --rm \
  --name graphics-dev \
  --mount type=bind,src="$(pwd)",target=/linux-wasm \
  linux-wasm-base:dev bash
```

### Step 2: Build System (First Time Only)

```bash
cd /linux-wasm
./linux-wasm.sh all
```

This takes a while but only needs to be done once.

### Step 3: Create Your Graphics App

```bash
# Create a new graphics application
cat > /linux-wasm/my-spinning-triangle.c << 'EOF'
#include "runtime/wasm-graphics.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>

int main() {
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    printf("Spinning Triangle Demo\n");

    if (graphics_initialize(&display, &surface, &context) != 0) {
        fprintf(stderr, "Graphics initialization failed\n");
        return 1;
    }

    glViewport(0, 0, 800, 600);
    printf("Graphics initialized!\n");

    // Simple animation loop
    for (int frame = 0; frame < 360; frame++) {
        float hue = frame / 360.0f;
        
        // Calculate color based on frame
        float r = (sin(hue * 6.28f) + 1.0f) / 2.0f;
        float g = (sin(hue * 6.28f + 2.09f) + 1.0f) / 2.0f;
        float b = (sin(hue * 6.28f + 4.18f) + 1.0f) / 2.0f;

        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        eglSwapBuffers(display, surface);

        if (frame % 60 == 0) {
            printf("Frame %d/360\n", frame);
        }

        usleep(16667); // ~60 FPS
    }

    printf("Animation complete!\n");
    return 0;
}
EOF
```

### Step 4: Compile

```bash
./tools/compile-graphics.sh my-spinning-triangle.c

# The .wasm file is created in the current directory
# and automatically copied to busybox/bin/
```

### Step 5: Rebuild Initramfs

```bash
./linux-wasm.sh build-initramfs
```

### Step 6: Deploy and Test

```bash
# Exit Docker (or open another terminal)
exit

# On host: Copy runtime files to web server
mkdir -p ~/webserver
cp workspace/install/kernel/vmlinux.wasm ~/webserver/
cp workspace/install/initramfs/initramfs.cpio.gz ~/webserver/
cp -r runtime/* ~/webserver/

# Start web server
cd ~/webserver
python3 -m http.server 8000
```

Then:
1. Open `http://localhost:8000` in browser
2. Wait for Linux to boot
3. In the terminal, run: `/bin/my-spinning-triangle.wasm`
4. Watch the graphics canvas appear with your animation!

## Directory Structure in Docker

```
/linux-wasm/
├── docker/                    # Docker configuration
├── patches/                   # Kernel and software patches
├── runtime/                   # JavaScript runtime + graphics
│   ├── wasm-graphics.h       # Graphics API header
│   ├── example-graphics.c    # Example program
│   ├── linux.js              # Main thread runtime
│   ├── linux-worker.js       # Worker thread runtime
│   └── index.html            # Web page with canvas
├── tools/
│   └── compile-graphics.sh   # Helper script
├── linux-wasm.sh             # Main build script
└── workspace/                # Build output (created during build)
    ├── src/                  # Downloaded sources
    ├── build/                # Build directories
    └── install/              # Installed components
        ├── llvm/             # Compiler toolchain
        ├── musl/             # C library
        ├── busybox/          # Userspace programs
        ├── kernel/           # vmlinux.wasm
        ├── initramfs/        # initramfs.cpio.gz
        └── graphics-examples/ # Compiled graphics programs
```

## Environment Variables

You can customize the build by setting environment variables:

```bash
# Use custom workspace directory
docker run -it \
  -e LW_WORKSPACE=/workspace \
  --mount type=bind,src="$(pwd)",target=/linux-wasm \
  linux-wasm-base:dev bash

# Disable debug symbols for smaller files
docker run -it \
  -e LW_DEBUG_CFLAGS="" \
  --mount type=bind,src="$(pwd)",target=/linux-wasm \
  linux-wasm-base:dev bash

# Custom parallel jobs
docker run -it \
  -e LW_JOBS_LLVM_COMPILE=8 \
  --mount type=bind,src="$(pwd)",target=/linux-wasm \
  linux-wasm-base:dev bash
```

## Incremental Development

After the initial `./linux-wasm.sh all`, you can rebuild individual components:

```bash
# Only rebuild graphics examples
./linux-wasm.sh build-graphics-examples

# Only rebuild initramfs (after adding new programs)
./linux-wasm.sh build-initramfs

# Rebuild busybox and initramfs
./linux-wasm.sh build-busybox build-initramfs
```

## Troubleshooting

### Graphics Example Not Found

If `/bin/example-graphics.wasm` is not in the initramfs:

```bash
# Explicitly rebuild
./linux-wasm.sh build-graphics-examples
./linux-wasm.sh build-initramfs
```

### Compilation Errors

If compilation fails, check that prerequisites are built:

```bash
# Ensure LLVM is built
ls -la workspace/install/llvm/bin/clang

# Ensure musl is built
ls -la workspace/install/musl/lib/

# If missing, rebuild
./linux-wasm.sh build-llvm
./linux-wasm.sh build-musl
```

### Permission Issues with Workspace

If you get permission errors accessing the workspace:

```bash
# On host, fix permissions
sudo chown -R $(id -u):$(id -g) workspace/

# Or use Docker volumes
docker run -it \
  -v linux-wasm-workspace:/linux-wasm/workspace \
  --mount type=bind,src="$(pwd)",target=/linux-wasm \
  linux-wasm-base:dev bash
```

## Building Without Host Mount

For CI/CD or build servers, use the contained build:

```bash
# Build everything in container
docker run -it linux-wasm-contained:dev

# Extract results to host
docker cp <container-id>:/linux-wasm/workspace/install ./install
```

## Advanced: Custom Graphics Libraries

To create reusable graphics libraries:

```bash
# Create a graphics utility library
cat > graphics-utils.c << 'EOF'
#include "runtime/wasm-graphics.h"

void clear_screen(float r, float g, float b) {
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void present_frame(EGLDisplay display, EGLSurface surface) {
    eglSwapBuffers(display, surface);
}
EOF

# Compile to object file
$LW_INSTALL/llvm/bin/clang \
  --target=wasm32-unknown-unknown \
  --sysroot=$LW_INSTALL/musl \
  -fPIC -c \
  -o graphics-utils.o \
  graphics-utils.c

# Link with your application
$LW_INSTALL/llvm/bin/clang \
  --target=wasm32-unknown-unknown \
  --sysroot=$LW_INSTALL/musl \
  -fPIC -shared \
  -o my-app.wasm \
  my-app.c graphics-utils.o
```

## Next Steps

1. **Extend the API**: See `runtime/GRAPHICS.md` for adding more OpenGL functions
2. **Integrate Emscripten**: Study Emscripten's WebGL implementation for advanced features
3. **Create Complex Apps**: Build games, visualizations, or GUI applications
4. **Performance Tuning**: Profile and optimize using browser DevTools

For more information:
- `runtime/GRAPHICS.md` - Complete graphics documentation
- `runtime/GRAPHICS-QUICKSTART.md` - Quick reference guide
- `patches/kernel/README-graphics.md` - Kernel driver information

Happy graphics development! 🎨

