#include"display.h"
#include<stdio.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int window_width = 800, window_height = 600;
uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;

bool init_window() {
    if(SDL_Init(SDL_INIT_EVERYTHING)) {
        fprintf(stderr, "SDL init error\n");
        return false;
    }

    SDL_DisplayMode dis_mod;
    SDL_GetCurrentDisplayMode(0, &dis_mod);
    window_width = dis_mod.w;
    window_height = dis_mod.h;

    window = SDL_CreateWindow(
        NULL, 
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_BORDERLESS    
    );

    if(!window) {
        fprintf(stderr, "Creating window error\n");
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer) {
        fprintf(stderr, "Creating renderer error\n");
        return false;
    }

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    return true;
}

void clear_color_buffer(uint32_t color) {
    for(int i = 0; i < window_height; i++) {
        for(int j = 0; j < window_width; j++) {
            color_buffer[window_width * i + j] = color;
        }
    }
}

void draw_pixel(int x, int y, uint32_t color) {
    if(x < 0 || x >= window_width || y < 0 || y >= window_height) return;
    color_buffer[window_width * y + x] = color;
}

void draw_grid(uint32_t color, int inv_w, int inv_h) {
    for(int i = 0; i < window_height; i += inv_h) {
        for(int j = 0; j < window_width; j += inv_w) {
            color_buffer[window_width * i + j] = color;
        }
    }
}

void draw_fill_rect(int x, int y, int w, int h, uint32_t color) {
    for(int i = 0; i < h; i++) {
        for(int j = 0; j < w; j++) {
            draw_pixel(x + j, y + i, color);
        }
    }
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    // draw line with DDA algorithm
    int delta_x = x1 - x0;
    int delta_y = y1 - y0;

    const int longest_len = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);

    const float x_inc = delta_x / (float)longest_len;
    const float y_inc = delta_y / (float)longest_len;

    float cur_x = x0, cur_y = y0;
    for(int i = 0; i <= longest_len; i++) {
        draw_pixel(round(cur_x), round(cur_y), color);
        cur_x += x_inc;
        cur_y += y_inc;
    }
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

void render_color_buffer() {
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        sizeof(uint32_t) * window_width
    );
    SDL_RenderCopy(
        renderer,
        color_buffer_texture,
        NULL,
        NULL
    );
}

void destroy_window() {
    SDL_DestroyTexture(color_buffer_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}