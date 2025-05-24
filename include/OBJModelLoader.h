#pragma once
#include "IModelLoader.h"

class OBJModelLoader : public IModelLoader {
public:
    std::shared_ptr<Model> loadModel(const std::string& filename) override;
private:
    void parseVertex(std::istringstream& iss, Model& model);
    void parseTextureCoord(std::istringstream& iss, Model& model);
    void parseNormal(std::istringstream& iss, Model& model);
    void parseFace(std::istringstream& iss, Model& model);
    void parseFaceVertex(const std::string& vertexData, Model& model, Model::Face& face);
    int parseIndex(const std::string& indexStr);
}; 