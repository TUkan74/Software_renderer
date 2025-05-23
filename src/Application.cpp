#include "Application.h"
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <iostream>

Application::Application(int width, int height) : width(width), height(height) {
    spdlog::info("Initializing Application with width={}, height={}", width, height);
}

bool Application::initialize() {
    try {
        renderer = std::make_unique<Renderer>(width, height);
        
        // Set default camera position and target
        renderer->setCameraPosition(Eigen::Vector3f(0, 0, 5));
        renderer->setCameraTarget(Eigen::Vector3f(0, 0, 0));
        
        // Set default rendering mode
        renderer->setRenderMode(RenderMode::WIREFRAME);
        
        spdlog::info("Application initialized successfully");
        return true;
    }
    catch (const std::exception& e) {
        spdlog::error("Failed to initialize application: {}", e.what());
        return false;
    }
}

bool Application::loadModel(const std::string& filename) {
    try {
        model = std::make_shared<Model>(filename);
        spdlog::info("Model loaded successfully");
        return true;
    }
    catch (const std::exception& e) {
        spdlog::error("Failed to load model: {}", e.what());
        return false;
    }
}

bool Application::setTexture(const std::string& filename) {
    if (!model) {
        spdlog::error("No model loaded. Load a model before setting a texture");
        return false;
    }
    
    try {
        model->setTexture(filename);
        spdlog::info("Texture set successfully");
        return true;
    }
    catch (const std::exception& e) {
        spdlog::error("Failed to set texture: {}", e.what());
        return false;
    }
}

void Application::setCameraPosition(float x, float y, float z) {
    if (renderer) {
        renderer->setCameraPosition(Eigen::Vector3f(x, y, z));
        spdlog::info("Camera position set to ({}, {}, {})", x, y, z);
    }
}

void Application::setRenderMode(RenderMode mode) {
    if (renderer) {
        renderer->setRenderMode(mode);
        
        std::string modeName;
        switch (mode) {
            case RenderMode::WIREFRAME:
                modeName = "WIREFRAME";
                break;
            case RenderMode::SOLID:
                modeName = "SOLID";
                break;
            case RenderMode::TEXTURED:
                modeName = "TEXTURED";
                break;
            case RenderMode::TEXTURED_SHADED:
                modeName = "TEXTURED_SHADED";
                break;
            case RenderMode::COLORFUL:
                modeName = "COLORFUL";
                break;
        }
        
        spdlog::info("Render mode set to {}", modeName);
    }
}

bool Application::render() {
    if (!renderer) {
        spdlog::error("Renderer not initialized");
        return false;
    }
    
    if (!model) {
        spdlog::error("No model loaded");
        return false;
    }
    
    try {
        renderer->clearBuffer();
        renderer->render(*model);
        spdlog::info("Model rendered successfully");
        return true;
    }
    catch (const std::exception& e) {
        spdlog::error("Rendering failed: {}", e.what());
        return false;
    }
}

bool Application::saveImage(const std::string& filename) {
    if (!renderer) {
        spdlog::error("Renderer not initialized");
        return false;
    }
    
    try {
        if (renderer->saveImage(filename)) {
            spdlog::info("Image saved successfully to {}", filename);
            return true;
        } else {
            spdlog::error("Failed to save image to {}", filename);
            return false;
        }
    }
    catch (const std::exception& e) {
        spdlog::error("Failed to save image: {}", e.what());
        return false;
    }
}

int Application::getWidth() const {
    return width;
}

int Application::getHeight() const {
    return height;
} 