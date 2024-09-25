
#include "clipping.h"
#include "lerp.h"
#include <math.h>

plane_t frustum_planes[FRUSTUM_PLANE_COUNT];

void init_frustum_planes(float fov_x, float fov_y, float z_near, float z_far) {
    const float cos_half_fov_x = cos(fov_x / 2);
    const float sin_half_fov_x = sin(fov_x / 2);
	const float cos_half_fov_y = cos(fov_y / 2);
	const float sin_half_fov_y = sin(fov_y / 2);

	frustum_planes[LEFT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[LEFT_FRUSTUM_PLANE].normal.x = cos_half_fov_x;
	frustum_planes[LEFT_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[LEFT_FRUSTUM_PLANE].normal.z = sin_half_fov_x;

	frustum_planes[RIGHT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fov_x;
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fov_x;

	frustum_planes[TOP_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[TOP_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[TOP_FRUSTUM_PLANE].normal.y = -cos_half_fov_y;
	frustum_planes[TOP_FRUSTUM_PLANE].normal.z = sin_half_fov_y;

	frustum_planes[BOTTOM_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fov_y;
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fov_y;

	frustum_planes[NEAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_near);
	frustum_planes[NEAR_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[NEAR_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[NEAR_FRUSTUM_PLANE].normal.z = 1;

	frustum_planes[FAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_far);
	frustum_planes[FAR_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[FAR_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[FAR_FRUSTUM_PLANE].normal.z = -1;
}

polygon_t create_polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2, tex2_t uv0, tex2_t uv1, tex2_t uv2) {
    polygon_t ret = {
        .vertices = { v0, v1, v2 },
        .texcoords = { uv0, uv1, uv2 },
        .num_vertices = 3
    };
    return ret;
}

void clip_polygon_against_plane(polygon_t* polygon, int plane) {
    vec3_t plane_point = frustum_planes[plane].point;
    vec3_t plane_normal = frustum_planes[plane].normal;

    vec3_t inside_vertex[MAX_NUM_POLY_VERTICES];
    tex2_t inside_uvs[MAX_NUM_POLY_VERTICES];
    int num_inside_vertex = 0;

    vec3_t* prev_vertex = &polygon->vertices[polygon->num_vertices - 1];
    tex2_t* prev_uv = &polygon->texcoords[polygon->num_vertices - 1];
    float prev_dot = vec3_dot(vec3_sub(*prev_vertex, plane_point), plane_normal);

    for(int i = 0; i < polygon->num_vertices; i++) {
        vec3_t* cur_vertex = &polygon->vertices[i];
        tex2_t* cur_uv = &polygon->texcoords[i];
        float cur_dot = vec3_dot(vec3_sub(*cur_vertex, plane_point), plane_normal);

        // 평면을 기준으로 두 점이 다른 쪽에 위치하는 경우 교차점 계산
        if(cur_dot * prev_dot < 0) {
            // linear fomula(i = q1 + t(q2 - q1))를 이용하여 교차 점 구하기 (q1 == cur_vertex, q2 == prev_vertex)
            // t를 구하는 상세 내용은 따로 정리해둔 문서 참고 (3drenderer/clipping)
            float t = prev_dot / (prev_dot - cur_dot);
            vec3_t insertion_vertex = {
                .x = float_lerp(prev_vertex->x, cur_vertex->x, t),
                .y = float_lerp(prev_vertex->y, cur_vertex->y, t),
                .z = float_lerp(prev_vertex->z, cur_vertex->z, t)
            };
            tex2_t insertion_uv = {
                .u = float_lerp(prev_uv->u, cur_uv->u, t),
                .v = float_lerp(prev_uv->v, cur_uv->v, t),
            };
            inside_vertex[num_inside_vertex] = insertion_vertex;
            inside_uvs[num_inside_vertex] = insertion_uv;
            num_inside_vertex++;
        }

        // 현재 점이 평면의 안쪽에 있으면 추가
        if(cur_dot > 0) {
            inside_vertex[num_inside_vertex] = *cur_vertex;
            inside_uvs[num_inside_vertex] = *cur_uv;
            num_inside_vertex++;
        }

        prev_vertex = cur_vertex;
        prev_uv = cur_uv;
        prev_dot = cur_dot; 
    }

    // 결과물 복사
    for(int i = 0; i < num_inside_vertex; i++) {
        polygon->vertices[i] = inside_vertex[i];
        polygon->texcoords[i] = inside_uvs[i];
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

void triangles_from_polygon(polygon_t* polygon, triangle_t* triangles, int* num_triangles) {
    int i;
    for(i = 0; i < polygon->num_vertices - 2; i++) {
        int index0 = 0;
        int index1 = i + 1;
        int index2 = i + 2;

        triangles[i].points[0] = vec4_from_vec3(polygon->vertices[index0]);
        triangles[i].points[1] = vec4_from_vec3(polygon->vertices[index1]);
        triangles[i].points[2] = vec4_from_vec3(polygon->vertices[index2]);
        triangles[i].texcoords[0] = polygon->texcoords[index0];
        triangles[i].texcoords[1] = polygon->texcoords[index1];
        triangles[i].texcoords[2] = polygon->texcoords[index2];
    }
    *num_triangles = i;
}