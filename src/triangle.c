#include "triangle.h"
#include "display.h"

void int_swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

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