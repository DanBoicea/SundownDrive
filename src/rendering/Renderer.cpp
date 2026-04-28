#include "rendering/Renderer.h"
#include "utils/Colors.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

bool Renderer::init() {
    if (!basicShader_.load("shaders/basic.vert", "shaders/basic.frag")) {
        std::cerr << "[Renderer] Failed to load basic shader" << std::endl;
        return false;
    }
    if (!skyboxShader_.load("shaders/skybox.vert", "shaders/skybox.frag")) {
        std::cerr << "[Renderer] Failed to load skybox shader" << std::endl;
        return false;
    }
    if (!shadowShader_.load("shaders/shadow_depth.vert", "shaders/shadow_depth.frag")) {
        std::cerr << "[Renderer] Failed to load shadow shader" << std::endl;
        return false;
    }
    if (!pointShadowShader_.load("shaders/shadow_point.vert", "shaders/shadow_point.frag")) {
        std::cerr << "[Renderer] Failed to load point shadow shader" << std::endl;
        return false;
    }
    if (!emissiveShader_.load("shaders/emissive.vert", "shaders/emissive.frag")) {
        std::cerr << "[Renderer] Failed to load emissive shader" << std::endl;
        return false;
    }

    glGenFramebuffers(1, &shadowFBO_);
    glGenTextures(1, &shadowMap_);
    glBindTexture(GL_TEXTURE_2D, shadowMap_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 shadowMapSize_, shadowMapSize_, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    const float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap_, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "[Renderer] Shadow framebuffer incomplete" << std::endl;
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    return true;
}

bool Renderer::initPointLightShadows(int count) {
    pointShadowMaps_.clear();
    if (count <= 0) {
        return true;
    }

    glGenFramebuffers(1, &pointShadowFBO_);
    pointShadowMaps_.resize(static_cast<size_t>(count), 0);
    glGenTextures(count, pointShadowMaps_.data());

    for (int i = 0; i < count; ++i) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, pointShadowMaps_[i]);
        for (int face = 0; face < 6; ++face) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_DEPTH_COMPONENT,
                         pointShadowMapSize_, pointShadowMapSize_, 0,
                         GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, pointShadowFBO_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X, pointShadowMaps_[0], 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "[Renderer] Point shadow framebuffer incomplete" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void Renderer::beginFrame() {
    glClearColor(Colors::SunsetOrange.r, Colors::SunsetOrange.g, Colors::SunsetOrange.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::beginShadowPass() {
    glm::vec3 lightPos = -sunLightDir_ * 80.0f;
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightProj = glm::ortho(-70.0f, 70.0f, -70.0f, 70.0f, 1.0f, 180.0f);
    lightSpaceMatrix_ = lightProj * lightView;

    glViewport(0, 0, shadowMapSize_, shadowMapSize_);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO_);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
}

void Renderer::endShadowPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK);
    glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

void Renderer::beginPointLightShadowPass(int lightIndex, int faceIndex) {
    glViewport(0, 0, pointShadowMapSize_, pointShadowMapSize_);
    glBindFramebuffer(GL_FRAMEBUFFER, pointShadowFBO_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex,
                           pointShadowMaps_[lightIndex], 0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
}

void Renderer::endPointLightShadowPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK);
    glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}
