#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

typedef struct {
    vec3_t* vertices; // dynamic array of vertices
    face_t* faces;    // dynamic array of faces
    vec3_t scale;
    vec3_t rotation;
    vec3_t translation;
} mesh_t;

extern mesh_t mesh;

#define N_CUBE_VERTICES 8
#define N_CUBE_FACES (6 * 2) // 6 cube faces, 2 triangles per face

extern vec3_t cube_vertices[N_CUBE_VERTICES];
extern face_t cube_faces[N_CUBE_FACES];

void load_cube_mesh_data();
void load_obj_file(const char* filename);

#endif