#include<stdio.h>
#include<SDL2/SDL.h>
#include<stdbool.h>

bool is_running = false;
SDL_Window* window;
SDL_Renderer* renderer;

bool init_window(void);
void process_input();
void setup();
void update();
void render();

int main(void) {
    is_running = init_window();
    setup();
    while(is_running) {
        process_input();
        update();
        render();
    }
    return 0;
}

bool init_window(void) {
    if(SDL_Init(SDL_INIT_EVERYTHING)) {
        fprintf(stderr, "SDL init error\n");
        return false;
    }

    // 윈도우 생성
    window = SDL_CreateWindow(
        NULL, 
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_BORDERLESS    
    );

    if(!window) {
        fprintf(stderr, "Creating window error\n");
        return false;
    }

    // sdl renderer 생성
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer) {
        fprintf(stderr, "Creating renderer error\n");
        return false;
    }

    return true;
}

void setup() {

}

void process_input() {
    SDL_Event evn;
    SDL_PollEvent(&evn);
    switch (evn.type)
    {
    case SDL_QUIT:
        is_running = false;
        break;
    case SDL_KEYDOWN:
        if(evn.key.keysym.sym == SDLK_ESCAPE) 
            is_running = false;
        break;
    default:
        break;
    }
}

void update() {

}

void render() {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);

    //...

    SDL_RenderPresent(renderer);
}
