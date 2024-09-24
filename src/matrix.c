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

mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar) {
    mat4_t ret = {{
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }};

    float fov_factor = 1 / tan(fov / 2.0);
    float lambda = zfar / (zfar - znear);
    ret.m[0][0] = aspect * fov_factor;
    ret.m[1][1] = fov_factor;
    ret.m[2][2] = lambda;
    ret.m[2][3] = -lambda * znear;
    ret.m[3][2] = 1.0;

    return ret;
}

mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up) {
    vec3_t z = vec3_sub(target, eye);
    vec3_normalize(&z);
    vec3_t x = vec3_cross(up, z);
    vec3_normalize(&x);
    vec3_t y = vec3_cross(z, x);

    mat4_t ret = {{
        { x.x, x.y, x.z, -vec3_dot(x, eye) },
        { y.x, y.y, y.z, -vec3_dot(y, eye) },
        { z.x, z.y, z.z, -vec3_dot(z, eye) },
        {   0,   0,   0,                 1 }
    }};
    return ret;
}

vec4_t mat4_mul_projection_vec4(mat4_t mat, vec4_t v) {
    // projection matrix를 곱하면 perspective matrix의 경우 w에 z값이 저장된다.
    vec4_t ret = mat4_mul_vec4(mat, v);

    //TODO:

    // perspective divide를 실행한다. (origin z값으로 나눈다)
    if(ret.w != 0) {
        ret.x /= ret.w;
        ret.y /= ret.w;
        ret.z /= ret.z; 
    }

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

mat4_t mat4_mul_mat4(mat4_t m1, mat4_t m2) {
    mat4_t ret;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            ret.m[i][j] = 0;
            for(int k = 0; k < 4; k++) {
                ret.m[i][j] += m1.m[i][k] * m2.m[k][j];
            }
        }
    }
    return ret;
}