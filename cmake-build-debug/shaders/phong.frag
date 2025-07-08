#version 330 core

// Input from vertex shader
in vec3 FragPos;              // Fragment position in world space
in vec3 Normal;               // Normal in world space
in vec2 TexCoord;             // Texture coordinates
in vec4 FragPosLightSpace;    // Fragment position in light space

// Output color
out vec4 FragColor;

// Material properties
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

// Light properties
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

// Material uniform
uniform Material material;

// Shadow mapping
uniform sampler2D shadowMap;

// Optional diffuse texture (set to -1 if not used)
uniform sampler2D diffuseTexture;

// Function to calculate shadow factor
float ShadowCalculation(vec4 fragPosLightSpace) {
    // Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Check if fragment is outside light's projection
    if(projCoords.z > 1.0)
    return 0.0;

    // Get closest depth value from light's perspective
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // Calculate shadow bias to prevent shadow acne
    vec3 normal = normalize(Normal);
    vec3 lightColor = normalize(lightPos - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightColor)), 0.005);

    // PCF (Percentage Closer Filtering) for softer shadows
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

void main() {
    // Normalize the normal vector
    vec3 norm = normalize(Normal);

    // Light direction from fragment to light
    vec3 lightDir = normalize(lightPos - FragPos);

    // View direction from fragment to camera
    vec3 viewDir = normalize(viewPos - FragPos);

    // Reflect direction for specular lighting
    vec3 reflectDir = reflect(-lightDir, norm);

    // Calculate distance attenuation (optional)
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

    // 1. AMBIENT LIGHTING
    vec3 ambient = material.ambient * lightColor;

    // 2. DIFFUSE LIGHTING (Lambertian)
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse * lightColor;

    // 3. SPECULAR LIGHTING (Phong)
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular * lightColor;

    // Calculate shadow factor
    float shadow = ShadowCalculation(FragPosLightSpace);

    // Apply attenuation to diffuse and specular (not ambient)
    diffuse *= attenuation;
    specular *= attenuation;

    // Combine lighting components
    // Ambient is not affected by shadows
    // Diffuse and specular are reduced by shadow factor
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);

    // Final color
    FragColor = vec4(lighting, 1.0);
}