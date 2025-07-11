#include "Geometry.hpp"
#include <cmath>
#include <iostream>

Geometry::Geometry() : VAO(0), VBO(0), EBO(0), initialized(false) {}

Geometry::~Geometry() {
    cleanup();
}

void Geometry::generateSphere(float radius, int sectorCount, int stackCount) {
    vertices.clear();
    indices.clear();

    // Calcul des constantes
    const float PI = 3.14159265359f;
    float sectorStep = 2 * PI / sectorCount;  // Pas horizontal (longitude)
    float stackStep = PI / stackCount;        // Pas vertical (latitude)

    // Génération des vertices
    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = PI / 2 - i * stackStep;  // Angle de -π/2 à +π/2
        float xy = radius * cosf(stackAngle);       // Rayon du cercle horizontal
        float z = radius * sinf(stackAngle);        // Hauteur

        // Vertices pour chaque secteur du stack actuel
        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * sectorStep;     // Angle de 0 à 2π

            Vertex vertex;
            // Position
            vertex.x = xy * cosf(sectorAngle);
            vertex.y = xy * sinf(sectorAngle);
            vertex.z = z;

            // Normale (pour une sphère centrée à l'origine, la normale = position normalisée)
            float length = sqrtf(vertex.x * vertex.x + vertex.y * vertex.y + vertex.z * vertex.z);
            vertex.nx = vertex.x / length;
            vertex.ny = vertex.y / length;
            vertex.nz = vertex.z / length;

            // Coordonnées de texture
            vertex.u = (float)j / sectorCount;
            vertex.v = (float)i / stackCount;

            vertices.push_back(vertex);
        }
    }

    // Génération des indices pour les triangles
    for (int i = 0; i < stackCount; ++i) {
        int k1 = i * (sectorCount + 1);        // Début du stack actuel
        int k2 = k1 + sectorCount + 1;         // Début du stack suivant

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            // Chaque quad est divisé en 2 triangles
            if (i != 0) {
                // Triangle 1: k1, k2, k1+1
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stackCount - 1)) {
                // Triangle 2: k1+1, k2, k2+1
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    setupMesh();
}

void Geometry::generateCube(float size) {
    vertices.clear();
    indices.clear();

    float half = size * 0.5f;

    // Définition des 8 sommets du cube
    Vertex cubeVertices[24]; // 6 faces * 4 vertices par face

    // Face avant (Z+)
    cubeVertices[0] = {-half, -half,  half,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f};
    cubeVertices[1] = { half, -half,  half,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f};
    cubeVertices[2] = { half,  half,  half,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f};
    cubeVertices[3] = {-half,  half,  half,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f};

    // Face arrière (Z-)
    cubeVertices[4] = { half, -half, -half,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f};
    cubeVertices[5] = {-half, -half, -half,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f};
    cubeVertices[6] = {-half,  half, -half,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f};
    cubeVertices[7] = { half,  half, -half,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f};

    // Face gauche (X-)
    cubeVertices[8]  = {-half, -half, -half, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f};
    cubeVertices[9]  = {-half, -half,  half, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f};
    cubeVertices[10] = {-half,  half,  half, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f};
    cubeVertices[11] = {-half,  half, -half, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f};

    // Face droite (X+)
    cubeVertices[12] = { half, -half,  half,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f};
    cubeVertices[13] = { half, -half, -half,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f};
    cubeVertices[14] = { half,  half, -half,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f};
    cubeVertices[15] = { half,  half,  half,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f};

    // Face bas (Y-)
    cubeVertices[16] = {-half, -half, -half,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f};
    cubeVertices[17] = { half, -half, -half,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f};
    cubeVertices[18] = { half, -half,  half,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f};
    cubeVertices[19] = {-half, -half,  half,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f};

    // Face haut (Y+)
    cubeVertices[20] = {-half,  half,  half,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f};
    cubeVertices[21] = { half,  half,  half,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f};
    cubeVertices[22] = { half,  half, -half,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f};
    cubeVertices[23] = {-half,  half, -half,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f};

    // Ajouter les vertices
    for (int i = 0; i < 24; ++i) {
        vertices.push_back(cubeVertices[i]);
    }

    // Indices pour les triangles (2 triangles par face)
    unsigned int cubeIndices[] = {
            // Face avant
            0,  1,  2,   2,  3,  0,
            // Face arrière
            4,  5,  6,   6,  7,  4,
            // Face gauche
            8,  9, 10,  10, 11,  8,
            // Face droite
            12, 13, 14,  14, 15, 12,
            // Face bas
            16, 17, 18,  18, 19, 16,
            // Face haut
            20, 21, 22,  22, 23, 20
    };

    for (int i = 0; i < 36; ++i) {
        indices.push_back(cubeIndices[i]);
    }

    setupMesh();
}

void Geometry::generatePlane(float width, float height) {
    vertices.clear();
    indices.clear();

    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;

    // 4 vertices pour le plan
    Vertex planeVertices[4] = {
            {-halfWidth, 0.0f, -halfHeight,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f},  // Bas-gauche
            { halfWidth, 0.0f, -halfHeight,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f},  // Bas-droite
            { halfWidth, 0.0f,  halfHeight,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f},  // Haut-droite
            {-halfWidth, 0.0f,  halfHeight,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f}   // Haut-gauche
    };

    for (int i = 0; i < 4; ++i) {
        vertices.push_back(planeVertices[i]);
    }

    // Indices pour 2 triangles
    unsigned int planeIndices[] = {
            0, 1, 2,    // Premier triangle
            2, 3, 0     // Deuxième triangle
    };

    for (int i = 0; i < 6; ++i) {
        indices.push_back(planeIndices[i]);
    }

    setupMesh();
}

void Geometry::setupMesh() {
    if (vertices.empty()) return;

    // Nettoyer les anciens buffers s'ils existent
    cleanup();

    // Générer et lier les buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    // Buffer des vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Buffer des indices (si présent)
    if (!indices.empty()) {
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }

    // Attribut 0: Position (x, y, z)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Attribut 1: Normale (nx, ny, nz)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx));

    // Attribut 2: Coordonnées de texture (u, v)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));

    glBindVertexArray(0);

    initialized = true;
}

void Geometry::render() const {
    if (!initialized) return;

    glBindVertexArray(VAO);

    if (!indices.empty()) {
        // Rendu avec indices
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    } else {
        // Rendu sans indices
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
    }

    glBindVertexArray(0);
}

void Geometry::cleanup() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (EBO != 0) {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
    initialized = false;
}