#include "Light.hpp"
#include <string>
#include <memory>
#include <cmath>

// Implémentation DirectionalLight
void DirectionalLight::sendToShader(Shader& shader, int index) const {
    std::string base = "lights[" + std::to_string(index) + "]";

    shader.setUniform(base + ".type", static_cast<int>(type));
    shader.setUniform(base + ".direction", direction);
    shader.setUniform(base + ".position", position);  // Pour les nouveaux shaders si nécessaire
    shader.setUniform(base + ".color", color);
    shader.setUniform(base + ".intensity", intensity);
    shader.setUniform(base + ".enabled", enabled);
}

// Implémentation PointLight
void PointLight::sendToShader(Shader& shader, int index) const {
    std::string base = "lights[" + std::to_string(index) + "]";

    shader.setUniform(base + ".type", static_cast<int>(type));
    shader.setUniform(base + ".position", position);
    shader.setUniform(base + ".color", color);
    shader.setUniform(base + ".intensity", intensity);
    shader.setUniform(base + ".enabled", enabled);

    // Paramètres d'atténuation
    shader.setUniform(base + ".constant", constant);
    shader.setUniform(base + ".linear", linear);
    shader.setUniform(base + ".quadratic", quadratic);
}

// Implémentation SpotLight
void SpotLight::sendToShader(Shader& shader, int index) const {
    std::string base = "lights[" + std::to_string(index) + "]";

    shader.setUniform(base + ".type", static_cast<int>(type));
    shader.setUniform(base + ".position", position);
    shader.setUniform(base + ".direction", direction);
    shader.setUniform(base + ".color", color);
    shader.setUniform(base + ".intensity", intensity);
    shader.setUniform(base + ".enabled", enabled);

    // Paramètres d'atténuation
    shader.setUniform(base + ".constant", constant);
    shader.setUniform(base + ".linear", linear);
    shader.setUniform(base + ".quadratic", quadratic);

    // Paramètres du spot
    shader.setUniform(base + ".cutOff", glm::cos(glm::radians(cutOff)));
    shader.setUniform(base + ".outerCutOff", glm::cos(glm::radians(outerCutOff)));
}

// Implémentation LightManager
void LightManager::addDirectionalLight(const DirectionalLight& light) {
    if (lights.size() < MAX_LIGHTS) {
        lights.push_back(std::make_unique<DirectionalLight>(light));
    }
}

void LightManager::addPointLight(const PointLight& light) {
    if (lights.size() < MAX_LIGHTS) {
        lights.push_back(std::make_unique<PointLight>(light));
    }
}

void LightManager::addSpotLight(const SpotLight& light) {
    if (lights.size() < MAX_LIGHTS) {
        lights.push_back(std::make_unique<SpotLight>(light));
    }
}

void LightManager::sendLightsToShader(Shader& shader) const {
    // Envoyer le nombre de lumières
    shader.setUniform("numLights", static_cast<int>(lights.size()));

    // Envoyer chaque lumière
    for (size_t i = 0; i < lights.size(); ++i) {
        lights[i]->sendToShader(shader, static_cast<int>(i));
    }

    // Désactiver les lumières inutilisées
    for (size_t i = lights.size(); i < MAX_LIGHTS; ++i) {
        std::string base = "lights[" + std::to_string(i) + "]";
        shader.setUniform(base + ".enabled", false);
    }
}