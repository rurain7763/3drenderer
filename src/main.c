#include "display.h"
#include "vector.h"
#include "mesh.h"

triangle_t triangles_to_render[N_MESH_FACES];
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

    for(int i = 0; i < N_MESH_FACES; i++) {
        face_t mesh_face = mesh_faces[i];

        vec3_t vertices[3];
        vertices[0] = mesh_vertices[mesh_face.a - 1];
        vertices[1] = mesh_vertices[mesh_face.b - 1];
        vertices[2] = mesh_vertices[mesh_face.c - 1];

        triangle_t triangle;
        for(int j = 0; j < 3; j++) {
            vec3_t transformed_vertex = vec3_roate_x(vertices[j], cube_roation.x);
            transformed_vertex = vec3_roate_y(transformed_vertex, cube_roation.y);
            transformed_vertex = vec3_roate_z(transformed_vertex, cube_roation.z);
            
            transformed_vertex.z -= camera_position.z;

            vec2_t projected_point = project(transformed_vertex);
            projected_point.x += window_width / 2;
            projected_point.y += window_height / 2;

            triangle.points[j] = projected_point;
        }

        triangles_to_render[i] = triangle;
    }
}

void render() {
    draw_grid(0xFF00FFFF, 10, 10);

    for(int i = 0; i < N_MESH_FACES; i++) {
        triangle_t triangle = triangles_to_render[i];
        
        draw_triangle(
            triangle.points[0].x,
            triangle.points[0].y,
            triangle.points[1].x,
            triangle.points[1].y,
            triangle.points[2].x,
            triangle.points[2].y,
            0xFFFFFFFF
        );

        for(int j = 0; j < 3; j++) {
            draw_fill_rect(
                triangle.points[j].x - 2, 
                triangle.points[j].y - 2, 
                4,
                4,
                0xFFFFFF00
            );
        }
    }

    render_color_buffer();
    clear_color_buffer(0x00000000);
    SDL_RenderPresent(renderer);
}

