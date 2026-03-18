#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D textureSampler;

// Simple directional "sun" light — will be expanded in P3
uniform vec3 lightDir   = vec3(-0.5, -0.8, -0.3);  // direction TO the light
uniform vec3 lightColor = vec3(1.0, 0.85, 0.6);     // warm sunset
uniform vec3 ambient    = vec3(0.25, 0.18, 0.2);     // dim purple ambient

void main() {
    vec3 norm = normalize(Normal);
    vec3 dir  = normalize(-lightDir);
    float diff = max(dot(norm, dir), 0.0);

    vec3 texColor = texture(textureSampler, TexCoords).rgb;
    vec3 result   = (ambient + diff * lightColor) * texColor;

    FragColor = vec4(result, 1.0);
}
