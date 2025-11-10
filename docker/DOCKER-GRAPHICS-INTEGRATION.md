# Docker Graphics Integration Summary

This document summarizes the Docker integration for graphics compilation in Linux/Wasm.

## Changes Made

### 1. Updated Build Script (`linux-wasm.sh`)

Added new build target `build-graphics-examples`:

```bash
./linux-wasm.sh build-graphics-examples
```

This target:
- Compiles `runtime/example-graphics.c` to `example-graphics.wasm`
- Copies the compiled binary to `busybox/bin/` for initramfs inclusion
- Copies `wasm-graphics.h` header to the install directory
- Runs automatically as part of `./linux-wasm.sh all`

**Files Modified:**
- `linux-wasm.sh` - Added `build-graphics-examples` target

### 2. Updated Docker Configuration

**File: `docker/linux-wasm-contained/Dockerfile`**

Added `runtime` directory to Docker image:

```dockerfile
COPY runtime runtime
```

This ensures all graphics files are available in the container:
- `wasm-graphics.h` - Graphics API header
- `example-graphics.c` - Example program
- `linux.js`, `linux-worker.js` - Runtime with graphics callbacks
- Documentation files

### 3. Created Helper Script (`tools/compile-graphics.sh`)

New utility script for compiling graphics programs:

```bash
./tools/compile-graphics.sh source.c [output.wasm]
```

Features:
- Automatically uses correct compiler flags
- Checks for LLVM and musl installation
- Shows helpful output and next steps
- Supports custom output file names
- Uses environment variables for paths

**Usage Examples:**

```bash
# Compile example
./tools/compile-graphics.sh runtime/example-graphics.c

# Compile custom program
./tools/compile-graphics.sh my-app.c my-app.wasm

# With custom workspace
LW_WORKSPACE=/custom ./tools/compile-graphics.sh app.c
```

### 4. Updated Documentation

**File: `README.md`**

Added Docker graphics compilation section:
- Instructions for compiling graphics in Docker
- Example commands
- Integration with build workflow

**File: `docker/GRAPHICS-DOCKER.md`**

Comprehensive Docker graphics guide including:
- Quick start instructions
- Complete workflow examples
- Directory structure reference
- Environment variable configuration
- Incremental development tips
- Troubleshooting section
- Advanced usage (custom libraries)

## Usage Workflow

### Development Mode (Recommended)

```bash
# 1. Start container with host mount
docker run -it --name my-linux-wasm \
  --mount type=bind,src="$(pwd)",target=/linux-wasm \
  linux-wasm-base:dev bash

# 2. Build everything (includes graphics examples)
/linux-wasm/linux-wasm.sh all

# 3. Develop and compile custom graphics programs
./tools/compile-graphics.sh my-graphics-app.c

# 4. Rebuild initramfs
./linux-wasm.sh build-initramfs

# 5. Exit and deploy
exit
cp workspace/install/kernel/vmlinux.wasm ~/webserver/
cp workspace/install/initramfs/initramfs.cpio.gz ~/webserver/
cp -r runtime/* ~/webserver/
```

### Contained Build Mode (CI/CD)

```bash
# Build everything in container
docker build -t linux-wasm-contained:dev ./docker/linux-wasm-contained
docker run -it linux-wasm-contained:dev

# Graphics examples are automatically compiled and included
```

## What Gets Built

When running `./linux-wasm.sh all` in Docker:

1. **LLVM** → `workspace/install/llvm/`
2. **Linux Kernel** → `workspace/install/kernel/vmlinux.wasm`
3. **musl libc** → `workspace/install/musl/`
4. **BusyBox** → `workspace/install/busybox/`
5. **Graphics Examples** → `workspace/install/graphics-examples/example-graphics.wasm`
6. **Initramfs** → `workspace/install/initramfs/initramfs.cpio.gz` (includes graphics example)

## File Locations After Build

```
workspace/
└── install/
    ├── llvm/
    │   └── bin/
    │       └── clang              # Compiler for graphics programs
    ├── musl/
    │   ├── lib/                   # C library
    │   └── include/               # C headers
    ├── kernel/
    │   └── vmlinux.wasm          # Linux kernel
    ├── busybox/
    │   └── bin/
    │       └── example-graphics.wasm  # Graphics example (copied here)
    ├── graphics-examples/
    │   ├── example-graphics.wasm # Original location
    │   └── wasm-graphics.h       # Graphics API header
    └── initramfs/
        └── initramfs.cpio.gz     # Contains busybox + graphics examples
```

## Environment Variables

All standard Linux/Wasm environment variables work in Docker:

```bash
docker run -it \
  -e LW_WORKSPACE=/workspace \
  -e LW_DEBUG_CFLAGS="" \
  -e LW_JOBS_LLVM_COMPILE=8 \
  --mount type=bind,src="$(pwd)",target=/linux-wasm \
  linux-wasm-base:dev bash
```

## Integration with Build System

The graphics compilation is fully integrated:

| Command | Graphics Behavior |
|---------|------------------|
| `./linux-wasm.sh all` | Builds graphics examples automatically |
| `./linux-wasm.sh build` | Builds graphics examples automatically |
| `./linux-wasm.sh build-os` | Builds graphics examples automatically |
| `./linux-wasm.sh build-graphics-examples` | Only builds graphics examples |
| `./linux-wasm.sh build-initramfs` | Includes any .wasm files in busybox/bin/ |

## Benefits of Docker Integration

1. **Consistent Environment**: Same build environment for everyone
2. **Automated Builds**: Graphics examples compile automatically
3. **Easy Compilation**: Helper script simplifies custom program compilation
4. **Clean Isolation**: No host system pollution
5. **Reproducible**: Same results on any platform
6. **CI/CD Ready**: Works in automated pipelines

## Testing in Docker

After building:

```bash
# In Docker container, verify graphics example exists
ls -la workspace/install/graphics-examples/example-graphics.wasm
ls -la workspace/install/busybox/bin/example-graphics.wasm

# Extract and test
exit  # Exit Docker

# On host
cd workspace/install
python3 -m http.server 8000 &
cd ../../runtime
cp ../workspace/install/kernel/vmlinux.wasm .
cp ../workspace/install/initramfs/initramfs.cpio.gz .

# Open browser to http://localhost:8000
# In Linux/Wasm terminal: /bin/example-graphics.wasm
```

## Adding New Graphics Programs

### Method 1: Using Helper Script

```bash
# Inside Docker container
./tools/compile-graphics.sh my-new-app.c
./linux-wasm.sh build-initramfs
```

### Method 2: Manual Compilation

```bash
# Inside Docker container
$LW_INSTALL/llvm/bin/clang \
  --target=wasm32-unknown-unknown \
  --sysroot=$LW_INSTALL/musl \
  -fPIC -shared \
  -o my-app.wasm \
  my-app.c

cp my-app.wasm $LW_INSTALL/busybox/bin/
./linux-wasm.sh build-initramfs
```

### Method 3: Extend Build Script

Add to `linux-wasm.sh` in the `build-graphics-examples` section:

```bash
if [ -f "$LW_ROOT/my-app.c" ]; then
    "$LW_INSTALL/llvm/bin/clang" \
        --target=wasm32-unknown-unknown \
        "--sysroot=$LW_INSTALL/musl" \
        -fPIC -shared \
        $LW_DEBUG_CFLAGS \
        -o "$LW_INSTALL/graphics-examples/my-app.wasm" \
        "$LW_ROOT/my-app.c"
    cp "$LW_INSTALL/graphics-examples/my-app.wasm" "$LW_INSTALL/busybox/bin/"
fi
```

## Troubleshooting

### Problem: Graphics example not in initramfs

**Solution:**
```bash
./linux-wasm.sh build-graphics-examples
./linux-wasm.sh build-initramfs
```

### Problem: Compile script not found

**Solution:**
```bash
# Make sure you're in /linux-wasm directory
cd /linux-wasm
./tools/compile-graphics.sh ...
```

### Problem: Clang not found

**Solution:**
```bash
# Build LLVM first
./linux-wasm.sh build-llvm
```

### Problem: Permission denied on compile script

**Solution:**
```bash
# In Linux/Docker
chmod +x tools/compile-graphics.sh
```

## Summary

✅ **Docker containers updated** to include runtime directory  
✅ **Build script enhanced** with graphics-examples target  
✅ **Helper script created** for easy graphics compilation  
✅ **Documentation added** for complete workflow  
✅ **Automatic inclusion** of graphics examples in initramfs  

Graphics development in Docker is now fully supported and integrated with the Linux/Wasm build system!

## Next Steps

1. Try the example: `./tools/compile-graphics.sh runtime/example-graphics.c`
2. Create your own graphics program
3. Extend the OpenGL API (see `runtime/GRAPHICS.md`)
4. Build more complex applications

For more information:
- `docker/GRAPHICS-DOCKER.md` - Complete Docker graphics guide
- `runtime/GRAPHICS.md` - Graphics API documentation
- `runtime/GRAPHICS-QUICKSTART.md` - Quick reference
- `README.md` - Main documentation

---
**Status**: Fully implemented and tested  
**Compatible with**: All Linux/Wasm Docker configurations  
**Added**: November 2025

