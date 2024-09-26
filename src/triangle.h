#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector.h"
#include "texture.h"
#include <stdint.h>

typedef struct {
    int a, b, c;
    tex2_t a_uv, b_uv, c_uv;
    uint32_t color;
} face_t;

typedef struct {
    vec4_t points[3];
    tex2_t texcoords[3];
    uint32_t color;
    texture_t* texture;
} triangle_t;

void draw_filled_triangle(
    int x0, int y0, float z0, float w0, 
    int x1, int y1, float z1, float w1,
    int x2, int y2, float z2, float w2,
    uint32_t color);
    
void draw_textured_triangle(
    int x0, int y0, float z0, float w0, float u0, float v0, 
    int x1, int y1, float z1, float w1, float u1, float v1,
    int x2, int y2, float z2, float w2, float u2, float v2,
    texture_t* texture
);

#endif