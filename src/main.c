#include<stdio.h>
#include<stdint.h>
#include<stdbool.h>
#include<SDL2/SDL.h>

bool is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int window_width = 800, window_height = 600;
uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;

bool init_window();
void process_input();
void setup();
void update();
void clear_color_buffer(uint32_t color);
void render_color_buffer();
void draw_grid(uint32_t color, int inv_w, int inv_h);
void draw_fill_rect(int x, int y, int w, int h, uint32_t color);
void render();
void destroy_window();

int main() {
    is_running = init_window();
    setup();
    while(is_running) {
        process_input();
        update();
        render();
    }
    destroy_window();
    return 0;
}

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

void setup() {
    color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);

    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );
}

void process_input() {
    SDL_Event evn;
    SDL_PollEvent(&evn);
    switch (evn.type) {
    case SDL_QUIT:
        is_running = false;
        break;
    case SDL_KEYDOWN:
        if(evn.key.keysym.sym == SDLK_ESCAPE) 
            is_running = false;
        break;
    }
}

void update() {

}

void clear_color_buffer(uint32_t color) {
    for(int i = 0; i < window_height; i++) {
        for(int j = 0; j < window_width; j++) {
            color_buffer[window_width * i + j] = color;
        }
    }
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
            color_buffer[window_width * (y + i) + (x + j)] = color;
        }
    }
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

void render() {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);
    draw_grid(0xFF00FFFF, 10, 10);
    draw_fill_rect(100, 100, 200, 400, 0xFFFFFFFF);
    render_color_buffer();
    clear_color_buffer(0x00000000);
    SDL_RenderPresent(renderer);
}

void destroy_window() {
    free(color_buffer);
    SDL_DestroyTexture(color_buffer_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}