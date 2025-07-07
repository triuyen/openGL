#include "GUI.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

GUI::GUI(GLFWwindow* window) : m_showDemoWindow(false), m_showMainWindow(true) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Alternative: ImGui::StyleColorsClassic(); or ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    std::cout << "ImGui initialized successfully" << std::endl;
}

GUI::~GUI() {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUI::newFrame() {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUI::render() {
    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::showMainWindow(bool* shadowsEnabled,
                         glm::vec3* lightPos,
                         glm::vec3* lightColor,
                         glm::vec3* cameraPos,
                         float* lightIntensity,
                         bool* wireframe) {

    if (!m_showMainWindow) return;

    // Create main control window
    ImGui::Begin("OpenGL Scene Controls", &m_showMainWindow);

    // FPS display
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::Separator();

    // Shadow controls
    ImGui::Text("Shadow Settings");
    ImGui::Checkbox("Enable Shadows", shadowsEnabled);

    if (*shadowsEnabled) {
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Toggle shadow mapping on/off");
        }
    }

    ImGui::Separator();

    // Light controls
    ImGui::Text("Light Settings");
    ImGui::SliderFloat3("Light Position", &lightPos->x, -10.0f, 10.0f);
    ImGui::ColorEdit3("Light Color", &lightColor->x);

    if (lightIntensity) {
        ImGui::SliderFloat("Light Intensity", lightIntensity, 0.0f, 3.0f);
    }

    ImGui::Separator();

    // Camera info (read-only)
    ImGui::Text("Camera Info");
    ImGui::Text("Position: (%.2f, %.2f, %.2f)", cameraPos->x, cameraPos->y, cameraPos->z);

    ImGui::Separator();

    // Rendering options
    ImGui::Text("Rendering Options");
    if (wireframe) {
        ImGui::Checkbox("Wireframe Mode", wireframe);
    }

    // Demo window toggle
    ImGui::Checkbox("Show ImGui Demo", &m_showDemoWindow);

    ImGui::End();

    // Show demo window if requested
    if (m_showDemoWindow) {
        ImGui::ShowDemoWindow(&m_showDemoWindow);
    }
}

bool GUI::wantCaptureMouse() const {
    return ImGui::GetIO().WantCaptureMouse;
}

bool GUI::wantCaptureKeyboard() const {
    return ImGui::GetIO().WantCaptureKeyboard;
}