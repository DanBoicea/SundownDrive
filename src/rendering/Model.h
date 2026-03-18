#ifndef SUNDOWNDRIVE_RENDERING_MODEL_H
#define SUNDOWNDRIVE_RENDERING_MODEL_H

#include "rendering/Mesh.h"
#include "rendering/Texture.h"
#include <string>
#include <vector>

// ── Model ────────────────────────────────────────────────────────────────────
// Loads a .obj file (via tinyobjloader) into one or more Mesh objects.
// Optionally loads associated textures.
class Model {
public:
    Model() = default;

    bool load(const std::string& objPath);
    void draw() const;

    const std::vector<Mesh>& getMeshes() const { return meshes_; }

private:
    std::vector<Mesh> meshes_;
};

#endif // SUNDOWNDRIVE_RENDERING_MODEL_H
