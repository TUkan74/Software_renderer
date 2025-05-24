#pragma once

#include <Eigen/Dense>

struct Vertex {
    Eigen::Vector4f position;  // Homogeneous coordinates (x, y, z, w)
    Eigen::Vector2f texCoord;  // Texture coordinates (u, v)
    Eigen::Vector3f normal;    // Normal vector
    float intensity;           // Light intensity
    
    Vertex() : position(0, 0, 0, 1), texCoord(0, 0), normal(0, 0, 0), intensity(1.0f) {}
    
    // Transform vertex by a matrix
    void transform(const Eigen::Matrix4f& matrix) {
        position = matrix * position;
    }
    
    // Perspective divide
    void perspectiveDivide() {
        if (position.w() != 0.0f) {
            position /= position.w();
        }
    }
    
    // Convert to screen coordinates
    void toScreenSpace(int width, int height) {
        position.x() = (position.x() + 1.0f) * 0.5f * width;
        position.y() = (position.y() + 1.0f) * 0.5f * height;
    }
}; 