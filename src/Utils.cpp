#include "Utils.hpp"
#include <glm/gtc/matrix_transform.hpp>

glm::vec3 Utils::screenToWorld(const glm::vec2& screenPos, const glm::vec2& screenSize,
                               const glm::mat4& view, const glm::mat4& projection, float depth) {
    // Normaliser les coordonnées d'écran vers [-1, 1]
    float x = (2.0f * screenPos.x) / screenSize.x - 1.0f;
    float y = 1.0f - (2.0f * screenPos.y) / screenSize.y;

    // Créer le point en coordonnées de clip
    glm::vec4 clipPos(x, y, depth, 1.0f);

    // Inverser les transformations
    glm::mat4 invProjection = glm::inverse(projection);
    glm::mat4 invView = glm::inverse(view);

    glm::vec4 eyePos = invProjection * clipPos;
    eyePos /= eyePos.w;

    glm::vec4 worldPos = invView * eyePos;

    return glm::vec3(worldPos);
}

glm::vec3 Utils::screenToWorldRay(const glm::vec2& screenPos, const glm::vec2& screenSize,
                                  const glm::mat4& view, const glm::mat4& projection) {
    glm::vec3 nearPoint = screenToWorld(screenPos, screenSize, view, projection, -1.0f);
    glm::vec3 farPoint = screenToWorld(screenPos, screenSize, view, projection, 1.0f);

    return glm::normalize(farPoint - nearPoint);
}

bool Utils::rayPlaneIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection,
                                 const glm::vec3& planePoint, const glm::vec3& planeNormal,
                                 glm::vec3& intersectionPoint) {
    float denom = glm::dot(planeNormal, rayDirection);
    if (abs(denom) < 1e-6) {
        return false; // Ray is parallel to plane
    }

    float t = glm::dot(planePoint - rayOrigin, planeNormal) / denom;
    if (t < 0) {
        return false; // Intersection is behind ray origin
    }

    intersectionPoint = rayOrigin + t * rayDirection;
    return true;
}

glm::vec3 Utils::getDistinctColor(int index) {
    static const glm::vec3 colors[] = {
            glm::vec3(1.0f, 0.2f, 0.2f),  // Rouge
            glm::vec3(0.2f, 1.0f, 0.2f),  // Vert
            glm::vec3(0.2f, 0.2f, 1.0f),  // Bleu
            glm::vec3(1.0f, 1.0f, 0.2f),  // Jaune
            glm::vec3(1.0f, 0.2f, 1.0f),  // Magenta
            glm::vec3(0.2f, 1.0f, 1.0f),  // Cyan
            glm::vec3(1.0f, 0.6f, 0.2f),  // Orange
            glm::vec3(0.6f, 0.2f, 1.0f)   // Violet
    };

    return colors[index % 8];
}