#ifndef LIGHT_H
#define LIGHT_H

#include "vector.h"
#include "stdint.h"

typedef struct {
    vec3_t direction;
} light_t;

void init_global_light(vec3_t direction);
vec3_t get_global_light_dir();

uint32_t apply_light_intensity(uint32_t origin, float factor);

#endif