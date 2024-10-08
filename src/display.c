#include"display.h"
#include<stdio.h>

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static int window_width = 320, window_height = 200;
static uint32_t* color_buffer = NULL;
static float* z_buffer = NULL;
static SDL_Texture* color_buffer_texture = NULL;
static uint32_t render_mod_mask = 0;

int get_window_width() {
    return window_width;
}

int get_window_height() {
    return window_height;
}

bool init_window() {
    if(SDL_Init(SDL_INIT_EVERYTHING)) {
        fprintf(stderr, "SDL init error\n");
        return false;
    }

    SDL_DisplayMode dis_mod;
    SDL_GetCurrentDisplayMode(0, &dis_mod);
    int full_screen_width = dis_mod.w;
    int full_screen_height = dis_mod.h;

    window_width = full_screen_width / 3;
    window_height = full_screen_height / 3;

    window = SDL_CreateWindow(
        NULL, 
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        full_screen_width,
        full_screen_height,
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

    color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);
    z_buffer = (float*)malloc(sizeof(float) * window_width * window_height);

    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    set_render_mod(RENDER_MOD_SOLID, true);
    set_render_mod(RENDER_MOD_BACKFACE, true);

    return true;
}

void clear_color_buffer(uint32_t color) {
    for(int i = 0; i < window_height * window_width; i++) {
        color_buffer[i] = color;
    }
}

void clear_z_buffer() {
    for(int i = 0; i < window_height * window_width; i++) {
        z_buffer[i] = 1.0f;
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
    SDL_RenderPresent(renderer);
}

void destroy_window() {
    free(z_buffer);
    free(color_buffer);
    SDL_DestroyTexture(color_buffer_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void set_render_mod(int mod, bool state) {
    if(state) {
        render_mod_mask |= (1 << mod);
    } else {
        render_mod_mask &= ~(1 << mod);
    }
}

void switch_render_mod(int mod) {
    render_mod_mask ^= (1 << mod);
}

bool is_set_render_mod(int mod) {
    return render_mod_mask & (1 << mod);
}

float get_zbuffer_at(int x, int y) {
    if(x < 0 || x >= window_width || y < 0 || y >= window_height) return -1;
    return z_buffer[window_width * y + x];
}

void update_zbuffer_at(int x, int y, float v) {
    if(x < 0 || x >= window_width || y < 0 || y >= window_height) return;
    z_buffer[window_width * y + x] = v;
}