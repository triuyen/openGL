#include "Skybox.h"

#include <fstream>
#include <iostream>
#include <stb/stb_image.h>

Skybox::Skybox(const std::vector<std::string>& faces)
    : skyboxVAO(0), skyboxVBO(0), cubemapTexture(0), loaded(false) {

    // Créer le shader
    shader = new Shader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");

    // Setup du mesh (cube)
    setupMesh();

    // Charger les textures
    loaded = loadCubemap(faces);

    // Configurer le shader
    shader->use();
    shader->setUniform("skybox", 0);
}

Skybox::~Skybox() {
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteTextures(1, &cubemapTexture);
    delete shader;
}

void Skybox::setupMesh() {
    float skyboxVertices[] = {
        // Positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // Création VAO et VBO
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

bool Skybox::loadCubemap(const std::vector<std::string>& faces) {
    if (faces.size() != 6) {
        std::cerr << "Error: Skybox requires exactly 6 faces!" << std::endl;
        return false;
    }

    if (cubemapTexture != 0) {
        glDeleteTextures(1, &cubemapTexture);
    }

    cubemapTexture = loadCubemapTexture(faces);
    return (cubemapTexture != 0);
}

unsigned int Skybox::loadCubemapTexture(const std::vector<std::string>& faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {

        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);

        std::cout << nrChannels << std::endl;

        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void Skybox::render(const glm::mat4& view, const glm::mat4& projection) {
    if (!loaded) {
        std::cerr << "Skybox not loaded, cannot render!" << std::endl;
        return;
    }

    glDepthFunc(GL_LEQUAL);

    shader->use();

    // Enlever la translation de la matrice view
    glm::mat4 viewWithoutTranslation = glm::mat4(glm::mat3(view));

    shader->setUniform("view", viewWithoutTranslation);
    shader->setUniform("projection", projection);

    // Render skybox cube
    glBindVertexArray(skyboxVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);
}