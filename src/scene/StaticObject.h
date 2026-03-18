#ifndef SUNDOWNDRIVE_SCENE_STATICOBJECT_H
#define SUNDOWNDRIVE_SCENE_STATICOBJECT_H

#include "rendering/Mesh.h"
#include "rendering/Texture.h"
#include "utils/Transform.h"
#include "core/Shader.h"

// ── StaticObject ─────────────────────────────────────────────────────────────
// A placed, non-moving object in the scene (building, tree, lamp post, etc.).
// Holds a reference to shared Mesh + Texture, plus its own Transform.
class StaticObject {
public:
    StaticObject() = default;
    StaticObject(Mesh* mesh, Texture* texture, const Transform& transform);

    void draw(Shader& shader) const;

    Transform&       getTransform()       { return transform_; }
    const Transform& getTransform() const { return transform_; }

    // AABB for collision detection (computed from mesh bounds + transform)
    // glm::vec3 getAABBMin() const;
    // glm::vec3 getAABBMax() const;

private:
    Mesh*     mesh_    = nullptr;   // shared, not owned
    Texture*  texture_ = nullptr;   // shared, not owned
    Transform transform_;
};

#endif // SUNDOWNDRIVE_SCENE_STATICOBJECT_H
