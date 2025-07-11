#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Material.hpp"
#include "Mesh.hpp"
#include <memory>
#include <string>

class Object3D {
public:
    glm::vec3 position;
    glm::vec3 rotation;  // Euler angles en radians
    glm::vec3 scale;

    Material material;
    std::shared_ptr<Mesh> mesh;
    std::string name;
    bool isSelected;

    Object3D(const std::string& objectName = "Object",
             const glm::vec3& pos = glm::vec3(0.0f),
             const glm::vec3& rot = glm::vec3(0.0f),
             const glm::vec3& scl = glm::vec3(1.0f));

    // Transformation
    glm::mat4 getModelMatrix() const;
    void setPosition(const glm::vec3& pos);
    void setRotation(const glm::vec3& rot);
    void setScale(const glm::vec3& scl);
    void translate(const glm::vec3& offset);
    void rotate(const glm::vec3& eulerAngles);

    // Rendu
    void render(const class Shader& shader) const;

    // Collision/Selection
    bool isPointInside(const glm::vec3& point, float threshold = 1.0f) const;
    glm::vec3 getBoundingBoxMin() const;
    glm::vec3 getBoundingBoxMax() const;

    // Factory methods pour créer des objets prédéfinis
    static std::shared_ptr<Object3D> CreateCube(const std::string& name, const glm::vec3& position = glm::vec3(0.0f));
    static std::shared_ptr<Object3D> CreateSphere(const std::string& name, const glm::vec3& position = glm::vec3(0.0f));
    static std::shared_ptr<Object3D> CreatePlane(const std::string& name, const glm::vec3& position = glm::vec3(0.0f));
};