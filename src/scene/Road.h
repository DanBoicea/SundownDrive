#ifndef SUNDOWNDRIVE_SCENE_ROAD_H
#define SUNDOWNDRIVE_SCENE_ROAD_H

#include "rendering/Mesh.h"
#include "rendering/Texture.h"
#include "core/Shader.h"

class Terrain;

// ── Road ─────────────────────────────────────────────────────────────────────
// A closed circuit (oval/rectangular) textured road strip.
// Geometry is generated procedurally from a set of control points.
class Road {
public:
    Road() = default;

    // Generate road geometry and load texture. roadWidth in world units.
    bool init(const Terrain& terrain, float roadWidth, const std::string& texturePath);

    void draw(Shader& shader);

private:
    Mesh    mesh_;
    Texture texture_;
    float   roadWidth_ = 4.0f;
};

#endif // SUNDOWNDRIVE_SCENE_ROAD_H
