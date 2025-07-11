#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include "Shader.hpp"
#include "GUI.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Material.hpp"
#include "Geometry.hpp"

// Window dimensions
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

// Shadow map dimensions
const unsigned int SHADOW_WIDTH = 1024;
const unsigned int SHADOW_HEIGHT = 1024;

// Camera
std::unique_ptr<Camera> camera;

// Mouse and time
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// GUI settings
bool shadowsEnabled = true;
bool wireframeMode = false;
bool uiMode = false;  // Mode interface utilisateur
bool tabKeyPressed = false;  // Pour éviter les répétitions de basculement

// Gestionnaire de lumières
LightManager lightManager;

// Géométries
std::unique_ptr<Geometry> sphereGeometry;
std::unique_ptr<Geometry> cubeGeometry;
std::unique_ptr<Geometry> planeGeometry;

// Matériaux
Material metalMaterial;
Material plasticMaterial;
Material groundMaterial;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void renderScene(Shader& shader);
void initializeScene();

int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Multi-Light Blinn-Phong Scene", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Commencer en mode UI pour faciliter l'utilisation
    uiMode = true;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Configure global OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Build and compile shaders
    Shader shadowMapShader("assets/shaders/shadow.vert", "assets/shaders/shadow.frag");
    Shader lightingShader("assets/shaders/blinn_phong.vert", "assets/shaders/blinn_phong.frag");  // Nouveaux shaders

    // Initialize scene (camera, lights, geometries, materials)
    initializeScene();

    // Initialize GUI
    GUI gui(window);

    // Configure depth map FBO
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    // Create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Lighting shader configuration
    lightingShader.use();
    lightingShader.setUniform("shadowMap", 1);

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        // Per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process input
        processInput(window);

        // Start ImGui frame
        gui.newFrame();

        // Clear screen
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set wireframe mode
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // Light space matrix calculation (using first directional light if available)
        glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
        if (lightManager.getLightCount() > 0) {
            auto& lights = lightManager.getLights();
            if (lights[0]->type == LightType::DIRECTIONAL) {
                DirectionalLight* dirLight = static_cast<DirectionalLight*>(lights[0].get());
                float near_plane = 1.0f, far_plane = 15.0f;
                glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
                glm::vec3 lightPos = -dirLight->direction * 5.0f; // Position arbitraire pour la shadow map
                glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                lightSpaceMatrix = lightProjection * lightView;
            }
        }

        // 1. Render depth of scene to texture (from light's perspective) - only if shadows enabled
        if (shadowsEnabled && lightManager.getLightCount() > 0) {
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            shadowMapShader.use();
            shadowMapShader.setUniform("lightSpaceMatrix", lightSpaceMatrix);
            renderScene(shadowMapShader);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // 2. Render scene normally with lighting
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        lightingShader.use();

        // View and projection matrices
        glm::mat4 projection = camera->getProjectionMatrix(static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT));
        glm::mat4 view = camera->getViewMatrix();
        lightingShader.setUniform("projection", projection);
        lightingShader.setUniform("view", view);

        // Camera position
        lightingShader.setUniform("viewPos", camera->getPosition());
        lightingShader.setUniform("lightSpaceMatrix", lightSpaceMatrix);
        lightingShader.setUniform("shadowsEnabled", shadowsEnabled);

        // Send lights to shader (pour les nouveaux shaders)
        lightManager.sendLightsToShader(lightingShader);

        // Bind shadow map only if shadows are enabled
        if (shadowsEnabled) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depthMap);
        }

        renderScene(lightingShader);

        // Render GUI
        glm::vec3 cameraPos = camera->getPosition();
        gui.showMainWindow(&shadowsEnabled, &lightManager, &cameraPos, &wireframeMode);
        gui.render();

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteFramebuffers(1, &depthMapFBO);
    glDeleteTextures(1, &depthMap);

    glfwTerminate();
    return 0;
}

void initializeScene() {
    // Initialize camera
    camera = std::make_unique<Camera>(glm::vec3(0.0f, 3.0f, 8.0f));

    // Initialize geometries
    sphereGeometry = std::make_unique<Geometry>();
    sphereGeometry->generateSphere(1.0f, 32, 16);

    cubeGeometry = std::make_unique<Geometry>();
    cubeGeometry->generateCube(2.0f);

    planeGeometry = std::make_unique<Geometry>();
    planeGeometry->generatePlane(20.0f, 20.0f);

    // Initialize materials
    metalMaterial = Material::createMetal(glm::vec3(0.7f, 0.7f, 0.8f));
    plasticMaterial = Material::createPlastic(glm::vec3(0.8f, 0.2f, 0.2f));
    groundMaterial = Material::createRubber(glm::vec3(0.3f, 0.3f, 0.3f));

    // Initialize lights
    lightManager.clear();

    // Lumière directionnelle (soleil)
    DirectionalLight dirLight(glm::vec3(-0.3f, -1.0f, -0.2f), glm::vec3(1.0f, 0.95f, 0.8f), 0.8f);
    lightManager.addDirectionalLight(dirLight);

    // Lumière ponctuelle (lampe)
    PointLight pointLight(glm::vec3(3.0f, 4.0f, 3.0f), glm::vec3(0.8f, 0.8f, 1.0f), 1.5f);
    lightManager.addPointLight(pointLight);

    // Lumière spot (projecteur)
    SpotLight spotLight(glm::vec3(-3.0f, 6.0f, -3.0f), glm::vec3(0.5f, -1.0f, 0.5f),
                        15.0f, 25.0f, glm::vec3(1.0f, 0.6f, 0.3f), 2.0f);
    lightManager.addSpotLight(spotLight);
}

void renderScene(Shader& shader) {
    // Ground plane
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
    shader.setUniform("model", model);

    // Material properties for ground
    shader.setUniform("material.ambient", groundMaterial.ambient);
    shader.setUniform("material.diffuse", groundMaterial.diffuse);
    shader.setUniform("material.specular", groundMaterial.specular);
    shader.setUniform("material.shininess", groundMaterial.shininess);

    planeGeometry->render();

    // Sphere - Metal material
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-2.0f, 1.0f, 0.0f));
    shader.setUniform("model", model);

    shader.setUniform("material.ambient", metalMaterial.ambient);
    shader.setUniform("material.diffuse", metalMaterial.diffuse);
    shader.setUniform("material.specular", metalMaterial.specular);
    shader.setUniform("material.shininess", metalMaterial.shininess);

    sphereGeometry->render();

    // Cube - Plastic material
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 1.0f, 0.0f));
    model = glm::rotate(model, static_cast<float>(glfwGetTime()) * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
    shader.setUniform("model", model);

    shader.setUniform("material.ambient", plasticMaterial.ambient);
    shader.setUniform("material.diffuse", plasticMaterial.diffuse);
    shader.setUniform("material.specular", plasticMaterial.specular);
    shader.setUniform("material.shininess", plasticMaterial.shininess);

    cubeGeometry->render();

    // Additional objects for variety
    // Second sphere - different metal
    Material goldMaterial = Material::createMetal(glm::vec3(1.0f, 0.8f, 0.3f));
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 2.0f, -3.0f));
    shader.setUniform("model", model);

    shader.setUniform("material.ambient", goldMaterial.ambient);
    shader.setUniform("material.diffuse", goldMaterial.diffuse);
    shader.setUniform("material.specular", goldMaterial.specular);
    shader.setUniform("material.shininess", goldMaterial.shininess);

    sphereGeometry->render();

    // Second cube - different plastic
    Material bluePlasticMaterial = Material::createPlastic(glm::vec3(0.2f, 0.2f, 0.8f));
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.0f, 3.0f));
    model = glm::rotate(model, static_cast<float>(glfwGetTime()) * -0.3f, glm::vec3(1.0f, 0.0f, 1.0f));
    shader.setUniform("model", model);

    shader.setUniform("material.ambient", bluePlasticMaterial.ambient);
    shader.setUniform("material.diffuse", bluePlasticMaterial.diffuse);
    shader.setUniform("material.specular", bluePlasticMaterial.specular);
    shader.setUniform("material.shininess", bluePlasticMaterial.shininess);

    cubeGeometry->render();
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Basculer entre mode caméra et mode UI avec TAB
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !tabKeyPressed) {
        uiMode = !uiMode;
        tabKeyPressed = true;

        if (uiMode) {
            // Mode UI : afficher le curseur
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            firstMouse = true;  // Reset pour éviter les sauts de caméra
        } else {
            // Mode caméra : cacher le curseur
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }

    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
        tabKeyPressed = false;
    }

    // Contrôles de caméra seulement si pas en mode UI
    if (!uiMode) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera->processKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera->processKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera->processKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera->processKeyboard(RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            camera->processKeyboard(UP, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            camera->processKeyboard(DOWN, deltaTime);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    // Ne traiter les mouvements de souris que si on n'est pas en mode UI
    if (uiMode) return;

    if (firstMouse) {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - lastX;
    float yoffset = lastY - static_cast<float>(ypos); // Reversed since y-coordinates go from bottom to top
    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    camera->processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    // Ne traiter le scroll que si on n'est pas en mode UI
    if (uiMode) return;

    camera->processMouseScroll(static_cast<float>(yoffset));
}