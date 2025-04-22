#pragma once

#include <vector>
#include <string>
#include <memory>
#include <Eigen/Dense>

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

    /**
     * @brief Constructor that loads a model from an OBJ file
     * @param filename Path to the OBJ file
     */
    explicit Model(const std::string& filename);

    /**
     * @brief Default destructor
     */
    ~Model() = default;

    /**
     * @brief Loads a model from an OBJ file
     * @param filename Path to the OBJ file
     * @return True if loading was successful, false otherwise
     */
    bool loadFromOBJ(const std::string& filename);
    
    /**
     * @brief Sets a texture for the model
     * @param texturePath Path to the TGA texture file
     */
    void setTexture(const std::string& texturePath);

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

private:
    std::vector<Eigen::Vector3f> vertices;      // Vertex positions
    std::vector<Eigen::Vector2f> textureCoords; // Texture coordinates
    std::vector<Eigen::Vector3f> normals;       // Normal vectors
    std::vector<Face> faces;                    // Faces (polygons)
    std::shared_ptr<Texture> texture;           // Texture for the model
}; 