#ifndef LIGHT_H
#define LIGHT_H

#include "vector.h"
#include "stdint.h"

typedef struct {
    vec3_t direction;
} light_t;

extern light_t global_light;

uint32_t apply_light_intensity(uint32_t origin, float factor);

#endif