#include "mesh.h"
#include "array.h"
#include "upng.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUM_MESHS 10

static mesh_t meshs[MAX_NUM_MESHS];
static int num_meshs = 0;

void load_mesh(const char* obj_filename, const char* png_filename, vec3_t scale, vec3_t rotation, vec3_t translation) {
    if(num_meshs >= MAX_NUM_MESHS) return;

    mesh_t* mesh = &meshs[num_meshs];
    mesh->faces = NULL;
    mesh->vertices = NULL;

    if(load_mesh_obj_data(obj_filename, mesh)) return;
    if(load_mesh_png_texture(png_filename, &mesh->texture)) return;
    mesh->scale = scale;
    mesh->rotation = rotation;
    mesh->translation = translation;
    
    num_meshs++;
}

int load_mesh_obj_data(const char* filename, mesh_t* mesh) {
    FILE* fp;
    fp = fopen(filename, "r");
    if(!fp) return 1;
    char line[256];
    tex2_t* texcoords = NULL;
    while(fgets(line, sizeof(line), fp)) {
        int len_line = strlen(line);
        if(len_line < 2) continue;
        if(line[0] == 'v' && line[1] == ' ') {
            // vertex information
            vec3_t vertex;
            sscanf(&line[2], "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh->vertices, vertex);
        } else if(line[0] == 'v' && line[1] == 't') {
            tex2_t texcoord;
            sscanf(&line[3], "%f %f", &texcoord.u, &texcoord.v);
            array_push(texcoords, texcoord);
        } else if(line[0] == 'f' && line[1] == ' ') {
            // face information
            char buff[3][64];
            sscanf(&line[2], "%s %s %s", buff[0], buff[1], buff[2]);

            int vertex_indices[3];
            int texture_indices[3];

            sscanf(buff[0], "%d/%d", &vertex_indices[0], &texture_indices[0]);
            sscanf(buff[1], "%d/%d", &vertex_indices[1], &texture_indices[1]);
            sscanf(buff[2], "%d/%d", &vertex_indices[2], &texture_indices[2]);

            face_t face = {
                .a = vertex_indices[0] - 1,
                .a_uv = texcoords[texture_indices[0] - 1],
                .b = vertex_indices[1] - 1,
                .b_uv = texcoords[texture_indices[1] - 1],
                .c = vertex_indices[2] - 1,
                .c_uv = texcoords[texture_indices[2] - 1],
                .color = 0xFF808080
            };
            
            array_push(mesh->faces, face);
        }
    }
    array_free(texcoords);
    fclose(fp);
    return 0;
}

int load_mesh_png_texture(const char* fileName, texture_t* texture) {
    upng_t* texture_png = upng_new_from_file(fileName);
    if(!texture_png) return 1;
    upng_decode(texture_png);
    if(upng_get_error(texture_png) != UPNG_EOK) return 1;

    int texture_width = upng_get_width(texture_png);
    int texture_height = upng_get_height(texture_png);

    texture->buffer = (uint32_t*)malloc(sizeof(uint32_t) * texture_width * texture_height);
    texture->width = texture_width;
    texture->height = texture_height;

    uint32_t* rgba_colors = (uint32_t*)upng_get_buffer(texture_png);
    for(int i = 0; i < texture_height * texture_width; i++) {
        uint32_t a = (rgba_colors[i] & 0xFF000000);
        uint32_t r = (rgba_colors[i] & 0x00FF0000) >> 16;
        uint32_t g = (rgba_colors[i] & 0x0000FF00);
        uint32_t b = (rgba_colors[i] & 0x000000FF) << 16;
        texture->buffer[i] = a | r | g | b;
    }

    upng_free(texture_png);
    return 0;
}

int get_num_meshs() {
    return num_meshs;
}

mesh_t* get_mesh_at(int idx) {
    if(idx < 0 || idx >= num_meshs) return NULL;
    return &meshs[idx];
}

void free_meshs() {
    for(int i = 0; i < num_meshs; i++) {
        free(meshs[i].texture.buffer);
        array_free(meshs[i].faces);
        array_free(meshs[i].vertices);
    }
    num_meshs = 0;
}