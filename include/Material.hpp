#pragma once

#include <glm/glm.hpp>

// Enum pour les types de matériaux
enum class MaterialType {
    PLASTIC,
    METAL,
    RUBBER,
    WOOD
};

// Structure pour définir les propriétés d'un matériau
struct Material {
    glm::vec3 ambient;      // Composante ambiante
    glm::vec3 diffuse;      // Composante diffuse
    glm::vec3 specular;     // Composante spéculaire
    float shininess;        // Brillance (exposant spéculaire)

    // Constructeur par défaut
    Material() : ambient(0.2f), diffuse(0.8f), specular(0.5f), shininess(32.0f) {}

    // Constructeur avec paramètres
    Material(const glm::vec3& amb, const glm::vec3& diff, const glm::vec3& spec, float shin)
            : ambient(amb), diffuse(diff), specular(spec), shininess(shin) {}

    // Matériaux prédéfinis
    static Material createPlastic(const glm::vec3& color = glm::vec3(1.0f, 0.0f, 0.0f));
    static Material createMetal(const glm::vec3& color = glm::vec3(0.7f, 0.7f, 0.7f));
    static Material createRubber(const glm::vec3& color = glm::vec3(0.0f, 0.0f, 1.0f));
    static Material createWood(const glm::vec3& color = glm::vec3(0.6f, 0.3f, 0.1f));
};