#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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
                        glm::vec3* lightPos,
                        glm::vec3* lightColor,
                        glm::vec3* cameraPos,
                        float* lightIntensity = nullptr,
                        bool* wireframe = nullptr);

    // Utility
    bool wantCaptureMouse() const;
    bool wantCaptureKeyboard() const;

private:
    bool m_showDemoWindow;
    bool m_showMainWindow;
};