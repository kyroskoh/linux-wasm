# Graphics Examples for Linux/Wasm

This directory contains example programs demonstrating the graphics capabilities of Linux/Wasm.

## Examples

### example-graphics.c
Basic EGL and OpenGL ES usage with animated color cycling.

**Features:**
- EGL initialization
- Viewport setup
- Color animation (HSV color space)
- Frame timing

**Compile:**
```bash
./tools/compile-graphics.sh runtime/examples/example-graphics.c
```

### example-shaders.c
Complete shader pipeline demonstration with a colored triangle.

**Features:**
- Vertex and fragment shaders (GLSL)
- Shader compilation and error checking
- Program linking
- Vertex buffer objects (VBO)
- Vertex attributes (position + color)
- Smooth color interpolation

**Compile:**
```bash
./tools/compile-graphics.sh runtime/examples/example-shaders.c
```

### example-texture.c
Textured quad rendering with procedural checkerboard texture.

**Features:**
- Texture generation (glGenTextures)
- Texture uploading (glTexImage2D)
- Texture parameters (filtering, wrapping)
- Texture coordinates
- Sampler uniforms
- Element buffer objects (EBO)

**Compile:**
```bash
./tools/compile-graphics.sh runtime/examples/example-texture.c
```

### example-cube.c ⭐ **Showcase Demo**
Spinning 3D textured cube with lighting and depth testing.

**Features:**
- Full 3D transformations (Model-View-Projection matrices)
- Depth testing (GL_DEPTH_TEST)
- Textured cube (6 faces)
- Per-vertex normals for lighting
- Procedural texture generation
- Smooth rotation animation
- Matrix math (perspective, rotation, translation)

**Compile:**
```bash
./tools/compile-graphics.sh runtime/examples/example-cube.c
```

## Creating Your Own Examples

1. Create a new `.c` file in this directory
2. Include the graphics header: `#include "../wasm-graphics.h"`
3. Compile with: `./tools/compile-graphics.sh runtime/examples/your-example.c`
4. Run in Linux/Wasm: `/bin/your-example.wasm`

## API Reference

See `../wasm-graphics.h` for the complete graphics API including:
- **EGL functions** - Display, context, surface management
- **Shaders** - Create, compile, link programs
- **Buffers** - VBOs, EBOs, vertex attributes
- **Textures** - Generate, bind, upload, sample (NEW!)
- **Uniforms** - 1f, 2f, 3f, 4f, matrix4fv (NEW!)
- **Drawing** - Arrays, elements
- **State** - Depth test, culling, blending (NEW!)

## Documentation

- **[../GRAPHICS.md](../GRAPHICS.md)** - Complete graphics system documentation
- **[../GRAPHICS-QUICKSTART.md](../GRAPHICS-QUICKSTART.md)** - Quick start guide
- **[../../SHADER-IMPLEMENTATION.md](../../SHADER-IMPLEMENTATION.md)** - Shader system details
- **[../../ROADMAP.md](../../ROADMAP.md)** - Development roadmap

## Automatic Compilation

All examples in this directory are automatically compiled when you run:
```bash
./linux-wasm.sh build-graphics-examples
# or
./linux-wasm.sh all
```

The compiled `.wasm` files are automatically copied to the initramfs for easy testing.

