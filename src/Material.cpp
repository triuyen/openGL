#include "Material.hpp"

Material Material::createPlastic(const glm::vec3& color) {
    // Plastique : faible composante ambiante, diffuse moyenne, spéculaire élevée, brillance modérée
    return Material(
            color * 0.1f,           // ambient: faible réflexion ambiante
            color * 0.7f,           // diffuse: bonne diffusion de la couleur
            glm::vec3(0.9f),        // specular: reflets blancs intenses
            64.0f                   // shininess: surface relativement lisse
    );
}

Material Material::createMetal(const glm::vec3& color) {
    // Métal : composante ambiante faible, diffuse faible, spéculaire très élevée, brillance élevée
    return Material(
            color * 0.05f,          // ambient: très faible réflexion ambiante
            color * 0.3f,           // diffuse: faible diffusion (le métal reflète plus qu'il ne diffuse)
            color * 1.2f,           // specular: reflets colorés très intenses
            128.0f                  // shininess: surface très lisse et brillante
    );
}

Material Material::createRubber(const glm::vec3& color) {
    // Caoutchouc : composante ambiante modérée, diffuse élevée, spéculaire faible, brillance faible
    return Material(
            color * 0.3f,           // ambient: réflexion ambiante modérée
            color * 0.8f,           // diffuse: bonne diffusion mate
            glm::vec3(0.1f),        // specular: très peu de reflets
            8.0f                    // shininess: surface rugueuse
    );
}

Material Material::createWood(const glm::vec3& color) {
    // Bois : composante ambiante modérée, diffuse élevée, spéculaire faible, brillance faible
    return Material(
            color * 0.2f,           // ambient: réflexion ambiante modérée
            color * 0.9f,           // diffuse: excellent diffuseur
            glm::vec3(0.05f),       // specular: presque aucun reflet
            4.0f                    // shininess: surface très rugueuse
    );
}