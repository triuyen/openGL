#version 330 core

// Input vertex attributes
layout (location = 0) in vec3 aPos;       // Vertex position
layout (location = 1) in vec3 aNormal;    // Vertex normal
layout (location = 2) in vec2 aTexCoord;  // Texture coordinates

// Output to fragment shader
out vec3 FragPos;        // Fragment position in world space
out vec3 Normal;         // Normal in world space
out vec2 TexCoord;       // Texture coordinates
out vec4 FragPosLightSpace; // Fragment position in light space (for shadow mapping)

// Uniform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
    // Calculate fragment position in world space
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Transform normal to world space
    // Use normal matrix (inverse transpose of model matrix) for non-uniform scaling
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // Pass through texture coordinates
    TexCoord = aTexCoord;

    // Calculate fragment position in light space for shadow mapping
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    // Transform vertex to clip space
    gl_Position = projection * view * vec4(FragPos, 1.0);
}