#include "OBJModelLoader.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>
#include <stdexcept>

std::shared_ptr<Model> OBJModelLoader::loadModel(const std::string& filename) {
    spdlog::info("Loading model from OBJ file: {}", filename);
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        spdlog::error("Could not open OBJ file: {}", filename);
        throw std::runtime_error("Failed to open OBJ file: " + filename);
    }
    
    auto model = std::make_shared<Model>();
    std::string line;
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        
        if (prefix == "v") {
            parseVertex(iss, *model);
        } 
        else if (prefix == "vt") {
            parseTextureCoord(iss, *model);
        } 
        else if (prefix == "vn") {
            parseNormal(iss, *model);
        } 
        else if (prefix == "f") {
            parseFace(iss, *model);
        }
    }
    
    file.close();
    
    spdlog::info("Model loaded successfully. Vertices: {}, Texture coords: {}, Normals: {}, Faces: {}", 
        model->getVertices().size(), 
        model->getTextureCoords().size(), 
        model->getNormals().size(), 
        model->getFaces().size());
    
    return model;
}

void OBJModelLoader::parseVertex(std::istringstream& iss, Model& model) {
    float x, y, z;
    iss >> x >> y >> z;
    model.addVertex(Eigen::Vector3f(x, y, z));
}

void OBJModelLoader::parseTextureCoord(std::istringstream& iss, Model& model) {
    float u, v;
    iss >> u >> v;
    model.addTextureCoord(Eigen::Vector2f(u, v));
}

void OBJModelLoader::parseNormal(std::istringstream& iss, Model& model) {
    float x, y, z;
    iss >> x >> y >> z;
    model.addNormal(Eigen::Vector3f(x, y, z));
}

void OBJModelLoader::parseFace(std::istringstream& iss, Model& model) {
    Model::Face face;
    std::string vertexData;
    
    while (iss >> vertexData) {
        parseFaceVertex(vertexData, model, face);
    }
    
    // Only add faces with at least 3 vertices
    if (face.vertexIndices.size() >= 3) {
        model.addFace(face);
    }
}

void OBJModelLoader::parseFaceVertex(const std::string& vertexData, Model& model, Model::Face& face) {
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

int OBJModelLoader::parseIndex(const std::string& indexStr) {
    // OBJ indices are 1-based, convert to 0-based
    return std::stoi(indexStr) - 1;
} 