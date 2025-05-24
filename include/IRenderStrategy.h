#pragma once
#include "Model.h"
#include "Renderer.h"

class IRenderStrategy {
public:
    virtual ~IRenderStrategy() = default;
    virtual void render(const Model& model, Renderer& renderer) = 0;
    virtual std::string getName() const = 0;
}; 