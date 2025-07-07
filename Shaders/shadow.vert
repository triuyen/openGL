#version 330 core

// Input vertex attributes
layout (location = 0) in vec3 aPos;

// Uniform matrices
uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    // Transform vertex position to light space
    // This gives us the position as seen from the light's perspective
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}