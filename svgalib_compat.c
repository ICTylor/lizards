#include "svgalib_compat.h"


// Global SDL variables
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* screen_texture = NULL;
uint32_t* pixel_buffer = NULL;
int screen_width = 640;
int screen_height = 480;

// Keyboard state
uint8_t keyboard_state[512] = {0};

// Mouse state
int mouse_x = 0;
int mouse_y = 0;
uint32_t mouse_buttons = 0;

// Function prototypes (including new ones)
int vga_init(void);

int vga_setmode(int mode);
void vga_setcolor(int color);
void vga_drawpixel(int x, int y);
void vga_drawline(int x1, int y1, int x2, int y2);
int keyboard_init(void);
void keyboard_close(void);
int keyboard_update(void);
int keyboard_keypressed(int scancode);
void keyboard_clearstate(void);
void gl_setcontextvga(int mode);
void gl_getcontext(GraphicsContext* context);
void gl_setcontextvgavirtual(int mode);
void gl_setcontext(GraphicsContext* context);
void gl_clearscreen(int color);
void gl_line(int x1, int y1, int x2, int y2, int color);
void gl_fillbox(int x, int y, int width, int height, int color);
void gl_putbox(int x, int y, int width, int height, void* buffer);
void gl_getbox(int x, int y, int width, int height, void* buffer);
void gl_putboxmask(int x, int y, int width, int height, void* buffer);
void gl_copyscreen();
//void gl_copyscreen(GraphicsContext* context);
void gl_copyboxfromcontext(GraphicsContext* source, int sx, int sy, int width, int height, int dx, int dy);
void gl_freecontext(GraphicsContext* context);
int vga_getmousetype(void);
int mouse_init(const char* device, int type, int baud);
void mouse_setdefaulteventhandler(void);
void mouse_setposition(int x, int y);
void mouse_setxrange(int x1, int x2);
void mouse_setyrange(int y1, int y2);
void mouse_setscale(int scale);
int mouse_getx(void);
int mouse_gety(void);
int mouse_getbutton(void);
void mouse_update(void);
void mouse_close(void);

int vga_init(void) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow("Lizards",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              screen_width,
                              screen_height,
                              SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    screen_texture = SDL_CreateTexture(renderer,
                                       SDL_PIXELFORMAT_RGB888,  // Change this line
                                       SDL_TEXTUREACCESS_STREAMING,
                                       screen_width,
                                       screen_height);
    if (screen_texture == NULL) {
        printf("Screen texture could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    pixel_buffer = (uint32_t*)malloc(screen_width * screen_height * sizeof(uint32_t));
    if (pixel_buffer == NULL) {
        printf("Failed to allocate pixel buffer!\n");
        return -1;
    }

    memset(pixel_buffer, 0, screen_width * screen_height * sizeof(uint32_t));

    // Initialize the VGA palette
    init_default_palette();
    // Force the window to show
    SDL_ShowWindow(window);

    return 0;
}

// Set video mode (simplified)
int vga_setmode(int mode) {
    // For simplicity, we'll ignore the mode and always use 640x480
    return 0;
}

static uint32_t current_color = 0xFFFFFF;  // Default to white

#include <stdint.h>
#include <string.h>

#define PALETTE_SIZE 256

typedef unsigned char uchar;

typedef struct {
    struct {
        unsigned char red;
        unsigned char green;
        unsigned char blue;
    } color[256];
} Palette;

static uint32_t color_palette[PALETTE_SIZE] = {0};


// Default palette values from the original svgalib
static const unsigned char default_red[256] = {
        0, 0, 0, 0, 42, 42, 42, 42, 21, 21, 21, 21, 63, 63, 63, 63,
        0, 5, 8, 11, 14, 17, 20, 24, 28, 32, 36, 40, 45, 50, 56, 63,
        0, 16, 31, 47, 63, 63, 63, 63, 63, 63, 63, 63, 63, 47, 31, 16,
        0, 0, 0, 0, 0, 0, 0, 0, 31, 39, 47, 55, 63, 63, 63, 63,
        63, 63, 63, 63, 63, 55, 47, 39, 31, 31, 31, 31, 31, 31, 31, 31,
        45, 49, 54, 58, 63, 63, 63, 63, 63, 63, 63, 63, 63, 58, 54, 49,
        45, 45, 45, 45, 45, 45, 45, 45, 0, 7, 14, 21, 28, 28, 28, 28,
        28, 28, 28, 28, 28, 21, 14, 7, 0, 0, 0, 0, 0, 0, 0, 0,
        14, 17, 21, 24, 28, 28, 28, 28, 28, 28, 28, 28, 28, 24, 21, 17,
        14, 14, 14, 14, 14, 14, 14, 14, 20, 22, 24, 26, 28, 28, 28, 28,
        28, 28, 28, 28, 28, 26, 24, 22, 20, 20, 20, 20, 20, 20, 20, 20,
        0, 4, 8, 12, 16, 16, 16, 16, 16, 16, 16, 16, 16, 12, 8, 4,
        0, 0, 0, 0, 0, 0, 0, 0, 8, 10, 12, 14, 16, 16, 16, 16,
        16, 16, 16, 16, 16, 14, 12, 10, 8, 8, 8, 8, 8, 8, 8, 8,
        11, 12, 13, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 15, 13, 12,
        11, 11, 11, 11, 11, 11, 11, 11, 0, 0, 0, 0, 0, 0, 0, 0
};

static const unsigned char default_green[256] = {
        0, 0, 42, 42, 0, 0, 21, 42, 21, 21, 63, 63, 21, 21, 63, 63,
        0, 5, 8, 11, 14, 17, 20, 24, 28, 32, 36, 40, 45, 50, 56, 63,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 31, 47, 63, 63, 63, 63,
        63, 63, 63, 63, 63, 47, 31, 16, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 39, 47, 55, 63, 63, 63, 63, 63, 63, 63, 63, 63, 55, 47, 39,
        45, 45, 45, 45, 45, 45, 45, 45, 45, 49, 54, 58, 63, 63, 63, 63,
        63, 63, 63, 63, 63, 58, 54, 49, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 7, 14, 21, 29, 28, 28, 28, 28, 28, 28, 28, 28, 21, 14, 7,
        14, 14, 14, 14, 14, 14, 14, 14, 14, 17, 21, 24, 28, 28, 28, 28,
        28, 28, 28, 28, 28, 24, 21, 17, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 22, 24, 26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 26, 24, 22,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 8, 12, 16, 16, 16, 16,
        16, 16, 16, 16, 16, 12, 8, 4, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 10, 12, 14, 16, 16, 16, 16, 16, 16, 16, 16, 16, 14, 12, 10,
        11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 13, 15, 16, 16, 16, 16,
        16, 16, 16, 16, 16, 15, 13, 12, 0, 0, 0, 0, 0, 0, 0, 0
};

static const unsigned char default_blue[256] = {
        0, 42, 0, 42, 0, 42, 0, 42, 21, 63, 21, 63, 21, 63, 21, 63,
        0, 5, 8, 11, 14, 17, 20, 24, 28, 32, 36, 40, 45, 50, 56, 63,
        63, 63, 63, 63, 63, 47, 31, 16, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 16, 31, 47, 63, 63, 63, 63, 63, 63, 63, 63, 63, 55, 47, 39,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 39, 47, 55, 63, 63, 63, 63,
        63, 63, 63, 63, 63, 58, 54, 49, 45, 45, 45, 45, 45, 45, 45, 45,
        45, 49, 54, 58, 63, 63, 63, 63, 28, 28, 28, 28, 28, 21, 14, 7,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 14, 21, 28, 28, 28, 28,
        28, 28, 28, 28, 28, 24, 21, 17, 14, 14, 14, 14, 14, 14, 14, 14,
        14, 17, 21, 24, 28, 28, 28, 28, 28, 28, 28, 28, 28, 26, 24, 22,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 22, 24, 26, 28, 28, 28, 28,
        16, 16, 16, 16, 16, 12, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 4, 8, 12, 16, 16, 16, 16, 16, 16, 16, 16, 16, 14, 12, 10,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 10, 12, 14, 16, 16, 16, 16,
        16, 16, 16, 16, 16, 15, 13, 12, 11, 11, 11, 11, 11, 11, 11, 11,
        11, 12, 13, 15, 16, 16, 16, 16, 0, 0, 0, 0, 0, 0, 0, 0
};


void vga_getpalette(unsigned char index, int* r, int* g, int* b) {
    uint32_t color = color_palette[index];
    *r = (color >> 16) & 0xFF;
    *g = (color >> 8) & 0xFF;
    *b = color & 0xFF;
}

void vga_setpalette(unsigned char index, int r, int g, int b) {
    // Scale up the color values from 0-63 to 0-255
    r = (r * 255) / 63;
    g = (g * 255) / 63;
    b = (b * 255) / 63;
    color_palette[index] = (b << 16) | (g << 8) | r;
}

void init_default_palette() {
    for (int i = 0; i < 256; i++) {
        vga_setpalette(i, default_red[i], default_green[i], default_blue[i]);
    }
    printf("Default palette initialized\n");
}

void gl_setrgbpalette() {
    int i;
    for (i = 0; i < 256; i++) {
        int r, g, b;
        r = (i & 0xE0) >> 2;  // 3 bits for red (0-7) scaled to 0-63
        g = (i & 0x1C) << 1;  // 3 bits for green (0-7) scaled to 0-63
        b = (i & 0x03) << 4;  // 2 bits for blue (0-3) scaled to 0-63
        vga_setpalette(i, r, g, b);
    }
    printf("RGB palette set\n");
}


void gl_getpalettecolor(unsigned char c, int *r, int *g, int *b) {
    vga_getpalette(c, r, g, b);
}

void gl_setpalettecolor(unsigned char c, int r, int g, int b) {
    vga_setpalette(c, r, g, b);
}

void gl_setpalettecolors(int s, int n, void *_dp) {
    uchar *dp = _dp;
    int i;
    for (i = s; i < s + n && i < PALETTE_SIZE; i++) {
        unsigned char r, g, b;
        r = *(dp++);
        g = *(dp++);
        b = *(dp++);
        vga_setpalette(i, r, g, b);
    }
}

void gl_getpalettecolors(int s, int n, void *_dp) {
    uchar *dp = _dp;
    int i;
    for (i = s; i < s + n && i < PALETTE_SIZE; i++) {
        int r, g, b;
        vga_getpalette(i, &r, &g, &b);
        *(dp++) = (unsigned char) r;
        *(dp++) = (unsigned char) g;
        *(dp++) = (unsigned char) b;
    }
}

void gl_getpalette(void *p) {
    gl_getpalettecolors(0, 256, p);
}

void gl_setpalette(void *p) {
    gl_setpalettecolors(0, 256, p);
}

// Function to convert palette color to SDL color
uint32_t palette_to_sdl_color(unsigned char index) {
    uint32_t color = color_palette[index & 0xFF];
    // SDL uses ABGR format for SDL_PIXELFORMAT_RGB888
    return ((color & 0xFF) << 16) | (color & 0x00FF00) | ((color & 0xFF0000) >> 16);
}

// Update existing drawing functions to use the palette
void vga_drawpixel(int x, int y) {
    if (x >= 0 && x < screen_width && y >= 0 && y < screen_height) {
        uint32_t sdl_color = palette_to_sdl_color(current_color);
        pixel_buffer[y * screen_width + x] = sdl_color;
    }
}


// Update vga_setcolor to use the palette
void vga_setcolor(int color_index) {
    if (color_index >= 0 && color_index < PALETTE_SIZE) {
        current_color = color_index;
    }
}

// Function to get a color from the palette (useful for debugging)
uint32_t get_palette_color(int index) {
    if (index >= 0 && index < PALETTE_SIZE) {
        return color_palette[index];
    }
    return 0;  // Return black for out-of-range indices
}

// Draw a line using Bresenham's algorithm
void vga_drawline(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;

    while (true) {
        vga_drawpixel(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

// Initialize keyboard
int keyboard_init(void) {
    // SDL handles keyboard initialization, so we don't need to do anything here
    return 0;
}

// Close keyboard
void keyboard_close(void) {
    // SDL handles keyboard cleanup, so we don't need to do anything here
    memset(keyboard_state, 0, sizeof(uint8_t) * 512);
}

// Update keyboard state
int keyboard_update(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return -1;  // Signal to quit the program
        } else if (event.type == SDL_KEYDOWN) {
            keyboard_state[event.key.keysym.scancode] = 1;
        } else if (event.type == SDL_KEYUP) {
            keyboard_state[event.key.keysym.scancode] = 0;
        }
    }
    return 0;
}

// Check if a key is pressed
int keyboard_keypressed(int scancode) {
    return keyboard_state[scancode];
}

// Set VGA context (simplified)
void gl_setcontextvga(int mode) {
    // We're always in the same mode, so this is a no-op
}

// Set virtual VGA context (simplified)
void gl_setcontextvgavirtual(int mode) {
    // We're always in the same mode, so this is a no-op
}

// Clear the screen
void gl_clearscreen(int color) {
    vga_setcolor(color);
    SDL_SetRenderDrawColor(renderer, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);
    SDL_RenderClear(renderer);
    memset(pixel_buffer, color, screen_width * screen_height * sizeof(uint32_t));
}

// Draw a line
void gl_line(int x1, int y1, int x2, int y2, int color) {
    vga_setcolor(color);
    vga_drawline(x1, y1, x2, y2);
}

// Fill a box
void gl_fillbox(int x, int y, int width, int height, int color) {
    vga_setcolor(color);
    for (int j = y; j < y + height; j++) {
        for (int i = x; i < x + width; i++) {
            vga_drawpixel(i, j);
        }
    }
}

// Put a box on the screen
void gl_putbox(int x, int y, int width, int height, void* buffer) {
    unsigned char* src = (unsigned char*)buffer;
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            vga_setcolor(*src++);
            vga_drawpixel(x + i, y + j);
        }
    }
}

// Get a box from the screen
void gl_getbox(int x, int y, int width, int height, void* buffer) {
    uint8_t* dest = (uint8_t*)buffer;
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            *dest++ = pixel_buffer[(y + j) * screen_width + (x + i)] & 0xFF;
        }
    }
}

// Put a masked box on the screen
void gl_putboxmask(int x, int y, int width, int height, void* buffer) {
    uint8_t* src = (uint8_t*)buffer;
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            if (*src != 0) {  // Assuming 0 is the transparent color
                vga_setcolor(*src);
                vga_drawpixel(x + i, y + j);
            }
            src++;
        }
    }
}

void keyboard_clearstate(void) {
    memset(keyboard_state, 0, sizeof(keyboard_state));
}

void gl_setcontext(GraphicsContext* context) {
    // In our simplified version, we'll just copy the context to the screen
    if (context && context->buffer) {
        memcpy(pixel_buffer, context->buffer, screen_width * screen_height * sizeof(uint32_t));
    }
}

void gl_getcontext(GraphicsContext* context) {
    if (context) {
        context->width = screen_width;
        context->height = screen_height;
        context->buffer = malloc(screen_width * screen_height * sizeof(uint32_t));
        if (context->buffer) {
            memcpy(context->buffer, pixel_buffer, screen_width * screen_height * sizeof(uint32_t));
        }
    }
}

//void gl_copyscreen(GraphicsContext* context) {
//    // Update the SDL texture with our pixel buffer
//    SDL_UpdateTexture(screen_texture, NULL, pixel_buffer, screen_width * sizeof(uint32_t));
//
//    // Clear the renderer
//    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
//    SDL_RenderClear(renderer);
//
//    // Copy the texture to the renderer
//    SDL_RenderCopy(renderer, screen_texture, NULL, NULL);
//
//    // Present the renderer
//    SDL_RenderPresent(renderer);
//}

void gl_copyscreen() {
    // Update the SDL texture with our pixel buffer
    SDL_UpdateTexture(screen_texture, NULL, pixel_buffer, screen_width * sizeof(uint32_t));

    // Clear the renderer
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Copy the texture to the renderer
    SDL_RenderCopy(renderer, screen_texture, NULL, NULL);

    // Present the renderer
    SDL_RenderPresent(renderer);
}


void gl_copyboxfromcontext(GraphicsContext* source, int sx, int sy, int width, int height, int dx, int dy) {
    if (!source || !source->buffer) return;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int src_x = sx + x;
            int src_y = sy + y;
            int dst_x = dx + x;
            int dst_y = dy + y;

            if (src_x >= 0 && src_x < source->width && src_y >= 0 && src_y < source->height &&
                dst_x >= 0 && dst_x < screen_width && dst_y >= 0 && dst_y < screen_height) {
                pixel_buffer[dst_y * screen_width + dst_x] = source->buffer[src_y * source->width + src_x];
            }
        }
    }
}

void gl_freecontext(GraphicsContext* context) {
    if (context && context->buffer) {
        free(context->buffer);
        context->buffer = NULL;
    }
}

int vga_getmousetype(void) {
    // We don't need to distinguish mouse types in SDL2
    return 0;
}

int mouse_init(const char* device, int type, int baud) {
    // SDL2 handles mouse initialization, so we just need to enable relative mouse mode
    if (SDL_SetRelativeMouseMode(SDL_TRUE) < 0) {
        printf("Unable to set relative mouse mode: %s\n", SDL_GetError());
        return -1;
    }
    return 0;
}

void mouse_setdefaulteventhandler(void) {
    // No equivalent in SDL2, as event handling is done manually
}

void mouse_setposition(int x, int y) {
    SDL_WarpMouseInWindow(window, x, y);
}

void mouse_setxrange(int x1, int x2) {
    // SDL2 doesn't have a direct equivalent, we'll have to handle this in mouse_update
}

void mouse_setyrange(int y1, int y2) {
    // SDL2 doesn't have a direct equivalent, we'll have to handle this in mouse_update
}

void mouse_setscale(int scale) {
    // SDL2 doesn't have a direct equivalent, we'll have to handle this in mouse_update
}

void mouse_reset() {
    mouse_buttons = 0;
    mouse_close();
    mouse_setposition(0, 0);
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

// Update or add these functions
int mouse_getx(void) {
    return mouse_x;
}

int mouse_gety(void) {
    return mouse_y;
}

int mouse_getbutton(void) {
    return mouse_buttons;
}

void mouse_update(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_MOUSEMOTION:
                mouse_x += event.motion.xrel;
                mouse_y += event.motion.yrel;
                break;
            case SDL_MOUSEBUTTONDOWN:
                mouse_buttons |= SDL_BUTTON(event.button.button);
                break;
            case SDL_MOUSEBUTTONUP:
                mouse_buttons &= ~SDL_BUTTON(event.button.button);
                break;
        }
    }
}

void mouse_close(void) {
    SDL_SetRelativeMouseMode(SDL_FALSE);
}

void test_draw(void) {
    // Draw a white rectangle
    vga_setcolor(7);  // White
    for (int y = 100; y < 200; y++) {
        for (int x = 100; x < 200; x++) {
            vga_drawpixel(x, y);
        }
    }

    // Draw some colored pixels
    vga_setcolor(4);  // Red
    for (int y = 100; y < 200; y++) {
        for (int x = 200; x < 300; x++) {
            vga_drawpixel(x, y);
        }
    }
    vga_setcolor(2);  // Green
    for (int y = 100; y < 200; y++) {
        for (int x = 300; x < 400; x++) {
            vga_drawpixel(x, y);
        }
    }
    vga_setcolor(1);  // Blue
    for (int y = 100; y < 200; y++) {
        for (int x = 400; x < 500; x++) {
            vga_drawpixel(x, y);
        }
    }

    // Update the screen
    gl_copyscreen(NULL);
}



// Constants for scancodes
#define SCANCODE_ESCAPE SDL_SCANCODE_ESCAPE
#define SCANCODE_SPACE SDL_SCANCODE_SPACE
#define SCANCODE_Q SDL_SCANCODE_Q
#define SCANCODE_A SDL_SCANCODE_A
#define SCANCODE_O SDL_SCANCODE_O
#define SCANCODE_P SDL_SCANCODE_P
#define SCANCODE_X SDL_SCANCODE_X
#define SCANCODE_F1 SDL_SCANCODE_F1
#define SCANCODE_CURSORBLOCKUP SDL_SCANCODE_UP
#define SCANCODE_CURSORBLOCKDOWN SDL_SCANCODE_DOWN
#define SCANCODE_CURSORBLOCKRIGHT SDL_SCANCODE_RIGHT
#define SCANCODE_CURSORBLOCKLEFT SDL_SCANCODE_LEFT
#define SCANCODE_CURSORUP SDL_SCANCODE_UP
#define SCANCODE_CURSORDOWN SDL_SCANCODE_DOWN

#define MOUSE_LEFTBUTTON SDL_BUTTON_LMASK

// Clean up SDL resources
void cleanup(void) {
    free(pixel_buffer);
    SDL_DestroyTexture(screen_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}