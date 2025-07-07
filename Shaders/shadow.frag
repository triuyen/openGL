#version 330 core

// No output needed - depth is automatically written to depth buffer
// This shader is intentionally minimal for shadow map generation

void main()
{
    // Empty fragment shader
    // OpenGL automatically writes the depth value to the depth buffer
    // We don't need to output any color information for shadow mapping
}