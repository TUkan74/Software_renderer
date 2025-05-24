#pragma once
#include <Eigen/Dense>

using Vector2 = Eigen::Vector2f;
using Vector3 = Eigen::Vector3f;

enum class RenderMode {
    WIREFRAME,
    SOLID,
    TEXTURED,
    TEXTURED_SHADED,
    COLORFUL
};

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