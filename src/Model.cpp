#include "Model.h"
#include "Texture.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <stdexcept>

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
            // Vertex
            float x, y, z;
            iss >> x >> y >> z;
            vertices.emplace_back(x, y, z);
        } 
        else if (prefix == "vt") {
            // Texture coordinate
            float u, v;
            iss >> u >> v;
            textureCoords.emplace_back(u, v);
        } 
        else if (prefix == "vn") {
            // Normal vector
            float x, y, z;
            iss >> x >> y >> z;
            normals.emplace_back(x, y, z);
        } 
        else if (prefix == "f") {
            // Face
            Face face;
            std::string vertexData;
            
            while (iss >> vertexData) {
                std::istringstream viss(vertexData);
                std::string indexStr;
                
                // Parse vertex index
                std::getline(viss, indexStr, '/');
                if (!indexStr.empty()) {
                    // OBJ indices are 1-based, convert to 0-based
                    face.vertexIndices.push_back(std::stoi(indexStr) - 1);
                }
                
                // Parse texture coordinate index
                std::getline(viss, indexStr, '/');
                if (!indexStr.empty()) {
                    face.textureIndices.push_back(std::stoi(indexStr) - 1);
                }
                
                // Parse normal vector index
                std::getline(viss, indexStr, '/');
                if (!indexStr.empty()) {
                    face.normalIndices.push_back(std::stoi(indexStr) - 1);
                }
            }
            
            // Only add faces with at least 3 vertices
            if (face.vertexIndices.size() >= 3) {
                faces.push_back(face);
            }
        }
    }
    
    file.close();
    
    spdlog::info("Model loaded successfully. Vertices: {}, Texture coords: {}, Normals: {}, Faces: {}", 
        vertices.size(), textureCoords.size(), normals.size(), faces.size());
    
    return !vertices.empty() && !faces.empty();
}

void Model::setTexture(const std::string& texturePath) {
    spdlog::info("Setting texture: {}", texturePath);
    texture = std::make_shared<Texture>(texturePath);
} 