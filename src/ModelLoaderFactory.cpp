#include "ModelLoaderFactory.h"
#include "OBJModelLoader.h"
#include <spdlog/spdlog.h>

std::unordered_map<std::string, std::shared_ptr<IModelLoader>> ModelLoaderFactory::loaders;

std::shared_ptr<IModelLoader> ModelLoaderFactory::createLoader(const std::string& fileExtension) {
    auto it = loaders.find(fileExtension);
    if (it != loaders.end()) {
        return it->second;
    }
    
    spdlog::error("No loader registered for extension: {}", fileExtension);
    return nullptr;
}

void ModelLoaderFactory::registerLoader(const std::string& extension, std::shared_ptr<IModelLoader> loader) {
    loaders[extension] = loader;
    spdlog::info("Registered model loader for extension: {}", extension);
}

// Register default loaders
namespace {
    struct DefaultLoaderRegistration {
        DefaultLoaderRegistration() {
            ModelLoaderFactory::registerLoader("obj", std::make_shared<OBJModelLoader>());
        }
    } defaultLoaderRegistration;
} 