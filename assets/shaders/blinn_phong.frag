#version 330 core

// Input from vertex shader
in vec3 FragPos;              // Fragment position in world space
in vec3 Normal;               // Normal in world space
in vec2 TexCoord;             // Texture coordinates
in vec4 FragPosLightSpace;    // Fragment position in light space

// Output color
out vec4 FragColor;

// Types de lumières (doivent correspondre à l'enum C++)
#define DIRECTIONAL 0
#define POINT 1
#define SPOT 2

// Structure de lumière
struct Light {
    int type;                 // Type de lumière (DIRECTIONAL, POINT, SPOT)
    bool enabled;             // Lumière activée/désactivée

// Propriétés communes
    vec3 color;               // Couleur de la lumière
    float intensity;          // Intensité

// Pour les lumières directionnelles
    vec3 direction;           // Direction de la lumière

// Pour les lumières ponctuelles et spots
    vec3 position;            // Position de la lumière
    float constant;           // Atténuation constante
    float linear;             // Atténuation linéaire
    float quadratic;          // Atténuation quadratique

// Pour les lumières spots uniquement
    float cutOff;             // Cosinus de l'angle intérieur
    float outerCutOff;        // Cosinus de l'angle extérieur
};

// Material properties
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

// Uniforms
uniform Light lights[8];      // Maximum 8 lumières
uniform int numLights;        // Nombre réel de lumières
uniform Material material;    // Matériau de l'objet
uniform vec3 viewPos;         // Position de la caméra
uniform sampler2D shadowMap;  // Shadow map
uniform bool shadowsEnabled; // Activation des ombres

// Function to calculate shadow factor
float ShadowCalculation(vec4 fragPosLightSpace) {
    if (!shadowsEnabled) return 0.0;

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
    vec3 lightDir = normalize(lights[0].type == DIRECTIONAL ? -lights[0].direction : lights[0].position - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

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

// Calcul de l'éclairage pour une lumière directionnelle
vec3 calculateDirectionalLight(Light light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    // Diffuse (Lambert)
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // Combine results
    vec3 ambient = light.color * material.ambient;
    vec3 diffuse = light.color * diff * material.diffuse;
    vec3 specular = light.color * spec * material.specular;

    return (ambient + diffuse + specular) * light.intensity;
}

// Calcul de l'éclairage pour une lumière ponctuelle
vec3 calculatePointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse (Lambert)
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Combine results
    vec3 ambient = light.color * material.ambient;
    vec3 diffuse = light.color * diff * material.diffuse;
    vec3 specular = light.color * spec * material.specular;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular) * light.intensity;
}

// Calcul de l'éclairage pour une lumière spot
vec3 calculateSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse (Lambert)
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // Combine results
    vec3 ambient = light.color * material.ambient;
    vec3 diffuse = light.color * diff * material.diffuse;
    vec3 specular = light.color * spec * material.specular;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular) * light.intensity;
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);

    // Calculer l'éclairage pour chaque lumière
    for (int i = 0; i < numLights && i < 8; ++i) {
        if (!lights[i].enabled) continue;

        vec3 lightContribution = vec3(0.0);

        if (lights[i].type == DIRECTIONAL) {
            lightContribution = calculateDirectionalLight(lights[i], norm, viewDir);
        } else if (lights[i].type == POINT) {
            lightContribution = calculatePointLight(lights[i], norm, FragPos, viewDir);
        } else if (lights[i].type == SPOT) {
            lightContribution = calculateSpotLight(lights[i], norm, FragPos, viewDir);
        }

        result += lightContribution;
    }

    // Apply shadow (only for the first light for simplicity)
    if (numLights > 0 && lights[0].enabled) {
        float shadow = ShadowCalculation(FragPosLightSpace);
        // Only reduce non-ambient lighting
        vec3 ambient = lights[0].color * material.ambient * lights[0].intensity;
        result = ambient + (1.0 - shadow) * (result - ambient);
    }

    FragColor = vec4(result, 1.0);
}