#include "mesh.h"
#include "array.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

mesh_t mesh = {
    .vertices = 0,
    .faces = 0,
    .rotation = {0, 0, 0}
};

void load_obj_file(const char* filename) {
    FILE* fp;
    fp = fopen(filename, "r");
    if(!fp) return;
    char line[256];
    while(fgets(line, sizeof(line), fp)) {
        int len_line = strlen(line);
        if(len_line < 2) continue;
        if(line[0] == 'v' && line[1] == ' ') {
            // vertex information
            vec3_t vertex;
            sscanf(&line[2], "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh.vertices, vertex);
        } else if(line[0] == 'f' && line[1] == ' ') {
            // face information
            char buff[3][64];
            sscanf(&line[2], "%s %s %s", buff[0], buff[1], buff[2]);

            face_t face;
            sscanf(buff[0], "%d", &face.a);
            sscanf(buff[1], "%d", &face.b);
            sscanf(buff[2], "%d", &face.c);

            // temp code for color
            int rnd_r = rand() % 256;
            int rnd_g = rand() % 256;
            int rnd_b = rand() % 256;
            face.color = 0xFF000000 | (rnd_r << 16) | (rnd_g << 8) | (rnd_b << 0);
            
            array_push(mesh.faces, face);
        }
    }
    fclose(fp);
}

