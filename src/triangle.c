#include "triangle.h"
#include "display.h"
#include "swap.h"

void draw_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    // 기울기를 y값 증가에 따른 x값으로 구함
    float inv_slop_1 = (x1 - x0) / (float)(y1 - y0);
    float inv_slop_2 = (x2 - x0) / (float)(y2 - y0);

    float start_x = x0, end_x = x0;
    for(int i = y0; i <= y2; i++) {
        draw_line(start_x, i, end_x, i, color);
        start_x += inv_slop_1;
        end_x += inv_slop_2;
    }
}

void draw_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    // 기울기를 y값 증가에 따른 x값으로 구함
    float inv_slop_1 = (x2 - x0) / (float)(y2 - y0);
    float inv_slop_2 = (x2 - x1) / (float)(y2 - y1);

    float start_x = x2, end_x = x2;
    for(int i = y2; i >= y1; i--) {
        draw_line(start_x, i, end_x, i, color);
        start_x -= inv_slop_1;
        end_x -= inv_slop_2;
    }
}

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    // y값이 큰 순서대로 정렬
    if(y0 > y1) {
        int_swap(&x0, &x1);
        int_swap(&y0, &y1);
    }

    if(y1 > y2) {
        int_swap(&x1, &x2);
        int_swap(&y1, &y2);
    }

    if(y0 > y1) {
        int_swap(&x0, &x1);
        int_swap(&y0, &y1);
    }

    if(y1 == y2) {
        // 위쪽 삼각형만 그리기
        draw_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
    } else if(y0 == y1) {
        // 아래쪽 삼각형만 그리기
        draw_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
    } else {
        // midpoint 찾기
        int my = y1;
        int mx = ((x2 - x0) * (y1 - y0)) / (float)(y2 - y0) + x0;

        // flat_bottom 삼각형 그리기
        draw_flat_bottom_triangle(x0, y0, x1, y1, mx, my, color);
        // flat_top 삼각형 그리기
        draw_flat_top_triangle(x1, y1, mx, my, x2, y2, color);
    }
}

vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p) {
    vec2_t ac = vec2_sub(c, a);
    vec2_t ab = vec2_sub(b, a);
    vec2_t ap = vec2_sub(p, a);
    vec2_t pc = vec2_sub(c, p);
    vec2_t pb = vec2_sub(b, p);

    // ac x ab 전체 삼각형 크기. 아래는 2d의 외적을 표현. z값의 크기가 나옴
    float total_area = ac.x * ab.y - ac.y * ab.x;
    vec3_t ret = {
        .x = (pc.x * pb.y - pc.y * pb.x) / total_area,
        .y = (ac.x * ap.y - ac.y * ap.x) / total_area
    };
    ret.z = 1.0 - ret.x - ret.y;
    
    return ret;
}

void draw_texel(
    int x, int y,
    vec2_t a, vec2_t b, vec2_t c, 
    float u0, float v0, float u1, float v1, float u2, float v2
) {
    vec2_t p = {x, y};
    vec3_t weights = barycentric_weights(a, b, c, p);

    float u = u0 * weights.x + u1 * weights.y + u2 * weights.z;
    float v = v0 * weights.x + v1 * weights.y + v2 * weights.z;

    int tex_x = texture_width * u;
    if(tex_x < 0) tex_x = 0;
    if(tex_x >= texture_width) tex_x = texture_width - 1;

    int tex_y = texture_height * v;
    if(tex_y < 0) tex_y = 0;
    if(tex_y >= texture_height) tex_y = texture_height - 1;

    draw_pixel(x, y, mesh_texture[texture_width * tex_y + tex_x]);
}

void draw_textured_triangle(
    int x0, int y0, float u0, float v0, 
    int x1, int y1, float u1, float v1,
    int x2, int y2, float u2, float v2,
    uint32_t* texture
) {
    // y값이 큰 순서대로 정렬
    if(y0 > y1) {
        int_swap(&x0, &x1);
        int_swap(&y0, &y1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }

    if(y1 > y2) {
        int_swap(&x1, &x2);
        int_swap(&y1, &y2);
        float_swap(&u1, &u2);
        float_swap(&v1, &v2);
    }

    if(y0 > y1) {
        int_swap(&x0, &x1);
        int_swap(&y0, &y1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }

    vec2_t a = { x0, y0 };
    vec2_t b = { x1, y1 };
    vec2_t c = { x2, y2 };

    float inv_slope_1 = y1 != y0 ? (x1 - x0) / (float)(y1 - y0) : 0.f;
    float inv_slope_2 = y2 != y0 ? (x2 - x0) / (float)(y2 - y0) : 0.f;

    if(y1 != y0) {
        for (int y = y0; y <= y1; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end);
            }

            for (int x = x_start; x < x_end; x++) {
                draw_texel(x, y, a, b, c, u0, v0, u1, v1, u2, v2);
            }
        }
    }

    inv_slope_1 = y2 != y1 ? (x2 - x1) / (float)(y2 - y1) : 0.f;
    inv_slope_2 = y2 != y0 ? (x2 - x0) / (float)(y2 - y0) : 0.f;
    if(y2 != y1) {
        for (int y = y1 + 1; y <= y2; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end);
            }

            for (int x = x_start; x < x_end; x++) {
                draw_texel(x, y, a, b, c, u0, v0, u1, v1, u2, v2);
            }
        }
    }
}