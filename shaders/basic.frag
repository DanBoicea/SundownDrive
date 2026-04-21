#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform sampler2D textureSampler;
uniform sampler2D shadowMap;
uniform bool useAlphaCutout = false;
uniform int pointLightCount = 0;
uniform vec3 pointLightPos[32];
uniform vec3 pointLightColor[32];

// Simple directional "sun" light — will be expanded in P3
uniform vec3 lightDir   = vec3(-0.5, -0.8, -0.3);  // direction TO the light
uniform vec3 lightColor = vec3(1.0, 0.85, 0.6);     // warm sunset
uniform vec3 ambient    = vec3(0.25, 0.18, 0.2);     // dim purple ambient

float calcShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDirection) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0) {
        return 0.0;
    }

    float currentDepth = projCoords.z;
    float bias = max(0.0025 * (1.0 - dot(normal, lightDirection)), 0.0006);
    float shadow = 0.0;

    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (currentDepth - bias > pcfDepth) ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 dir  = normalize(-lightDir);
    float diff = max(dot(norm, dir), 0.0);

    vec4 texel = texture(textureSampler, TexCoords);
    if (useAlphaCutout && texel.a < 0.3) {
        discard;
    }

    float shadow = calcShadow(FragPosLightSpace, norm, dir);

    vec3 texColor = texel.rgb;
    vec3 sunTerm = (1.0 - shadow) * diff * lightColor;

    vec3 lampTerm = vec3(0.0);
    for (int i = 0; i < pointLightCount; ++i) {
        vec3 toLight = pointLightPos[i] - FragPos;
        float dist = length(toLight);
        vec3 ldir = toLight / max(dist, 0.0001);

        float ndotl = max(dot(norm, ldir), 0.0);
        float atten = 1.0 / (1.0 + 0.12 * dist + 0.045 * dist * dist);
        lampTerm += ndotl * atten * pointLightColor[i];
    }

    vec3 result = (ambient + sunTerm + lampTerm) * texColor;

    FragColor = vec4(result, 1.0);
}
