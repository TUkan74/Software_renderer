#pragma once
#include "ITextureLoader.h"

class TGATextureLoader : public ITextureLoader {
public:
    std::shared_ptr<Texture> loadTexture(const std::string& filename) override;
    bool saveTexture(const Texture& texture, const std::string& filename) override;
}; 