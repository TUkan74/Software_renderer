#include "Texture.h"
#include <spdlog/spdlog.h>
#include <cmath>

/**
 * @brief Generates a test checkerboard texture
 * @param width Width of the texture
 * @param height Height of the texture
 * @param checkerSize Size of the checker pattern
 * @param filename Output filename
 * @return True if successful, false otherwise
 */
bool generateCheckerboardTexture(int width, int height, int checkerSize, const std::string& filename) {
    Texture texture;
    texture.setWidth(width);
    texture.setHeight(height);
    
    // Generate checkerboard pattern
    std::vector<uint32_t> data(width * height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            bool isEvenX = (x / checkerSize) % 2 == 0;
            bool isEvenY = (y / checkerSize) % 2 == 0;
            
            // Create a checker pattern
            uint32_t color;
            if (isEvenX == isEvenY) {
                color = 0xFFFFFFFF; // White
            } else {
                color = 0xFF000000; // Black
            }
            
            data[y * width + x] = color;
        }
    }
    
    texture.setData(data);
    return texture.saveToTGA(filename);
}

/**
 * @brief Generates a test gradient texture
 * @param width Width of the texture
 * @param height Height of the texture
 * @param filename Output filename
 * @return True if successful, false otherwise
 */
bool generateGradientTexture(int width, int height, const std::string& filename) {
    Texture texture;
    texture.setWidth(width);
    texture.setHeight(height);
    
    // Generate gradient pattern
    std::vector<uint32_t> data(width * height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            uint8_t r = static_cast<uint8_t>(255.0f * x / width);
            uint8_t g = static_cast<uint8_t>(255.0f * y / height);
            uint8_t b = static_cast<uint8_t>(255.0f * (1.0f - static_cast<float>(x) / width));
            
            uint32_t color = (0xFF << 24) | (r << 16) | (g << 8) | b;
            data[y * width + x] = color;
        }
    }
    
    texture.setData(data);
    return texture.saveToTGA(filename);
}

// This function can be called from main.cpp to generate test textures
void generateTestTextures() {
    spdlog::info("Generating test textures...");
    
    // Create checkerboard textures with different sizes
    generateCheckerboardTexture(256, 256, 32, "examples/checker_32.tga");
    generateCheckerboardTexture(256, 256, 16, "examples/checker_16.tga");
    
    // Create a gradient texture
    generateGradientTexture(256, 256, "examples/gradient.tga");
    
    spdlog::info("Test textures generated successfully");
} 