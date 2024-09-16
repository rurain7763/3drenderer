#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

typedef struct {
    vec3_t* vertices; // dynamic array of vertices
    face_t* faces;    // dynamic array of faces
    vec3_t rotation;
} mesh_t;

extern mesh_t mesh;

void load_obj_file(const char* filename);

#endif