#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace[8];

out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    int type;           // 0 = directional, 1 = point, 2 = spot
    vec3 position;      // Pour point et spot
    vec3 direction;     // Pour directional et spot
    vec3 color;
    float intensity;

// Atténuation (point et spot)
    float constant;
    float linear;
    float quadratic;

// Spot light
    float cutOff;
    float outerCutOff;

    bool castShadows;
};

uniform Material material;
uniform Light lights[8];
uniform int numLights;
uniform vec3 viewPos;

uniform sampler2D shadowMaps[8];
uniform samplerCube shadowCubeMaps[8];

// Calcul des ombres pour lumières directionnelles et spot
float ShadowCalculation(vec4 fragPosLightSpace, int lightIndex) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0)
    return 0.0;

    float closestDepth = texture(shadowMaps[lightIndex], projCoords.xy).r;
    float currentDepth = projCoords.z;

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lights[lightIndex].position - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMaps[lightIndex], 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMaps[lightIndex], projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

// Calcul des ombres pour lumières point (cube map)
float ShadowCalculationPoint(int lightIndex) {
    vec3 fragToLight = FragPos - lights[lightIndex].position;
    float closestDepth = texture(shadowCubeMaps[lightIndex], fragToLight).r;
    closestDepth *= 25.0; // far plane

    float currentDepth = length(fragToLight);
    float bias = 0.05;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

// Calcul de l'éclairage pour une lumière
vec3 CalcLight(Light light, int lightIndex) {
    vec3 color = vec3(0.0);
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 lightDir;
    float attenuation = 1.0;

    if (light.type == 0) { // Directional
        lightDir = normalize(-light.direction);
    } else if (light.type == 1) { // Point
        lightDir = normalize(light.position - FragPos);
        float distance = length(light.position - FragPos);
        attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    } else if (light.type == 2) { // Spot
        lightDir = normalize(light.position - FragPos);
        float distance = length(light.position - FragPos);
        attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        // Calcul spot
        float theta = dot(lightDir, normalize(-light.direction));
        float epsilon = light.cutOff - light.outerCutOff;
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        attenuation *= intensity;
    }

    // Ambient
    vec3 ambient = light.color * material.ambient;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * diff * material.diffuse;

    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.color * spec * material.specular;

    // Shadow
    float shadow = 0.0;
    if (light.castShadows) {
        if (light.type == 1) { // Point light
            shadow = ShadowCalculationPoint(lightIndex);
        } else { // Directional or Spot
            shadow = ShadowCalculation(FragPosLightSpace[lightIndex], lightIndex);
        }
    }

    // Combiner
    color = ambient + (1.0 - shadow) * (diffuse + specular);
    color *= attenuation * light.intensity;

    return color;
}

void main() {
    vec3 result = vec3(0.0);

    for (int i = 0; i < numLights && i < 8; ++i) {
        result += CalcLight(lights[i], i);
    }

    FragColor = vec4(result, 1.0);
}