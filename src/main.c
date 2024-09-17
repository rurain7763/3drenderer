#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"

triangle_t* triangles_to_render = NULL;

vec3_t camera_position = { .x = 0, .y = 0, .z = 0};

bool is_running = false;
int previous_frame_time = 0;

void process_input();
void setup();
void update();
void render();
void destroy_resources();

int main() {
    is_running = init_window();
    setup();
    while(is_running) {
        process_input();
        update();
        render();
    }
    destroy_window();
    destroy_resources();
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

    load_obj_file("./assets/cube.obj");
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

    mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    mesh.rotation.z += 0.01;

    triangles_to_render = NULL;

    int len_faces = array_length(mesh.faces);
    for(int i = 0; i < len_faces; i++) {
        face_t mesh_face = mesh.faces[i];

        vec3_t vertices[3];
        vertices[0] = mesh.vertices[mesh_face.a - 1];
        vertices[1] = mesh.vertices[mesh_face.b - 1];
        vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec3_t transformed_vertices[3];
        for(int j = 0; j < 3; j++) {
            vec3_t transformed_vertex = vec3_roate_x(vertices[j], mesh.rotation.x);
            transformed_vertex = vec3_roate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = vec3_roate_z(transformed_vertex, mesh.rotation.z);
            
            transformed_vertex.z += 5.f;

            transformed_vertices[j] = transformed_vertex;
        }

        // back face culling
        vec3_t ab = vec3_sub(transformed_vertices[1], transformed_vertices[0]);
        vec3_t ac = vec3_sub(transformed_vertices[2], transformed_vertices[0]);
        vec3_t normal = vec3_cross(ab, ac);
        vec3_t to_camera = vec3_sub(camera_position, transformed_vertices[0]);
        if(vec3_dot(normal, to_camera) < 0) {
            continue;
        }

        triangle_t triangle;
        for(int j = 0; j < 3; j++) {
            vec2_t projected_point = project(transformed_vertices[j]);
            projected_point.x += window_width / 2;
            projected_point.y += window_height / 2;

            triangle.points[j] = projected_point;
        }

        array_push(triangles_to_render, triangle);
    }
}

void render() {
    draw_grid(0xFF00FFFF, 10, 10);

    int len_triangles = array_length(triangles_to_render);
    for(int i = 0; i < len_triangles; i++) {
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
    array_free(triangles_to_render);

    render_color_buffer();
    clear_color_buffer(0x00000000);
    SDL_RenderPresent(renderer);
}

void destroy_resources() {
    free(color_buffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);
}

