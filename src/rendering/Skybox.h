#ifndef SUNDOWNDRIVE_RENDERING_SKYBOX_H
#define SUNDOWNDRIVE_RENDERING_SKYBOX_H

#include "rendering/Mesh.h"
#include "rendering/Texture.h"
#include "core/Shader.h"

// ── Skybox ───────────────────────────────────────────────────────────────────
// Renders a cubemap around the camera.  The skybox is drawn last with depth
// trick (depth func ≤) so it always appears behind everything.
class Skybox {
public:
    Skybox() = default;

    // Load 6 face images and build the cube mesh.
    bool init(const std::string& folderPath);

    void draw(const Shader& shader, const glm::mat4& view, const glm::mat4& projection);

    bool isLoaded() const { return loaded_; }

private:
    Mesh    cubeMesh_;
    Texture cubemap_;
    bool    loaded_ = false;
};

#endif // SUNDOWNDRIVE_RENDERING_SKYBOX_H
