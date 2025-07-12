#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Shader.hpp"

class Skybox {
public:
    Skybox(const std::vector<std::string>& faces);
    
    ~Skybox();

    void render(const glm::mat4& view, const glm::mat4& projection);

    unsigned int getTextureID() const { return cubemapTexture; }
    bool isLoaded() const { return loaded; }

    bool loadCubemap(const std::vector<std::string>& faces);

private:
    unsigned int skyboxVAO, skyboxVBO;
    unsigned int cubemapTexture;
    Shader* shader;
    bool loaded;

    void setupMesh();
    
    unsigned int loadCubemapTexture(const std::vector<std::string>& faces);
};

#endif