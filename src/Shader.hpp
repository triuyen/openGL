#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader {
private:
    GLuint programID;

    // Utility function for checking shader compilation/linking errors
    void checkCompileErrors(GLuint shader, std::string type);

public:
    // Constructor reads and builds the shader
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    // Destructor
    ~Shader();

    // Use/activate the shader
    void use();

    // Utility uniform functions
    void setUniform(const std::string& name, bool value);
    void setUniform(const std::string& name, int value);
    void setUniform(const std::string& name, float value);
    void setUniform(const std::string& name, const glm::vec2& value);
    void setUniform(const std::string& name, float x, float y);
    void setUniform(const std::string& name, const glm::vec3& value);
    void setUniform(const std::string& name, float x, float y, float z);
    void setUniform(const std::string& name, const glm::vec4& value);
    void setUniform(const std::string& name, float x, float y, float z, float w);
    void setUniform(const std::string& name, const glm::mat2& mat);
    void setUniform(const std::string& name, const glm::mat3& mat);
    void setUniform(const std::string& name, const glm::mat4& mat);

    // Get shader program ID
    GLuint getID() const;
};