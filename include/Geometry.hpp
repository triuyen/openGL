#pragma once

#include <GL/glew.h>
#include <vector>

// Structure pour stocker les données d'un vertex
struct Vertex {
    float x, y, z;          // Position
    float nx, ny, nz;       // Normale
    float u, v;             // Coordonnées de texture
};

// Classe pour générer et gérer des géométries
class Geometry {
private:
    GLuint VAO, VBO, EBO;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    bool initialized;

public:
    Geometry();
    ~Geometry();

    // Génération de formes basiques
    void generateSphere(float radius = 1.0f, int sectorCount = 36, int stackCount = 18);
    void generateCube(float size = 1.0f);
    void generatePlane(float width = 1.0f, float height = 1.0f);

    // Formes pour visualiser les lumières
    void generateWireSphere(float radius = 0.2f, int sectorCount = 12, int stackCount = 8);
    void generateCone(float radius = 1.0f, float height = 2.0f, int sectorCount = 16);
    void generateCylinder(float radius = 0.1f, float height = 2.0f, int sectorCount = 8);
    void generateWireCylinder(float radius = 0.1f, float height = 2.0f, int sectorCount = 8);

    // Initialisation des buffers OpenGL
    void setupMesh();

    // Rendu
    void render() const;
    void renderWireframe() const;  // Nouveau: rendu en lignes pour wireframe

    // Nettoyage
    void cleanup();

    // Accesseurs
    const std::vector<Vertex>& getVertices() const { return vertices; }
    const std::vector<unsigned int>& getIndices() const { return indices; }
    bool isInitialized() const { return initialized; }
};