#include "camera.h"

static camera_t camera;

void init_camera(vec3_t pos) {
    camera.position = pos;
    camera.direction = vec3_new(0, 0, 1);
    camera.rotation = vec2_new(0, 0);
}

vec3_t get_camera_dir() {
    return camera.direction;
}

void update_camera_pos(vec3_t pos) {
    camera.position = pos;
}

vec3_t get_camera_pos() {
    return camera.position;
}

void update_camera_rot(vec2_t rot) {
    camera.rotation = rot;
    camera.direction = vec3_new(0, 0, 1);
    camera.direction = vec3_rotate_x(camera.direction, camera.rotation.x);
    camera.direction = vec3_rotate_y(camera.direction, camera.rotation.y);
}

vec2_t get_camera_rot() {
    return camera.rotation;
}