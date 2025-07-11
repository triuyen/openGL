#pragma once

#include <vector>
#include <memory>
#include <GL/glew.h>

struct Vertex {
    float position[3];
    float normal[3];
    float texCoords[2];
};

class Mesh {
private:
    GLuint VAO, VBO, EBO;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    void setupMesh();

public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    void render() const;

    // Factory methods pour créer des primitives
    static std::shared_ptr<Mesh> CreateCube();
    static std::shared_ptr<Mesh> CreateSphere(int segments = 32, int rings = 16);
    static std::shared_ptr<Mesh> CreatePlane();

    // Getters
    const std::vector<Vertex>& getVertices() const { return vertices; }
    const std::vector<unsigned int>& getIndices() const { return indices; }
};