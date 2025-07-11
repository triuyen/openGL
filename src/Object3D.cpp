#include "Object3D.hpp"
#include "Shader.hpp"
#include <glm/gtc/matrix_transform.hpp>

Object3D::Object3D(const std::string& objectName, const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl)
        : name(objectName), position(pos), rotation(rot), scale(scl), material(Material::Plastic()), isSelected(false) {
}

glm::mat4 Object3D::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    return model;
}

void Object3D::setPosition(const glm::vec3& pos) {
    position = pos;
}

void Object3D::setRotation(const glm::vec3& rot) {
    rotation = rot;
}

void Object3D::setScale(const glm::vec3& scl) {
    scale = scl;
}

void Object3D::translate(const glm::vec3& offset) {
    position += offset;
}

void Object3D::rotate(const glm::vec3& eulerAngles) {
    rotation += eulerAngles;
}

void Object3D::render(const Shader& shader) const {
    if (!mesh) return;

    // Bind material
    material.bind(shader);

    // Set model matrix
    shader.setUniform("model", getModelMatrix());

    // Render mesh
    mesh->render();
}

bool Object3D::isPointInside(const glm::vec3& point, float threshold) const {
    glm::vec3 localPoint = point - position;
    return glm::length(localPoint) <= threshold;
}

glm::vec3 Object3D::getBoundingBoxMin() const {
    return position - scale;
}

glm::vec3 Object3D::getBoundingBoxMax() const {
    return position + scale;
}

std::shared_ptr<Object3D> Object3D::CreateCube(const std::string& name, const glm::vec3& position) {
    auto object = std::make_shared<Object3D>(name, position);
    object->mesh = Mesh::CreateCube();
    object->material = Material::Wood();
    return object;
}

std::shared_ptr<Object3D> Object3D::CreateSphere(const std::string& name, const glm::vec3& position) {
    auto object = std::make_shared<Object3D>(name, position);
    object->mesh = Mesh::CreateSphere();
    object->material = Material::Metal();
    return object;
}

std::shared_ptr<Object3D> Object3D::CreatePlane(const std::string& name, const glm::vec3& position) {
    auto object = std::make_shared<Object3D>(name, position);
    object->mesh = Mesh::CreatePlane();
    object->material = Material::Plastic();
    return object;
}