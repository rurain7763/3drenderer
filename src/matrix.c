#include "matrix.h"
#include <math.h>

mat4_t mat4_identity() {
    mat4_t ret = {{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};
    return ret;
}

mat4_t mat4_make_scale(float sx, float sy, float sz) {
    mat4_t ret = mat4_identity();
    ret.m[0][0] = sx;
    ret.m[1][1] = sy;
    ret.m[2][2] = sz;
    return ret;
}

mat4_t mat4_make_rotation_x(float rot) {
    float c = cos(rot);
    float s = sin(rot);
    mat4_t ret = mat4_identity();
    ret.m[1][1] = c;
    ret.m[1][2] = -s;
    ret.m[2][1] = s;
    ret.m[2][2] = c;
    return ret;
}

mat4_t mat4_make_rotation_y(float rot) {
    float c = cos(rot);
    float s = sin(rot);
    mat4_t ret = mat4_identity();
    ret.m[0][0] = c;
    ret.m[0][2] = s;
    ret.m[2][0] = -s;
    ret.m[2][2] = c;
    return ret;
}

mat4_t mat4_make_rotation_z(float rot) {
    float c = cos(rot);
    float s = sin(rot);
    mat4_t ret = mat4_identity();
    ret.m[0][0] = c;
    ret.m[0][1] = -s;
    ret.m[1][0] = s;
    ret.m[1][1] = c;
    return ret;
}

mat4_t mat4_make_translation(float tx, float ty, float tz) {
    mat4_t ret = mat4_identity();
    ret.m[0][3] = tx;
    ret.m[1][3] = ty;
    ret.m[2][3] = tz;
    return ret;
}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v) {
    vec4_t ret = {
        .x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w,
        .y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w,
        .z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w,
        .w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w,
    };
    return ret;
}