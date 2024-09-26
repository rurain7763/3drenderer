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

#define MAX_TRIANGLES_PER_MESH 10000

triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
int num_triangles_to_render = 0;

mat4_t perspective_mat;
mat4_t view_mat;
mat4_t world_mat;

bool is_running = false;
int previous_frame_time = 0;
float delta_time = 0;

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
    const float aspect_y = get_window_height() / (float)get_window_width();
    const float aspect_x = get_window_width() / (float)get_window_height();
    const float fov_y = M_PI / 3.0; // 60 degree
    const float fov_x = 2 * atan(tan(fov_y / 2) * aspect_x); // 60 degree
    const float z_near = 0.1;
    const float z_far = 100.0;
    perspective_mat = mat4_make_perspective(fov_y, aspect_y, z_near, z_far);
    init_frustum_planes(fov_x, fov_y, z_near, z_far);

    init_global_light(vec3_new(0, 0, 1));

    load_obj_file("./assets/f117.obj");
    load_png_texture("./assets/f117.png");
    if(mesh_texture) {
        set_render_mod(RENDER_MOD_TEXTURED, true);
    }
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
                switch_render_mod(RENDER_MOD_BACKFACE);
            } else if(evn.key.keysym.sym == SDLK_5) {
                if(mesh_texture) {
                    switch_render_mod(RENDER_MOD_TEXTURED);
                }
            } else if(evn.key.keysym.sym == SDLK_DOWN) {
                camera.pitch += 5.f * delta_time;
            } else if(evn.key.keysym.sym == SDLK_UP) {
                camera.pitch -= 5.f * delta_time;
            } else if(evn.key.keysym.sym == SDLK_LEFT) {
                camera.yaw += 5.f * delta_time;
            } else if(evn.key.keysym.sym == SDLK_RIGHT) {
                camera.yaw -= 5.f * delta_time;
            } else if(evn.key.keysym.sym == SDLK_a) {
                vec3_t right = vec3_cross(camera.direction, vec3_new(0, 1, 0));
                camera.position = vec3_add(camera.position, vec3_mul(right, 50 * delta_time));
            } else if(evn.key.keysym.sym == SDLK_d) {
                vec3_t right = vec3_cross(camera.direction, vec3_new(0, 1, 0));
                camera.position = vec3_sub(camera.position, vec3_mul(right, 50 * delta_time));
            } else if(evn.key.keysym.sym == SDLK_w) {
                vec3_t velocity = vec3_mul(camera.direction, 50.0 * delta_time);
                camera.position = vec3_add(camera.position, velocity);
            } else if(evn.key.keysym.sym == SDLK_s) {
                vec3_t velocity = vec3_mul(camera.direction, 50.0 * delta_time);
                camera.position = vec3_sub(camera.position, velocity);
            }
            break;
        }
    }
}

void update() {
    int wait_time = previous_frame_time + FRAME_TARGET_TIME - SDL_GetTicks();
    if(wait_time > 0) {
        SDL_Delay(wait_time);
    }
    delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.f;
    previous_frame_time = SDL_GetTicks();

    //mesh.rotation.x += 1 * delta_time;
    //mesh.rotation.y += 1 * delta_time;
    //mesh.rotation.z += 1 * delta_time;
    mesh.translation.z = 5.0;

    num_triangles_to_render = 0;

    vec3_t up = {0, 1, 0};
    camera.direction = vec3_new(0, 0, 1);
    camera.direction = vec3_rotate_x(camera.direction, camera.pitch);
    camera.direction = vec3_rotate_y(camera.direction, camera.yaw);
    vec3_t target = vec3_add(camera.position, camera.direction);
    view_mat = mat4_look_at(camera.position, target, up);

    mat4_t scale_mat = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t rotx_mat = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t roty_mat = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotz_mat = mat4_make_rotation_z(mesh.rotation.z);
    mat4_t trans_mat = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);

    int len_faces = array_length(mesh.faces);
    for(int i = 0; i < len_faces; i++) {
        face_t mesh_face = mesh.faces[i];

        vec3_t vertices[3];
        vertices[0] = mesh.vertices[mesh_face.a];
        vertices[1] = mesh.vertices[mesh_face.b];
        vertices[2] = mesh.vertices[mesh_face.c];

        vec4_t transformed_vertices[3];
        for(int j = 0; j < 3; j++) {
            vec4_t transformed_vertex = vec4_from_vec3(vertices[j]);

            world_mat = mat4_identity();

            world_mat = mat4_mul_mat4(world_mat, trans_mat);
            world_mat = mat4_mul_mat4(world_mat, rotz_mat);
            world_mat = mat4_mul_mat4(world_mat, roty_mat);
            world_mat = mat4_mul_mat4(world_mat, rotx_mat);
            world_mat = mat4_mul_mat4(world_mat, scale_mat);

            transformed_vertex = mat4_mul_vec4(world_mat, transformed_vertex);
            transformed_vertex = mat4_mul_vec4(view_mat, transformed_vertex);

            transformed_vertices[j] = transformed_vertex;
        }

        vec3_t a = vec3_from_vec4(transformed_vertices[0]);
        vec3_t b = vec3_from_vec4(transformed_vertices[1]);
        vec3_t c = vec3_from_vec4(transformed_vertices[2]);

        vec3_t ab = vec3_sub(b, a);
        vec3_t ac = vec3_sub(c, a);
        vec3_normalize(&ab);
        vec3_normalize(&ac);

        vec3_t normal = vec3_cross(ab, ac);
        vec3_normalize(&normal);

        // back face culling
        if(is_set_render_mod(RENDER_MOD_BACKFACE)) {
            vec3_t origin = {0, 0, 0};
            vec3_t to_camera = vec3_sub(origin, a);
            if(vec3_dot(normal, to_camera) < 0) {
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

        for(int i = 0; i < num_triangles_after_clipping; i++) {
            triangle_t triangle = triangles_after_clipping[i];

            vec4_t projected_points[3];
            for(int j = 0; j < 3; j++) {
                projected_points[j] = mat4_mul_projection_vec4(perspective_mat, triangle.points[j]);

                projected_points[j].x *= get_window_width() / 2.0;
                projected_points[j].y *= get_window_height() / 2.0;
                projected_points[j].y *= -1.0;

                projected_points[j].x += get_window_width() / 2.0;
                projected_points[j].y += get_window_height() / 2.0;
            }

            // calculate light intensity and apply
            uint32_t triangle_color = mesh_face.color;
            float light_factor = -vec3_dot(get_global_light_dir(), normal);
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

        if(is_set_render_mod(RENDER_MOD_TEXTURED)) {
            draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v
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

    render_color_buffer();
}

void destroy_resources() {
    free(mesh_texture);
    array_free(mesh.faces);
    array_free(mesh.vertices);
}

