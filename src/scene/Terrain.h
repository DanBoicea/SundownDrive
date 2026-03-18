#ifndef SUNDOWNDRIVE_SCENE_TERRAIN_H
#define SUNDOWNDRIVE_SCENE_TERRAIN_H

#include "rendering/Mesh.h"
#include "rendering/Texture.h"
#include "core/Shader.h"
#include <glm/glm.hpp>
#include <vector>

// ── Terrain ──────────────────────────────────────────────────────────────────
// Generates a procedurally-displaced terrain mesh with tiled grass texture.
// An oval corridor is kept flat for the future road (P2).
// Heights are computed from layered sine waves and stored for fast querying.
class Terrain {
public:
    Terrain() = default;

    // Generate the terrain grid mesh and load its texture.
    bool init(float halfSize, int resolution, const std::string& texturePath);

    void draw(Shader& shader);

    // Query height at any world (x, z) position via bilinear interpolation.
    float getHeightAt(float x, float z) const;

private:
    Mesh    mesh_;
    Texture texture_;
    float   halfSize_    = 50.0f;
    int     resolution_  = 64;

    // Stored heightmap grid for fast getHeightAt queries (row-major, (res+1)^2)
    std::vector<float> heights_;

    // Procedural height at a world position (layered sine, no road mask)
    static float rawHeight(float wx, float wz);

    // Returns 0..1 blend factor: 0 = on the road (flat), 1 = full terrain relief
    static float roadMask(float wx, float wz);

    // Compute the normal at (wx, wz) via central differences
    glm::vec3 computeNormal(float wx, float wz, float step) const;
};

#endif // SUNDOWNDRIVE_SCENE_TERRAIN_H
