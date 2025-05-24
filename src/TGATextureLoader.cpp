#include "TGATextureLoader.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <stdexcept>

std::shared_ptr<Texture> TGATextureLoader::loadTexture(const std::string& filename) {
    try {
        auto texture = std::make_shared<Texture>(filename);
        return texture;
    } catch (const std::exception& e) {
        spdlog::error("Failed to load TGA texture: {}", e.what());
        throw;
    }
}

bool TGATextureLoader::saveTexture(const Texture& texture, const std::string& filename) {
    try {
        return texture.saveToTGA(filename);
    } catch (const std::exception& e) {
        spdlog::error("Failed to save TGA texture: {}", e.what());
        return false;
    }
} 