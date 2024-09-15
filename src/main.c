#include "display.h"
#include "vector.h"

#define N_POINTS (9 * 9 * 9)
vec3_t cube[N_POINTS];
vec2_t projected_points[N_POINTS];
vec3_t cube_roation = { .x = 0, .y = 0, .z = 0};

vec3_t camera_position = { .x = 0, .y = 0, .z = -5};

bool is_running = false;
int previous_frame_time = 0;

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
            for(float z = -1; z <= 1; z += 0.25) {
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
    const float fov_factor = 640;
    vec2_t projected = {
        .x = (point.x * fov_factor) / point.z,
        .y = (point.y * fov_factor) / point.z
    };
    return projected;
}

void update() {
    int wait_time = previous_frame_time + FRAME_TARGET_TIME - SDL_GetTicks();
    if(wait_time > 0) {
        SDL_Delay(wait_time);
    }
    previous_frame_time = SDL_GetTicks();

    cube_roation.y += 0.01;
    cube_roation.z += 0.01;
    cube_roation.x += 0.01;

    for(int i = 0; i < N_POINTS; i++) {
        vec3_t point = cube[i];

        vec3_t transformed_point = vec3_roate_x(point, cube_roation.x);
        transformed_point = vec3_roate_y(transformed_point, cube_roation.y);
        transformed_point = vec3_roate_z(transformed_point, cube_roation.z);

        transformed_point.z -= camera_position.z;

        vec2_t projected_point = project(transformed_point);
        projected_points[i] = projected_point;
    }
}

void render() {
    draw_grid(0xFF00FFFF, 10, 10);

    for(int i = 0; i < N_POINTS; i++) {
        vec2_t projected_point = projected_points[i];
        draw_fill_rect(
            projected_point.x + (window_width / 2), 
            projected_point.y + (window_height / 2), 
            4,
            4,
            0xFFFFFF00
        );
    }

    render_color_buffer();
    clear_color_buffer(0x00000000);
    SDL_RenderPresent(renderer);
}

