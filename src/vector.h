#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    float x, y;
} vec2_t;

typedef struct {
    float x, y, z;
} vec3_t;

vec3_t vec3_roate_x(vec3_t point, float rot);
vec3_t vec3_roate_y(vec3_t point, float rot);
vec3_t vec3_roate_z(vec3_t point, float rot);

#endif