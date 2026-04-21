#ifndef SUNDOWNDRIVE_SCENE_SCENE_H
#define SUNDOWNDRIVE_SCENE_SCENE_H

#include "scene/Terrain.h"
#include "scene/Road.h"
#include "rendering/Skybox.h"
#include "rendering/Renderer.h"
#include "rendering/Model.h"
#include "rendering/Mesh.h"
#include "rendering/Texture.h"
#include "utils/Transform.h"
#include "core/Camera.h"
#include <glm/glm.hpp>
#include <vector>

class Scene {
public:
    Scene() = default;

    bool init();
    void update(float deltaTime);
    void draw();

    Camera&   getCamera()   { return camera_; }
    Renderer& getRenderer() { return renderer_; }

private:
    Renderer  renderer_;
    Camera    camera_;
    Terrain   terrain_;
    Skybox    skybox_;
    Road      road_;
    Model     treeModel_;
    std::vector<Transform> treeTransforms_;
    Mesh      lampPoleMesh_;
    Texture   lampPoleTexture_;
    Texture   lampGlowTexture_;
    std::vector<Transform> lampPoleTransforms_;
    std::vector<Transform> lampHeadTransforms_;
    std::vector<glm::vec3> lampLightPositions_;

    // Future (C1/C2): Car player_, vector<TrafficCar> traffic_;
};

#endif // SUNDOWNDRIVE_SCENE_SCENE_H
