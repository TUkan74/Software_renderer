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

bool Texture::loadRLEData(std::ifstream& in,
                          int width, int height,
                          int bytespp,
                          uint8_t imageDescriptor)
{
    size_t pixelCount = size_t(width) * size_t(height);
    size_t currentPixel = 0;
    std::vector<uint8_t> raw(bytespp);

    while (currentPixel < pixelCount) {
        uint8_t chunkHeader = 0;
        in.read(reinterpret_cast<char*>(&chunkHeader), 1);
        if (!in.good()) return false;

        if (chunkHeader < 128) {
            // Raw packet: next (chunkHeader+1) pixels
            int count = chunkHeader + 1;
            for (int i = 0; i < count; i++) {
                in.read(reinterpret_cast<char*>(raw.data()), bytespp);
                if (!in.good()) return false;

                // build RGBA
                uint8_t b = raw[0], g = raw[1], r = (bytespp>2? raw[2] : raw[0]);
                uint8_t a = (bytespp==4 ? raw[3] : 0xFF);
                uint32_t pixelColor = (uint32_t(a) << 24)
                                    | (uint32_t(r) << 16)
                                    | (uint32_t(g) << 8)
                                    |  uint32_t(b);

                // compute destination index, honoring top-bit of imageDescriptor
                size_t y = currentPixel / width;
                size_t x = currentPixel % width;
                size_t idx = (imageDescriptor & 0x20)
                            ? (y * width + x)
                            : ((height - 1 - y) * width + x);

                data[idx] = pixelColor;
                currentPixel++;
            }
        } else {
            // RLE packet: next pixel repeated (chunkHeader - 127) times
            int count = chunkHeader - 127;
            in.read(reinterpret_cast<char*>(raw.data()), bytespp);
            if (!in.good()) return false;

            uint8_t b = raw[0], g = raw[1], r = (bytespp>2? raw[2] : raw[0]);
            uint8_t a = (bytespp==4 ? raw[3] : 0xFF);
            uint32_t pixelColor = (uint32_t(a) << 24)
                                | (uint32_t(r) << 16)
                                | (uint32_t(g) << 8)
                                |  uint32_t(b);

            for (int i = 0; i < count; i++) {
                size_t y = currentPixel / width;
                size_t x = currentPixel % width;
                size_t idx = (imageDescriptor & 0x20)
                            ? (y * width + x)
                            : ((height - 1 - y) * width + x);

                data[idx] = pixelColor;
                currentPixel++;
            }
        }
    }
    return true;
}


bool Texture::loadFromTGA(const std::string& filename) {
    spdlog::info("Loading texture from TGA file: {}", filename);

    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        spdlog::error("Could not open TGA file: {}", filename);
        return false;
    }

    // === PACKED HEADER ===
    #pragma pack(push,1)
    struct TGAHeader {
        uint8_t  idLength;
        uint8_t  colorMapType;
        uint8_t  imageType;
        uint16_t colorMapStart;
        uint16_t colorMapLength;
        uint8_t  colorMapDepth;
        uint16_t xOrigin;
        uint16_t yOrigin;
        uint16_t width;
        uint16_t height;
        uint8_t  bitsPerPixel;
        uint8_t  imageDescriptor;
    };
    #pragma pack(pop)

    TGAHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    if (!file.good()) {
        spdlog::error("Failed to read TGA header from file: {}", filename);
        return false;
    }

    spdlog::info(
      "TGA Header - type={}  {}x{}  bpp={}  desc=0x{:02X}",
      header.imageType,
      header.width, header.height,
      header.bitsPerPixel,
      header.imageDescriptor
    );

    // skip ID field
    if (header.idLength)
        file.seekg(header.idLength, std::ios::cur);

    // basic sanity checks
    if (header.width == 0 || header.height == 0) {
        spdlog::error("Bad dimensions {}x{}", header.width, header.height);
        return false;
    }
    int bytespp = header.bitsPerPixel / 8;
    if (header.imageType == 2) {
        if (bytespp!=3 && bytespp!=4) {
            spdlog::error("Unsupported bpp={} for type2", header.bitsPerPixel);
            return false;
        }
    } else if (header.imageType == 3) {
        if (bytespp!=1) {
            spdlog::error("Unsupported bpp={} for type3", header.bitsPerPixel);
            return false;
        }
    } else if (header.imageType != 10) {
        spdlog::error("Unsupported TGA type {} (only 2,3,10)", header.imageType);
        return false;
    }

    width  = header.width;
    height = header.height;
    data.clear();
    data.resize(size_t(width)*height);

    // dispatch RLE vs uncompressed
    if (header.imageType == 10) {
        spdlog::info("Decoding RLE-compressed data...");
        if (!loadRLEData(file, width, height, bytespp, header.imageDescriptor)) {
            spdlog::error("RLE decode failed");
            return false;
        }
    }
    else {
        // uncompressed
        spdlog::info("Reading uncompressed data...");
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                uint8_t buf[4] = {0,0,0,255};
                if (header.imageType == 3) {
                    // grayscale
                    uint8_t g;
                    file.read((char*)&g,1);
                    buf[0]=buf[1]=buf[2]=g;
                } else {
                    // RGB/RGBA
                    file.read((char*)buf, bytespp);
                    // BGR(A) → RGB(A)
                    std::swap(buf[0], buf[2]);
                }
                size_t row = (header.imageDescriptor & 0x20)
                             ? y
                             : (height-1 - y);
                size_t idx = row*width + x;
                data[idx] =
                  (uint32_t(buf[3])<<24) |
                  (uint32_t(buf[2])<<16) |
                  (uint32_t(buf[1])<< 8) |
                   uint32_t(buf[0]);
            }
        }
    }

    file.close();
    spdlog::info("Texture loaded: {}x{}", width, height);
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
    
    // Write pixel data - TGA expects BGRA format
    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            uint32_t pixel = data[y * width + x];
            uint8_t bgra[4];
            // Our framebuffer is in ARGB format (0xAARRGGBB)
            bgra[0] = pixel & 0xFF;             // B (from B in our format)
            bgra[1] = (pixel >> 8) & 0xFF;      // G (from G in our format)
            bgra[2] = (pixel >> 16) & 0xFF;     // R (from R in our format)
            bgra[3] = (pixel >> 24) & 0xFF;     // A
            
            file.write(reinterpret_cast<const char*>(bgra), 4);
        }
    }
    
    file.close();
    spdlog::info("Texture saved successfully to {}", filename);
    return true;
}

uint32_t Texture::getColorAt(float u, float v) const {
    if (data.empty() || width == 0 || height == 0) {
        spdlog::warn("Attempted to sample from an empty texture");
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