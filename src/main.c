#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"
#include "matrix.h"

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

    render_mod_mask |= 1 << RENDER_MOD_FACE;
    render_mod_mask |= 1 << RENDER_MOD_BACKFACE;

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
        if(evn.key.keysym.sym == SDLK_ESCAPE) {
            is_running = false;
        } else if(evn.key.keysym.sym == SDLK_1) {
            render_mod_mask ^= 1 << RENDER_MOD_WIREFRAME;
        } else if(evn.key.keysym.sym == SDLK_2) {
            render_mod_mask ^= 1 << RENDER_MOD_VERTEX;
        } else if(evn.key.keysym.sym == SDLK_3) {
            render_mod_mask ^= 1 << RENDER_MOD_FACE;
        } else if(evn.key.keysym.sym == SDLK_4) {
            render_mod_mask ^= 1 << RENDER_MOD_BACKFACE;
        }
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

    //mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    //mesh.rotation.z += 0.01;
    //mesh.scale.x += 0.001;
    //mesh.scale.y += 0.003;
    //mesh.translation.x += 0.01;
    mesh.translation.z = 5.0;

    triangles_to_render = NULL;

    mat4_t scale_mat = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t rotx_mat = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t roty_mat = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotz_mat = mat4_make_rotation_z(mesh.rotation.z);
    mat4_t trans_mat = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);

    int len_faces = array_length(mesh.faces);
    for(int i = 0; i < len_faces; i++) {
        face_t mesh_face = mesh.faces[i];

        vec3_t vertices[3];
        vertices[0] = mesh.vertices[mesh_face.a - 1];
        vertices[1] = mesh.vertices[mesh_face.b - 1];
        vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec4_t transformed_vertices[3];
        for(int j = 0; j < 3; j++) {
            vec4_t transformed_vertex = vec4_from_vec3(vertices[j]);

            transformed_vertex = mat4_mul_vec4(scale_mat, transformed_vertex);
            transformed_vertex = mat4_mul_vec4(rotx_mat, transformed_vertex);
            transformed_vertex = mat4_mul_vec4(roty_mat, transformed_vertex);
            transformed_vertex = mat4_mul_vec4(rotz_mat, transformed_vertex);
            transformed_vertex = mat4_mul_vec4(trans_mat, transformed_vertex);

            transformed_vertices[j] = transformed_vertex;
        }

        // back face culling
        if(render_mod_mask & (1 << RENDER_MOD_BACKFACE)) {
            vec3_t a = vec3_from_vec4(transformed_vertices[0]);
            vec3_t b = vec3_from_vec4(transformed_vertices[1]);
            vec3_t c = vec3_from_vec4(transformed_vertices[2]);

            vec3_t ab = vec3_sub(b, a);
            vec3_t ac = vec3_sub(c, a);
            vec3_normalize(&ab);
            vec3_normalize(&ac);
            vec3_t normal = vec3_cross(ab, ac);
            vec3_normalize(&normal);
            vec3_t to_camera = vec3_sub(camera_position, a);
            if(vec3_dot(normal, to_camera) < 0) {
                continue;
            }
        }

        vec2_t projected_points[3];
        for(int j = 0; j < 3; j++) {
            vec3_t vertex = vec3_from_vec4(transformed_vertices[j]);
            projected_points[j] = project(vertex);
            projected_points[j].x += window_width / 2;
            projected_points[j].y += window_height / 2;
        }

        // 평균 z 값 구하기
        float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3;

        triangle_t projected_triangle = {
            .points = {
                projected_points[0],
                projected_points[1],
                projected_points[2]
            },
            .color = mesh_face.color,
            .avg_depth = avg_depth
        };

        array_push(triangles_to_render, projected_triangle);
    }

    // 평균 z 값이 작은 순서대로 정렬
    int len_triangle = array_length(triangles_to_render);
    for(int i = len_triangle - 1; i > 0; i--) {
        for(int j = 0; j < i; j++) {
            if(triangles_to_render[j].avg_depth < triangles_to_render[j + 1].avg_depth) {
                triangle_t tmp = triangles_to_render[j];
                triangles_to_render[j] = triangles_to_render[j + 1];
                triangles_to_render[j + 1] = tmp;
            }
        }
    }
}

void render() {
    draw_grid(0xFF00FFFF, 10, 10);

    int len_triangles = array_length(triangles_to_render);
    for(int i = 0; i < len_triangles; i++) {
        triangle_t triangle = triangles_to_render[i];

        if(render_mod_mask & (1 << RENDER_MOD_FACE)) {
            draw_filled_triangle(
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[2].x,
                triangle.points[2].y,
                triangle.color
            );
        }
        
        if(render_mod_mask & (1 << RENDER_MOD_WIREFRAME)) {
            draw_triangle(
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[2].x,
                triangle.points[2].y,
                0xFF00FF00
            );
        }
        
        if(render_mod_mask & (1 << RENDER_MOD_VERTEX)) {
            for(int j = 0; j < 3; j++) {
                draw_fill_rect(
                    triangle.points[j].x - 3, 
                    triangle.points[j].y - 3, 
                    6,
                    6,
                    0xFFFFFF00
                );
            }
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

