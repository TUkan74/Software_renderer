#include "Renderer.h"
#include "datatypes.h"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <random>

// Define M_PI if not available (Windows MSVC doesn't define it by default)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Renderer::Renderer(int width, int height)
    : width(width), height(height), renderMode(RenderMode::WIREFRAME)
{
    spdlog::info("Initializing Renderer with width={}, height={}", width, height);
    frameBuffer.resize(width * height, 0);
    zBuffer.resize(width * height, std::numeric_limits<float>::infinity());

    // Default camera setup
    cameraPosition = Eigen::Vector3f(2.0f, 0.5f, 3.0f);
    cameraTarget   = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    cameraUp       = Eigen::Vector3f(0.0f, 1.0f, 0.0f);
    updateViewMatrix();

    // Model and projection matrices
    modelMatrix      = Eigen::Matrix4f::Identity();
    projectionMatrix = Eigen::Matrix4f::Identity();
    float aspect     = float(width) / float(height);
    float fov        = 45.0f * M_PI / 180.0f;
    float nearPlane  = 0.1f;
    float farPlane   = 100.0f;
    float tanHalfFov = std::tan(fov / 2.0f);

    projectionMatrix(0,0) = 1.0f / (aspect * tanHalfFov);
    projectionMatrix(1,1) = 1.0f /            tanHalfFov;
    projectionMatrix(2,2) = -(farPlane + nearPlane) / (farPlane - nearPlane);
    projectionMatrix(2,3) = -2.0f * farPlane * nearPlane / (farPlane - nearPlane);
    projectionMatrix(3,2) = -1.0f;
    projectionMatrix(3,3) =  0.0f;

    spdlog::info("Renderer initialized successfully");
}

void Renderer::setCameraPosition(const Eigen::Vector3f& position) {
    cameraPosition = position;
    updateViewMatrix();
}

void Renderer::setCameraTarget(const Eigen::Vector3f& target) {
    cameraTarget = target;
    updateViewMatrix();
}

void Renderer::updateViewMatrix() {
    // Compute camera basis
    Eigen::Vector3f forward = (cameraTarget - cameraPosition).normalized();
    Eigen::Vector3f right   = forward.cross(cameraUp).normalized();
    Eigen::Vector3f up      = right.cross(forward).normalized();

    // Build view matrix
    viewMatrix = Eigen::Matrix4f::Identity();
    viewMatrix.block<1,3>(0,0) = right;
    viewMatrix.block<1,3>(1,0) = up;
    viewMatrix.block<1,3>(2,0) = -forward;

    Eigen::Vector3f trans(
        -right.dot(cameraPosition),
        -up.dot(cameraPosition),
         forward.dot(cameraPosition)
    );
    viewMatrix.block<3,1>(0,3) = trans;
}

void Renderer::setRenderMode(RenderMode mode) {
    renderMode = mode;
}

void Renderer::render(const Model& model) {
    spdlog::info("Rendering model with mode {}", static_cast<int>(renderMode));
    switch (renderMode) {
        case RenderMode::WIREFRAME:      renderWireframe(model);        break;
        case RenderMode::SOLID:          renderSolid(model);            break;
        case RenderMode::TEXTURED:       renderTextured(model);         break;
        case RenderMode::TEXTURED_SHADED: renderTexturedShaded(model);   break;
        case RenderMode::COLORFUL:       renderColorful(model);         break;
    }
}

void Renderer::renderWireframe(const Model& model) {
    const auto& V = model.getVertices();
    const auto& F = model.getFaces();
    Eigen::Matrix4f mvp = projectionMatrix * viewMatrix * modelMatrix;
    std::fill(zBuffer.begin(), zBuffer.end(), std::numeric_limits<float>::infinity());

    for (const auto& face : F) {
        if (face.vertexIndices.size() != 3) continue;
        std::array<Eigen::Vector4f,3> proj;
        std::array<Vertex,3> verts;
        for (int i = 0; i < 3; ++i) {
            auto v = V[face.vertexIndices[i]];
            Eigen::Vector4f vh(v.x(), v.y(), v.z(), 1.0f);
            proj[i] = mvp * vh;
            proj[i] /= proj[i].w();
            verts[i] = {proj[i].x(), proj[i].y(), proj[i].z(), 0, 0, 0, 0, 0};
        }
        drawLine(verts[0], verts[1], static_cast<uint32_t>(Color::RED));
        drawLine(verts[1], verts[2], static_cast<uint32_t>(Color::RED));
        drawLine(verts[2], verts[0], static_cast<uint32_t>(Color::RED));
    }
}

static uint32_t generateRandomColor() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dist(0, 255);
    uint8_t r = static_cast<uint8_t>(dist(gen));
    uint8_t g = static_cast<uint8_t>(dist(gen));
    uint8_t b = static_cast<uint8_t>(dist(gen));
    return (0xFF << 24) | (r << 16) | (g << 8) | b;
}

static Eigen::Vector3f barycentricInt(const Vertex& v1, const Vertex& v2, const Vertex& v3, int px, int py) {
    Eigen::Vector3f u(v3.x - v1.x, v2.x - v1.x, v1.x - px);
    Eigen::Vector3f v(v3.y - v1.y, v2.y - v1.y, v1.y - py);
    Eigen::Vector3f w = u.cross(v);
    if (std::abs(w.z()) < 1) return Eigen::Vector3f(-1, 1, 1);
    return Eigen::Vector3f(1.0f - (w.x() + w.y()) / w.z(), w.y() / w.z(), w.x() / w.z());
}

void Renderer::drawTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3, uint32_t color) {
    // Find bounding box
    int minX = std::min({int(v1.x), int(v2.x), int(v3.x)});
    int minY = std::min({int(v1.y), int(v2.y), int(v3.y)});
    int maxX = std::max({int(v1.x), int(v2.x), int(v3.x)});
    int maxY = std::max({int(v1.y), int(v2.y), int(v3.y)});
    
    // Make sure we don't go outside the frame buffer
    minX = std::max(0, minX);
    minY = std::max(0, minY);
    maxX = std::min(width-1, maxX);
    maxY = std::min(height-1, maxY);
    
    // Loop through the bounding box
    for (int px = minX; px <= maxX; px++) {
        for (int py = minY; py <= maxY; py++) {
            Eigen::Vector3f bc = barycentricInt(v1, v2, v3, px, py);
            if (bc.x() < 0 || bc.y() < 0 || bc.z() < 0) continue;
            setPixel(px, py, color);
        }
    }
}

void Renderer::renderColorful(const Model& model) {
    const auto& V = model.getVertices();
    const auto& F = model.getFaces();
    std::fill(zBuffer.begin(), zBuffer.end(), std::numeric_limits<float>::infinity());
    Eigen::Matrix4f mvp = projectionMatrix * viewMatrix * modelMatrix;
    for (const auto& face : F) {
        if (face.vertexIndices.size() != 3) continue;
        Vertex v1, v2, v3;
        for (int j = 0; j < 3; ++j) {
            auto v = V[face.vertexIndices[j]];
            Eigen::Vector4f vh(v.x(), v.y(), v.z(), 1.0f);
            Eigen::Vector4f ph = mvp * vh;
            ph /= ph.w();
            float sx = (ph.x() + 1) * 0.5f * width;
            float sy = (ph.y() + 1) * 0.5f * height;
            float sz = ph.z();
            if (j == 0) { v1.x = sx; v1.y = sy; v1.z = sz; }
            if (j == 1) { v2.x = sx; v2.y = sy; v2.z = sz; }
            if (j == 2) { v3.x = sx; v3.y = sy; v3.z = sz; }
        }
        // Backface culling using signed area in screen space
        float signed_area = (v2.x - v1.x) * (v3.y - v1.y) - (v3.x - v1.x) * (v2.y - v1.y);
        if (signed_area <= 0) continue;
        uint32_t color = generateRandomColor();
        drawTriangle(v1, v2, v3, color);
    }
}

void Renderer::renderTextured(const Model& model) {
    // TODO: Implement textured rendering
}

void Renderer::renderTexturedShaded(const Model& model) {
    // TODO: Implement textured shaded rendering
}

void Renderer::renderSolid(const Model& model) {
    // Not implemented yet
}

void Renderer::clearBuffer(uint32_t color) {
    std::fill(frameBuffer.begin(), frameBuffer.end(), color);
    std::fill(zBuffer.begin(), zBuffer.end(), std::numeric_limits<float>::infinity());
}

bool Renderer::saveImage(const std::string& filename) const {
    Texture tex;
    tex.setWidth(width);
    tex.setHeight(height);
    tex.setData(frameBuffer);
    return tex.saveToTGA(filename);
}

void Renderer::drawLine(const Vertex& v0, const Vertex& v1, uint32_t color) {
    int x0 = static_cast<int>((v0.x + 1) * 0.5f * width);
    int y0 = static_cast<int>((v0.y + 1) * 0.5f * height);
    int x1 = static_cast<int>((v1.x + 1) * 0.5f * width);
    int y1 = static_cast<int>((v1.y + 1) * 0.5f * height);
    int dx  = std::abs(x1 - x0);
    int dy  = std::abs(y1 - y0);
    int sx  = (x0 < x1 ? 1 : -1);
    int sy  = (y0 < y1 ? 1 : -1);
    int err = dx - dy;
    while (true) {
        setPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = err*2;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 <  dx) { err += dx; y0 += sy; }
    }
}

void Renderer::setPixel(int x, int y, uint32_t color) {
    if (x>=0 && x<width && y>=0 && y<height) 
        frameBuffer[y*width + x] = color;
}