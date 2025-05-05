#pragma once

#include <memory>
#include <string>
#include <vector>
#include <Eigen/Dense>
#include "Model.h"
#include "Texture.h"

/**
 * @brief Enum defining different rendering modes
 */
enum class RenderMode {
    WIREFRAME,          // Wireframe rendering
    SOLID,              // Solid color rendering
    TEXTURED,           // Textured rendering
    TEXTURED_SHADED     // Textured with shading
};

/**
 * @brief Class for rendering 3D models
 */
class Renderer {
public:
    /**
     * @brief Constructor
     * @param width Width of the output image
     * @param height Height of the output image
     */
    Renderer(int width, int height);
    
    /**
     * @brief Default destructor
     */
    ~Renderer() = default;
    
    /**
     * @brief Sets the camera position
     * @param position Position of the camera
     */
    void setCameraPosition(const Eigen::Vector3f& position);
    
    /**
     * @brief Sets the camera target (where it's looking at)
     * @param target Target position
     */
    void setCameraTarget(const Eigen::Vector3f& target);
    
    /**
     * @brief Sets the rendering mode
     * @param mode Rendering mode
     */
    void setRenderMode(RenderMode mode);
    
    /**
     * @brief Renders a model
     * @param model Model to render
     */
    void render(const Model& model);
    
    /**
     * @brief Clears the frame buffer
     * @param color Color to clear with
     */
    void clearBuffer(uint32_t color = 0);
    
    /**
     * @brief Saves the rendered image to a TGA file
     * @param filename Path to save the image
     * @return True if saving was successful, false otherwise
     */
    bool saveImage(const std::string& filename) const;
    
    /**
     * @brief Gets the width of the output image
     * @return Width in pixels
     */
    int getWidth() const { return width; }
    
    /**
     * @brief Gets the height of the output image
     * @return Height in pixels
     */
    int getHeight() const { return height; }

private:
    /**
     * @brief Updates the view matrix based on camera position and target
     */
    void updateViewMatrix();

    /**
     * @brief Renders a model in wireframe mode
     * @param model Model to render
     */
    void renderWireframe(const Model& model);
    
    /**
     * @brief Renders a model in solid color mode
     * @param model Model to render
     */
    void renderSolid(const Model& model);
    
    /**
     * @brief Renders a model in textured mode
     * @param model Model to render
     */
    void renderTextured(const Model& model);
    
    /**
     * @brief Renders a model in textured and shaded mode
     * @param model Model to render
     */
    void renderTexturedShaded(const Model& model);
    
    /**
     * @brief Draws a textured triangle
     * @param x0, y0, z0 Coordinates of the first vertex
     * @param u0, v0 Texture coordinates of the first vertex
     * @param x1, y1, z1 Coordinates of the second vertex
     * @param u1, v1 Texture coordinates of the second vertex
     * @param x2, y2, z2 Coordinates of the third vertex
     * @param u2, v2 Texture coordinates of the third vertex
     * @param lightIntensity Intensity of the light
     * @param texture Texture to use
     */
    void renderTexturedTriangle(
        int x0, int y0, float z0, float u0, float v0,
        int x1, int y1, float z1, float u1, float v1,
        int x2, int y2, float z2, float u2, float v2,
        float lightIntensity, const Texture& texture);
    
    /**
     * @brief Draws a shaded textured triangle with per-vertex lighting
     * @param x0, y0, z0 Coordinates of the first vertex
     * @param u0, v0 Texture coordinates of the first vertex
     * @param i0 Light intensity at the first vertex
     * @param x1, y1, z1 Coordinates of the second vertex
     * @param u1, v1 Texture coordinates of the second vertex
     * @param i1 Light intensity at the second vertex
     * @param x2, y2, z2 Coordinates of the third vertex
     * @param u2, v2 Texture coordinates of the third vertex
     * @param i2 Light intensity at the third vertex
     * @param texture Texture to use
     */
    void renderShadedTriangle(
        int x0, int y0, float z0, float u0, float v0, float i0,
        int x1, int y1, float z1, float u1, float v1, float i1,
        int x2, int y2, float z2, float u2, float v2, float i2,
        const Texture& texture);
    
    /**
     * @brief Draws textured scanlines for triangle rasterization
     * @param yStart, yEnd Start and end Y coordinates
     * @param xLeft, dxLeft Left edge X and its increment
     * @param xRight, dxRight Right edge X and its increment
     * @param zLeft, dzLeft Left edge Z and its increment
     * @param zRight, dzRight Right edge Z and its increment
     * @param uLeft, duLeft Left edge U and its increment
     * @param uRight, duRight Right edge U and its increment
     * @param vLeft, dvLeft Left edge V and its increment
     * @param vRight, dvRight Right edge V and its increment
     * @param lightIntensity Intensity of the light
     * @param texture Texture to use
     */
    void drawTexturedScanlines(
        int yStart, int yEnd,
        float xLeft, float dxLeft, float xRight, float dxRight,
        float zLeft, float dzLeft, float zRight, float dzRight,
        float uLeft, float duLeft, float uRight, float duRight,
        float vLeft, float dvLeft, float vRight, float dvRight,
        float lightIntensity, const Texture& texture);
        
    /**
     * @brief Draws shaded textured scanlines with per-vertex lighting interpolation
     * @param yStart, yEnd Start and end Y coordinates
     * @param xLeft, dxLeft Left edge X and its increment
     * @param xRight, dxRight Right edge X and its increment
     * @param zLeft, dzLeft Left edge Z and its increment
     * @param zRight, dzRight Right edge Z and its increment
     * @param uLeft, duLeft Left edge U and its increment
     * @param uRight, duRight Right edge U and its increment
     * @param vLeft, dvLeft Left edge V and its increment
     * @param vRight, dvRight Right edge V and its increment
     * @param iLeft, diLeft Left edge lighting intensity and its increment
     * @param iRight, diRight Right edge lighting intensity and its increment
     * @param texture Texture to use
     */
    void drawShadedScanlines(
        int yStart, int yEnd,
        float xLeft, float dxLeft, float xRight, float dxRight,
        float zLeft, float dzLeft, float zRight, float dzRight,
        float uLeft, float duLeft, float uRight, float duRight,
        float vLeft, float dvLeft, float vRight, float dvRight,
        float iLeft, float diLeft, float iRight, float diRight,
        const Texture& texture);
    
    /**
     * @brief Draws a line between two points
     * @param x0 X coordinate of the first point
     * @param y0 Y coordinate of the first point
     * @param x1 X coordinate of the second point
     * @param y1 Y coordinate of the second point
     * @param color Color of the line
     */
    void drawLine(int x0, int y0, int x1, int y1, uint32_t color);
    
    /**
     * @brief Draws a triangle
     * @param x0 X coordinate of the first vertex
     * @param y0 Y coordinate of the first vertex
     * @param x1 X coordinate of the second vertex
     * @param y1 Y coordinate of the second vertex
     * @param x2 X coordinate of the third vertex
     * @param y2 Y coordinate of the third vertex
     * @param color Color of the triangle
     */
    void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
    
    /**
     * @brief Sets a pixel in the frame buffer
     * @param x X coordinate
     * @param y Y coordinate
     * @param color Color of the pixel
     */
    void setPixel(int x, int y, uint32_t color);

    int width;                          // Width of the output image
    int height;                         // Height of the output image
    std::vector<uint32_t> frameBuffer;  // Frame buffer
    std::vector<float> zBuffer;         // Z-buffer for depth testing
    
    Eigen::Vector3f cameraPosition;     // Camera position
    Eigen::Vector3f cameraTarget;       // Camera target
    Eigen::Vector3f cameraUp;           // Camera up vector
    
    Eigen::Matrix4f modelMatrix;        // Model matrix
    Eigen::Matrix4f viewMatrix;         // View matrix
    Eigen::Matrix4f projectionMatrix;   // Projection matrix
    
    RenderMode renderMode;              // Current rendering mode
}; 