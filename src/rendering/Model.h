#ifndef SUNDOWNDRIVE_RENDERING_MODEL_H
#define SUNDOWNDRIVE_RENDERING_MODEL_H

#include "rendering/Mesh.h"
#include "rendering/Texture.h"
#include "core/Shader.h"
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
    void draw(Shader& shader) const;

    const std::vector<Mesh>& getMeshes() const { return meshes_; }

private:
    struct SubMesh {
        Mesh mesh;
        int materialId = -1;
    };

    std::vector<Mesh>    meshes_;
    std::vector<SubMesh> subMeshes_;
    std::vector<Texture> materialTextures_;
    std::vector<bool>    materialHasTexture_;
    std::vector<bool>    materialAlphaCutout_;
};

#endif // SUNDOWNDRIVE_RENDERING_MODEL_H
