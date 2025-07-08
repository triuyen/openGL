#version 330 core

// Input from vertex shader
in vec3 FragPos;     // Fragment position (if needed)
in vec3 Normal;      // Normal vector (if needed)
in vec2 TexCoord;    // Texture coordinates

// Output color
out vec4 FragColor;

// Uniforms
uniform vec3 objectColor;     // Solid color for the object
uniform sampler2D texture1;   // Optional texture
uniform bool useTexture;      // Whether to use texture or solid color

void main()
{
    if (useTexture) {
        // Use texture if available
        FragColor = texture(texture1, TexCoord);
    } else {
        // Use solid color
        FragColor = vec4(objectColor, 1.0);
    }
}