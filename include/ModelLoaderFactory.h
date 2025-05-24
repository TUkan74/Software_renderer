#pragma once
#include "IModelLoader.h"
#include <memory>
#include <string>
#include <unordered_map>

class ModelLoaderFactory {
public:
    static std::shared_ptr<IModelLoader> createLoader(const std::string& fileExtension);
    static void registerLoader(const std::string& extension, std::shared_ptr<IModelLoader> loader);
private:
    static std::unordered_map<std::string, std::shared_ptr<IModelLoader>> loaders;
}; 