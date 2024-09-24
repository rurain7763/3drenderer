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
    vec4_t a, vec4_t b, vec4_t c, 
    tex2_t a_uv, tex2_t b_uv, tex2_t c_uv
) {
    vec2_t p = {x, y};
    vec2_t _a = vec2_from_vec4(a);
    vec2_t _b = vec2_from_vec4(b);
    vec2_t _c = vec2_from_vec4(c);

    vec3_t weights = barycentric_weights(_a, _b, _c, p);

    float u = (a_uv.u / a.w) * weights.x + (b_uv.u / b.w) * weights.y + (c_uv.u / c.w) * weights.z;
    float v = (a_uv.v / a.w) * weights.x + (b_uv.v / b.w) * weights.y + (c_uv.v / c.w) * weights.z;

    float inv_w = (1 / a.w) * weights.x + (1 / b.w) * weights.y + (1 / c.w) * weights.z;

    u /= inv_w;
    v /= inv_w;

    int tex_x = abs((int)(texture_width * u)) % texture_width;
    int tex_y = abs((int)(texture_height * v)) % texture_height;

    draw_pixel(x, y, mesh_texture[texture_width * tex_y + tex_x]);
}

void draw_textured_triangle(
    int x0, int y0, float z0, float w0, float u0, float v0, 
    int x1, int y1, float z1, float w1, float u1, float v1,
    int x2, int y2, float z2, float w2, float u2, float v2
) {
    // y값이 큰 순서대로 정렬
    if(y0 > y1) {
        int_swap(&x0, &x1);
        int_swap(&y0, &y1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }

    if(y1 > y2) {
        int_swap(&x1, &x2);
        int_swap(&y1, &y2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
        float_swap(&u1, &u2);
        float_swap(&v1, &v2);
    }

    if(y0 > y1) {
        int_swap(&x0, &x1);
        int_swap(&y0, &y1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }

    // v값 반전
    v0 = 1.f - v0;
    v1 = 1.f - v1;
    v2 = 1.f - v2;

    vec4_t a = { x0, y0, z0, w0 };
    vec4_t b = { x1, y1, z1, w1 };
    vec4_t c = { x2, y2, z2, w2 };

    tex2_t a_uv = { u0, v0 };
    tex2_t b_uv = { u1, v1 };
    tex2_t c_uv = { u2, v2 };

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
                draw_texel(x, y, a, b, c, a_uv, b_uv, c_uv);
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
                draw_texel(x, y, a, b, c, a_uv, b_uv, c_uv);
            }
        }
    }
}