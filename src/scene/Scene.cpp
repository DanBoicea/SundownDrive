#include "scene/Scene.h"
#include "core/Input.h"
#include "utils/Constants.h"

#include <GL/glew.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace {
bool isInsideOval(float x, float z) {
    constexpr float a = 30.0f;
    constexpr float b = 20.0f;
    float ex = x / a;
    float ez = z / b;
    return (ex * ex + ez * ez) <= 1.0f;
}

float distanceToOvalRoadCenterline(float x, float z) {
    constexpr float a = 30.0f;
    constexpr float b = 20.0f;

    float ex = x / a;
    float ez = z / b;
    float ovalDist = std::sqrt(ex * ex + ez * ez);

    if (ovalDist < 1e-4f) {
        return (a < b) ? a : b;
    }

    float nx = ex / ovalDist;
    float nz = ez / ovalDist;
    float cx = nx * a;
    float cz = nz * b;
    float dx = x - cx;
    float dz = z - cz;
    return std::sqrt(dx * dx + dz * dz);
}
}

bool Scene::init() {
    if (!renderer_.init()) return false;

    terrain_.init(Constants::WorldHalfSize, 128, "assets/textures/grass.jpg"); // 128 = smooth hills
    road_.init(terrain_, 6.0f, "assets/textures/road.jpg");

    skybox_.init("assets/textures/skybox");

    if (!treeModel_.load("assets/models/tree.obj")) {
        std::cerr << "[Scene] Tree model load failed: assets/models/tree.obj" << std::endl;
        return false;
    }

    if (!streetLampModel_.load("assets/models/street_lamp.obj")) {
        std::cerr << "[Scene] Street lamp model load failed: assets/models/street_lamp.obj" << std::endl;
        return false;
    }

    if (!placeholderTex_.load2D("assets/textures/road.jpg")) {
        std::cerr << "[Scene] Placeholder texture load failed" << std::endl;
        return false;
    }

    if (!lampGlowTex_.load2D("assets/textures/lamp_pole/lighting_pole_model_1_lamp_part_BaseColor.png")) {
        std::cerr << "[Scene] Lamp glow texture load failed" << std::endl;
        return false;
    }

    {
        std::vector<Vertex> verts(4);
        verts[0].position = glm::vec3(-0.5f, -0.5f, 0.0f);
        verts[1].position = glm::vec3( 0.5f, -0.5f, 0.0f);
        verts[2].position = glm::vec3( 0.5f,  0.5f, 0.0f);
        verts[3].position = glm::vec3(-0.5f,  0.5f, 0.0f);
        for (auto& v : verts) {
            v.normal = glm::vec3(0.0f, 0.0f, 1.0f);
        }
        verts[0].texCoords = glm::vec2(0.0f, 0.0f);
        verts[1].texCoords = glm::vec2(1.0f, 0.0f);
        verts[2].texCoords = glm::vec2(1.0f, 1.0f);
        verts[3].texCoords = glm::vec2(0.0f, 1.0f);

        std::vector<unsigned int> idx = {0, 1, 2, 0, 2, 3};
        lampGlowMesh_ = Mesh(verts, idx);
    }

    {
        std::vector<Vertex> verts = {
            {{-0.5f, -0.5f,  0.5f}, {0, 0, 1}, {0, 0}},
            {{ 0.5f, -0.5f,  0.5f}, {0, 0, 1}, {1, 0}},
            {{ 0.5f,  0.5f,  0.5f}, {0, 0, 1}, {1, 1}},
            {{-0.5f,  0.5f,  0.5f}, {0, 0, 1}, {0, 1}},

            {{ 0.5f, -0.5f, -0.5f}, {0, 0, -1}, {0, 0}},
            {{-0.5f, -0.5f, -0.5f}, {0, 0, -1}, {1, 0}},
            {{-0.5f,  0.5f, -0.5f}, {0, 0, -1}, {1, 1}},
            {{ 0.5f,  0.5f, -0.5f}, {0, 0, -1}, {0, 1}},

            {{-0.5f, -0.5f, -0.5f}, {-1, 0, 0}, {0, 0}},
            {{-0.5f, -0.5f,  0.5f}, {-1, 0, 0}, {1, 0}},
            {{-0.5f,  0.5f,  0.5f}, {-1, 0, 0}, {1, 1}},
            {{-0.5f,  0.5f, -0.5f}, {-1, 0, 0}, {0, 1}},

            {{ 0.5f, -0.5f,  0.5f}, {1, 0, 0}, {0, 0}},
            {{ 0.5f, -0.5f, -0.5f}, {1, 0, 0}, {1, 0}},
            {{ 0.5f,  0.5f, -0.5f}, {1, 0, 0}, {1, 1}},
            {{ 0.5f,  0.5f,  0.5f}, {1, 0, 0}, {0, 1}},

            {{-0.5f,  0.5f,  0.5f}, {0, 1, 0}, {0, 0}},
            {{ 0.5f,  0.5f,  0.5f}, {0, 1, 0}, {1, 0}},
            {{ 0.5f,  0.5f, -0.5f}, {0, 1, 0}, {1, 1}},
            {{-0.5f,  0.5f, -0.5f}, {0, 1, 0}, {0, 1}},

            {{-0.5f, -0.5f, -0.5f}, {0, -1, 0}, {0, 0}},
            {{ 0.5f, -0.5f, -0.5f}, {0, -1, 0}, {1, 0}},
            {{ 0.5f, -0.5f,  0.5f}, {0, -1, 0}, {1, 1}},
            {{-0.5f, -0.5f,  0.5f}, {0, -1, 0}, {0, 1}},
        };
        std::vector<unsigned int> idx = {
            0, 1, 2, 0, 2, 3,
            4, 5, 6, 4, 6, 7,
            8, 9,10, 8,10,11,
           12,13,14,12,14,15,
           16,17,18,16,18,19,
           20,21,22,20,22,23
        };
        placeholderMesh_ = Mesh(verts, idx);
    }

    std::mt19937 rng(1337u);
    std::uniform_real_distribution<float> jitterDist(-1.0f, 1.0f);
    std::uniform_real_distribution<float> yawDist(0.0f, 360.0f);
    std::uniform_real_distribution<float> scaleDist(4.2f, 6.2f);
    std::uniform_real_distribution<float> keepDist(0.0f, 1.0f);

    treeTransforms_.clear();
    treeTransforms_.reserve(160);

    constexpr float mapHalf = 46.0f;
    constexpr float gridStep = 7.2f;
    constexpr float roadKeepout = 6.5f;

    for (float z = -mapHalf; z <= mapHalf; z += gridStep) {
        for (float x = -mapHalf; x <= mapHalf; x += gridStep) {
            float px = x + jitterDist(rng);
            float pz = z + jitterDist(rng);

            if (std::abs(px) > 48.5f || std::abs(pz) > 48.5f) {
                continue;
            }

            if (isInsideOval(px, pz)) {
                continue;
            }

            if (keepDist(rng) > 0.7f) {
                continue;
            }

            float treeScale = scaleDist(rng);
            float canopyRadius = treeScale * 0.65f;

            float roadDist = distanceToOvalRoadCenterline(px, pz);
            if (roadDist < roadKeepout + canopyRadius) {
                continue;
            }

            Transform t;
            t.position = glm::vec3(px, 0.0f, pz);
            t.rotation.y = yawDist(rng);
            t.scale = glm::vec3(treeScale);
            t.position.y = terrain_.getHeightAt(t.position.x, t.position.z);
            treeTransforms_.push_back(t);
        }
    }

    streetLampTransforms_.clear();
    lampLightPositions_.clear();
    constexpr int poleCount = 8;
    constexpr float a = 30.0f;
    constexpr float b = 20.0f;
    constexpr float inwardOffset = 5.0f;
    constexpr float lampScale = 2.8f;
    constexpr float lampMinY = 0.017458f;
    constexpr float lampHeight = 2.378824f - 0.017458f;
    constexpr float worldLampHeight = lampHeight * lampScale;

    for (int i = 0; i < poleCount; ++i) {
        float t = 2.0f * 3.14159265f * static_cast<float>(i) / static_cast<float>(poleCount);
        glm::vec3 center(a * std::cos(t), 0.0f, b * std::sin(t));
        glm::vec3 toCenter = glm::normalize(glm::vec3(-center.x, 0.0f, -center.z));
        glm::vec3 basePos = center + toCenter * inwardOffset;
        basePos.y = terrain_.getHeightAt(basePos.x, basePos.z);

        Transform pole;
        pole.position = basePos;
        pole.position.y -= lampMinY * lampScale;
        pole.rotation.y = -glm::degrees(std::atan2(toCenter.z, toCenter.x)) - 90.0f;
        pole.scale = glm::vec3(lampScale);
        streetLampTransforms_.push_back(pole);

        glm::vec3 lampLightPos = basePos + glm::vec3(0.0f, worldLampHeight + 0.25f, 0.0f);
        lampLightPositions_.push_back(lampLightPos);
    }

    placeholderTransforms_.clear();
    int placeholderCount = std::min(3, static_cast<int>(lampLightPositions_.size()));
    for (int i = 0; i < placeholderCount; ++i) {
        int next = (i + 1) % static_cast<int>(lampLightPositions_.size());
        glm::vec3 mid = 0.5f * (lampLightPositions_[i] + lampLightPositions_[next]);
        mid.y = terrain_.getHeightAt(mid.x, mid.z);

        Transform t;
        t.position = mid + glm::vec3(0.0f, 0.6f, 0.0f);
        t.scale = glm::vec3(1.2f, 1.2f, 1.2f);
        placeholderTransforms_.push_back(t);
    }

    if (!renderer_.initPointLightShadows(static_cast<int>(lampLightPositions_.size()))) {
        std::cerr << "[Scene] Failed to init point light shadows" << std::endl;
        return false;
    }

    return true;
}

void Scene::update(float deltaTime) {
    if (Input::isKeyDown('w')) camera_.processKeyboard(0, deltaTime);
    if (Input::isKeyDown('s')) camera_.processKeyboard(1, deltaTime);
    if (Input::isKeyDown('a')) camera_.processKeyboard(2, deltaTime);
    if (Input::isKeyDown('d')) camera_.processKeyboard(3, deltaTime);

    int dx = Input::getMouseDeltaX();
    int dy = Input::getMouseDeltaY();
    if (dx || dy) camera_.processMouse(static_cast<float>(dx), static_cast<float>(dy));

    if (Input::isKeyPressed(27)) {
        Input::setMouseCaptured(!Input::isMouseCaptured());
    }

    Input::resetMouseDelta();

    camera_.update(deltaTime);
}

void Scene::draw() {
    // Point light shadow cubemaps (one cubemap per lamp)
    Shader& pointShadow = renderer_.getPointShadowShader();
    pointShadow.use();
    pointShadow.setFloat("farPlane", renderer_.getPointShadowFarPlane());

    static const glm::vec3 faceDirs[6] = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
    };
    static const glm::vec3 faceUps[6] = {
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
    };

    glm::mat4 pointProj = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f,
                                           renderer_.getPointShadowFarPlane());

    int pointShadowCount = renderer_.getPointShadowCount();
    int lampCount = static_cast<int>(lampLightPositions_.size());
    if (lampCount > pointShadowCount) lampCount = pointShadowCount;

    for (int i = 0; i < lampCount; ++i) {
        const glm::vec3& lightPos = lampLightPositions_[i];
        pointShadow.setVec3("lightPos", lightPos);

        for (int face = 0; face < 6; ++face) {
            renderer_.beginPointLightShadowPass(i, face);
            glm::mat4 view = glm::lookAt(lightPos, lightPos + faceDirs[face], faceUps[face]);
            pointShadow.setMat4("projection", pointProj);
            pointShadow.setMat4("view", view);

            terrain_.draw(pointShadow);
            road_.draw(pointShadow);

            for (const auto& tree : treeTransforms_) {
                pointShadow.setMat4("model", tree.getModelMatrix());
                treeModel_.draw(pointShadow);
            }

            for (const auto& pole : streetLampTransforms_) {
                pointShadow.setMat4("model", pole.getModelMatrix());
                streetLampModel_.draw(pointShadow);
            }

            for (const auto& box : placeholderTransforms_) {
                pointShadow.setMat4("model", box.getModelMatrix());
                placeholderMesh_.draw();
            }

            renderer_.endPointLightShadowPass();
        }
    }

    renderer_.beginShadowPass();
    Shader& shadow = renderer_.getShadowShader();
    shadow.use();
    shadow.setMat4("lightSpaceMatrix", renderer_.getLightSpaceMatrix());

    terrain_.draw(shadow);
    road_.draw(shadow);

    for (const auto& tree : treeTransforms_) {
        shadow.setMat4("model", tree.getModelMatrix());
        treeModel_.draw(shadow);
    }

    for (const auto& pole : streetLampTransforms_) {
        shadow.setMat4("model", pole.getModelMatrix());
        streetLampModel_.draw(shadow);
    }

    for (const auto& box : placeholderTransforms_) {
        shadow.setMat4("model", box.getModelMatrix());
        placeholderMesh_.draw();
    }

    renderer_.endShadowPass();

    renderer_.beginFrame();

    float aspect = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);
    glm::mat4 view = camera_.getViewMatrix();
    glm::mat4 proj = camera_.getProjectionMatrix(aspect);

    // Set VP matrices on the basic shader
    Shader& basic = renderer_.getBasicShader();
    basic.use();
    basic.setMat4("view", view);
    basic.setMat4("projection", proj);
    basic.setMat4("lightSpaceMatrix", renderer_.getLightSpaceMatrix());
    basic.setVec3("lightDir", renderer_.getSunLightDir());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, renderer_.getShadowMap());
    basic.setInt("shadowMap", 1);

    int activeLampCount = static_cast<int>(lampLightPositions_.size());
    if (activeLampCount > 32) activeLampCount = 32;
    basic.setInt("pointLightCount", activeLampCount);
    basic.setInt("pointShadowCount", lampCount);
    basic.setFloat("pointShadowFar", renderer_.getPointShadowFarPlane());

    for (int i = 0; i < lampCount; ++i) {
        glActiveTexture(GL_TEXTURE2 + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, renderer_.getPointShadowMap(i));
        basic.setInt("pointShadowMap[" + std::to_string(i) + "]", 2 + i);
    }
    for (int i = 0; i < activeLampCount; ++i) {
        basic.setVec3("pointLightPos[" + std::to_string(i) + "]", lampLightPositions_[i]);
        basic.setVec3("pointLightColor[" + std::to_string(i) + "]", glm::vec3(2.10f, 1.70f, 1.20f));
    }

    // Draw scene objects
    terrain_.draw(basic);
    road_.draw(basic);

    for (const auto& pole : streetLampTransforms_) {
        basic.setMat4("model", pole.getModelMatrix());
        streetLampModel_.draw(basic);
    }

    placeholderTex_.bind(GL_TEXTURE0);
    basic.setInt("textureSampler", 0);
    for (const auto& box : placeholderTransforms_) {
        basic.setMat4("model", box.getModelMatrix());
        placeholderMesh_.draw();
    }

    for (const auto& tree : treeTransforms_) {
        basic.setMat4("model", tree.getModelMatrix());
        treeModel_.draw(basic);
    }

    // Lamp glow billboards (visual light sources)
    Shader& emissive = renderer_.getEmissiveShader();
    emissive.use();
    emissive.setMat4("view", view);
    emissive.setMat4("projection", proj);
    emissive.setFloat("intensity", 1.4f);
    emissive.setInt("glowTexture", 0);

    glm::mat4 invView = glm::inverse(view);
    glm::vec3 camRight = glm::normalize(glm::vec3(invView[0]));
    glm::vec3 camUp = glm::normalize(glm::vec3(invView[1]));
    glm::vec3 camForward = glm::normalize(glm::vec3(invView[2]));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);
    lampGlowTex_.bind(GL_TEXTURE0);

    for (const auto& pos : lampLightPositions_) {
        glm::mat4 model(1.0f);
        model[0] = glm::vec4(camRight * lampGlowSize_, 0.0f);
        model[1] = glm::vec4(camUp * lampGlowSize_, 0.0f);
        model[2] = glm::vec4(camForward * lampGlowSize_, 0.0f);
        model[3] = glm::vec4(pos, 1.0f);
        emissive.setMat4("model", model);
        lampGlowMesh_.draw();
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    // Skybox last (depth trick)
    skybox_.draw(renderer_.getSkyboxShader(), view, proj);
}
