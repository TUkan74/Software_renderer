#include "Renderer.h"
#include "datatypes.h"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <random>

// Define M_PI if not available (Windows MSVC doesn't define it by default)
#ifndef M_PI
#define M_PI 3.14159265358979323846f
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
    // Use a more numerically stable approach for barycentric coordinates
    float x1 = v1.x, y1 = v1.y;
    float x2 = v2.x, y2 = v2.y;
    float x3 = v3.x, y3 = v3.y;
    
    float area = 0.5f * std::abs((x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1));
    if (area < 0.00001f) {
        // Triangle is degenerate (has no area)
        return Eigen::Vector3f(-1, 1, 1);
    }
    
    float alpha = 0.5f * std::abs((x2 - px) * (y3 - py) - (x3 - px) * (y2 - py)) / area;
    float beta = 0.5f * std::abs((x1 - px) * (y3 - py) - (x3 - px) * (y1 - py)) / area;
    float gamma = 1.0f - alpha - beta;
    
    return Eigen::Vector3f(gamma, alpha, beta);
}

void Renderer::drawTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3, uint32_t color) {
    // Find bounding box with a safety margin
    int minX = static_cast<int>(std::floor(std::min({v1.x, v2.x, v3.x})) - 1.0f);
    int minY = static_cast<int>(std::floor(std::min({v1.y, v2.y, v3.y})) - 1.0f);
    int maxX = static_cast<int>(std::ceil(std::max({v1.x, v2.x, v3.x})) + 1.0f);
    int maxY = static_cast<int>(std::ceil(std::max({v1.y, v2.y, v3.y})) + 1.0f);
    
    // Clamp to screen boundaries
    minX = std::max(0, minX);
    minY = std::max(0, minY);
    maxX = std::min(width-1, maxX);
    maxY = std::min(height-1, maxY);
    
    // Loop through the bounding box
    for (int px = minX; px <= maxX; px++) {
        for (int py = minY; py <= maxY; py++) {
            Eigen::Vector3f bc = barycentricInt(v1, v2, v3, px, py);
            // Use a more permissive boundary check
            if (bc.x() < -0.01f || bc.y() < -0.01f || bc.z() < -0.01f) continue;
            
            // Depth interpolation
            float z = bc.x() * v1.z + bc.y() * v2.z + bc.z() * v3.z;
            
            // Check depth buffer
            int idx = py * width + px;
            if (z < zBuffer[idx] + 0.0001f) { // Small epsilon for z-fighting
                frameBuffer[idx] = color;
                zBuffer[idx] = z;
            }
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
        // Disable backface culling to fix missing triangles
        //if (signed_area <= 0) continue;
        uint32_t color = generateRandomColor();
        drawTriangle(v1, v2, v3, color);
    }
}

void Renderer::renderTextured(const Model& model) {
    const auto& vertices = model.getVertices();
    const auto& faces = model.getFaces();
    const auto& texCoords = model.getTextureCoords();
    auto texture = model.getTexture();
    
    // Check if texture is available
    if (!texture) {
        spdlog::warn("No texture available for textured rendering, falling back to colorful");
        renderColorful(model);
        return;
    }
    
    // Check if texture coordinates are available
    if (texCoords.empty()) {
        spdlog::warn("No texture coordinates available for textured rendering, falling back to colorful");
        renderColorful(model);
        return;
    }
    
    // Clear z-buffer
    std::fill(zBuffer.begin(), zBuffer.end(), std::numeric_limits<float>::infinity());
    
    // Model-view-projection matrix
    Eigen::Matrix4f mvp = projectionMatrix * viewMatrix * modelMatrix;
    
    // Draw each face as a textured triangle
    for (const auto& face : faces) {
        const auto& vertexIndices = face.vertexIndices;
        const auto& textureIndices = face.textureIndices;
        
        // Skip faces that are not triangles or don't have texture coordinates
        if (vertexIndices.size() != 3 || textureIndices.size() != 3) {
            continue;
        }
        
        // Get vertices
        Eigen::Vector3f modelV0 = vertices[vertexIndices[0]];
        Eigen::Vector3f modelV1 = vertices[vertexIndices[1]];
        Eigen::Vector3f modelV2 = vertices[vertexIndices[2]];
        
        // Get texture coordinates
        Eigen::Vector2f tex0 = texCoords[textureIndices[0]];
        Eigen::Vector2f tex1 = texCoords[textureIndices[1]];
        Eigen::Vector2f tex2 = texCoords[textureIndices[2]];
        
        // Convert to homogeneous coordinates
        Eigen::Vector4f v0h(modelV0.x(), modelV0.y(), modelV0.z(), 1.0f);
        Eigen::Vector4f v1h(modelV1.x(), modelV1.y(), modelV1.z(), 1.0f);
        Eigen::Vector4f v2h(modelV2.x(), modelV2.y(), modelV2.z(), 1.0f);
        
        // Apply MVP transformation
        Eigen::Vector4f p0 = mvp * v0h;
        Eigen::Vector4f p1 = mvp * v1h;
        Eigen::Vector4f p2 = mvp * v2h;
        
        // Perspective division
        p0 /= p0.w();
        p1 /= p1.w();
        p2 /= p2.w();
        
        // Backface culling - skip triangles facing away from the camera
        Eigen::Vector3f edge1(p1.x() - p0.x(), p1.y() - p0.y(), 0);
        Eigen::Vector3f edge2(p2.x() - p0.x(), p2.y() - p0.y(), 0);
        // Disable backface culling to fix missing triangles
        //if (edge1.cross(edge2).z() < 0) {
        //    continue;
        //}
        
        // Viewport transformation
        int x0 = static_cast<int>((p0.x() + 1.0f) * 0.5f * width);
        int y0 = static_cast<int>((p0.y() + 1.0f) * 0.5f * height);
        float z0 = p0.z();
        float u0 = tex0.x();
        float v0 = 1.0f - tex0.y();
        
        int x1 = static_cast<int>((p1.x() + 1.0f) * 0.5f * width);
        int y1 = static_cast<int>((p1.y() + 1.0f) * 0.5f * height);
        float z1 = p1.z();
        float u1 = tex1.x();
        float v1 = 1.0f - tex1.y();
        
        int x2 = static_cast<int>((p2.x() + 1.0f) * 0.5f * width);
        int y2 = static_cast<int>((p2.y() + 1.0f) * 0.5f * height);
        float z2 = p2.z();
        float u2 = tex2.x();
        float v2 = 1.0f - tex2.y();
        
        // Set full lighting (no shading) for all vertices
        float l0 = 1.0f;
        float l1 = 1.0f;
        float l2 = 1.0f;
        
        // Render the textured triangle
        renderTexturedTriangle(
            x0, y0, z0, u0, v0, l0,
            x1, y1, z1, u1, v1, l1,
            x2, y2, z2, u2, v2, l2,
            *texture
        );
    }
}

void Renderer::renderTexturedShaded(const Model& model) {
    const auto& vertices = model.getVertices();
    const auto& faces = model.getFaces();
    const auto& normals = model.getNormals();
    const auto& texCoords = model.getTextureCoords();
    auto texture = model.getTexture();
    
    // Check if texture is available
    if (!texture) {
        spdlog::warn("No texture available for textured shaded rendering, falling back to solid");
        renderSolid(model);
        return;
    }
    
    // Check if texture coordinates are available
    if (texCoords.empty()) {
        spdlog::warn("No texture coordinates available for textured shaded rendering, falling back to solid");
        renderSolid(model);
        return;
    }
    
    // Clear z-buffer
    std::fill(zBuffer.begin(), zBuffer.end(), std::numeric_limits<float>::infinity());
    
    // Model-view-projection matrix
    Eigen::Matrix4f mvp = projectionMatrix * viewMatrix * modelMatrix;
    
    // Direction to light source (for simple diffuse lighting)
    Eigen::Vector3f lightDir = (Eigen::Vector3f(1, 1, 1)).normalized();
    
    // Draw each face as a textured triangle
    for (const auto& face : faces) {
        const auto& vertexIndices = face.vertexIndices;
        const auto& textureIndices = face.textureIndices;
        const auto& normalIndices = face.normalIndices;
        
        // Skip faces that are not triangles or don't have texture coordinates
        if (vertexIndices.size() != 3 || textureIndices.size() != 3) {
            continue;
        }
        
        // Get vertices
        Eigen::Vector3f modelV0 = vertices[vertexIndices[0]];
        Eigen::Vector3f modelV1 = vertices[vertexIndices[1]];
        Eigen::Vector3f modelV2 = vertices[vertexIndices[2]];
        
        // Get texture coordinates
        Eigen::Vector2f tex0 = texCoords[textureIndices[0]];
        Eigen::Vector2f tex1 = texCoords[textureIndices[1]];
        Eigen::Vector2f tex2 = texCoords[textureIndices[2]];
        
        // Convert to homogeneous coordinates
        Eigen::Vector4f v0h(modelV0.x(), modelV0.y(), modelV0.z(), 1.0f);
        Eigen::Vector4f v1h(modelV1.x(), modelV1.y(), modelV1.z(), 1.0f);
        Eigen::Vector4f v2h(modelV2.x(), modelV2.y(), modelV2.z(), 1.0f);
        
        // Apply MVP transformation
        Eigen::Vector4f p0 = mvp * v0h;
        Eigen::Vector4f p1 = mvp * v1h;
        Eigen::Vector4f p2 = mvp * v2h;
        
        // Perspective division
        p0 /= p0.w();
        p1 /= p1.w();
        p2 /= p2.w();
        
        // Backface culling - skip triangles facing away from the camera
        Eigen::Vector3f edge1(p1.x() - p0.x(), p1.y() - p0.y(), 0);
        Eigen::Vector3f edge2(p2.x() - p0.x(), p2.y() - p0.y(), 0);
        // Disable backface culling to fix missing triangles
        //if (edge1.cross(edge2).z() < 0) {
        //    continue;
        //}
        
        // Viewport transformation
        int x0 = static_cast<int>((p0.x() + 1.0f) * 0.5f * width);
        int y0 = static_cast<int>((p0.y() + 1.0f) * 0.5f * height);
        float z0 = p0.z();
        float u0 = tex0.x();
        float v0 = 1.0f - tex0.y();
        
        int x1 = static_cast<int>((p1.x() + 1.0f) * 0.5f * width);
        int y1 = static_cast<int>((p1.y() + 1.0f) * 0.5f * height);
        float z1 = p1.z();
        float u1 = tex1.x();
        float v1 = 1.0f - tex1.y();
        
        int x2 = static_cast<int>((p2.x() + 1.0f) * 0.5f * width);
        int y2 = static_cast<int>((p2.y() + 1.0f) * 0.5f * height);
        float z2 = p2.z();
        float u2 = tex2.x();
        float v2 = 1.0f - tex2.y();
        
        // Calculate lighting intensities for each vertex
        float l0 = 0.2f; // Ambient light
        float l1 = 0.2f;
        float l2 = 0.2f;
        
        // Add diffuse lighting if normals are available
        if (!normalIndices.empty()) {
            Eigen::Vector3f n0 = normals[normalIndices[0]].normalized();
            Eigen::Vector3f n1 = normals[normalIndices[1]].normalized();
            Eigen::Vector3f n2 = normals[normalIndices[2]].normalized();
            
            // Calculate diffuse light component and add to ambient
            l0 += std::max(0.0f, n0.dot(lightDir)) * 0.8f;
            l1 += std::max(0.0f, n1.dot(lightDir)) * 0.8f;
            l2 += std::max(0.0f, n2.dot(lightDir)) * 0.8f;
        } else {
            // Use face normal if vertex normals are not available
            Eigen::Vector3f edge1_3d = modelV1 - modelV0;
            Eigen::Vector3f edge2_3d = modelV2 - modelV0;
            Eigen::Vector3f normal = edge1_3d.cross(edge2_3d).normalized();
            
            // Calculate diffuse light intensity and add to ambient
            float diffuse = std::max(0.0f, normal.dot(lightDir)) * 0.8f;
            l0 += diffuse;
            l1 += diffuse;
            l2 += diffuse;
        }
        
        // Ensure lighting doesn't exceed 1.0
        l0 = std::min(1.0f, l0);
        l1 = std::min(1.0f, l1);
        l2 = std::min(1.0f, l2);
        
        // Render the textured and shaded triangle
        renderTexturedTriangle(
            x0, y0, z0, u0, v0, l0,
            x1, y1, z1, u1, v1, l1,
            x2, y2, z2, u2, v2, l2,
            *texture
        );
    }
}

void Renderer::renderTexturedTriangle(
    int x0, int y0, float z0, float u0, float v0, float l0,
    int x1, int y1, float z1, float u1, float v1, float l1,
    int x2, int y2, float z2, float u2, float v2, float l2,
    const Texture& texture)
{
    // Sort vertices by Y-coordinate (y0 <= y1 <= y2)
    if (y0 > y1) {
        std::swap(x0, x1); std::swap(y0, y1); std::swap(z0, z1);
        std::swap(u0, u1); std::swap(v0, v1); std::swap(l0, l1);
    }
    if (y0 > y2) {
        std::swap(x0, x2); std::swap(y0, y2); std::swap(z0, z2);
        std::swap(u0, u2); std::swap(v0, v2); std::swap(l0, l2);
    }
    if (y1 > y2) {
        std::swap(x1, x2); std::swap(y1, y2); std::swap(z1, z2);
        std::swap(u1, u2); std::swap(v1, v2); std::swap(l1, l2);
    }

    // Trivial case: triangle has no height
    if (y0 == y2) {
        return;
    }

    // Calculate edge slopes and interpolation parameters
    bool longSideIsLeft = false;

    // Calculate change in X per Y step for each edge
    float dxdy_02 = (y2 != y0) ? float(x2 - x0) / float(y2 - y0) : 0;
    float dxdy_01 = (y1 != y0) ? float(x1 - x0) / float(y1 - y0) : 0;
    float dxdy_12 = (y2 != y1) ? float(x2 - x1) / float(y2 - y1) : 0;

    // Determine which edge is the "long" side
    if (dxdy_02 < dxdy_01 || y0 == y1) {
        longSideIsLeft = true;
    }

    // Upper part of the triangle (between y0 and y1)
    if (y0 != y1) {
        float leftX = float(x0);
        float rightX = float(x0);
        
        float leftStepX = longSideIsLeft ? dxdy_02 : dxdy_01;
        float rightStepX = longSideIsLeft ? dxdy_01 : dxdy_02;
        
        // Interpolation values for the left edge
        float leftZ = z0;
        float leftU = u0;
        float leftV = v0;
        float leftL = l0;
        
        // Interpolation values for the right edge
        float rightZ = z0;
        float rightU = u0;
        float rightV = v0;
        float rightL = l0;
        
        // Calculate change per Y step for the left edge
        float dzdy_left = longSideIsLeft ? 
            ((y2 != y0) ? (z2 - z0) / float(y2 - y0) : 0) : 
            ((y1 != y0) ? (z1 - z0) / float(y1 - y0) : 0);
        
        float dudy_left = longSideIsLeft ? 
            ((y2 != y0) ? (u2 - u0) / float(y2 - y0) : 0) : 
            ((y1 != y0) ? (u1 - u0) / float(y1 - y0) : 0);
        
        float dvdy_left = longSideIsLeft ? 
            ((y2 != y0) ? (v2 - v0) / float(y2 - y0) : 0) : 
            ((y1 != y0) ? (v1 - v0) / float(y1 - y0) : 0);
            
        float dldy_left = longSideIsLeft ? 
            ((y2 != y0) ? (l2 - l0) / float(y2 - y0) : 0) : 
            ((y1 != y0) ? (l1 - l0) / float(y1 - y0) : 0);
        
        // Calculate change per Y step for the right edge
        float dzdy_right = longSideIsLeft ? 
            ((y1 != y0) ? (z1 - z0) / float(y1 - y0) : 0) : 
            ((y2 != y0) ? (z2 - z0) / float(y2 - y0) : 0);
        
        float dudy_right = longSideIsLeft ? 
            ((y1 != y0) ? (u1 - u0) / float(y1 - y0) : 0) : 
            ((y2 != y0) ? (u2 - u0) / float(y2 - y0) : 0);
        
        float dvdy_right = longSideIsLeft ? 
            ((y1 != y0) ? (v1 - v0) / float(y1 - y0) : 0) : 
            ((y2 != y0) ? (v2 - v0) / float(y2 - y0) : 0);
            
        float dldy_right = longSideIsLeft ? 
            ((y1 != y0) ? (l1 - l0) / float(y1 - y0) : 0) : 
            ((y2 != y0) ? (l2 - l0) / float(y2 - y0) : 0);
        
        // Draw the upper part of the triangle
        drawTexturedScanlines(
            y0, y1,
            leftX, leftStepX, rightX, rightStepX,
            leftZ, dzdy_left, rightZ, dzdy_right,
            leftU, dudy_left, rightU, dudy_right,
            leftV, dvdy_left, rightV, dvdy_right,
            leftL, dldy_left, rightL, dldy_right,
            texture
        );
    }
    
    // Lower part of the triangle (between y1 and y2)
    if (y1 != y2) {
        float leftX, rightX;
        float leftStepX, rightStepX;
        
        // Calculate starting X positions and slopes for the lower part
        if (longSideIsLeft) {
            leftX = x0 + dxdy_02 * (y1 - y0);
            leftStepX = dxdy_02;
            rightX = x1;
            rightStepX = dxdy_12;
        } else {
            leftX = x1;
            leftStepX = dxdy_12;
            rightX = x0 + dxdy_02 * (y1 - y0);
            rightStepX = dxdy_02;
        }
        
        // Interpolation values for the left edge
        float leftZ, leftU, leftV, leftL;
        float rightZ, rightU, rightV, rightL;
        
        float dzdy_left, dudy_left, dvdy_left, dldy_left;
        float dzdy_right, dudy_right, dvdy_right, dldy_right;
        
        if (longSideIsLeft) {
            // Interpolate along edge 0-2 for the left edge
            float t = (y1 - y0) / float(y2 - y0);
            leftZ = z0 + t * (z2 - z0);
            leftU = u0 + t * (u2 - u0);
            leftV = v0 + t * (v2 - v0);
            leftL = l0 + t * (l2 - l0);
            
            // Edge 1-2 for the right edge
            rightZ = z1;
            rightU = u1;
            rightV = v1;
            rightL = l1;
            
            // Calculate change per Y step for the left edge (along 0-2)
            dzdy_left = (z2 - leftZ) / float(y2 - y1);
            dudy_left = (u2 - leftU) / float(y2 - y1);
            dvdy_left = (v2 - leftV) / float(y2 - y1);
            dldy_left = (l2 - leftL) / float(y2 - y1);
            
            // Calculate change per Y step for the right edge (along 1-2)
            dzdy_right = (z2 - z1) / float(y2 - y1);
            dudy_right = (u2 - u1) / float(y2 - y1);
            dvdy_right = (v2 - v1) / float(y2 - y1);
            dldy_right = (l2 - l1) / float(y2 - y1);
        } else {
            // Edge 1-2 for the left edge
            leftZ = z1;
            leftU = u1;
            leftV = v1;
            leftL = l1;
            
            // Interpolate along edge 0-2 for the right edge
            float t = (y1 - y0) / float(y2 - y0);
            rightZ = z0 + t * (z2 - z0);
            rightU = u0 + t * (u2 - u0);
            rightV = v0 + t * (v2 - v0);
            rightL = l0 + t * (l2 - l0);
            
            // Calculate change per Y step for the left edge (along 1-2)
            dzdy_left = (z2 - z1) / float(y2 - y1);
            dudy_left = (u2 - u1) / float(y2 - y1);
            dvdy_left = (v2 - v1) / float(y2 - y1);
            dldy_left = (l2 - l1) / float(y2 - y1);
            
            // Calculate change per Y step for the right edge (along 0-2)
            dzdy_right = (z2 - rightZ) / float(y2 - y1);
            dudy_right = (u2 - rightU) / float(y2 - y1);
            dvdy_right = (v2 - rightV) / float(y2 - y1);
            dldy_right = (l2 - rightL) / float(y2 - y1);
        }
        
        // Draw the lower part of the triangle
        drawTexturedScanlines(
            y1, y2,
            leftX, leftStepX, rightX, rightStepX,
            leftZ, dzdy_left, rightZ, dzdy_right,
            leftU, dudy_left, rightU, dudy_right,
            leftV, dvdy_left, rightV, dvdy_right,
            leftL, dldy_left, rightL, dldy_right,
            texture
        );
    }
}

void Renderer::drawTexturedScanlines(
    int yStart, int yEnd,
    float xLeft, float dxLeft, float xRight, float dxRight,
    float zLeft, float dzLeft, float zRight, float dzRight,
    float uLeft, float duLeft, float uRight, float duRight,
    float vLeft, float dvLeft, float vRight, float dvRight,
    float lLeft, float dlLeft, float lRight, float dlRight,
    const Texture& texture)
{
    // Make sure we don't go outside the frame buffer
    yStart = std::max(0, yStart);
    yEnd = std::min(height - 1, yEnd);

    // For each scanline
    for (int y = yStart; y <= yEnd; ++y) {
        // Calculate start and end positions for this scanline
        int xStart = std::max(0, static_cast<int>(xLeft + 0.5f));
        int xEnd = std::min(width - 1, static_cast<int>(xRight + 0.5f));
        
        // Skip if the scanline has no width
        if (xStart >= xEnd) {
            // Update edge parameters for the next scanline
            xLeft += dxLeft;
            xRight += dxRight;
            zLeft += dzLeft;
            zRight += dzRight;
            uLeft += duLeft;
            uRight += duRight;
            vLeft += dvLeft;
            vRight += dvRight;
            lLeft += dlLeft;
            lRight += dlRight;
            continue;
        }
        
        // Calculate increments for each parameter along the scanline
        float dx = static_cast<float>(xEnd - xStart);
        float dz = (zRight - zLeft) / dx;
        float du = (uRight - uLeft) / dx;
        float dv = (vRight - vLeft) / dx;
        float dl = (lRight - lLeft) / dx;
        
        // Starting values for this scanline
        float z = zLeft + dz * (xStart - xLeft);
        float u = uLeft + du * (xStart - xLeft);
        float v = vLeft + dv * (xStart - xLeft);
        float l = lLeft + dl * (xStart - xLeft);
        
        // Draw the scanline
        for (int x = xStart; x <= xEnd; ++x) {
            int pixelIdx = y * width + x;
            
            // More permissive depth test with small epsilon to avoid z-fighting
            if (z < zBuffer[pixelIdx] + 0.0001f) {
                // Get texture color and apply lighting
                uint32_t texColor = texture.getColorAt(u, v);
                
                // Apply shading by scaling RGB components (keep alpha)
                uint32_t a = (texColor >> 24) & 0xFF;
                uint32_t r = static_cast<uint32_t>(std::min(255.0f, ((texColor >> 16) & 0xFF) * l));
                uint32_t g = static_cast<uint32_t>(std::min(255.0f, ((texColor >> 8) & 0xFF) * l));
                uint32_t b = static_cast<uint32_t>(std::min(255.0f, (texColor & 0xFF) * l));
                
                // Set pixel with shaded color
                uint32_t finalColor = (a << 24) | (r << 16) | (g << 8) | b;
                frameBuffer[pixelIdx] = finalColor;
                zBuffer[pixelIdx] = z;
            }
            
            // Update parameters for the next pixel
            z += dz;
            u += du;
            v += dv;
            l += dl;
        }
        
        // Update edge parameters for the next scanline
        xLeft += dxLeft;
        xRight += dxRight;
        zLeft += dzLeft;
        zRight += dzRight;
        uLeft += duLeft;
        uRight += duRight;
        vLeft += dvLeft;
        vRight += dvRight;
        lLeft += dlLeft;
        lRight += dlRight;
    }
}

void Renderer::renderSolid(const Model& model) {
    const auto& vertices = model.getVertices();
    const auto& faces = model.getFaces();
    const auto& normals = model.getNormals();
    
    // Model-view-projection matrix
    Eigen::Matrix4f mvp = projectionMatrix * viewMatrix * modelMatrix;
    
    // Direction to light source (for simple diffuse lighting)
    Eigen::Vector3f lightDir = (Eigen::Vector3f(1, 1, 1)).normalized();
    
    // Draw each face as a filled triangle
    for (const auto& face : faces) {
        const auto& vertexIndices = face.vertexIndices;
        const auto& normalIndices = face.normalIndices;
        
        // Skip faces that are not triangles
        if (vertexIndices.size() != 3) {
            continue;
        }
        
        // Get vertices
        Eigen::Vector3f v0 = vertices[vertexIndices[0]];
        Eigen::Vector3f v1 = vertices[vertexIndices[1]];
        Eigen::Vector3f v2 = vertices[vertexIndices[2]];
        
        // Convert to homogeneous coordinates
        Eigen::Vector4f v0h(v0.x(), v0.y(), v0.z(), 1.0f);
        Eigen::Vector4f v1h(v1.x(), v1.y(), v1.z(), 1.0f);
        Eigen::Vector4f v2h(v2.x(), v2.y(), v2.z(), 1.0f);
        
        // Apply MVP transformation
        Eigen::Vector4f p0 = mvp * v0h;
        Eigen::Vector4f p1 = mvp * v1h;
        Eigen::Vector4f p2 = mvp * v2h;
        
        // Perspective division
        p0 /= p0.w();
        p1 /= p1.w();
        p2 /= p2.w();
        
        // Viewport transformation
        float x0 = (p0.x() + 1.0f) * 0.5f * width;
        float y0 = (p0.y() + 1.0f) * 0.5f * height;
        float z0 = p0.z();
        float x1 = (p1.x() + 1.0f) * 0.5f * width;
        float y1 = (p1.y() + 1.0f) * 0.5f * height;
        float z1 = p1.z();
        float x2 = (p2.x() + 1.0f) * 0.5f * width;
        float y2 = (p2.y() + 1.0f) * 0.5f * height;
        float z2 = p2.z();
        
        // Calculate face normal for shading if no vertex normals are provided
        Eigen::Vector3f normal;
        if (normalIndices.empty()) {
            Eigen::Vector3f edge1 = v1 - v0;
            Eigen::Vector3f edge2 = v2 - v0;
            normal = edge1.cross(edge2).normalized();
        } else {
            // Use average of vertex normals for simple shading
            normal = (normals[normalIndices[0]] + 
                     normals[normalIndices[1]] + 
                     normals[normalIndices[2]]).normalized();
        }
        
        // Simple diffuse lighting
        float intensity = std::max(0.2f, normal.dot(lightDir));
        
        // Create a diffuse shaded color (white base color)
        uint8_t r = static_cast<uint8_t>(255 * intensity);
        uint8_t g = static_cast<uint8_t>(255 * intensity);
        uint8_t b = static_cast<uint8_t>(255 * intensity);
        uint32_t color = (0xFF << 24) | (r << 16) | (g << 8) | b;
        
        // Create Vertex objects for triangle rendering
        Vertex vert0, vert1, vert2;
        
        vert0.x = x0; vert0.y = y0; vert0.z = z0;
        vert1.x = x1; vert1.y = y1; vert1.z = z1;
        vert2.x = x2; vert2.y = y2; vert2.z = z2;
        
        // Draw the triangle
        drawTriangle(vert0, vert1, vert2, color);
    }
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
    int x0 = static_cast<int>((v0.x + 1.0f) * 0.5f * static_cast<float>(width));
    int y0 = static_cast<int>((v0.y + 1.0f) * 0.5f * static_cast<float>(height));
    int x1 = static_cast<int>((v1.x + 1.0f) * 0.5f * static_cast<float>(width));
    int y1 = static_cast<int>((v1.y + 1.0f) * 0.5f * static_cast<float>(height));
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