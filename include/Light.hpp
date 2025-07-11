#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include <string>
#include "Shader.hpp"

// Types de lumières
enum class LightType {
    DIRECTIONAL,    // Lumière directionnelle (comme le soleil)
    POINT,          // Lumière ponctuelle (comme une ampoule)
    SPOT            // Lumière spot (comme une lampe torche)
};

// Structure de base pour une lumière
struct Light {
    LightType type;
    glm::vec3 color;        // Couleur de la lumière
    float intensity;        // Intensité de la lumière
    bool enabled;           // Lumière activée/désactivée

    // Propriétés pour l'atténuation (Point et Spot)
    float constant;         // Terme constant
    float linear;           // Terme linéaire
    float quadratic;        // Terme quadratique

    // Constructeur
    Light(LightType t, const glm::vec3& col = glm::vec3(1.0f), float intens = 1.0f)
            : type(t), color(col), intensity(intens), enabled(true),
              constant(1.0f), linear(0.09f), quadratic(0.032f) {}

    // Méthode virtuelle pour envoyer les uniformes au shader
    virtual void sendToShader(Shader& shader, int index) const = 0;

    // Destructeur virtuel
    virtual ~Light() {}
};

// Lumière directionnelle
struct DirectionalLight : public Light {
    glm::vec3 direction;    // Direction de la lumière

    DirectionalLight(const glm::vec3& dir = glm::vec3(-0.2f, -1.0f, -0.3f),
                     const glm::vec3& col = glm::vec3(1.0f),
                     float intens = 1.0f)
            : Light(LightType::DIRECTIONAL, col, intens), direction(glm::normalize(dir)) {}

    void sendToShader(Shader& shader, int index) const override;
    virtual ~DirectionalLight() {}
};

// Lumière ponctuelle
struct PointLight : public Light {
    glm::vec3 position;     // Position de la lumière

    PointLight(const glm::vec3& pos = glm::vec3(0.0f, 0.0f, 0.0f),
               const glm::vec3& col = glm::vec3(1.0f),
               float intens = 1.0f)
            : Light(LightType::POINT, col, intens), position(pos) {}

    void sendToShader(Shader& shader, int index) const override;
    virtual ~PointLight() {}
};

// Lumière spot
struct SpotLight : public Light {
    glm::vec3 position;     // Position de la lumière
    glm::vec3 direction;    // Direction du spot
    float cutOff;           // Angle intérieur du cône (en degrés)
    float outerCutOff;      // Angle extérieur du cône (en degrés)

    SpotLight(const glm::vec3& pos = glm::vec3(0.0f, 0.0f, 0.0f),
              const glm::vec3& dir = glm::vec3(0.0f, -1.0f, 0.0f),
              float cutoff = 12.5f,
              float outerCutoff = 17.5f,
              const glm::vec3& col = glm::vec3(1.0f),
              float intens = 1.0f)
            : Light(LightType::SPOT, col, intens), position(pos),
              direction(glm::normalize(dir)), cutOff(cutoff), outerCutOff(outerCutoff) {}

    void sendToShader(Shader& shader, int index) const override;
    virtual ~SpotLight() {}
};

// Gestionnaire de lumières
class LightManager {
private:
    std::vector<std::unique_ptr<Light>> lights;
    static const int MAX_LIGHTS = 8;  // Nombre maximum de lumières supportées

public:
    // Ajouter une lumière
    void addDirectionalLight(const DirectionalLight& light);
    void addPointLight(const PointLight& light);
    void addSpotLight(const SpotLight& light);

    // Envoyer toutes les lumières au shader
    void sendLightsToShader(Shader& shader) const;

    // Accès aux lumières pour l'UI
    std::vector<std::unique_ptr<Light>>& getLights() { return lights; }
    const std::vector<std::unique_ptr<Light>>& getLights() const { return lights; }

    // Effacer toutes les lumières
    void clear() { lights.clear(); }

    // Obtenir le nombre de lumières
    size_t getLightCount() const { return lights.size(); }
};