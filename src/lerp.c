#include "lerp.h"

float float_lerp(float a, float b, float t) {
    return a + t * (b - a);
}