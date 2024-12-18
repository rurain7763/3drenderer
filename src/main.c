#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"
#include "matrix.h"
#include "light.h"
#include "texture.h"
#include "triangle.h"
#include "upng.h"
#include "camera.h"
#include "clipping.h"
#include "input.h"
#include <stdio.h>

#define MAX_TRIANGLES_PER_MESH 10000

triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
int num_triangles_to_render = 0;

mat4_t perspective_mat;
mat4_t view_mat;
mat4_t world_mat;

bool is_running = false;
int previous_frame_time = 0;
float delta_time = 0;

const int mesh_count = 6;
const char* mesh_names[] = {
    "cube", "efa", "f22", "f117", "crab", "drone"
};
int current_present_mesh = 0;

void process_input();
void update_camera();
void update_next_mesh();
void setup();
void update();
void process_graphics_pipeline(mesh_t* mesh);
void render();

void main() {
    is_running = init_window();
    setup();
    while(is_running) {
        update_input();
        process_input();
        update();
        render();
    }
    free_meshs();
}

void setup() {
    // projection matrix 구하기
    const float aspect_y = get_window_height() / (float)get_window_width();
    const float aspect_x = get_window_width() / (float)get_window_height();
    const float fov_y = M_PI / 3.0; // 60 degree
    const float fov_x = 2 * atan(tan(fov_y / 2) * aspect_x); // 60 degree
    const float z_near = 0.1;
    const float z_far = 100.0;
    perspective_mat = mat4_make_perspective(fov_y, aspect_y, z_near, z_far);
    init_frustum_planes(fov_x, fov_y, z_near, z_far);

    init_global_light(vec3_new(0, 0, 1));
    init_camera(vec3_new(0, 0, 0));

    load_mesh("./assets/cube.obj", "./assets/cube.png", vec3_new(1, 1, 1), vec3_new(0, 0, 0), vec3_new(0, 0, 5));

    set_render_mod(RENDER_MOD_TEXTURED, true);
}

void process_input() {
    SDL_Event evn;
    while(SDL_PollEvent(&evn)) {
        switch (evn.type) {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if(evn.key.keysym.sym == SDLK_ESCAPE) {
                is_running = false;
            } else if(evn.key.keysym.sym == SDLK_1) {
                switch_render_mod(RENDER_MOD_WIREFRAME);
            } else if(evn.key.keysym.sym == SDLK_2) {
                switch_render_mod(RENDER_MOD_VERTEX);
            } else if(evn.key.keysym.sym == SDLK_3) {
                switch_render_mod(RENDER_MOD_SOLID);
            } else if(evn.key.keysym.sym == SDLK_4) {
                switch_render_mod(RENDER_MOD_TEXTURED);
            } else if (evn.key.keysym.sym == SDLK_5) {
                switch_render_mod(RENDER_MOD_BACKFACE);
            }
            on_key_down(evn.key.keysym.sym);         
            break;
        case SDL_KEYUP:
			on_key_up(evn.key.keysym.sym);
			break;
        }
    }
}

void update_camera() {
    const float camera_speed = 5.f;
    const float camera_rotation_speed = 1.f;

    if (is_key_pressed(SDLK_DOWN)) {
        vec2_t new_rot = get_camera_rot();
        new_rot.x += camera_rotation_speed * delta_time;
        update_camera_rot(new_rot);
    }
    else if (is_key_pressed(SDLK_UP)) {
        vec2_t new_rot = get_camera_rot();
        new_rot.x -= camera_rotation_speed * delta_time;
        update_camera_rot(new_rot);
    }

    if (is_key_pressed(SDLK_LEFT)) {
        vec2_t new_rot = get_camera_rot();
        new_rot.y += camera_rotation_speed * delta_time;
        update_camera_rot(new_rot);
    }
    else if (is_key_pressed(SDLK_RIGHT)) {
        vec2_t new_rot = get_camera_rot();
        new_rot.y -= camera_rotation_speed * delta_time;
        update_camera_rot(new_rot);
    }

    if (is_key_pressed(SDLK_a)) {
        vec3_t right = vec3_cross(get_camera_dir(), vec3_new(0, 1, 0));
        vec3_t new_pos = get_camera_pos();
        new_pos = vec3_add(new_pos, vec3_mul(right, camera_speed * delta_time));
        update_camera_pos(new_pos);
    }
    else if (is_key_pressed(SDLK_d)) {
        vec3_t right = vec3_cross(get_camera_dir(), vec3_new(0, 1, 0));
        vec3_t new_pos = get_camera_pos();
        new_pos = vec3_sub(new_pos, vec3_mul(right, camera_speed * delta_time));
        update_camera_pos(new_pos);
    }

    if (is_key_pressed(SDLK_w)) {
        vec3_t velocity = vec3_mul(get_camera_dir(), camera_speed * delta_time);
        vec3_t new_pos = get_camera_pos();
        new_pos = vec3_add(new_pos, velocity);
        update_camera_pos(new_pos);
    }
    else if (is_key_pressed(SDLK_s)) {
        vec3_t velocity = vec3_mul(get_camera_dir(), camera_speed * delta_time);
        vec3_t new_pos = get_camera_pos();
        new_pos = vec3_sub(new_pos, velocity);
        update_camera_pos(new_pos);
    }
}

void update_next_mesh() {
    if (is_key_down(SDLK_n)) {
        free_meshs();
        current_present_mesh = (current_present_mesh + 1) % mesh_count;
        char* file_name[252];
        sprintf(file_name, "./assets/%s.obj", mesh_names[current_present_mesh]);

        char* texture_file_name[252];
        sprintf(texture_file_name, "./assets/%s.png", mesh_names[current_present_mesh]);

        load_mesh(file_name, texture_file_name, vec3_new(1, 1, 1), vec3_new(0, 0, 0), vec3_new(0, 0, 5));
    }
}

void update() {
    int wait_time = previous_frame_time + FRAME_TARGET_TIME - SDL_GetTicks();
    if(wait_time > 0) {
        SDL_Delay(wait_time);
    }
    delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.f;
    previous_frame_time = SDL_GetTicks();

    num_triangles_to_render = 0;

    // view matrix 구하기
    vec3_t up = {0, 1, 0};
    vec3_t target = vec3_add(get_camera_pos(), get_camera_dir());
    view_mat = mat4_look_at(get_camera_pos(), target, up);

    for(int i = 0; i < get_num_meshs(); i++) {
        mesh_t* mesh = get_mesh_at(i);

        // TODO: 메쉬 값 수정 (이동, 회전, 크기...)
        mesh->rotation.y += 0.5f * delta_time;

        process_graphics_pipeline(mesh);
    }

    update_next_mesh();
    update_camera();
}

void process_graphics_pipeline(mesh_t* mesh) {
    mat4_t scale_mat = mat4_make_scale(mesh->scale.x, mesh->scale.y, mesh->scale.z);
    mat4_t rotx_mat = mat4_make_rotation_x(mesh->rotation.x);
    mat4_t roty_mat = mat4_make_rotation_y(mesh->rotation.y);
    mat4_t rotz_mat = mat4_make_rotation_z(mesh->rotation.z);
    mat4_t trans_mat = mat4_make_translation(mesh->translation.x, mesh->translation.y, mesh->translation.z);

    int len_faces = array_length(mesh->faces);
    for(int face_idx = 0; face_idx < len_faces; face_idx++) {
        face_t mesh_face = mesh->faces[face_idx];

        vec3_t vertices[3];
        vertices[0] = mesh->vertices[mesh_face.a];
        vertices[1] = mesh->vertices[mesh_face.b];
        vertices[2] = mesh->vertices[mesh_face.c];

        vec4_t transformed_vertices[3];
        for(int i = 0; i < 3; i++) {
            vec4_t transformed_vertex = vec4_from_vec3(vertices[i]);

            // world matrix 적용
            world_mat = mat4_identity();
            world_mat = mat4_mul_mat4(world_mat, trans_mat);
            world_mat = mat4_mul_mat4(world_mat, rotz_mat);
            world_mat = mat4_mul_mat4(world_mat, roty_mat);
            world_mat = mat4_mul_mat4(world_mat, rotx_mat);
            world_mat = mat4_mul_mat4(world_mat, scale_mat);

            transformed_vertex = mat4_mul_vec4(world_mat, transformed_vertex);

            //view matrix 적용
            transformed_vertex = mat4_mul_vec4(view_mat, transformed_vertex);

            transformed_vertices[i] = transformed_vertex;
        }

        vec3_t face_normal = get_triangle_normal(transformed_vertices);

        // back face culling
        if(is_set_render_mod(RENDER_MOD_BACKFACE)) {
            vec3_t to_camera = vec3_sub(vec3_new(0, 0, 0), vec3_from_vec4(transformed_vertices[0]));
            if(vec3_dot(face_normal, to_camera) < 0) {
                continue;
            }
        }

        // clipping
        polygon_t polygon = create_polygon_from_triangle(
            vec3_from_vec4(transformed_vertices[0]),
            vec3_from_vec4(transformed_vertices[1]),
            vec3_from_vec4(transformed_vertices[2]),
            mesh_face.a_uv,
            mesh_face.b_uv,
            mesh_face.c_uv
        );
        clip_polygon(&polygon);
        triangle_t triangles_after_clipping[MAX_NUM_POLY_TRIANGLES];
        int num_triangles_after_clipping = 0;
        triangles_from_polygon(&polygon, triangles_after_clipping, &num_triangles_after_clipping);

        for(int triangle_idx = 0; triangle_idx < num_triangles_after_clipping; triangle_idx++) {
            triangle_t triangle = triangles_after_clipping[triangle_idx];

            vec4_t projected_points[3];
            for(int i = 0; i < 3; i++) {
                projected_points[i] = mat4_mul_projection_vec4(perspective_mat, triangle.points[i]);

                projected_points[i].x *= get_window_width() / 2.0;
                projected_points[i].y *= get_window_height() / 2.0;
                projected_points[i].y *= -1.0;

                projected_points[i].x += get_window_width() / 2.0;
                projected_points[i].y += get_window_height() / 2.0;
            }

            // calculate light intensity and apply
            uint32_t triangle_color = mesh_face.color;
            float light_factor = -vec3_dot(get_global_light_dir(), face_normal);
            triangle_color = apply_light_intensity(triangle_color, light_factor);

            triangle_t triangle_to_render = {
                .points = {
                    { projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
                    { projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w  },
                    { projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w  }
                },
                .texcoords = {
                    { triangle.texcoords[0].u, triangle.texcoords[0].v },
                    { triangle.texcoords[1].u, triangle.texcoords[1].v },
                    { triangle.texcoords[2].u, triangle.texcoords[2].v },
                },
                .color = triangle_color,
                .texture = &mesh->texture
            };

            if(num_triangles_to_render < MAX_TRIANGLES_PER_MESH) {
                triangles_to_render[num_triangles_to_render++] = triangle_to_render;
            }
        }
    }
}

void render() {
    clear_color_buffer(0x00000000);
    clear_z_buffer();

    draw_grid(0xFFFFFFFF, 10, 10);

    for(int i = 0; i < num_triangles_to_render; i++) {
        triangle_t triangle = triangles_to_render[i];

        if(is_set_render_mod(RENDER_MOD_TEXTURED) && triangle.texture) {
            draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v,
                triangle.texture
            );
        } else if(is_set_render_mod(RENDER_MOD_SOLID)) {
            draw_filled_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,
                triangle.color
            );
        }

        if(is_set_render_mod(RENDER_MOD_WIREFRAME)) {
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
        
        if(is_set_render_mod(RENDER_MOD_VERTEX)) {
            const float hRtSize = 1.f;
            for(int j = 0; j < 3; j++) {
                draw_fill_rect(
                    triangle.points[j].x - hRtSize,
                    triangle.points[j].y - hRtSize,
                    hRtSize * 2.f,
                    hRtSize * 2.f,
                    0xFFFFFF00
                );
            }
        }
    }

    render_color_buffer();
}


