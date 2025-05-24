#pragma once
#include "Texture.h"
#include <memory>
#include <string>

class ITextureLoader {
public:
    virtual ~ITextureLoader() = default;
    virtual std::shared_ptr<Texture> loadTexture(const std::string& filename) = 0;
    virtual bool saveTexture(const Texture& texture, const std::string& filename) = 0;
}; 