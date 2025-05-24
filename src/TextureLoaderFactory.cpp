#include "TextureLoaderFactory.h"
#include <spdlog/spdlog.h>

std::unordered_map<std::string, std::shared_ptr<ITextureLoader>> TextureLoaderFactory::loaders;

std::shared_ptr<ITextureLoader> TextureLoaderFactory::createLoader(const std::string& fileExtension) {
    auto it = loaders.find(fileExtension);
    if (it != loaders.end()) {
        return it->second;
    }
    
    spdlog::error("No texture loader registered for extension: {}", fileExtension);
    return nullptr;
}

void TextureLoaderFactory::registerLoader(const std::string& extension, std::shared_ptr<ITextureLoader> loader) {
    loaders[extension] = loader;
    spdlog::info("Registered texture loader for extension: {}", extension);
} 