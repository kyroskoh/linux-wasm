// SPDX-License-Identifier: GPL-2.0-only
//
// Example Graphics Program for Linux/Wasm
// Demonstrates basic EGL and OpenGL ES usage
//
// Compile with:
//   clang --target=wasm32-unknown-unknown \
//     --sysroot=$LW_INSTALL/musl \
//     -fPIC -shared \
//     -o example-graphics.wasm \
//     example-graphics.c
//
// To use in Linux/Wasm, copy the .wasm file to the initramfs and run it.

#include "../wasm-graphics.h"
#include <stdio.h>
#include <unistd.h>
#include <math.h>

// Animation state
static float hue = 0.0f;

// HSV to RGB conversion
static void hsv_to_rgb(float h, float s, float v, float *r, float *g, float *b) {
    int i;
    float f, p, q, t;

    if (s == 0.0f) {
        *r = *g = *b = v;
        return;
    }

    h /= 60.0f;
    i = (int)h;
    f = h - i;
    p = v * (1.0f - s);
    q = v * (1.0f - s * f);
    t = v * (1.0f - s * (1.0f - f));

    switch (i % 6) {
        case 0: *r = v; *g = t; *b = p; break;
        case 1: *r = q; *g = v; *b = p; break;
        case 2: *r = p; *g = v; *b = t; break;
        case 3: *r = p; *g = q; *b = v; break;
        case 4: *r = t; *g = p; *b = v; break;
        case 5: *r = v; *g = p; *b = q; break;
    }
}

int main(void) {
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    printf("Linux/Wasm Graphics Example\n");
    printf("===========================\n\n");

    // Initialize graphics
    printf("Initializing graphics subsystem...\n");
    if (graphics_initialize(&display, &surface, &context) != 0) {
        fprintf(stderr, "Failed to initialize graphics\n");
        return 1;
    }

    printf("Graphics initialized successfully!\n");
    printf("  Display: %p\n", display);
    printf("  Surface: %p\n", surface);
    printf("  Context: %p\n", context);
    printf("\n");

    // Set viewport
    glViewport(0, 0, 800, 600);
    printf("Viewport set to 800x600\n\n");

    printf("Starting color animation loop...\n");
    printf("(Press Ctrl+C to stop)\n\n");

    // Animation loop
    for (int frame = 0; frame < 1000; frame++) {
        // Calculate color based on HSV (cycling hue)
        float r, g, b;
        hsv_to_rgb(hue, 1.0f, 1.0f, &r, &g, &b);

        // Clear screen with current color
        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Swap buffers to present
        eglSwapBuffers(display, surface);

        // Update hue for next frame
        hue += 0.5f;
        if (hue >= 360.0f) {
            hue = 0.0f;
        }

        // Print status every 60 frames
        if (frame % 60 == 0) {
            printf("Frame %d: RGB(%.2f, %.2f, %.2f) Hue=%.1f°\n", 
                   frame, r, g, b, hue);
        }

        // Sleep to limit frame rate (~60 FPS)
        usleep(16667);
    }

    printf("\nAnimation complete!\n");
    return 0;
}

