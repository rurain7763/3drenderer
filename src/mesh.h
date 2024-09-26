#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"
#include "texture.h"

typedef struct {
    vec3_t* vertices; // dynamic array of vertices
    face_t* faces;    // dynamic array of faces
    texture_t texture;
    vec3_t scale;
    vec3_t rotation;
    vec3_t translation;
} mesh_t;

void load_mesh(const char* obj_filename, const char* png_filename, vec3_t scale, vec3_t rotation, vec3_t translation);
int load_mesh_obj_data(const char* filename, mesh_t* mesh);
int load_mesh_png_texture(const char* fileName, texture_t* texture);

int get_num_meshs();
mesh_t* get_mesh_at(int idx);

void free_meshs();

#endif