#include<stdio.h>
#include"display.h"

bool is_running = false;

void process_input();
void setup();
void update();
void render();

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

void render() {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);
    draw_grid(0xFF00FFFF, 10, 10);
    draw_fill_rect(100, 100, 200, 400, 0xFFFFFFFF);
    render_color_buffer();
    clear_color_buffer(0x00000000);
    SDL_RenderPresent(renderer);
}

