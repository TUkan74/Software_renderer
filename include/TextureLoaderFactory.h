#pragma once
#include "ITextureLoader.h"
#include <memory>
#include <string>
#include <unordered_map>

class TextureLoaderFactory {
public:
    static std::shared_ptr<ITextureLoader> createLoader(const std::string& fileExtension);
    static void registerLoader(const std::string& extension, std::shared_ptr<ITextureLoader> loader);
private:
    static std::unordered_map<std::string, std::shared_ptr<ITextureLoader>> loaders;
}; 