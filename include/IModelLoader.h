#pragma once
#include "Model.h"
#include <memory>
#include <string>

class IModelLoader {
public:
    virtual ~IModelLoader() = default;
    virtual std::shared_ptr<Model> loadModel(const std::string& filename) = 0;
}; 