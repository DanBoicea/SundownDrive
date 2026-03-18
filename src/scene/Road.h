#ifndef SUNDOWNDRIVE_SCENE_ROAD_H
#define SUNDOWNDRIVE_SCENE_ROAD_H

#include "rendering/Mesh.h"
#include "rendering/Texture.h"
#include "core/Shader.h"

// ── Road ─────────────────────────────────────────────────────────────────────
// A closed circuit (oval/rectangular) textured road strip.
// Geometry is generated procedurally from a set of control points.
// Stub for P2.
class Road {
public:
    Road() = default;

    // Generate road geometry and load texture. roadWidth in world units.
    bool init(float roadWidth, const std::string& texturePath);

    void draw(Shader& shader);

    // Get the spline/path points for AI cars to follow
    // const std::vector<glm::vec3>& getPathPoints() const { return pathPoints_; }

private:
    Mesh    mesh_;
    Texture texture_;
    float   roadWidth_ = 4.0f;
    // std::vector<glm::vec3> pathPoints_;  // center-line for AI
};

#endif // SUNDOWNDRIVE_SCENE_ROAD_H
