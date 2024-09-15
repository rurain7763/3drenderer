#include "vector.h"
#include <math.h>

vec3_t vec3_roate_x(vec3_t point, float rot) {
    vec3_t rotated = {
        .x = point.x,
        .y = point.y * cos(rot) - point.z * sin(rot),
        .z = point.y * sin(rot) + point.z * cos(rot)
    };
    return rotated;
}

vec3_t vec3_roate_y(vec3_t point, float rot) {
    vec3_t rotated = {
        .x = point.x * cos(rot) - point.z * sin(rot),
        .y = point.y,
        .z = point.x * sin(rot) + point.z * cos(rot)
    };
    return rotated;
}

vec3_t vec3_roate_z(vec3_t point, float rot) {
    vec3_t rotated = {
        .x = point.x * cos(rot) - point.y * sin(rot),
        .y = point.x * sin(rot) + point.y * cos(rot),
        .z = point.z
    };
    return rotated;
}