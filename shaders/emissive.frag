#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D glowTexture;
uniform float intensity = 1.0;

void main() {
    vec4 texel = texture(glowTexture, TexCoords);
    FragColor = vec4(texel.rgb * intensity, texel.a);
}
