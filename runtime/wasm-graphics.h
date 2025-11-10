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
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_ARRAY_BUFFER 0x8892
#define GL_STATIC_DRAW 0x88E4
#define GL_FLOAT 0x1406
#define GL_FALSE 0
#define GL_TRUE 1

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

