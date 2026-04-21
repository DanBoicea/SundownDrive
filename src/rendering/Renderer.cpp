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
