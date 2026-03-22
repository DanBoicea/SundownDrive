#include "scene/Road.h"
#include "scene/Terrain.h"

#include <glm/gtc/constants.hpp>
#include <iostream>
#include <vector>

namespace {
    constexpr float kRoadYOffset = 0.03f;
    constexpr float kOvalA = 30.0f;
    constexpr float kOvalB = 20.0f;
    constexpr int kSegments = 160;
    constexpr float kUvVScale = 0.25f;

    glm::vec3 sampleTerrainNormal(const Terrain& terrain, float x, float z) {
        constexpr float eps = 0.35f;
        float hL = terrain.getHeightAt(x - eps, z);
        float hR = terrain.getHeightAt(x + eps, z);
        float hD = terrain.getHeightAt(x, z - eps);
        float hU = terrain.getHeightAt(x, z + eps);
        glm::vec3 n(hL - hR, 2.0f * eps, hD - hU);
        return glm::normalize(n);
    }
}

bool Road::init(const Terrain& terrain, float roadWidth, const std::string& texturePath) {
    roadWidth_ = roadWidth;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    vertices.reserve(kSegments * 2);
    indices.reserve(kSegments * 6);

    const float halfWidth = roadWidth_ * 0.5f;
    float accumulatedLength = 0.0f;
    glm::vec3 prevCenter(kOvalA, terrain.getHeightAt(kOvalA, 0.0f) + kRoadYOffset, 0.0f);

    for (int i = 0; i < kSegments; ++i) {
        float t = glm::two_pi<float>() * static_cast<float>(i) / static_cast<float>(kSegments);
        glm::vec3 center(kOvalA * std::cos(t), 0.0f, kOvalB * std::sin(t));

        glm::vec3 tangent(-kOvalA * std::sin(t), 0.0f, kOvalB * std::cos(t));
        tangent = glm::normalize(tangent);
        glm::vec3 lateral(-tangent.z, 0.0f, tangent.x);

        if (i > 0) {
            accumulatedLength += glm::length(center - prevCenter);
        }
        prevCenter = center;

        float v = accumulatedLength * kUvVScale;

        glm::vec3 leftPos = center - lateral * halfWidth;
        glm::vec3 rightPos = center + lateral * halfWidth;

        leftPos.y = terrain.getHeightAt(leftPos.x, leftPos.z) + kRoadYOffset;
        rightPos.y = terrain.getHeightAt(rightPos.x, rightPos.z) + kRoadYOffset;

        glm::vec3 leftNormal = sampleTerrainNormal(terrain, leftPos.x, leftPos.z);
        glm::vec3 rightNormal = sampleTerrainNormal(terrain, rightPos.x, rightPos.z);

        Vertex left{};
        left.position = leftPos;
        left.normal = leftNormal;
        left.texCoords = glm::vec2(0.0f, v);

        Vertex right{};
        right.position = rightPos;
        right.normal = rightNormal;
        right.texCoords = glm::vec2(1.0f, v);

        vertices.push_back(left);
        vertices.push_back(right);
    }

    for (int i = 0; i < kSegments; ++i) {
        unsigned int i0 = static_cast<unsigned int>(i * 2);
        unsigned int i1 = i0 + 1;
        unsigned int n0 = static_cast<unsigned int>(((i + 1) % kSegments) * 2);
        unsigned int n1 = n0 + 1;

        indices.push_back(i0);
        indices.push_back(i1);
        indices.push_back(n0);

        indices.push_back(i1);
        indices.push_back(n1);
        indices.push_back(n0);
    }

    mesh_ = Mesh(vertices, indices);

    if (!texturePath.empty()) {
        if (!texture_.load2D(texturePath)) {
            std::cerr << "[Road] Texture load failed: " << texturePath << std::endl;
            texture_.load2D("assets/textures/grass.jpg");
        }
    } else {
        texture_.load2D("assets/textures/grass.jpg");
    }

    return true;
}

void Road::draw(Shader& shader) {
    shader.use();
    shader.setMat4("model", glm::mat4(1.0f));
    texture_.bind(GL_TEXTURE0);
    shader.setInt("textureSampler", 0);
    mesh_.draw();
}
