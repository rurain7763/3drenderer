#include "light.h"

light_t global_light = {
    .direction = {0, 0, 1}
};

uint32_t apply_light_intensity(uint32_t origin, float factor) {
    if(factor < 0) factor = 0;
    else if(factor > 1) factor = 1;

    uint32_t a = (origin & 0xFF000000);
    
    uint32_t r = (origin & 0x00FF0000) >> 16;
    uint32_t g = (origin & 0x0000FF00) >> 8;
    uint32_t b = (origin & 0x000000FF);
    
    r = r * factor;
    g = g * factor;
    b = b * factor;

    return a | (r << 16) | (g << 8) | b;
}