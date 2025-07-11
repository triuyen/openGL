#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Forward declarations
class LightManager;
struct DirectionalLight;
struct PointLight;
struct SpotLight;
#include "Light.hpp"

class GUI {
public:
    // Constructor and destructor
    GUI(GLFWwindow* window);
    ~GUI();

    // Frame management
    void newFrame();
    void render();

    // UI controls
    void showMainWindow(bool* shadowsEnabled,
                        LightManager* lightManager,
                        glm::vec3* cameraPos,
                        bool* wireframe = nullptr,
                        bool* showLightSources = nullptr);

    // Utility
    bool wantCaptureMouse() const;
    bool wantCaptureKeyboard() const;

private:
    bool m_showDemoWindow;
    bool m_showMainWindow;

    // Helper functions for light UI
    void showDirectionalLightControls(DirectionalLight* light, int index);
    void showPointLightControls(PointLight* light, int index);
    void showSpotLightControls(SpotLight* light, int index);
};