#include "vector.h"
#include <math.h>

// vec2 methods
vec2_t vec2_new(float x, float y) {
    vec2_t ret = {
        .x = x,
        .y = y    
    };
    return ret;
}

float vec2_length(vec2_t v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

vec2_t vec2_add(vec2_t a, vec2_t b) {
    vec2_t ret = {
        .x = a.x + b.x,
        .y = a.y + b.y
    };
    return ret;
}

vec2_t vec2_sub(vec2_t a, vec2_t b) {
    vec2_t ret = {
        .x = a.x - b.x,
        .y = a.y - b.y
    };
    return ret;
}

vec2_t vec2_mul(vec2_t v, float factor) {
    vec2_t ret = {
        .x = v.x * factor,
        .y = v.y * factor,
    };
    return ret;
}

vec2_t vec2_div(vec2_t v, float factor) {
    vec2_t ret = {
        .x = v.x / factor,
        .y = v.y / factor
    };
    return ret;
}

float vec2_dot(vec2_t a, vec2_t b) {
    return a.x * b.x + a.y * b.y;
}

void vec2_normalize(vec2_t* v) {
    float len = sqrt(v->x * v->x + v->y * v-> y);
    v->x /= len;
    v->y /= len;
}

vec2_t vec2_from_vec4(vec4_t v) {
    vec2_t ret = {
        .x = v.x,
        .y = v.y
    };
    return ret;
}

// vec3 methods
vec3_t vec3_new(float x, float y, float z) {
    vec3_t ret = {x, y, z};
    return ret;
}

float vec3_length(vec3_t v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3_t vec3_add(vec3_t a, vec3_t b) {
    vec3_t ret = {
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z
    };
    return ret;
}

vec3_t vec3_sub(vec3_t a, vec3_t b) {
    vec3_t ret = {
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z
    };
    return ret;
}

vec3_t vec3_mul(vec3_t v, float factor) {
    vec3_t ret = {
        .x = v.x * factor,
        .y = v.y * factor,
        .z = v.z * factor
    };
    return ret;
}

vec3_t vec3_div(vec3_t v, float factor) {
    vec3_t ret = {
        .x = v.x / factor,
        .y = v.y / factor,
        .z = v.z / factor
    };
    return ret;
}

vec3_t vec3_cross(vec3_t a, vec3_t b) {
    vec3_t ret = {
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x
    };
    return ret;
}

float vec3_dot(vec3_t a, vec3_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

void vec3_normalize(vec3_t* v) {
    float len = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    v->x /= len;
    v->y /= len;
    v->z /= len;
}

vec3_t vec3_rotate_x(vec3_t v, float rot) {
    vec3_t rotated = {
        .x = v.x,
        .y = v.y * cos(rot) - v.z * sin(rot),
        .z = v.y * sin(rot) + v.z * cos(rot)
    };
    return rotated;
}

vec3_t vec3_rotate_y(vec3_t v, float rot) {
    vec3_t rotated = {
        .x = v.x * cos(rot) - v.z * sin(rot),
        .y = v.y,
        .z = v.x * sin(rot) + v.z * cos(rot)
    };
    return rotated;
}

vec3_t vec3_rotate_z(vec3_t v, float rot) {
    vec3_t rotated = {
        .x = v.x * cos(rot) - v.y * sin(rot),
        .y = v.x * sin(rot) + v.y * cos(rot),
        .z = v.z
    };
    return rotated;
}

vec3_t vec3_from_vec4(vec4_t v) {
    vec3_t ret = {
        .x = v.x,
        .y = v.y,
        .z = v.z
    };
    return ret;
}

// vec4 methods
vec4_t vec4_from_vec3(vec3_t v) {
    vec4_t ret = {
        .x = v.x,
        .y = v.y,
        .z = v.z,
        .w = 1
    };
    return ret;
}