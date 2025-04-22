#pragma once

#include <memory>
#include <string>
#include "Model.h"
#include "Renderer.h"

/**
 * @brief Main application class for the software renderer
 */
class Application {
public:
    /**
     * @brief Constructor
     * @param width Width of the output image
     * @param height Height of the output image
     */
    Application(int width, int height);
    
    /**
     * @brief Default destructor
     */
    ~Application() = default;
    
    /**
     * @brief Initializes the application
     * @return True if initialization was successful, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Loads a 3D model
     * @param filename Path to the OBJ file
     * @return True if loading was successful, false otherwise
     */
    bool loadModel(const std::string& filename);
    
    /**
     * @brief Sets a texture for the model
     * @param filename Path to the TGA texture file
     * @return True if loading was successful, false otherwise
     */
    bool setTexture(const std::string& filename);
    
    /**
     * @brief Sets the camera position
     * @param x X coordinate
     * @param y Y coordinate
     * @param z Z coordinate
     */
    void setCameraPosition(float x, float y, float z);
    
    /**
     * @brief Sets the rendering mode
     * @param mode Rendering mode
     */
    void setRenderMode(RenderMode mode);
    
    /**
     * @brief Renders the current model
     * @return True if rendering was successful, false otherwise
     */
    bool render();
    
    /**
     * @brief Saves the rendered image to a file
     * @param filename Path to save the image
     * @return True if saving was successful, false otherwise
     */
    bool saveImage(const std::string& filename);
    
    /**
     * @brief Gets the width of the output image
     * @return Width in pixels
     */
    int getWidth() const;
    
    /**
     * @brief Gets the height of the output image
     * @return Height in pixels
     */
    int getHeight() const;

private:
    int width;                              // Width of the output image
    int height;                             // Height of the output image
    std::shared_ptr<Model> model;           // Current 3D model
    std::unique_ptr<Renderer> renderer;     // Renderer
}; 