// SPDX-License-Identifier: GPL-2.0-only
// 
// WebAssembly Graphics Support for Linux/Wasm
// EGL and OpenGL ES interface backed by WebGL
//
// This header provides a simplified EGL and OpenGL ES interface that
// can be used from user programs running in Linux/Wasm. The implementation
// is backed by WebGL in the browser through host callbacks.

#ifndef WASM_GRAPHICS_H
#define WASM_GRAPHICS_H

#include <stdint.h>

// EGL types
typedef int32_t EGLint;
typedef uint32_t EGLBoolean;
typedef void* EGLDisplay;
typedef void* EGLConfig;
typedef void* EGLSurface;
typedef void* EGLContext;
typedef void* EGLNativeDisplayType;
typedef void* EGLNativeWindowType;

// EGL constants
#define EGL_FALSE 0
#define EGL_TRUE 1
#define EGL_SUCCESS 0x3000
#define EGL_DEFAULT_DISPLAY ((EGLNativeDisplayType)0)
#define EGL_NO_CONTEXT ((EGLContext)0)
#define EGL_NO_DISPLAY ((EGLDisplay)0)
#define EGL_NO_SURFACE ((EGLSurface)0)
#define EGL_NONE 0x3038

// EGL attribute names
#define EGL_BUFFER_SIZE 0x3020
#define EGL_RED_SIZE 0x3024
#define EGL_GREEN_SIZE 0x3023
#define EGL_BLUE_SIZE 0x3022
#define EGL_ALPHA_SIZE 0x3021
#define EGL_DEPTH_SIZE 0x3025
#define EGL_STENCIL_SIZE 0x3026
#define EGL_RENDERABLE_TYPE 0x3040
#define EGL_OPENGL_ES2_BIT 0x0004
#define EGL_CONTEXT_CLIENT_VERSION 0x3098

// OpenGL ES types
typedef unsigned int GLenum;
typedef unsigned int GLbitfield;
typedef unsigned int GLuint;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLboolean;
typedef signed char GLbyte;
typedef short GLshort;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef float GLfloat;
typedef float GLclampf;
typedef void GLvoid;

// OpenGL ES constants
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_STENCIL_BUFFER_BIT 0x00000400
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005
#define GL_TRIANGLE_FAN 0x0006
#define GL_LINES 0x0001
#define GL_LINE_STRIP 0x0003
#define GL_POINTS 0x0000

// Shader types
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30

// Shader/Program status
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_VALIDATE_STATUS 0x8B83
#define GL_INFO_LOG_LENGTH 0x8B84
#define GL_ATTACHED_SHADERS 0x8B85
#define GL_ACTIVE_UNIFORMS 0x8B86
#define GL_ACTIVE_ATTRIBUTES 0x8B89

// Buffer types
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893

// Buffer usage
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_STREAM_DRAW 0x88E0

// Data types
#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406

#define GL_FALSE 0
#define GL_TRUE 1

// Texture targets
#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE_CUBE_MAP 0x8513
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A

// Texture formats
#define GL_RGB 0x1907
#define GL_RGBA 0x1908
#define GL_LUMINANCE 0x1909
#define GL_LUMINANCE_ALPHA 0x190A
#define GL_ALPHA 0x1906

// Texture parameters
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803

// Texture filter modes
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_LINEAR_MIPMAP_LINEAR 0x2703

// Texture wrap modes
#define GL_REPEAT 0x2901
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_MIRRORED_REPEAT 0x8370

// Texture units
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE1 0x84C1
#define GL_TEXTURE2 0x84C2
#define GL_TEXTURE3 0x84C3
#define GL_TEXTURE4 0x84C4
#define GL_TEXTURE5 0x84C5
#define GL_TEXTURE6 0x84C6
#define GL_TEXTURE7 0x84C7

// Depth test
#define GL_DEPTH_TEST 0x0B71
#define GL_CULL_FACE 0x0B44
#define GL_BLEND 0x0BE2

// Host callback declarations (implemented in JavaScript runtime)
__attribute__((import_module("env"), import_name("wasm_graphics_init")))
int wasm_graphics_init(void);

__attribute__((import_module("env"), import_name("wasm_graphics_swap_buffers")))
int wasm_graphics_swap_buffers(void);

// EGL functions
__attribute__((import_module("env"), import_name("wasm_egl_get_display")))
EGLDisplay wasm_egl_get_display(EGLNativeDisplayType display_id);

__attribute__((import_module("env"), import_name("wasm_egl_initialize")))
EGLBoolean wasm_egl_initialize(EGLDisplay dpy, EGLint *major, EGLint *minor);

__attribute__((import_module("env"), import_name("wasm_egl_choose_config")))
EGLBoolean wasm_egl_choose_config(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config);

__attribute__((import_module("env"), import_name("wasm_egl_create_window_surface")))
EGLSurface wasm_egl_create_window_surface(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list);

__attribute__((import_module("env"), import_name("wasm_egl_create_context")))
EGLContext wasm_egl_create_context(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list);

__attribute__((import_module("env"), import_name("wasm_egl_make_current")))
EGLBoolean wasm_egl_make_current(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx);

__attribute__((import_module("env"), import_name("wasm_egl_swap_buffers")))
EGLBoolean wasm_egl_swap_buffers(EGLDisplay dpy, EGLSurface surface);

// OpenGL ES basic functions
__attribute__((import_module("env"), import_name("wasm_gl_clear")))
void wasm_gl_clear(GLbitfield mask);

__attribute__((import_module("env"), import_name("wasm_gl_clear_color")))
void wasm_gl_clear_color(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);

__attribute__((import_module("env"), import_name("wasm_gl_viewport")))
void wasm_gl_viewport(GLint x, GLint y, GLsizei width, GLsizei height);

// Shader functions
__attribute__((import_module("env"), import_name("wasm_gl_create_shader")))
GLuint wasm_gl_create_shader(GLenum type);

__attribute__((import_module("env"), import_name("wasm_gl_shader_source")))
void wasm_gl_shader_source(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);

__attribute__((import_module("env"), import_name("wasm_gl_compile_shader")))
void wasm_gl_compile_shader(GLuint shader);

__attribute__((import_module("env"), import_name("wasm_gl_get_shaderiv")))
void wasm_gl_get_shaderiv(GLuint shader, GLenum pname, GLint* params);

__attribute__((import_module("env"), import_name("wasm_gl_get_shader_info_log")))
void wasm_gl_get_shader_info_log(GLuint shader, GLsizei max_length, GLsizei* length, GLchar* info_log);

// Program functions
__attribute__((import_module("env"), import_name("wasm_gl_create_program")))
GLuint wasm_gl_create_program(void);

__attribute__((import_module("env"), import_name("wasm_gl_attach_shader")))
void wasm_gl_attach_shader(GLuint program, GLuint shader);

__attribute__((import_module("env"), import_name("wasm_gl_link_program")))
void wasm_gl_link_program(GLuint program);

__attribute__((import_module("env"), import_name("wasm_gl_use_program")))
void wasm_gl_use_program(GLuint program);

__attribute__((import_module("env"), import_name("wasm_gl_get_programiv")))
void wasm_gl_get_programiv(GLuint program, GLenum pname, GLint* params);

__attribute__((import_module("env"), import_name("wasm_gl_get_program_info_log")))
void wasm_gl_get_program_info_log(GLuint program, GLsizei max_length, GLsizei* length, GLchar* info_log);

// Attribute and uniform functions
__attribute__((import_module("env"), import_name("wasm_gl_get_attrib_location")))
GLint wasm_gl_get_attrib_location(GLuint program, const GLchar* name);

__attribute__((import_module("env"), import_name("wasm_gl_get_uniform_location")))
GLint wasm_gl_get_uniform_location(GLuint program, const GLchar* name);

__attribute__((import_module("env"), import_name("wasm_gl_enable_vertex_attrib_array")))
void wasm_gl_enable_vertex_attrib_array(GLuint index);

__attribute__((import_module("env"), import_name("wasm_gl_disable_vertex_attrib_array")))
void wasm_gl_disable_vertex_attrib_array(GLuint index);

__attribute__((import_module("env"), import_name("wasm_gl_vertex_attrib_pointer")))
void wasm_gl_vertex_attrib_pointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);

// Buffer functions
__attribute__((import_module("env"), import_name("wasm_gl_gen_buffers")))
void wasm_gl_gen_buffers(GLsizei n, GLuint* buffers);

__attribute__((import_module("env"), import_name("wasm_gl_bind_buffer")))
void wasm_gl_bind_buffer(GLenum target, GLuint buffer);

__attribute__((import_module("env"), import_name("wasm_gl_buffer_data")))
void wasm_gl_buffer_data(GLenum target, GLsizei size, const void* data, GLenum usage);

// Drawing functions
__attribute__((import_module("env"), import_name("wasm_gl_draw_arrays")))
void wasm_gl_draw_arrays(GLenum mode, GLint first, GLsizei count);

__attribute__((import_module("env"), import_name("wasm_gl_draw_elements")))
void wasm_gl_draw_elements(GLenum mode, GLsizei count, GLenum type, const void* indices);

// Uniform functions
__attribute__((import_module("env"), import_name("wasm_gl_uniform1f")))
void wasm_gl_uniform1f(GLint location, GLfloat v0);

__attribute__((import_module("env"), import_name("wasm_gl_uniform1i")))
void wasm_gl_uniform1i(GLint location, GLint v0);

__attribute__((import_module("env"), import_name("wasm_gl_uniform2f")))
void wasm_gl_uniform2f(GLint location, GLfloat v0, GLfloat v1);

__attribute__((import_module("env"), import_name("wasm_gl_uniform3f")))
void wasm_gl_uniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);

__attribute__((import_module("env"), import_name("wasm_gl_uniform4f")))
void wasm_gl_uniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

__attribute__((import_module("env"), import_name("wasm_gl_uniform2fv")))
void wasm_gl_uniform2fv(GLint location, GLsizei count, const GLfloat* value);

__attribute__((import_module("env"), import_name("wasm_gl_uniform3fv")))
void wasm_gl_uniform3fv(GLint location, GLsizei count, const GLfloat* value);

__attribute__((import_module("env"), import_name("wasm_gl_uniform4fv")))
void wasm_gl_uniform4fv(GLint location, GLsizei count, const GLfloat* value);

__attribute__((import_module("env"), import_name("wasm_gl_uniform_matrix4fv")))
void wasm_gl_uniform_matrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

// Texture functions
__attribute__((import_module("env"), import_name("wasm_gl_gen_textures")))
void wasm_gl_gen_textures(GLsizei n, GLuint* textures);

__attribute__((import_module("env"), import_name("wasm_gl_bind_texture")))
void wasm_gl_bind_texture(GLenum target, GLuint texture);

__attribute__((import_module("env"), import_name("wasm_gl_delete_textures")))
void wasm_gl_delete_textures(GLsizei n, const GLuint* textures);

__attribute__((import_module("env"), import_name("wasm_gl_tex_image_2d")))
void wasm_gl_tex_image_2d(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* data);

__attribute__((import_module("env"), import_name("wasm_gl_tex_parameteri")))
void wasm_gl_tex_parameteri(GLenum target, GLenum pname, GLint param);

__attribute__((import_module("env"), import_name("wasm_gl_tex_parameterf")))
void wasm_gl_tex_parameterf(GLenum target, GLenum pname, GLfloat param);

__attribute__((import_module("env"), import_name("wasm_gl_active_texture")))
void wasm_gl_active_texture(GLenum texture);

// State functions
__attribute__((import_module("env"), import_name("wasm_gl_enable")))
void wasm_gl_enable(GLenum cap);

__attribute__((import_module("env"), import_name("wasm_gl_disable")))
void wasm_gl_disable(GLenum cap);

// Convenience macros for EGL functions
#define eglGetDisplay(x) wasm_egl_get_display(x)
#define eglInitialize(d, ma, mi) wasm_egl_initialize(d, ma, mi)
#define eglChooseConfig(d, a, c, s, n) wasm_egl_choose_config(d, a, c, s, n)
#define eglCreateWindowSurface(d, c, w, a) wasm_egl_create_window_surface(d, c, w, a)
#define eglCreateContext(d, c, s, a) wasm_egl_create_context(d, c, s, a)
#define eglMakeCurrent(d, dr, r, c) wasm_egl_make_current(d, dr, r, c)
#define eglSwapBuffers(d, s) wasm_egl_swap_buffers(d, s)

// Convenience macros for OpenGL ES functions
#define glClear(m) wasm_gl_clear(m)
#define glClearColor(r, g, b, a) wasm_gl_clear_color(r, g, b, a)
#define glViewport(x, y, w, h) wasm_gl_viewport(x, y, w, h)

// Shader macros
#define glCreateShader(t) wasm_gl_create_shader(t)
#define glShaderSource(s, c, str, len) wasm_gl_shader_source(s, c, str, len)
#define glCompileShader(s) wasm_gl_compile_shader(s)
#define glGetShaderiv(s, p, params) wasm_gl_get_shaderiv(s, p, params)
#define glGetShaderInfoLog(s, ml, l, log) wasm_gl_get_shader_info_log(s, ml, l, log)

// Program macros
#define glCreateProgram() wasm_gl_create_program()
#define glAttachShader(p, s) wasm_gl_attach_shader(p, s)
#define glLinkProgram(p) wasm_gl_link_program(p)
#define glUseProgram(p) wasm_gl_use_program(p)
#define glGetProgramiv(p, pn, params) wasm_gl_get_programiv(p, pn, params)
#define glGetProgramInfoLog(p, ml, l, log) wasm_gl_get_program_info_log(p, ml, l, log)

// Attribute/Uniform macros
#define glGetAttribLocation(p, n) wasm_gl_get_attrib_location(p, n)
#define glGetUniformLocation(p, n) wasm_gl_get_uniform_location(p, n)
#define glEnableVertexAttribArray(i) wasm_gl_enable_vertex_attrib_array(i)
#define glDisableVertexAttribArray(i) wasm_gl_disable_vertex_attrib_array(i)
#define glVertexAttribPointer(i, s, t, n, st, p) wasm_gl_vertex_attrib_pointer(i, s, t, n, st, p)

// Buffer macros
#define glGenBuffers(n, b) wasm_gl_gen_buffers(n, b)
#define glBindBuffer(t, b) wasm_gl_bind_buffer(t, b)
#define glBufferData(t, s, d, u) wasm_gl_buffer_data(t, s, d, u)

// Drawing macros
#define glDrawArrays(m, f, c) wasm_gl_draw_arrays(m, f, c)
#define glDrawElements(m, c, t, i) wasm_gl_draw_elements(m, c, t, i)

// Uniform macros
#define glUniform1f(l, v) wasm_gl_uniform1f(l, v)
#define glUniform1i(l, v) wasm_gl_uniform1i(l, v)
#define glUniform2f(l, v0, v1) wasm_gl_uniform2f(l, v0, v1)
#define glUniform3f(l, v0, v1, v2) wasm_gl_uniform3f(l, v0, v1, v2)
#define glUniform4f(l, v0, v1, v2, v3) wasm_gl_uniform4f(l, v0, v1, v2, v3)
#define glUniform2fv(l, c, v) wasm_gl_uniform2fv(l, c, v)
#define glUniform3fv(l, c, v) wasm_gl_uniform3fv(l, c, v)
#define glUniform4fv(l, c, v) wasm_gl_uniform4fv(l, c, v)
#define glUniformMatrix4fv(l, c, t, v) wasm_gl_uniform_matrix4fv(l, c, t, v)
#define glGenTextures(n, t) wasm_gl_gen_textures(n, t)
#define glBindTexture(tg, t) wasm_gl_bind_texture(tg, t)
#define glDeleteTextures(n, t) wasm_gl_delete_textures(n, t)
#define glTexImage2D(tg, l, i, w, h, b, f, ty, d) wasm_gl_tex_image_2d(tg, l, i, w, h, b, f, ty, d)
#define glTexParameteri(tg, p, v) wasm_gl_tex_parameteri(tg, p, v)
#define glTexParameterf(tg, p, v) wasm_gl_tex_parameterf(tg, p, v)
#define glActiveTexture(t) wasm_gl_active_texture(t)
#define glEnable(c) wasm_gl_enable(c)
#define glDisable(c) wasm_gl_disable(c)

// Initialization helper function
static inline int graphics_initialize(EGLDisplay *out_display, EGLSurface *out_surface, EGLContext *out_context) {
    // Initialize graphics subsystem
    if (wasm_graphics_init() != 0) {
        return -1;
    }

    // Get EGL display
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        return -1;
    }

    // Initialize EGL
    EGLint major, minor;
    if (!eglInitialize(display, &major, &minor)) {
        return -1;
    }

    // Choose config
    const EGLint config_attribs[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    EGLConfig config;
    EGLint num_configs;
    if (!eglChooseConfig(display, config_attribs, &config, 1, &num_configs)) {
        return -1;
    }

    // Create surface
    EGLSurface surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)0, NULL);
    if (surface == EGL_NO_SURFACE) {
        return -1;
    }

    // Create context
    const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);
    if (context == EGL_NO_CONTEXT) {
        return -1;
    }

    // Make current
    if (!eglMakeCurrent(display, surface, surface, context)) {
        return -1;
    }

    // Set output parameters
    if (out_display) *out_display = display;
    if (out_surface) *out_surface = surface;
    if (out_context) *out_context = context;

    return 0;
}

#endif // WASM_GRAPHICS_H

