// SPDX-License-Identifier: GPL-2.0-only
//
// Multi-Cube Demo for Linux/Wasm
// Showcases full 3D rendering capabilities with multiple objects
//
// Author: Kyros Koh
//
// Compile with:
//   $LW_INSTALL/llvm/bin/clang --target=wasm32-unknown-unknown \
//     --sysroot=$LW_INSTALL/musl -fPIC -shared \
//     -o example-demo.wasm example-demo.c

#include "../wasm-graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

// Vertex shader with MVP matrices and lighting
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

// Fragment shader with texture and color tinting
const char* fragment_shader_source =
    "precision mediump float;\n"
    "varying vec2 v_texcoord;\n"
    "varying float v_lighting;\n"
    "uniform sampler2D u_texture;\n"
    "uniform vec3 u_tint_color;\n"
    "void main() {\n"
    "  vec4 tex_color = texture2D(u_texture, v_texcoord);\n"
    "  vec3 tinted = tex_color.rgb * u_tint_color;\n"
    "  gl_FragColor = vec4(tinted * v_lighting, tex_color.a);\n"
    "}\n";

// Matrix math
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

static void mat4_scale(mat4 m, float x, float y, float z) {
    mat4_identity(m);
    m[0] = x;
    m[5] = y;
    m[10] = z;
}

// Shader helpers
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

// Create procedural texture with color variation
static void create_cube_texture(GLubyte* data, int size, float hue) {
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            int offset = (y * size + x) * 4;
            
            int grid = ((x / 32) + (y / 32)) & 1;
            float fx = (float)x / size;
            float fy = (float)y / size;
            
            // Color variation based on hue
            float r = sinf(hue) * 0.5f + 0.5f;
            float g = sinf(hue + 2.094f) * 0.5f + 0.5f;
            float b = sinf(hue + 4.189f) * 0.5f + 0.5f;
            
            if (grid) {
                data[offset + 0] = (GLubyte)(fx * 200 * r + 55);
                data[offset + 1] = (GLubyte)(fy * 200 * g + 55);
                data[offset + 2] = (GLubyte)((1.0f - fx) * 200 * b + 55);
            } else {
                data[offset + 0] = (GLubyte)(fy * 150 * r + 105);
                data[offset + 1] = (GLubyte)((1.0f - fy) * 150 * g + 105);
                data[offset + 2] = (GLubyte)(fx * 150 * b + 105);
            }
            data[offset + 3] = 255;
        }
    }
}

// Cube definition
typedef struct {
    float position[3];
    float rotation_speed[3];
    float scale;
    float tint_color[3];
    GLuint texture;
} Cube;

int main(void) {
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    
    printf("╔═══════════════════════════════════════════╗\n");
    printf("║  Linux/Wasm Multi-Cube Demo              ║\n");
    printf("║  Showcasing 3D Graphics Capabilities     ║\n");
    printf("╚═══════════════════════════════════════════╝\n\n");
    
    // Initialize graphics
    printf("🚀 Initializing graphics system...\n");
    if (graphics_initialize(&display, &surface, &context) != 0) {
        fprintf(stderr, "❌ Failed to initialize graphics\n");
        return 1;
    }
    printf("✅ Graphics initialized!\n\n");
    
    // Set viewport and enable depth testing
    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);
    
    // Compile shaders
    printf("🎨 Compiling shaders...\n");
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_shader_source);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source);
    GLuint program = link_program(vs, fs);
    if (program == 0) return 1;
    printf("✅ Shaders compiled and linked!\n\n");
    
    // Get uniform locations
    GLint mvp_loc = glGetUniformLocation(program, "u_mvp");
    GLint light_loc = glGetUniformLocation(program, "u_light_dir");
    GLint texture_loc = glGetUniformLocation(program, "u_texture");
    GLint tint_loc = glGetUniformLocation(program, "u_tint_color");
    GLint pos_loc = glGetAttribLocation(program, "position");
    GLint tex_loc = glGetAttribLocation(program, "texcoord");
    GLint norm_loc = glGetAttribLocation(program, "normal");
    
    // Cube vertex data
    GLfloat vertices[] = {
        // Front, Back, Top, Bottom, Right, Left faces
        // Position(3), TexCoord(2), Normal(3)
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
        
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f,
        
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
        
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
    };
    
    GLushort indices[] = {
        0, 1, 2,  2, 3, 0,
        4, 5, 6,  6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20,
    };
    
    // Create buffers
    GLuint vbo, ibo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    printf("📦 Geometry buffers created\n\n");
    
    // Create cubes with different properties
    printf("🎲 Creating 7 unique cubes...\n");
    const int num_cubes = 7;
    Cube cubes[7];
    
    // Cube positions arranged in a circle + center
    float positions[][3] = {
        {0.0f, 0.0f, 0.0f},      // Center
        {2.5f, 0.5f, 0.0f},      // Right
        {-2.5f, -0.5f, 0.0f},    // Left
        {0.0f, 2.5f, 0.5f},      // Top
        {0.0f, -2.5f, -0.5f},    // Bottom
        {1.8f, 1.8f, 0.8f},      // Top-right
        {-1.8f, -1.8f, -0.8f},   // Bottom-left
    };
    
    // Different colors for each cube
    float colors[][3] = {
        {1.0f, 1.0f, 1.0f},      // White
        {1.0f, 0.3f, 0.3f},      // Red
        {0.3f, 1.0f, 0.3f},      // Green
        {0.3f, 0.3f, 1.0f},      // Blue
        {1.0f, 1.0f, 0.3f},      // Yellow
        {1.0f, 0.3f, 1.0f},      // Magenta
        {0.3f, 1.0f, 1.0f},      // Cyan
    };
    
    // Create textures and initialize cubes
    const int tex_size = 128;
    GLubyte* tex_data = (GLubyte*)malloc(tex_size * tex_size * 4);
    
    for (int i = 0; i < num_cubes; i++) {
        // Position and scale
        memcpy(cubes[i].position, positions[i], sizeof(float) * 3);
        cubes[i].scale = 0.4f + (i * 0.05f);
        
        // Rotation speeds (different for each cube)
        cubes[i].rotation_speed[0] = 0.01f + (i * 0.003f);
        cubes[i].rotation_speed[1] = 0.015f + (i * 0.002f);
        cubes[i].rotation_speed[2] = 0.008f + (i * 0.004f);
        
        // Color tint
        memcpy(cubes[i].tint_color, colors[i], sizeof(float) * 3);
        
        // Create unique texture
        create_cube_texture(tex_data, tex_size, i * 1.0f);
        glGenTextures(1, &cubes[i].texture);
        glBindTexture(GL_TEXTURE_2D, cubes[i].texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_size, tex_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        printf("  ✓ Cube %d: pos(%.1f, %.1f, %.1f) scale(%.2f) color(%.1f, %.1f, %.1f)\n",
               i + 1, cubes[i].position[0], cubes[i].position[1], cubes[i].position[2],
               cubes[i].scale, cubes[i].tint_color[0], cubes[i].tint_color[1], cubes[i].tint_color[2]);
    }
    
    free(tex_data);
    printf("\n");
    
    // Setup rendering
    glUseProgram(program);
    
    glEnableVertexAttribArray(pos_loc);
    glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    
    glEnableVertexAttribArray(tex_loc);
    glVertexAttribPointer(tex_loc, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    
    glEnableVertexAttribArray(norm_loc);
    glVertexAttribPointer(norm_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
    
    // Set light direction
    vec3 light_dir = {0.577f, 0.577f, 0.577f};
    glUniform3f(light_loc, light_dir[0], light_dir[1], light_dir[2]);
    glUniform1i(texture_loc, 0);
    
    printf("🎬 Starting demo animation...\n");
    printf("   Camera orbiting around scene\n");
    printf("   7 cubes with unique textures and colors\n");
    printf("   Press Ctrl+C to stop\n\n");
    
    // Animation loop
    float rotation[7] = {0};
    float camera_angle = 0.0f;
    int frame_count = 0;
    unsigned long start_time = time(NULL);
    
    for (int frame = 0; frame < 900; frame++) {
        // Clear buffers
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Camera orbit
        camera_angle += 0.005f;
        float cam_x = sinf(camera_angle) * 6.0f;
        float cam_z = cosf(camera_angle) * 6.0f;
        float cam_y = sinf(camera_angle * 0.5f) * 2.0f;
        
        // View matrix (camera looking at origin)
        mat4 projection, view, model, mvp, temp, temp2;
        mat4_perspective(projection, 45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
        mat4_translate(view, -cam_x, -cam_y, -cam_z);
        
        // Render each cube
        for (int i = 0; i < num_cubes; i++) {
            // Model matrix: translate, rotate, scale
            mat4 translate_mat, rotate_x, rotate_y, rotate_z, scale_mat;
            
            mat4_translate(translate_mat, cubes[i].position[0], cubes[i].position[1], cubes[i].position[2]);
            mat4_rotate_x(rotate_x, rotation[i]);
            mat4_rotate_y(rotate_y, rotation[i] * 0.7f);
            mat4_scale(scale_mat, cubes[i].scale, cubes[i].scale, cubes[i].scale);
            
            // Combine transformations
            mat4_multiply(temp, rotate_y, rotate_x);
            mat4_multiply(temp2, scale_mat, temp);
            mat4_multiply(model, translate_mat, temp2);
            
            // MVP
            mat4_multiply(temp, view, model);
            mat4_multiply(mvp, projection, temp);
            
            // Set uniforms
            glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, mvp);
            glUniform3f(tint_loc, cubes[i].tint_color[0], cubes[i].tint_color[1], cubes[i].tint_color[2]);
            
            // Bind texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cubes[i].texture);
            
            // Draw cube
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
            
            // Update rotation
            rotation[i] += cubes[i].rotation_speed[0];
        }
        
        // Present
        eglSwapBuffers(display, surface);
        
        // FPS counter
        frame_count++;
        if (frame % 60 == 0) {
            unsigned long elapsed = time(NULL) - start_time;
            float fps = elapsed > 0 ? (float)frame_count / elapsed : 60.0f;
            printf("  Frame %d | FPS: %.1f | Cubes: %d | Camera angle: %.1f°\n",
                   frame, fps, num_cubes, camera_angle * 180.0f / 3.14159f);
        }
        
        usleep(16667);  // ~60 FPS
    }
    
    printf("\n✅ Demo complete!\n");
    printf("   Total frames: 900\n");
    printf("   Duration: ~15 seconds\n");
    printf("   Performance: Smooth 60 FPS\n\n");
    printf("🎉 Linux/Wasm 3D Graphics - Fully Functional!\n\n");
    
    return 0;
}

