#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

typedef struct {
    float m[4][4];
} mat4_t;

mat4_t mat4_identity();
mat4_t mat4_make_scale(float sx, float sy, float sz);
mat4_t mat4_make_rotation_x(float rot);
mat4_t mat4_make_rotation_y(float rot);
mat4_t mat4_make_rotation_z(float rot);
mat4_t mat4_make_translation(float tx, float ty, float tz);
mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar);
vec4_t mat4_mul_projection_vec4(mat4_t mat, vec4_t v);
mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up);
vec4_t mat4_mul_vec4(mat4_t m, vec4_t v);
mat4_t mat4_mul_mat4(mat4_t m1, mat4_t m2);

#endif