#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

typedef struct {
    float u, v;
} tex2_t;

typedef struct {
    uint32_t* buffer;
    int width, height;
} texture_t;

#endif