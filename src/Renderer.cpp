#include "Renderer.h"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cmath>
#include <stdexcept>

// Define M_PI if not available (Windows MSVC doesn't define it by default)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Renderer::Renderer(int width, int height) : width(width), height(height), renderMode(RenderMode::WIREFRAME) {
    spdlog::info("Initializing Renderer with width={}, height={}", width, height);
    
    frameBuffer.resize(width * height, 0);
    zBuffer.resize(width * height, std::numeric_limits<float>::infinity());
    
    // Initialize camera
    cameraPosition = Eigen::Vector3f(0, 0, 5);
    cameraTarget = Eigen::Vector3f(0, 0, 0);
    cameraUp = Eigen::Vector3f(0, 1, 0);
    
    // Initialize matrices
    modelMatrix = Eigen::Matrix4f::Identity();
    viewMatrix = Eigen::Matrix4f::Identity();
    projectionMatrix = Eigen::Matrix4f::Identity();
    
    // Set up perspective projection
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    float fov = 45.0f * M_PI / 180.0f; // 45 degrees in radians
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    
    float tanHalfFov = std::tan(fov / 2.0f);
    
    projectionMatrix(0, 0) = 1.0f / (aspectRatio * tanHalfFov);
    projectionMatrix(1, 1) = 1.0f / tanHalfFov;
    projectionMatrix(2, 2) = -(farPlane + nearPlane) / (farPlane - nearPlane);
    projectionMatrix(2, 3) = -2.0f * farPlane * nearPlane / (farPlane - nearPlane);
    projectionMatrix(3, 2) = -1.0f;
    projectionMatrix(3, 3) = 0.0f;
    
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
    // Calculate camera basis vectors
    Eigen::Vector3f forward = (cameraTarget - cameraPosition).normalized();
    Eigen::Vector3f right = forward.cross(cameraUp).normalized();
    Eigen::Vector3f up = right.cross(forward).normalized();
    
    // Build view matrix
    viewMatrix = Eigen::Matrix4f::Identity();
    viewMatrix.block<1, 3>(0, 0) = right;
    viewMatrix.block<1, 3>(1, 0) = up;
    viewMatrix.block<1, 3>(2, 0) = -forward;
    
    Eigen::Vector3f translation = Eigen::Vector3f(
        -right.dot(cameraPosition),
        -up.dot(cameraPosition),
        forward.dot(cameraPosition)
    );
    
    viewMatrix.block<3, 1>(0, 3) = translation;
}

void Renderer::setRenderMode(RenderMode mode) {
    renderMode = mode;
}

void Renderer::render(const Model& model) {
    spdlog::info("Rendering model with {} mode", static_cast<int>(renderMode));
    
    switch (renderMode) {
        case RenderMode::WIREFRAME:
            renderWireframe(model);
            break;
        case RenderMode::SOLID:
            renderSolid(model);
            break;
        case RenderMode::TEXTURED:
            renderTextured(model);
            break;
        case RenderMode::TEXTURED_SHADED:
            renderTexturedShaded(model);
            break;
    }
}

void Renderer::renderWireframe(const Model& model) {
    const auto& vertices = model.getVertices();
    const auto& faces = model.getFaces();
    
    // Model-view-projection matrix
    Eigen::Matrix4f mvp = projectionMatrix * viewMatrix * modelMatrix;
    
    // Draw each face as a wireframe
    for (const auto& face : faces) {
        const auto& vertexIndices = face.vertexIndices;
        
        // Draw edges of the face
        for (size_t i = 0; i < vertexIndices.size(); ++i) {
            size_t j = (i + 1) % vertexIndices.size(); // Next vertex (wrap around for the last one)
            
            // Get 3D vertices
            Eigen::Vector3f v0 = vertices[vertexIndices[i]];
            Eigen::Vector3f v1 = vertices[vertexIndices[j]];
            
            // Convert to homogeneous coordinates
            Eigen::Vector4f v0h(v0.x(), v0.y(), v0.z(), 1.0f);
            Eigen::Vector4f v1h(v1.x(), v1.y(), v1.z(), 1.0f);
            
            // Apply MVP transformation
            Eigen::Vector4f p0 = mvp * v0h;
            Eigen::Vector4f p1 = mvp * v1h;
            
            // Perspective division
            p0 /= p0.w();
            p1 /= p1.w();
            
            // Viewport transformation - FIXED: invert Y coordinate to fix upside-down rendering
            int x0 = static_cast<int>((p0.x() + 1.0f) * 0.5f * width);
            int y0 = static_cast<int>((p0.y() + 1.0f) * 0.5f * height); // Changed from (1.0f - p0.y())
            int x1 = static_cast<int>((p1.x() + 1.0f) * 0.5f * width);
            int y1 = static_cast<int>((p1.y() + 1.0f) * 0.5f * height); // Changed from (1.0f - p1.y())
            
            // Draw the line
            drawLine(x0, y0, x1, y1, 0xFFFFFFFF); // White color
        }
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
        
        // Viewport transformation - FIXED: invert Y coordinate to fix upside-down rendering
        int x0 = static_cast<int>((p0.x() + 1.0f) * 0.5f * width);
        int y0 = static_cast<int>((p0.y() + 1.0f) * 0.5f * height); // Changed from (1.0f - p0.y())
        int x1 = static_cast<int>((p1.x() + 1.0f) * 0.5f * width);
        int y1 = static_cast<int>((p1.y() + 1.0f) * 0.5f * height); // Changed from (1.0f - p1.y())
        int x2 = static_cast<int>((p2.x() + 1.0f) * 0.5f * width);
        int y2 = static_cast<int>((p2.y() + 1.0f) * 0.5f * height); // Changed from (1.0f - p2.y())
        
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
        
        // Draw the triangle
        drawTriangle(x0, y0, x1, y1, x2, y2, color);
    }
}

void Renderer::renderTextured(const Model& model) {
    const auto& vertices = model.getVertices();
    const auto& faces = model.getFaces();
    const auto& textureCoords = model.getTextureCoords();
    const auto& normals = model.getNormals();
    std::shared_ptr<Texture> texture = model.getTexture();
    
    // Check if texture is available
    if (!texture) {
        spdlog::warn("No texture available, falling back to solid rendering");
        renderSolid(model);
        return;
    }
    
    // Model-view-projection matrix
    Eigen::Matrix4f mvp = projectionMatrix * viewMatrix * modelMatrix;
    
    // Direction to light source (for simple diffuse lighting)
    Eigen::Vector3f lightDir = (Eigen::Vector3f(1, 1, 1)).normalized();
    
    // Draw each face as a textured triangle
    for (const auto& face : faces) {
        const auto& vertexIndices = face.vertexIndices;
        const auto& textureIndices = face.textureIndices;
        const auto& normalIndices = face.normalIndices;
        
        // Skip faces that are not triangles
        if (vertexIndices.size() != 3 || textureIndices.size() != 3) {
            continue;
        }
        
        // Get vertices
        Eigen::Vector3f v0 = vertices[vertexIndices[0]];
        Eigen::Vector3f v1 = vertices[vertexIndices[1]];
        Eigen::Vector3f v2 = vertices[vertexIndices[2]];
        
        // Get texture coordinates
        Eigen::Vector2f t0 = textureCoords[textureIndices[0]];
        Eigen::Vector2f t1 = textureCoords[textureIndices[1]];
        Eigen::Vector2f t2 = textureCoords[textureIndices[2]];
        
        // Convert to homogeneous coordinates
        Eigen::Vector4f v0h(v0.x(), v0.y(), v0.z(), 1.0f);
        Eigen::Vector4f v1h(v1.x(), v1.y(), v1.z(), 1.0f);
        Eigen::Vector4f v2h(v2.x(), v2.y(), v2.z(), 1.0f);
        
        // Apply MVP transformation
        Eigen::Vector4f p0 = mvp * v0h;
        Eigen::Vector4f p1 = mvp * v1h;
        Eigen::Vector4f p2 = mvp * v2h;
        
        // Store z-values for depth testing
        float z0 = p0.z() / p0.w();
        float z1 = p1.z() / p1.w();
        float z2 = p2.z() / p2.w();
        
        // Perspective division
        p0 /= p0.w();
        p1 /= p1.w();
        p2 /= p2.w();
        
        // Viewport transformation - FIXED: invert Y coordinate to fix upside-down rendering
        int x0 = static_cast<int>((p0.x() + 1.0f) * 0.5f * width);
        int y0 = static_cast<int>((p0.y() + 1.0f) * 0.5f * height); // Changed from (1.0f - p0.y())
        int x1 = static_cast<int>((p1.x() + 1.0f) * 0.5f * width);
        int y1 = static_cast<int>((p1.y() + 1.0f) * 0.5f * height); // Changed from (1.0f - p1.y())
        int x2 = static_cast<int>((p2.x() + 1.0f) * 0.5f * width);
        int y2 = static_cast<int>((p2.y() + 1.0f) * 0.5f * height); // Changed from (1.0f - p2.y())
        
        // Calculate face normal for simple lighting
        Eigen::Vector3f normal;
        if (normalIndices.empty()) {
            Eigen::Vector3f edge1 = v1 - v0;
            Eigen::Vector3f edge2 = v2 - v0;
            normal = edge1.cross(edge2).normalized();
        } else {
            // Use average of vertex normals
            normal = (normals[normalIndices[0]] + 
                    normals[normalIndices[1]] + 
                    normals[normalIndices[2]]).normalized();
        }
        
        // Simple diffuse lighting intensity
        float intensity = std::max(0.2f, normal.dot(lightDir));
        
        // Draw the textured triangle
        renderTexturedTriangle(
            x0, y0, z0, t0.x(), t0.y(),
            x1, y1, z1, t1.x(), t1.y(),
            x2, y2, z2, t2.x(), t2.y(),
            intensity, *texture);
    }
}

void Renderer::renderTexturedShaded(const Model& model) {
    // For now, this is identical to renderTextured
    renderTextured(model);
}

void Renderer::renderTexturedTriangle(
    int x0, int y0, float z0, float u0, float v0,
    int x1, int y1, float z1, float u1, float v1,
    int x2, int y2, float z2, float u2, float v2,
    float lightIntensity, const Texture& texture) {
    
    // Sort vertices by y-coordinate
    if (y0 > y1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
        std::swap(z0, z1);
        std::swap(u0, u1);
        std::swap(v0, v1);
    }
    if (y0 > y2) {
        std::swap(x0, x2);
        std::swap(y0, y2);
        std::swap(z0, z2);
        std::swap(u0, u2);
        std::swap(v0, v2);
    }
    if (y1 > y2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
        std::swap(z1, z2);
        std::swap(u1, u2);
        std::swap(v1, v2);
    }
    
    // Calculate slopes and interpolation values
    float dy1 = y1 - y0;
    float dy2 = y2 - y0;
    float dy3 = y2 - y1;
    
    // Handle special cases for vertical edges
    float dx1 = (dy1 != 0) ? (x1 - x0) / dy1 : 0;
    float dx2 = (dy2 != 0) ? (x2 - x0) / dy2 : 0;
    float dx3 = (dy3 != 0) ? (x2 - x1) / dy3 : 0;
    
    // Z interpolation
    float dz1 = (dy1 != 0) ? (z1 - z0) / dy1 : 0;
    float dz2 = (dy2 != 0) ? (z2 - z0) / dy2 : 0;
    float dz3 = (dy3 != 0) ? (z2 - z1) / dy3 : 0;
    
    // Texture coordinate interpolation
    float du1 = (dy1 != 0) ? (u1 - u0) / dy1 : 0;
    float du2 = (dy2 != 0) ? (u2 - u0) / dy2 : 0;
    float du3 = (dy3 != 0) ? (u2 - u1) / dy3 : 0;
    
    float dv1 = (dy1 != 0) ? (v1 - v0) / dy1 : 0;
    float dv2 = (dy2 != 0) ? (v2 - v0) / dy2 : 0;
    float dv3 = (dy3 != 0) ? (v2 - v1) / dy3 : 0;
    
    // Draw the upper part of the triangle
    if (dy1 > 0) {
        drawTexturedScanlines(
            y0, y1,
            x0, dx1, x0, dx2,
            z0, dz1, z0, dz2,
            u0, du1, u0, du2,
            v0, dv1, v0, dv2,
            lightIntensity, texture);
    }
    
    // Draw the lower part of the triangle
    if (dy3 > 0) {
        drawTexturedScanlines(
            y1, y2,
            x1, dx3, x0 + dx2 * dy1, dx2,
            z1, dz3, z0 + dz2 * dy1, dz2,
            u1, du3, u0 + du2 * dy1, du2,
            v1, dv3, v0 + dv2 * dy1, dv2,
            lightIntensity, texture);
    }
}

void Renderer::drawTexturedScanlines(
    int yStart, int yEnd,
    float xLeft, float dxLeft, float xRight, float dxRight,
    float zLeft, float dzLeft, float zRight, float dzRight,
    float uLeft, float duLeft, float uRight, float duRight,
    float vLeft, float dvLeft, float vRight, float dvRight,
    float lightIntensity, const Texture& texture) {
    
    for (int y = yStart; y < yEnd; y++) {
        float xL = xLeft;
        float xR = xRight;
        float zL = zLeft;
        float zR = zRight;
        float uL = uLeft;
        float vL = vLeft;
        float uR = uRight;
        float vR = vRight;
        
        // Swap if needed to ensure left to right
        if (xL > xR) {
            std::swap(xL, xR);
            std::swap(zL, zR);
            std::swap(uL, uR);
            std::swap(vL, vR);
        }
        
        // Get starting and ending x-coordinates
        int xStart = static_cast<int>(std::ceil(xL));
        int xEnd = static_cast<int>(std::ceil(xR));
        
        // Calculate interpolation steps along the scanline
        float dx = xR - xL;
        float dz = (dx != 0) ? (zR - zL) / dx : 0;
        float du = (dx != 0) ? (uR - uL) / dx : 0;
        float dv = (dx != 0) ? (vR - vL) / dx : 0;
        
        // Adjust for the first pixel
        float xOffset = xStart - xL;
        float z = zL + dz * xOffset;
        float u = uL + du * xOffset;
        float v = vL + dv * xOffset;
        
        // Draw the scanline
        for (int x = xStart; x < xEnd; x++) {
            // Check if this pixel is in front (using z-buffer)
            int index = y * width + x;
            if (x >= 0 && x < width && y >= 0 && y < height && z < zBuffer[index]) {
                // Get texel color
                uint32_t color = texture.getColorAt(u, v);
                
                // Apply lighting
                uint8_t r = static_cast<uint8_t>(((color >> 16) & 0xFF) * lightIntensity);
                uint8_t g = static_cast<uint8_t>(((color >> 8) & 0xFF) * lightIntensity);
                uint8_t b = static_cast<uint8_t>((color & 0xFF) * lightIntensity);
                uint8_t a = static_cast<uint8_t>((color >> 24) & 0xFF);
                
                uint32_t shadedColor = (a << 24) | (r << 16) | (g << 8) | b;
                
                // Update frame buffer and z-buffer
                frameBuffer[index] = shadedColor;
                zBuffer[index] = z;
            }
            
            // Increment for the next pixel
            z += dz;
            u += du;
            v += dv;
        }
        
        // Update for the next scanline
        xLeft += dxLeft;
        xRight += dxRight;
        zLeft += dzLeft;
        zRight += dzRight;
        uLeft += duLeft;
        uRight += duRight;
        vLeft += dvLeft;
        vRight += dvRight;
    }
}

void Renderer::clearBuffer(uint32_t color) {
    std::fill(frameBuffer.begin(), frameBuffer.end(), color);
    std::fill(zBuffer.begin(), zBuffer.end(), std::numeric_limits<float>::infinity());
}

bool Renderer::saveImage(const std::string& filename) const {
    // Create a texture with the frame buffer data
    Texture texture;
    
    // Set the width, height, and data
    texture.setWidth(width);
    texture.setHeight(height);
    texture.setData(frameBuffer);
    
    // Save the texture to a TGA file
    return texture.saveToTGA(filename);
}

void Renderer::drawLine(int x0, int y0, int x1, int y1, uint32_t color) {
    // Bresenham's line algorithm
    bool steep = false;
    
    // If the line is steep, transpose the coordinates
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    
    // Make sure x0 <= x1
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror2 = std::abs(dy) * 2;
    int error2 = 0;
    int y = y0;
    
    for (int x = x0; x <= x1; x++) {
        if (steep) {
            // If the line is steep, de-transpose
            setPixel(y, x, color);
        } else {
            setPixel(x, y, color);
        }
        
        error2 += derror2;
        if (error2 > dx) {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

void Renderer::drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    // Sort vertices by y-coordinate
    if (y0 > y1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    if (y0 > y2) {
        std::swap(x0, x2);
        std::swap(y0, y2);
    }
    if (y1 > y2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }
    
    // Compute slopes for the three edges
    float dx1 = (x1 - x0) / static_cast<float>(y1 - y0);
    float dx2 = (x2 - x0) / static_cast<float>(y2 - y0);
    float dx3 = (x2 - x1) / static_cast<float>(y2 - y1);
    
    // Handle special case where y1 == y0
    if (y1 == y0) {
        dx1 = 0;
    }
    
    // Handle special case where y2 == y0
    if (y2 == y0) {
        dx2 = 0;
    }
    
    // Handle special case where y2 == y1
    if (y2 == y1) {
        dx3 = 0;
    }
    
    float x_left, x_right;
    
    // Draw the upper part of the triangle
    if (y1 > y0) {
        for (int y = y0; y < y1; y++) {
            x_left = x0 + (y - y0) * dx1;
            x_right = x0 + (y - y0) * dx2;
            
            if (x_left > x_right) {
                std::swap(x_left, x_right);
            }
            
            for (int x = static_cast<int>(x_left); x <= static_cast<int>(x_right); x++) {
                setPixel(x, y, color);
            }
        }
    }
    
    // Draw the lower part of the triangle
    if (y2 > y1) {
        for (int y = y1; y <= y2; y++) {
            x_left = x1 + (y - y1) * dx3;
            x_right = x0 + (y - y0) * dx2;
            
            if (x_left > x_right) {
                std::swap(x_left, x_right);
            }
            
            for (int x = static_cast<int>(x_left); x <= static_cast<int>(x_right); x++) {
                setPixel(x, y, color);
            }
        }
    }
}

void Renderer::setPixel(int x, int y, uint32_t color) {
    // Check if the pixel is within bounds
    if (x >= 0 && x < width && y >= 0 && y < height) {
        frameBuffer[y * width + x] = color;
    }
} 