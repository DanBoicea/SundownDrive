#include "scene/Scene.h"
#include "core/Input.h"
#include "utils/Constants.h"

#include <GL/glew.h>
#include <cmath>
#include <iostream>
#include <random>
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

    for (const auto& tree : treeTransforms_) {
        basic.setMat4("model", tree.getModelMatrix());
        treeModel_.draw(basic);
    }

    // Skybox last (depth trick)
    skybox_.draw(renderer_.getSkyboxShader(), view, proj);
}
