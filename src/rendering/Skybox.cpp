#include "rendering/Skybox.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

static const float skyboxVertices[] = {
    -1, 1,-1,  -1,-1,-1,   1,-1,-1,   1,-1,-1,   1, 1,-1,  -1, 1,-1,
    -1,-1, 1,  -1,-1,-1,  -1, 1,-1,  -1, 1,-1,  -1, 1, 1,  -1,-1, 1,
     1,-1,-1,   1,-1, 1,   1, 1, 1,   1, 1, 1,   1, 1,-1,   1,-1,-1,
    -1,-1, 1,  -1, 1, 1,   1, 1, 1,   1, 1, 1,   1,-1, 1,  -1,-1, 1,
    -1, 1,-1,   1, 1,-1,   1, 1, 1,   1, 1, 1,  -1, 1, 1,  -1, 1,-1,
    -1,-1,-1,  -1,-1, 1,   1,-1, 1,   1,-1, 1,   1,-1,-1,  -1,-1,-1,
};

bool Skybox::init(const std::string& folderPath) {
    std::vector<Vertex> verts(36);
    std::vector<unsigned int> indices(36);
    for (int i = 0; i < 36; ++i) {
        verts[i].position = glm::vec3(skyboxVertices[i*3], skyboxVertices[i*3+1], skyboxVertices[i*3+2]);
        verts[i].normal   = glm::vec3(0);
        verts[i].texCoords= glm::vec2(0);
        indices[i] = i;
    }
    cubeMesh_ = Mesh(verts, indices);

    // Load the 6 cubemap faces
    std::vector<std::string> faces = {
        folderPath + "/right.jpg",
        folderPath + "/left.jpg",
        folderPath + "/top.jpg",
        folderPath + "/bottom.jpg",
        folderPath + "/front.jpg",
        folderPath + "/back.jpg",
    };
    loaded_ = cubemap_.loadCubemap(faces);
    if (!loaded_) {
        std::cerr << "[Skybox] Cubemap not loaded — skybox will be skipped until textures are placed.\n";
    }
    return loaded_;
}

void Skybox::draw(const Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    if (!loaded_) return;  // no cubemap textures yet — skip gracefully

    // Remove translation from the view matrix so the skybox stays "at infinity"
    glm::mat4 skyView = glm::mat4(glm::mat3(view));

    glDepthFunc(GL_LEQUAL);
    shader.use();
    shader.setMat4("view", skyView);
    shader.setMat4("projection", projection);
    shader.setInt("skybox", 0);
    cubemap_.bind(GL_TEXTURE0);
    cubeMesh_.draw();
    glDepthFunc(GL_LESS);
}
