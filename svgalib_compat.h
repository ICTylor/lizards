#ifndef SVGALIB_COMPAT_H
#define SVGALIB_COMPAT_H

#include <emscripten.h>
#include <emscripten/html5.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

// GraphicsContext structure (simplified)
typedef struct {
    int width;
    int height;
    uint32_t* buffer;
} GraphicsContext;

// Function prototypes
int vga_init(void);

int vga_setmode(int mode);
void vga_setcolor(int color);
void vga_drawpixel(int x, int y);
void vga_drawline(int x1, int y1, int x2, int y2);
void init_default_palette();

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
void mouse_reset(void);


void test_draw(void);


// Cleanup function
void cleanup(void);

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

// Mouse button constant
#define MOUSE_LEFTBUTTON SDL_BUTTON_LMASK

#endif // SVGALIB_COMPAT_H