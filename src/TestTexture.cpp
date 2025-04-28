#include "Texture.h"
#include <spdlog/spdlog.h>
#include <cmath>
#include <fstream>

/**
 * @brief Generates a test checkerboard texture and saves it directly using a robust approach
 * @param width Width of the texture
 * @param height Height of the texture
 * @param checkerSize Size of the checker pattern
 * @param filename Output filename
 * @return True if successful, false otherwise
 */
bool generateCheckerboardTexture(int width, int height, int checkerSize, const std::string& filename) {
    spdlog::info("Generating checkerboard texture {}x{} with checker size {}", width, height, checkerSize);
    
    // Create the texture data
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
    
    // Create and save TGA file directly
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        spdlog::error("Could not open file for writing: {}", filename);
        return false;
    }
    
    // TGA header - Simple uncompressed true color with no color map
    uint8_t header[18] = {0};
    header[2] = 2;  // Uncompressed RGB
    header[12] = width & 0xFF;
    header[13] = (width >> 8) & 0xFF;
    header[14] = height & 0xFF;
    header[15] = (height >> 8) & 0xFF;
    header[16] = 32;  // 32 bits per pixel (RGBA)
    header[17] = 0x28;  // 0x20 = top-left origin, 0x08 = 8 bits alpha
    
    file.write(reinterpret_cast<const char*>(header), sizeof(header));
    
    // Write pixel data
    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            uint32_t pixel = data[y * width + x];
            uint8_t bgra[4];
            bgra[0] = (pixel >> 16) & 0xFF;  // B (from R)
            bgra[1] = (pixel >> 8) & 0xFF;   // G
            bgra[2] = pixel & 0xFF;          // R (from B)
            bgra[3] = (pixel >> 24) & 0xFF;  // A
            
            file.write(reinterpret_cast<const char*>(bgra), 4);
        }
    }
    
    file.close();
    spdlog::info("Checkerboard texture saved successfully to {}", filename);
    return true;
}

/**
 * @brief Generates a test gradient texture and saves it directly
 * @param width Width of the texture
 * @param height Height of the texture
 * @param filename Output filename
 * @return True if successful, false otherwise
 */
bool generateGradientTexture(int width, int height, const std::string& filename) {
    spdlog::info("Generating gradient texture {}x{}", width, height);
    
    // Create the texture data
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
    
    // Create and save TGA file directly
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        spdlog::error("Could not open file for writing: {}", filename);
        return false;
    }
    
    // TGA header - Simple uncompressed true color with no color map
    uint8_t header[18] = {0};
    header[2] = 2;  // Uncompressed RGB
    header[12] = width & 0xFF;
    header[13] = (width >> 8) & 0xFF;
    header[14] = height & 0xFF;
    header[15] = (height >> 8) & 0xFF;
    header[16] = 32;  // 32 bits per pixel (RGBA)
    header[17] = 0x28;  // 0x20 = top-left origin, 0x08 = 8 bits alpha
    
    file.write(reinterpret_cast<const char*>(header), sizeof(header));
    
    // Write pixel data
    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            uint32_t pixel = data[y * width + x];
            uint8_t bgra[4];
            bgra[0] = (pixel >> 16) & 0xFF;  // B (from R)
            bgra[1] = (pixel >> 8) & 0xFF;   // G
            bgra[2] = pixel & 0xFF;          // R (from B)
            bgra[3] = (pixel >> 24) & 0xFF;  // A
            
            file.write(reinterpret_cast<const char*>(bgra), 4);
        }
    }
    
    file.close();
    spdlog::info("Gradient texture saved successfully to {}", filename);
    return true;
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