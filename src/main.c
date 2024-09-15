#include "display.h"
#include "vector.h"

#define N_POINTS (9 * 9 * 9)
vec3_t cube[N_POINTS];
vec2_t projected_points[N_POINTS];

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

    int idx = 0;
    for(float x = -1; x <= 1; x += 0.25) {
        for(float y = -1; y <= 1; y += 0.25) {
            for(float z = 01; z <= 1; z += 0.25) {
                vec3_t p = { .x = x, .y = y, .z = z};
                cube[idx++] = p;
            }
        }
    }
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

vec2_t project(vec3_t point) {
    const float fov_factor = 120;

    vec2_t projected = {
        .x = point.x * fov_factor + window_width / 2,
        .y = point.y * fov_factor + window_height / 2,
    };
    return projected;
}

void update() {
    for(int i = 0; i < N_POINTS; i++) {
        vec3_t point = cube[i];
        vec2_t projected_point = project(point);
        projected_points[i] = projected_point;
    }
}

void render() {
    draw_grid(0xFF00FFFF, 10, 10);

    for(int i = 0; i < N_POINTS; i++) {
        vec2_t projected_point = projected_points[i];
        draw_fill_rect(
            projected_point.x, 
            projected_point.y, 
            4,
            4,
            0xFFFFFF00
        );
    }

    render_color_buffer();
    clear_color_buffer(0x00000000);
    SDL_RenderPresent(renderer);
}

