#ifndef DISPLAY_H
#define DISPLAY_H

#include<stdint.h>
#include<stdbool.h>
#include<SDL2/SDL.h>

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS) //millisec

enum RENDER_MOD {
    RENDER_MOD_WIREFRAME,
    RENDER_MOD_VERTEX,
    RENDER_MOD_SOLID,
    RENDER_MOD_BACKFACE,
    RENDER_MOD_TEXTURED,
};

bool init_window();
void destroy_window();
int get_window_width();
int get_window_height();

void render_color_buffer();
void clear_color_buffer(uint32_t color);
void clear_z_buffer();
float get_zbuffer_at(int x, int y);
void update_zbuffer_at(int x, int y, float v);

void draw_pixel(int x, int y, uint32_t color);
void draw_grid(uint32_t color, int inv_w, int inv_h);
void draw_fill_rect(int x, int y, int w, int h, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

void set_render_mod(int mod, bool state);
void switch_render_mod(int mod);
bool is_set_render_mod(int mod);

#endif