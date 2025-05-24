#pragma once

#include <vector>
#include <string>
#include <memory>
#include <Eigen/Dense>
#include <sstream>
#include "Texture.h"

// Forward declarations
class Texture;

/**
 * @brief Class representing a 3D model loaded from an OBJ file
 */
class Model {
public:
    /**
     * @brief Represents a face in the model (polygon)
     */
    struct Face {
        std::vector<int> vertexIndices;       // Indices for vertices
        std::vector<int> textureIndices;      // Indices for texture coordinates
        std::vector<int> normalIndices;       // Indices for normal vectors
    };

    Model() = default;
    
    /**
     * @brief Constructs a model from an OBJ file
     * @param filename Path to the OBJ file
     * @throws std::runtime_error if the model fails to load
     */
    explicit Model(const std::string& filename);

    /**
     * @brief Loads model data from an OBJ file
     * @param filename Path to the OBJ file
     * @return true if loading was successful, false otherwise
     */
    bool loadFromOBJ(const std::string& filename);

    /**
     * @brief Gets the vertices of the model
     * @return Vector of vertices (positions in 3D space)
     */
    const std::vector<Eigen::Vector3f>& getVertices() const { return vertices; }
    
    /**
     * @brief Gets the texture coordinates of the model
     * @return Vector of texture coordinates
     */
    const std::vector<Eigen::Vector2f>& getTextureCoords() const { return textureCoords; }
    
    /**
     * @brief Gets the normal vectors of the model
     * @return Vector of normal vectors
     */
    const std::vector<Eigen::Vector3f>& getNormals() const { return normals; }
    
    /**
     * @brief Gets the faces of the model
     * @return Vector of faces
     */
    const std::vector<Face>& getFaces() const { return faces; }
    
    /**
     * @brief Gets the texture associated with the model
     * @return Pointer to the texture
     */
    std::shared_ptr<Texture> getTexture() const { return texture; }

    // Setters
    void setVertices(const std::vector<Eigen::Vector3f>& v) { vertices = v; }
    void setTextureCoords(const std::vector<Eigen::Vector2f>& tc) { textureCoords = tc; }
    void setNormals(const std::vector<Eigen::Vector3f>& n) { normals = n; }
    void setFaces(const std::vector<Face>& f) { faces = f; }
    void setTexture(std::shared_ptr<Texture> t) { texture = t; }
    
    /**
     * @brief Sets the texture for the model from a file
     * @param texturePath Path to the texture file
     * @throws std::runtime_error if the texture fails to load
     */
    void setTexture(const std::string& texturePath);
    
    // Add data
    void addVertex(const Eigen::Vector3f& vertex) { vertices.push_back(vertex); }
    void addTextureCoord(const Eigen::Vector2f& texCoord) { textureCoords.push_back(texCoord); }
    void addNormal(const Eigen::Vector3f& normal) { normals.push_back(normal); }
    void addFace(const Face& face) { faces.push_back(face); }

private:
    // OBJ parsing helper methods
    void parseVertex(std::istringstream& iss);
    void parseTextureCoord(std::istringstream& iss);
    void parseNormal(std::istringstream& iss);
    void parseFace(std::istringstream& iss);
    
    // Face parsing helper methods
    void parseFaceVertex(const std::string& vertexData, Face& face);
    int parseIndex(const std::string& indexStr);
    
    // Data members
    std::vector<Eigen::Vector3f> vertices;      // Vertex positions
    std::vector<Eigen::Vector2f> textureCoords; // Texture coordinates
    std::vector<Eigen::Vector3f> normals;       // Normal vectors
    std::vector<Face> faces;                    // Faces (polygons)
    std::shared_ptr<Texture> texture;           // Texture for the model
}; 