// SPDX-License-Identifier: GPL-2.0-only
//
// Shader Test Program for Linux/Wasm
// Demonstrates basic shader usage with a colored triangle
//
// Compile with:
//   $LW_INSTALL/llvm/bin/clang --target=wasm32-unknown-unknown \
//     --sysroot=$LW_INSTALL/musl -fPIC -shared \
//     -o example-shaders.wasm example-shaders.c

#include "../wasm-graphics.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Simple vertex shader
const char* vertex_shader_source =
    "attribute vec2 position;\n"
    "attribute vec3 color;\n"
    "varying vec3 v_color;\n"
    "void main() {\n"
    "  gl_Position = vec4(position, 0.0, 1.0);\n"
    "  v_color = color;\n"
    "}\n";

// Simple fragment shader
const char* fragment_shader_source =
    "precision mediump float;\n"
    "varying vec3 v_color;\n"
    "void main() {\n"
    "  gl_FragColor = vec4(v_color, 1.0);\n"
    "}\n";

// Helper function to compile a shader
static GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    if (shader == 0) {
        fprintf(stderr, "Failed to create shader\n");
        return 0;
    }

    // Set shader source
    glShaderSource(shader, 1, &source, NULL);

    // Compile shader
    glCompileShader(shader);

    // Check compilation status
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    
    if (!compiled) {
        GLchar log[512];
        GLsizei log_length;
        glGetShaderInfoLog(shader, sizeof(log), &log_length, log);
        fprintf(stderr, "Shader compilation failed:\n%s\n", log);
        return 0;
    }

    printf("Shader compiled successfully (ID: %u)\n", shader);
    return shader;
}

// Helper function to link a program
static GLuint link_program(GLuint vertex_shader, GLuint fragment_shader) {
    GLuint program = glCreateProgram();
    if (program == 0) {
        fprintf(stderr, "Failed to create program\n");
        return 0;
    }

    // Attach shaders
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    // Link program
    glLinkProgram(program);

    // Check link status
    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    
    if (!linked) {
        GLchar log[512];
        GLsizei log_length;
        glGetProgramInfoLog(program, sizeof(log), &log_length, log);
        fprintf(stderr, "Program linking failed:\n%s\n", log);
        return 0;
    }

    printf("Program linked successfully (ID: %u)\n", program);
    return program;
}

int main(void) {
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    printf("Linux/Wasm Shader Test\n");
    printf("======================\n\n");

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
    if (vertex_shader == 0) {
        return 1;
    }

    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source);
    if (fragment_shader == 0) {
        return 1;
    }
    printf("\n");

    // Link program
    printf("Linking program...\n");
    GLuint program = link_program(vertex_shader, fragment_shader);
    if (program == 0) {
        return 1;
    }
    printf("\n");

    // Get attribute locations
    GLint pos_attrib = glGetAttribLocation(program, "position");
    GLint color_attrib = glGetAttribLocation(program, "color");
    
    printf("Attribute locations:\n");
    printf("  position: %d\n", pos_attrib);
    printf("  color: %d\n", color_attrib);
    printf("\n");

    // Triangle vertex data: position (x, y) and color (r, g, b)
    GLfloat vertices[] = {
        // Position    // Color
         0.0f,  0.5f,  1.0f, 0.0f, 0.0f,  // Top vertex (red)
        -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  // Bottom-left (green)
         0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  // Bottom-right (blue)
    };

    // Create and bind vertex buffer
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    printf("Vertex buffer created (ID: %u)\n", vbo);
    printf("Buffer size: %zu bytes\n", sizeof(vertices));
    printf("\n");

    // Use the program
    glUseProgram(program);

    // Setup vertex attributes
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);

    glEnableVertexAttribArray(color_attrib);
    glVertexAttribPointer(color_attrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    printf("Rendering triangle...\n");
    printf("Press Ctrl+C to stop\n\n");

    // Render loop
    for (int frame = 0; frame < 300; frame++) {
        // Clear screen
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Swap buffers
        eglSwapBuffers(display, surface);

        // Status update
        if (frame % 60 == 0) {
            printf("Frame %d rendered\n", frame);
        }

        // ~60 FPS
        usleep(16667);
    }

    printf("\nTest complete! Triangle rendered successfully.\n");
    printf("Shaders are working! 🎨\n");
    
    return 0;
}

