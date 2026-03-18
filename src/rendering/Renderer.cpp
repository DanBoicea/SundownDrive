#include "rendering/Renderer.h"
#include "utils/Colors.h"
#include <GL/glew.h>
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
