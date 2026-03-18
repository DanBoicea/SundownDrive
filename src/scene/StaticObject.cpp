#include "scene/StaticObject.h"

StaticObject::StaticObject(Mesh* mesh, Texture* texture, const Transform& transform)
    : mesh_(mesh), texture_(texture), transform_(transform) {}

void StaticObject::draw(Shader& shader) const {
    if (!mesh_) return;

    shader.use();
    shader.setMat4("model", transform_.getModelMatrix());

    if (texture_) {
        texture_->bind(GL_TEXTURE0);
        shader.setInt("textureSampler", 0);
    }

    mesh_->draw();
}
