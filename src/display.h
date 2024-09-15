#ifndef DISPLAY_H
#define DISPLAY_H

#include<stdint.h>
#include<stdbool.h>
#include<SDL2/SDL.h>

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS) //millisec

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern int window_width, window_height;
extern uint32_t* color_buffer;
extern SDL_Texture* color_buffer_texture;

bool init_window();
void clear_color_buffer(uint32_t color);
void draw_pixel(int x, int y, uint32_t color);
void render_color_buffer();
void draw_grid(uint32_t color, int inv_w, int inv_h);
void draw_fill_rect(int x, int y, int w, int h, uint32_t color);
void destroy_window();

#endif