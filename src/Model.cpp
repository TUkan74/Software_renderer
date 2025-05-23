#include "Model.h"
#include "Texture.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <stdexcept>
#include <iostream>

Model::Model(const std::string& filename) {
    if (!loadFromOBJ(filename)) {
        throw std::runtime_error("Failed to load model from " + filename);
    }
}

bool Model::loadFromOBJ(const std::string& filename) {
    spdlog::info("Loading model from OBJ file: {}", filename);
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        spdlog::error("Could not open OBJ file: {}", filename);
        return false;
    }
    
    // Clear existing data
    vertices.clear();
    textureCoords.clear();
    normals.clear();
    faces.clear();
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        
        if (prefix == "v") {
            parseVertex(iss);
        } 
        else if (prefix == "vt") {
            parseTextureCoord(iss);
        } 
        else if (prefix == "vn") {
            parseNormal(iss);
        } 
        else if (prefix == "f") {
            parseFace(iss);
        }
    }
    
    file.close();
    
    spdlog::info("Model loaded successfully. Vertices: {}, Texture coords: {}, Normals: {}, Faces: {}", 
        vertices.size(), textureCoords.size(), normals.size(), faces.size());
    
    return !vertices.empty() && !faces.empty();
}

void Model::parseVertex(std::istringstream& iss) {
    float x, y, z;
    iss >> x >> y >> z;
    vertices.emplace_back(x, y, z);
}

void Model::parseTextureCoord(std::istringstream& iss) {
    float u, v;
    iss >> u >> v;
    textureCoords.emplace_back(u, v);
}

void Model::parseNormal(std::istringstream& iss) {
    float x, y, z;
    iss >> x >> y >> z;
    normals.emplace_back(x, y, z);
}

void Model::parseFace(std::istringstream& iss) {
    Face face;
    std::string vertexData;
    
    while (iss >> vertexData) {
        parseFaceVertex(vertexData, face);
    }
    
    // Only add faces with at least 3 vertices
    if (face.vertexIndices.size() >= 3) {
        faces.push_back(face);
    }
}

void Model::parseFaceVertex(const std::string& vertexData, Face& face) {
    std::istringstream viss(vertexData);
    std::string indexStr;
    
    // Parse vertex index
    std::getline(viss, indexStr, '/');
    if (!indexStr.empty()) {
        face.vertexIndices.push_back(parseIndex(indexStr));
    }
    
    // Parse texture coordinate index
    std::getline(viss, indexStr, '/');
    if (!indexStr.empty()) {
        face.textureIndices.push_back(parseIndex(indexStr));
    }
    
    // Parse normal vector index
    std::getline(viss, indexStr, '/');
    if (!indexStr.empty()) {
        face.normalIndices.push_back(parseIndex(indexStr));
    }
}

int Model::parseIndex(const std::string& indexStr) {
    // OBJ indices are 1-based, convert to 0-based
    return std::stoi(indexStr) - 1;
}

void Model::setTexture(const std::string& texturePath) {
    try {
        spdlog::info("Loading texture from {}", texturePath);
        texture = std::make_shared<Texture>(texturePath);
        
        // Basic validation of texture coordinates
        if (textureCoords.empty() && !faces.empty()) {
            spdlog::warn("Model has faces but no texture coordinates. Texture mapping may not work correctly.");
        }
        
        // Check if any face has texture indices
        bool hasTextureIndices = false;
        for (const auto& face : faces) {
            if (!face.textureIndices.empty()) {
                hasTextureIndices = true;
                break;
            }
        }
        
        if (!hasTextureIndices) {
            spdlog::warn("Model has no texture indices in faces. Texture mapping may not work correctly.");
        }
        
        spdlog::info("Texture loaded successfully: {}x{}", texture->getWidth(), texture->getHeight());
    } catch (const std::exception& e) {
        spdlog::error("Failed to load texture: {}", e.what());
        throw;
    }
} 