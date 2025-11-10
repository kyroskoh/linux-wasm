// SPDX-License-Identifier: GPL-2.0-only
//
// Spinning 3D Cube Demo for Linux/Wasm
// Demonstrates full 3D rendering with textures, depth testing, and transformations
//
// Author: Kyros Koh
//
// Compile with:
//   $LW_INSTALL/llvm/bin/clang --target=wasm32-unknown-unknown \
//     --sysroot=$LW_INSTALL/musl -fPIC -shared \
//     -o example-cube.wasm example-cube.c

#include "../wasm-graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

// Vertex shader with MVP matrices
const char* vertex_shader_source =
    "attribute vec3 position;\n"
    "attribute vec2 texcoord;\n"
    "attribute vec3 normal;\n"
    "varying vec2 v_texcoord;\n"
    "varying float v_lighting;\n"
    "uniform mat4 u_mvp;\n"
    "uniform vec3 u_light_dir;\n"
    "void main() {\n"
    "  gl_Position = u_mvp * vec4(position, 1.0);\n"
    "  v_texcoord = texcoord;\n"
    "  v_lighting = max(dot(normal, u_light_dir), 0.3);\n"
    "}\n";

// Fragment shader with texture and lighting
const char* fragment_shader_source =
    "precision mediump float;\n"
    "varying vec2 v_texcoord;\n"
    "varying float v_lighting;\n"
    "uniform sampler2D u_texture;\n"
    "void main() {\n"
    "  vec4 tex_color = texture2D(u_texture, v_texcoord);\n"
    "  gl_FragColor = vec4(tex_color.rgb * v_lighting, tex_color.a);\n"
    "}\n";

// Simple matrix math functions
typedef float mat4[16];
typedef float vec3[3];

static void mat4_identity(mat4 m) {
    for (int i = 0; i < 16; i++) m[i] = 0.0f;
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

static void mat4_multiply(mat4 result, const mat4 a, const mat4 b) {
    mat4 temp;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            temp[i * 4 + j] = 0.0f;
            for (int k = 0; k < 4; k++) {
                temp[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
            }
        }
    }
    memcpy(result, temp, sizeof(mat4));
}

static void mat4_perspective(mat4 m, float fov, float aspect, float near, float far) {
    float f = 1.0f / tanf(fov * 0.5f * 3.14159265f / 180.0f);
    mat4_identity(m);
    m[0] = f / aspect;
    m[5] = f;
    m[10] = (far + near) / (near - far);
    m[11] = -1.0f;
    m[14] = (2.0f * far * near) / (near - far);
    m[15] = 0.0f;
}

static void mat4_translate(mat4 m, float x, float y, float z) {
    mat4_identity(m);
    m[12] = x;
    m[13] = y;
    m[14] = z;
}

static void mat4_rotate_x(mat4 m, float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    mat4_identity(m);
    m[5] = c;
    m[6] = s;
    m[9] = -s;
    m[10] = c;
}

static void mat4_rotate_y(mat4 m, float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    mat4_identity(m);
    m[0] = c;
    m[2] = -s;
    m[8] = s;
    m[10] = c;
}

// Helper functions for shaders
static GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    if (shader == 0) return 0;
    
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLchar log[512];
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        fprintf(stderr, "Shader error: %s\n", log);
        return 0;
    }
    return shader;
}

static GLuint link_program(GLuint vs, GLuint fs) {
    GLuint program = glCreateProgram();
    if (program == 0) return 0;
    
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    
    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLchar log[512];
        glGetProgramInfoLog(program, sizeof(log), NULL, log);
        fprintf(stderr, "Link error: %s\n", log);
        return 0;
    }
    return program;
}

// Create a procedural texture for the cube faces
static void create_cube_texture(GLubyte* data, int size) {
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            int offset = (y * size + x) * 4;
            
            // Create a gradient pattern with grid
            int grid = ((x / 32) + (y / 32)) & 1;
            float fx = (float)x / size;
            float fy = (float)y / size;
            
            if (grid) {
                data[offset + 0] = (GLubyte)(fx * 200 + 55);
                data[offset + 1] = (GLubyte)(fy * 200 + 55);
                data[offset + 2] = (GLubyte)((1.0f - fx) * 200 + 55);
            } else {
                data[offset + 0] = (GLubyte)(fy * 150 + 105);
                data[offset + 1] = (GLubyte)((1.0f - fy) * 150 + 105);
                data[offset + 2] = (GLubyte)(fx * 150 + 105);
            }
            data[offset + 3] = 255;
        }
    }
}

int main(void) {
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    
    printf("Linux/Wasm Spinning Cube Demo\n");
    printf("=============================\n\n");
    
    // Initialize graphics
    printf("Initializing graphics...\n");
    if (graphics_initialize(&display, &surface, &context) != 0) {
        fprintf(stderr, "Failed to initialize graphics\n");
        return 1;
    }
    printf("✓ Graphics initialized\n\n");
    
    // Set viewport
    glViewport(0, 0, 800, 600);
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    printf("✓ Depth testing enabled\n");
    
    // Compile shaders
    printf("✓ Compiling shaders...\n");
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_shader_source);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source);
    GLuint program = link_program(vs, fs);
    if (program == 0) return 1;
    
    // Get locations
    GLint pos_loc = glGetAttribLocation(program, "position");
    GLint tex_loc = glGetAttribLocation(program, "texcoord");
    GLint norm_loc = glGetAttribLocation(program, "normal");
    GLint mvp_loc = glGetUniformLocation(program, "u_mvp");
    GLint light_loc = glGetUniformLocation(program, "u_light_dir");
    GLint texture_loc = glGetUniformLocation(program, "u_texture");
    
    printf("✓ Shader program linked\n\n");
    
    // Cube vertex data: pos(3), texcoord(2), normal(3) per vertex
    GLfloat vertices[] = {
        // Front face (z = 0.5)
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
        
        // Back face (z = -0.5)
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f,
        
        // Top face (y = 0.5)
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
        
        // Bottom face (y = -0.5)
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
        
        // Right face (x = 0.5)
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
        
        // Left face (x = -0.5)
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
    };
    
    // Cube indices (6 faces * 2 triangles * 3 vertices)
    GLushort indices[] = {
        0, 1, 2,  2, 3, 0,      // Front
        4, 5, 6,  6, 7, 4,      // Back
        8, 9, 10, 10, 11, 8,    // Top
        12, 13, 14, 14, 15, 12, // Bottom
        16, 17, 18, 18, 19, 16, // Right
        20, 21, 22, 22, 23, 20, // Left
    };
    
    // Create buffers
    GLuint vbo, ibo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    printf("✓ Cube geometry created (%zu vertices, %zu indices)\n", 
           sizeof(vertices) / (8 * sizeof(GLfloat)), sizeof(indices) / sizeof(GLushort));
    
    // Create texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    const int tex_size = 256;
    GLubyte* tex_data = (GLubyte*)malloc(tex_size * tex_size * 4);
    if (!tex_data) {
        fprintf(stderr, "Failed to allocate texture\n");
        return 1;
    }
    
    create_cube_texture(tex_data, tex_size);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_size, tex_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
    free(tex_data);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    printf("✓ Texture created (%dx%d)\n\n", tex_size, tex_size);
    
    // Setup rendering
    glUseProgram(program);
    
    glEnableVertexAttribArray(pos_loc);
    glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    
    glEnableVertexAttribArray(tex_loc);
    glVertexAttribPointer(tex_loc, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    
    glEnableVertexAttribArray(norm_loc);
    glVertexAttribPointer(norm_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(texture_loc, 0);
    
    // Set light direction
    vec3 light_dir = {0.577f, 0.577f, 0.577f};  // Normalized (1,1,1)
    glUniform3f(light_loc, light_dir[0], light_dir[1], light_dir[2]);
    
    printf("🎮 Rendering spinning cube...\n");
    printf("   Press Ctrl+C to stop\n\n");
    
    // Animation loop
    float rotation = 0.0f;
    for (int frame = 0; frame < 600; frame++) {
        // Clear buffers
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Build transformation matrices
        mat4 projection, view, model, mvp, temp;
        
        // Projection matrix (perspective)
        mat4_perspective(projection, 45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
        
        // View matrix (camera at z=3)
        mat4_translate(view, 0.0f, 0.0f, -3.0f);
        
        // Model matrix (rotation)
        mat4 rot_x, rot_y;
        mat4_rotate_x(rot_x, rotation * 0.7f);
        mat4_rotate_y(rot_y, rotation);
        mat4_multiply(model, rot_y, rot_x);
        
        // MVP = Projection * View * Model
        mat4_multiply(temp, view, model);
        mat4_multiply(mvp, projection, temp);
        
        // Upload MVP matrix
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, mvp);
        
        // Draw cube
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
        
        // Present
        eglSwapBuffers(display, surface);
        
        // Update rotation
        rotation += 0.02f;
        
        // Status
        if (frame % 60 == 0) {
            printf("  Frame %d (rotation: %.1f°)\n", frame, rotation * 180.0f / 3.14159f);
        }
        
        usleep(16667);  // ~60 FPS
    }
    
    printf("\n✅ Demo complete! Spinning cube rendered successfully.\n");
    printf("🎨 Full 3D graphics with textures, lighting, and depth testing!\n\n");
    
    return 0;
}

