#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"

typedef struct {
    vec3_t position;
    vec3_t direction;
    vec2_t rotation;
} camera_t;

void init_camera(vec3_t pos);
vec3_t get_camera_dir();
void update_camera_pos(vec3_t pos);
vec3_t get_camera_pos();
void update_camera_rot(vec2_t rot);
vec2_t get_camera_rot();

#endif