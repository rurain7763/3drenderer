#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"

typedef struct {
    vec3_t position;
    vec3_t direction;
    float pitch, yaw;
} camera_t;

extern camera_t camera;

#endif