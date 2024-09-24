#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

typedef struct {
    float u, v;
} tex2_t;

extern uint32_t* mesh_texture;
extern int texture_width;
extern int texture_height;

extern const uint8_t REDBRICK_TEXTURE[];

void load_png_texture(const char* fileName);

#endif