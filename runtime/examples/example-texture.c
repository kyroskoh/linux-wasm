// SPDX-License-Identifier: GPL-2.0-only
//
// Texture Test Program for Linux/Wasm
// Demonstrates textured quad rendering
//
// Author: Kyros Koh
//
// Compile with:
//   $LW_INSTALL/llvm/bin/clang --target=wasm32-unknown-unknown \
//     --sysroot=$LW_INSTALL/musl -fPIC -shared \
//     -o example-texture.wasm example-texture.c

#include "../wasm-graphics.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

// Vertex shader with texture coordinates
const char* vertex_shader_source =
    "attribute vec2 position;\n"
    "attribute vec2 texcoord;\n"
    "varying vec2 v_texcoord;\n"
    "void main() {\n"
    "  gl_Position = vec4(position, 0.0, 1.0);\n"
    "  v_texcoord = texcoord;\n"
    "}\n";

// Fragment shader with texture sampling
const char* fragment_shader_source =
    "precision mediump float;\n"
    "varying vec2 v_texcoord;\n"
    "uniform sampler2D u_texture;\n"
    "void main() {\n"
    "  gl_FragColor = texture2D(u_texture, v_texcoord);\n"
    "}\n";

// Helper function to compile a shader
static GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    if (shader == 0) {
        fprintf(stderr, "Failed to create shader\n");
        return 0;
    }

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    
    if (!compiled) {
        GLchar log[512];
        GLsizei log_length;
        glGetShaderInfoLog(shader, sizeof(log), &log_length, log);
        fprintf(stderr, "Shader compilation failed:\n%s\n", log);
        return 0;
    }

    return shader;
}

// Helper function to link a program
static GLuint link_program(GLuint vertex_shader, GLuint fragment_shader) {
    GLuint program = glCreateProgram();
    if (program == 0) {
        fprintf(stderr, "Failed to create program\n");
        return 0;
    }

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    
    if (!linked) {
        GLchar log[512];
        GLsizei log_length;
        glGetProgramInfoLog(program, sizeof(log), &log_length, log);
        fprintf(stderr, "Program linking failed:\n%s\n", log);
        return 0;
    }

    return program;
}

// Create a procedural checkerboard texture
static void create_checkerboard_texture(GLubyte* data, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int checker = ((x / 16) + (y / 16)) & 1;
            int offset = (y * width + x) * 4;
            
            if (checker) {
                // White square
                data[offset + 0] = 255;
                data[offset + 1] = 255;
                data[offset + 2] = 255;
                data[offset + 3] = 255;
            } else {
                // Colored square (changes with position)
                data[offset + 0] = (x * 255) / width;   // Red gradient
                data[offset + 1] = (y * 255) / height;  // Green gradient
                data[offset + 2] = 128;                 // Blue constant
                data[offset + 3] = 255;                 // Alpha
            }
        }
    }
}

int main(void) {
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    printf("Linux/Wasm Texture Test\n");
    printf("=======================\n\n");

    // Initialize graphics
    printf("Initializing graphics...\n");
    if (graphics_initialize(&display, &surface, &context) != 0) {
        fprintf(stderr, "Failed to initialize graphics\n");
        return 1;
    }
    printf("Graphics initialized!\n\n");

    // Set viewport
    glViewport(0, 0, 800, 600);

    // Compile shaders
    printf("Compiling shaders...\n");
    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_source);
    if (vertex_shader == 0) return 1;

    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source);
    if (fragment_shader == 0) return 1;

    // Link program
    GLuint program = link_program(vertex_shader, fragment_shader);
    if (program == 0) return 1;
    
    printf("Shaders compiled and linked!\n\n");

    // Get attribute and uniform locations
    GLint pos_attrib = glGetAttribLocation(program, "position");
    GLint texcoord_attrib = glGetAttribLocation(program, "texcoord");
    GLint texture_uniform = glGetUniformLocation(program, "u_texture");
    
    printf("Locations:\n");
    printf("  position: %d\n", pos_attrib);
    printf("  texcoord: %d\n", texcoord_attrib);
    printf("  u_texture: %d\n", texture_uniform);
    printf("\n");

    // Quad vertex data: position (x, y) and texcoord (u, v)
    GLfloat vertices[] = {
        // Position   // TexCoords
        -0.7f,  0.7f,  0.0f, 0.0f,  // Top-left
        -0.7f, -0.7f,  0.0f, 1.0f,  // Bottom-left
         0.7f, -0.7f,  1.0f, 1.0f,  // Bottom-right
         0.7f,  0.7f,  1.0f, 0.0f,  // Top-right
    };

    GLushort indices[] = {
        0, 1, 2,  // First triangle
        2, 3, 0,  // Second triangle
    };

    // Create and bind vertex buffer
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create and bind index buffer
    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    printf("Buffers created (VBO: %u, IBO: %u)\n", vbo, ibo);

    // Create texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    printf("Texture created (ID: %u)\n", texture);

    // Generate checkerboard texture data
    const int tex_width = 256;
    const int tex_height = 256;
    GLubyte* texture_data = (GLubyte*)malloc(tex_width * tex_height * 4);
    if (!texture_data) {
        fprintf(stderr, "Failed to allocate texture memory\n");
        return 1;
    }
    
    create_checkerboard_texture(texture_data, tex_width, tex_height);
    printf("Generated %dx%d checkerboard texture\n", tex_width, tex_height);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
    free(texture_data);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    printf("Texture uploaded and configured!\n\n");

    // Use the program
    glUseProgram(program);

    // Setup vertex attributes
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);

    glEnableVertexAttribArray(texcoord_attrib);
    glVertexAttribPointer(texcoord_attrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    // Set texture uniform to texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(texture_uniform, 0);

    printf("Rendering textured quad...\n");
    printf("Press Ctrl+C to stop\n\n");

    // Render loop
    for (int frame = 0; frame < 300; frame++) {
        // Clear screen
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw textured quad
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);

        // Swap buffers
        eglSwapBuffers(display, surface);

        // Status update
        if (frame % 60 == 0) {
            printf("Frame %d rendered\n", frame);
        }

        // ~60 FPS
        usleep(16667);
    }

    printf("\nTest complete! Textured quad rendered successfully.\n");
    printf("Textures are working! 🎨🖼️\n");
    
    return 0;
}

