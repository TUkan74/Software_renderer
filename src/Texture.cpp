#include "Texture.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <cstring>

Texture::Texture(const std::string& filename) {
    if (!loadFromTGA(filename)) {
        throw std::runtime_error("Failed to load texture from " + filename);
    }
}

bool Texture::loadFromTGA(const std::string& filename) {
    spdlog::info("Loading texture from TGA file: {}", filename);
    
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        spdlog::error("Could not open TGA file: {}", filename);
        return false;
    }
    
    // TGA header structure
    struct TGAHeader {
        uint8_t idLength;          // Length of ID field
        uint8_t colorMapType;      // Color map type
        uint8_t imageType;         // Image type
        uint16_t colorMapStart;    // First color map entry
        uint16_t colorMapLength;   // Color map length
        uint8_t colorMapDepth;     // Color map entry size
        uint16_t xOrigin;          // X origin of image
        uint16_t yOrigin;          // Y origin of image
        uint16_t width;            // Width of image
        uint16_t height;           // Height of image
        uint8_t bitsPerPixel;      // Bits per pixel
        uint8_t imageDescriptor;   // Image descriptor
    };
    
    TGAHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    
    if (!file.good()) {
        spdlog::error("Failed to read TGA header from file: {}", filename);
        file.close();
        return false;
    }
    
    // Log header information for debugging
    spdlog::debug("TGA Header - imageType: {}, width: {}, height: {}, bitsPerPixel: {}", 
        header.imageType, header.width, header.height, header.bitsPerPixel);
    
    // Skip the ID field
    if (header.idLength > 0) {
        file.seekg(header.idLength, std::ios::cur);
    }
    
    // Check for file validity
    if (header.width <= 0 || header.height <= 0) {
        spdlog::error("Invalid TGA dimensions: {}x{}", header.width, header.height);
        file.close();
        return false;
    }
    
    // Currently supporting uncompressed RGB/RGBA images (type 2) and grayscale (type 3)
    if (header.imageType != 2 && header.imageType != 3) {
        spdlog::error("Unsupported TGA image type: {}", header.imageType);
        file.close();
        return false;
    }
    
    // Check bits per pixel
    if (header.bitsPerPixel == 0) {
        spdlog::error("Invalid bits per pixel: 0. File may be corrupted or empty.");
        file.close();
        return false;
    }
    
    // Supporting 8-bit (grayscale), 24-bit (RGB) or 32-bit (RGBA) images
    if (header.imageType == 2 && header.bitsPerPixel != 24 && header.bitsPerPixel != 32) {
        spdlog::error("Unsupported bits per pixel: {} for RGB/RGBA image", header.bitsPerPixel);
        file.close();
        return false;
    }
    
    if (header.imageType == 3 && header.bitsPerPixel != 8) {
        spdlog::error("Unsupported bits per pixel: {} for grayscale image", header.bitsPerPixel);
        file.close();
        return false;
    }
    
    width = header.width;
    height = header.height;
    data.resize(width * height, 0);  // Initialize with black
    
    // Read the pixel data
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint8_t pixel[4] = {0, 0, 0, 255}; // Default to black with full alpha
            
            try {
                // Read pixel data based on image type and bits per pixel
                if (header.imageType == 2) {  // RGB/RGBA
                    file.read(reinterpret_cast<char*>(pixel), header.bitsPerPixel / 8);
                    
                    // For 24-bit, pixel data is BGR; for 32-bit, it's BGRA
                    if (header.bitsPerPixel == 24) {
                        // BGR to RGBA
                        std::swap(pixel[0], pixel[2]);  // Swap B and R
                    } else if (header.bitsPerPixel == 32) {
                        // BGRA to RGBA
                        std::swap(pixel[0], pixel[2]);  // Swap B and R
                    }
                } else if (header.imageType == 3) {  // Grayscale
                    uint8_t gray;
                    file.read(reinterpret_cast<char*>(&gray), 1);
                    pixel[0] = pixel[1] = pixel[2] = gray;
                }
                
                if (!file.good()) {
                    spdlog::error("Error reading pixel data at position ({}, {})", x, y);
                    file.close();
                    return false;
                }
                
                // Store the pixel in the data array
                int pixelIndex = (header.imageDescriptor & 0x20) ? 
                    (y * width + x) : ((height - 1 - y) * width + x);
                data[pixelIndex] = (pixel[3] << 24) | (pixel[0] << 16) | (pixel[1] << 8) | pixel[2];
            }
            catch (const std::exception& e) {
                spdlog::error("Exception while reading pixel at ({}, {}): {}", x, y, e.what());
                file.close();
                return false;
            }
        }
    }
    
    file.close();
    
    spdlog::info("Texture loaded successfully. Width: {}, Height: {}", width, height);
    return true;
}

bool Texture::saveToTGA(const std::string& filename) const {
    spdlog::info("Saving texture to TGA file: {}", filename);
    
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        spdlog::error("Could not open file for writing: {}", filename);
        return false;
    }
    
    // TGA header
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
    spdlog::info("Texture saved successfully to {}", filename);
    return true;
}

uint32_t Texture::getColorAt(float u, float v) const {
    if (data.empty() || width == 0 || height == 0) {
        return 0xFF000000;  // Black with full alpha
    }
    
    // Wrap texture coordinates
    u = u - std::floor(u);
    v = v - std::floor(v);
    
    // Convert texture coordinates to pixel coordinates
    int x = static_cast<int>(u * width);
    int y = static_cast<int>(v * height);
    
    // Clamp to texture bounds
    x = std::clamp(x, 0, width - 1);
    y = std::clamp(y, 0, height - 1);
    
    return data[y * width + x];
} 