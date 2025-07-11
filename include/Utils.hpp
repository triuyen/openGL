#pragma once

#include <glm/glm.hpp>

class Utils {
public:
    // Conversion de coordonnées d'écran vers coordonnées monde
    static glm::vec3 screenToWorld(const glm::vec2& screenPos, const glm::vec2& screenSize,
                                   const glm::mat4& view, const glm::mat4& projection,
                                   float depth = 0.0f);

    // Ray casting pour la sélection d'objets
    static glm::vec3 screenToWorldRay(const glm::vec2& screenPos, const glm::vec2& screenSize,
                                      const glm::mat4& view, const glm::mat4& projection);

    // Intersection rayon-plan (pour la vue du dessus)
    static bool rayPlaneIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection,
                                     const glm::vec3& planePoint, const glm::vec3& planeNormal,
                                     glm::vec3& intersectionPoint);

    // Génération de couleurs distinctes pour les lumières
    static glm::vec3 getDistinctColor(int index);
};