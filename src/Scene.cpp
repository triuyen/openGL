#include "Scene.hpp"
#include "Shader.hpp"
#include <iostream>
#include <algorithm>

Scene::Scene() : selectedObject(nullptr), selectedLightIndex(-1) {
    // Caméra par défaut
    camera = Camera(glm::vec3(0.0f, 5.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -20.0f);
}

void Scene::addObject(std::shared_ptr<Object3D> object) {
    objects.push_back(object);
}

void Scene::removeObject(std::shared_ptr<Object3D> object) {
    auto it = std::find(objects.begin(), objects.end(), object);
    if (it != objects.end()) {
        if (selectedObject == object) {
            selectedObject = nullptr;
        }
        objects.erase(it);
    }
}

std::shared_ptr<Object3D> Scene::getObjectByName(const std::string& name) {
    for (auto& object : objects) {
        if (object->name == name) {
            return object;
        }
    }
    return nullptr;
}

void Scene::addLight(const Light& light) {
    lights.push_back(light);
}

void Scene::removeLight(int index) {
    if (index >= 0 && index < lights.size()) {
        lights[index].cleanup();
        lights.erase(lights.begin() + index);
        if (selectedLightIndex == index) {
            selectedLightIndex = -1;
        } else if (selectedLightIndex > index) {
            selectedLightIndex--;
        }
    }
}

Light* Scene::getLightByIndex(int index) {
    if (index >= 0 && index < lights.size()) {
        return &lights[index];
    }
    return nullptr;
}

void Scene::selectObject(std::shared_ptr<Object3D> object) {
    if (selectedObject) {
        selectedObject->isSelected = false;
    }
    selectedObject = object;
    selectedLightIndex = -1;
    if (selectedObject) {
        selectedObject->isSelected = true;
    }
}

void Scene::selectLight(int index) {
    if (selectedObject) {
        selectedObject->isSelected = false;
        selectedObject = nullptr;
    }
    selectedLightIndex = index;
}

void Scene::clearSelection() {
    if (selectedObject) {
        selectedObject->isSelected = false;
        selectedObject = nullptr;
    }
    selectedLightIndex = -1;
}

void Scene::render(const Shader& shader) const {
    // Bind lights
    shader.setUniform("numLights", static_cast<int>(lights.size()));
    for (size_t i = 0; i < lights.size() && i < 8; ++i) {  // Maximum 8 lights
        lights[i].bind(i, shader);

        // Bind shadow maps
        if (lights[i].castShadows) {
            if (lights[i].type == LightType::POINT) {
                glActiveTexture(GL_TEXTURE1 + i);
                glBindTexture(GL_TEXTURE_CUBE_MAP, lights[i].shadowCubeMap);
                shader.setUniform("shadowCubeMaps[" + std::to_string(i) + "]", static_cast<int>(1 + i));
            } else {
                glActiveTexture(GL_TEXTURE1 + i);
                glBindTexture(GL_TEXTURE_2D, lights[i].shadowMap);
                shader.setUniform("shadowMaps[" + std::to_string(i) + "]", static_cast<int>(1 + i));
                shader.setUniform("lightSpaceMatrices[" + std::to_string(i) + "]", lights[i].lightSpaceMatrix);
            }
        }
    }

    // Camera uniforms
    shader.setUniform("viewPos", camera.getPosition());

    // Render objects
    for (const auto& object : objects) {
        object->render(shader);
    }
}

void Scene::renderShadowMaps(const Shader& shadowShader) {
    for (auto& light : lights) {
        if (!light.castShadows) continue;

        glViewport(0, 0, 1024, 1024);
        glBindFramebuffer(GL_FRAMEBUFFER, light.shadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        shadowShader.use();

        if (light.type == LightType::POINT) {
            // Point light shadow mapping (cube map)
            light.updateShadowTransforms();
            for (int i = 0; i < 6; ++i) {
                shadowShader.setUniform("shadowMatrices[" + std::to_string(i) + "]", light.shadowTransforms[i]);
            }
            shadowShader.setUniform("lightPos", light.position);
            shadowShader.setUniform("farPlane", 25.0f);
        } else {
            // Directional/Spot light shadow mapping
            light.updateLightSpaceMatrix();
            shadowShader.setUniform("lightSpaceMatrix", light.lightSpaceMatrix);
        }

        // Render objects for shadow map
        for (const auto& object : objects) {
            shadowShader.setUniform("model", object->getModelMatrix());
            if (object->mesh) {
                object->mesh->render();
            }
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::shared_ptr<Object3D> Scene::getObjectAtPosition(const glm::vec3& worldPos, float threshold) {
    for (auto& object : objects) {
        if (object->isPointInside(worldPos, threshold)) {
            return object;
        }
    }
    return nullptr;
}

int Scene::getLightAtPosition(const glm::vec3& worldPos, float threshold) {
    for (size_t i = 0; i < lights.size(); ++i) {
        if (glm::distance(lights[i].position, worldPos) <= threshold) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void Scene::setupDefaultScene() {
    // Ajouter des objets par défaut
    auto cube = Object3D::CreateCube("Cube en Bois", glm::vec3(-2.0f, 1.0f, 0.0f));
    auto sphere = Object3D::CreateSphere("Sphère en Métal", glm::vec3(2.0f, 1.0f, 0.0f));
    auto ground = Object3D::CreatePlane("Sol", glm::vec3(0.0f, 0.0f, 0.0f));
    ground->setScale(glm::vec3(10.0f, 1.0f, 10.0f));
    ground->material = Material::Plastic();

    addObject(cube);
    addObject(sphere);
    addObject(ground);

    // Ajouter des lumières par défaut
    Light directionalLight = Light::CreateDirectional(glm::vec3(-0.5f, -1.0f, -0.3f), glm::vec3(0.2f, 0.2f, 1.0f), 0.8f);
    directionalLight.setPosition(glm::vec3(0.0f, 8.0f, 5.0f));

    Light pointLight = Light::CreatePoint(glm::vec3(3.0f, 4.0f, 3.0f), glm::vec3(0.2f, 1.0f, 0.2f), 1.0f);

    Light spotLight = Light::CreateSpot(glm::vec3(-3.0f, 4.0f, 3.0f), glm::vec3(0.3f, -0.8f, -0.3f),
                                        glm::vec3(1.0f, 0.2f, 0.2f), 15.0f, 20.0f, 1.2f);

    addLight(directionalLight);
    addLight(pointLight);
    addLight(spotLight);
}