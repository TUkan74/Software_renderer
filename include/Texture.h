#pragma once

#include <string>
#include <vector>
#include <cstdint>

/**
 * @brief Class representing a texture loaded from a TGA file
 */
class Texture {
public:
    /**
     * @brief Default constructor
     */
    Texture() = default;
    
    /**
     * @brief Constructor that loads a texture from a TGA file
     * @param filename Path to the TGA file
     */
    explicit Texture(const std::string& filename);
    
    /**
     * @brief Default destructor
     */
    ~Texture() = default;
    
    /**
     * @brief Loads a texture from a TGA file
     * @param filename Path to the TGA file
     * @return True if loading was successful, false otherwise
     */
    bool loadFromTGA(const std::string& filename);
    
    /**
     * @brief Saves the texture to a TGA file
     * @param filename Path to save the TGA file
     * @return True if saving was successful, false otherwise
     */
    bool saveToTGA(const std::string& filename) const;
    
    /**
     * @brief Gets the color at the specified texture coordinates
     * @param u Horizontal texture coordinate (0.0 to 1.0)
     * @param v Vertical texture coordinate (0.0 to 1.0)
     * @return Color at the specified texture coordinates as RGBA
     */
    uint32_t getColorAt(float u, float v) const;
    
    /**
     * @brief Gets the width of the texture
     * @return Width in pixels
     */
    int getWidth() const { return width; }
    
    /**
     * @brief Gets the height of the texture
     * @return Height in pixels
     */
    int getHeight() const { return height; }
    
    /**
     * @brief Gets the raw pixel data
     * @return Pointer to the pixel data
     */
    const std::vector<uint32_t>& getData() const { return data; }

    /**
     * @brief Sets the width of the texture
     * @param w Width in pixels
     */
    void setWidth(int w) { width = w; }
    
    /**
     * @brief Sets the height of the texture
     * @param h Height in pixels
     */
    void setHeight(int h) { height = h; }
    
    /**
     * @brief Sets the raw pixel data
     * @param pixelData Vector of pixel data in RGBA format
     */
    void setData(const std::vector<uint32_t>& pixelData) { data = pixelData; }

private:
    int width = 0;                  // Width of the texture in pixels
    int height = 0;                 // Height of the texture in pixels
    std::vector<uint32_t> data;     // Pixel data (RGBA format, 8 bits per channel)
}; 