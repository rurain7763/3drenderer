
#include "clipping.h"
#include <math.h>

plane_t frustum_planes[FRUSTUM_PLANE_COUNT];

void init_frustum_planes(float fov, float z_near, float z_far) {
	float cos_half_fov = cos(fov / 2);
	float sin_half_fov = sin(fov / 2);

	frustum_planes[LEFT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[LEFT_FRUSTUM_PLANE].normal.x = cos_half_fov;
	frustum_planes[LEFT_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[LEFT_FRUSTUM_PLANE].normal.z = sin_half_fov;

	frustum_planes[RIGHT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fov;
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fov;

	frustum_planes[TOP_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[TOP_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[TOP_FRUSTUM_PLANE].normal.y = -cos_half_fov;
	frustum_planes[TOP_FRUSTUM_PLANE].normal.z = sin_half_fov;

	frustum_planes[BOTTOM_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fov;
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fov;

	frustum_planes[NEAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_near);
	frustum_planes[NEAR_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[NEAR_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[NEAR_FRUSTUM_PLANE].normal.z = 1;

	frustum_planes[FAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_far);
	frustum_planes[FAR_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[FAR_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[FAR_FRUSTUM_PLANE].normal.z = -1;
}

polygon_t create_polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2) {
    polygon_t ret = {
        .vertices = { v0, v1, v2 },
        .num_vertices = 3
    };
    return ret;
}

void clip_polygon_against_plane(polygon_t* polygon, int plane) {
    vec3_t plane_point = frustum_planes[plane].point;
    vec3_t plane_normal = frustum_planes[plane].normal;

    vec3_t inside_vertex[MAX_NUM_POLY_VERTICES];
    int num_inside_vertex = 0;

    vec3_t* prev_vertex = &polygon->vertices[polygon->num_vertices - 1];
    float prev_dot = vec3_dot(vec3_sub(*prev_vertex, plane_point), plane_normal);

    for(int i = 0; i < polygon->num_vertices; i++) {
        vec3_t* cur_vertex = &polygon->vertices[i];
        float cur_dot = vec3_dot(vec3_sub(*cur_vertex, plane_point), plane_normal);

        // 평면을 기준으로 두 점이 다른 쪽에 위치하는 경우 교차점 계산
        if(cur_dot * prev_dot < 0) {
            // linear fomula(i = q1 + t(q2 - q1))를 이용하여 교차 점 구하기 (q1 == cur_vertex, q2 == prev_vertex)
            // t를 구하는 상세 내용은 따로 정리해둔 문서 참고 (3drenderer/clipping)
            float t = prev_dot / (prev_dot - cur_dot);
            vec3_t insertion_vertex = vec3_add(*prev_vertex, vec3_mul(vec3_sub(*cur_vertex, *prev_vertex), t));
            inside_vertex[num_inside_vertex++] = insertion_vertex;
        }

        // 현재 점이 평면의 안쪽에 있으면 추가
        if(cur_dot > 0) {
            inside_vertex[num_inside_vertex++] = *cur_vertex;
        }

        prev_vertex = cur_vertex;
        prev_dot = cur_dot; 
    }

    // 결과물 복사
    for(int i = 0; i < num_inside_vertex; i++) {
        polygon->vertices[i] = inside_vertex[i];
    }
    polygon->num_vertices = num_inside_vertex;
}

void clip_polygon(polygon_t* polygon) {
    clip_polygon_against_plane(polygon, LEFT_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, RIGHT_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, TOP_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, BOTTOM_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, NEAR_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, FAR_FRUSTUM_PLANE);
}