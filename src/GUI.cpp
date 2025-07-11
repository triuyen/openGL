#include "GUI.hpp"
#include "Light.hpp"
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

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
                         LightManager* lightManager,
                         glm::vec3* cameraPos,
                         bool* wireframe) {

    if (!m_showMainWindow) return;

    // Create main control window
    ImGui::Begin("Scene Controls", &m_showMainWindow);

    // Instructions de navigation
    ImGui::TextColored(ImVec4(0.7f, 0.9f, 0.7f, 1.0f), "Navigation:");
    ImGui::Text("TAB - Basculer mode UI / Caméra");
    ImGui::Text("Mode Caméra: WASD + Souris pour se déplacer");
    ImGui::Text("Q/E - Monter/Descendre, Molette - Zoom");
    ImGui::Separator();

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
    ImGui::Text("Lighting System (%zu lights)", lightManager->getLightCount());

    // Boutons pour ajouter des lumières
    if (ImGui::Button("Add Directional Light")) {
        DirectionalLight dirLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(1.0f, 1.0f, 0.8f));
        lightManager->addDirectionalLight(dirLight);
    }
    ImGui::SameLine();
    if (ImGui::Button("Add Point Light")) {
        PointLight pointLight(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.8f, 0.8f, 1.0f));
        lightManager->addPointLight(pointLight);
    }
    ImGui::SameLine();
    if (ImGui::Button("Add Spot Light")) {
        SpotLight spotLight(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
                            12.5f, 17.5f, glm::vec3(1.0f, 0.8f, 0.6f));
        lightManager->addSpotLight(spotLight);
    }

    if (ImGui::Button("Clear All Lights")) {
        lightManager->clear();
    }

    ImGui::Separator();

    // Controls pour chaque lumière
    auto& lights = lightManager->getLights();
    for (size_t i = 0; i < lights.size(); ++i) {
        ImGui::PushID(static_cast<int>(i));

        if (lights[i]->type == LightType::DIRECTIONAL) {
            showDirectionalLightControls(static_cast<DirectionalLight*>(lights[i].get()), static_cast<int>(i));
        } else if (lights[i]->type == LightType::POINT) {
            showPointLightControls(static_cast<PointLight*>(lights[i].get()), static_cast<int>(i));
        } else if (lights[i]->type == LightType::SPOT) {
            showSpotLightControls(static_cast<SpotLight*>(lights[i].get()), static_cast<int>(i));
        }

        ImGui::PopID();
        ImGui::Separator();
    }

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

void GUI::showDirectionalLightControls(DirectionalLight* light, int index) {
    std::string label = "Directional Light " + std::to_string(index);

    if (ImGui::CollapsingHeader(label.c_str())) {
        ImGui::Checkbox("Enabled", &light->enabled);
        ImGui::SliderFloat3("Direction", &light->direction.x, -1.0f, 1.0f);
        ImGui::ColorEdit3("Color", &light->color.x);
        ImGui::SliderFloat("Intensity", &light->intensity, 0.0f, 3.0f);

        // Normaliser la direction automatiquement
        light->direction = glm::normalize(light->direction);
    }
}

void GUI::showPointLightControls(PointLight* light, int index) {
    std::string label = "Point Light " + std::to_string(index);

    if (ImGui::CollapsingHeader(label.c_str())) {
        ImGui::Checkbox("Enabled", &light->enabled);
        ImGui::SliderFloat3("Position", &light->position.x, -10.0f, 10.0f);
        ImGui::ColorEdit3("Color", &light->color.x);
        ImGui::SliderFloat("Intensity", &light->intensity, 0.0f, 3.0f);

        if (ImGui::TreeNode("Attenuation")) {
            ImGui::SliderFloat("Constant", &light->constant, 0.1f, 2.0f);
            ImGui::SliderFloat("Linear", &light->linear, 0.001f, 0.5f);
            ImGui::SliderFloat("Quadratic", &light->quadratic, 0.0001f, 0.1f);
            ImGui::TreePop();
        }
    }
}

void GUI::showSpotLightControls(SpotLight* light, int index) {
    std::string label = "Spot Light " + std::to_string(index);

    if (ImGui::CollapsingHeader(label.c_str())) {
        ImGui::Checkbox("Enabled", &light->enabled);
        ImGui::SliderFloat3("Position", &light->position.x, -10.0f, 10.0f);
        ImGui::SliderFloat3("Direction", &light->direction.x, -1.0f, 1.0f);
        ImGui::ColorEdit3("Color", &light->color.x);
        ImGui::SliderFloat("Intensity", &light->intensity, 0.0f, 3.0f);

        if (ImGui::TreeNode("Spot Parameters")) {
            ImGui::SliderFloat("Inner Angle", &light->cutOff, 1.0f, 45.0f);
            ImGui::SliderFloat("Outer Angle", &light->outerCutOff, light->cutOff, 60.0f);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Attenuation")) {
            ImGui::SliderFloat("Constant", &light->constant, 0.1f, 2.0f);
            ImGui::SliderFloat("Linear", &light->linear, 0.001f, 0.5f);
            ImGui::SliderFloat("Quadratic", &light->quadratic, 0.0001f, 0.1f);
            ImGui::TreePop();
        }

        // Normaliser la direction et s'assurer que outer >= inner
        light->direction = glm::normalize(light->direction);
        if (light->outerCutOff < light->cutOff) {
            light->outerCutOff = light->cutOff;
        }
    }
}

bool GUI::wantCaptureMouse() const {
    return ImGui::GetIO().WantCaptureMouse;
}

bool GUI::wantCaptureKeyboard() const {
    return ImGui::GetIO().WantCaptureKeyboard;
}