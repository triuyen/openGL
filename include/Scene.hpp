#pragma once

#include "Object3D.hpp"
#include "Light.hpp"
#include "Camera.hpp"
#include <vector>
#include <memory>
#include <algorithm>

class Scene {
public:
    std::vector<std::shared_ptr<Object3D>> objects;
    std::vector<Light> lights;
    Camera camera;

    // Sélection
    std::shared_ptr<Object3D> selectedObject;
    int selectedLightIndex;

    Scene();

    // Gestion des objets
    void addObject(std::shared_ptr<Object3D> object);
    void removeObject(std::shared_ptr<Object3D> object);
    std::shared_ptr<Object3D> getObjectByName(const std::string& name);

    // Gestion des lumières
    void addLight(const Light& light);
    void removeLight(int index);
    Light* getLightByIndex(int index);

    // Sélection
    void selectObject(std::shared_ptr<Object3D> object);
    void selectLight(int index);
    void clearSelection();

    // Rendu
    void render(const class Shader& shader) const;
    void renderShadowMaps(const class Shader& shadowShader);

    // Utilitaires
    std::shared_ptr<Object3D> getObjectAtPosition(const glm::vec3& worldPos, float threshold = 1.0f);
    int getLightAtPosition(const glm::vec3& worldPos, float threshold = 1.0f);

    // Initialisation de scène par défaut
    void setupDefaultScene();
};