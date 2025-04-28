#ifndef DATATYPES_H
#define DATATYPES_H

#include <vector>

struct Vertex {
    float x, y, z;
    float nx, ny, nz;
    float u, v;
};

struct Triangle {
    Vertex v1, v2, v3;
};


#endif