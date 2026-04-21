#ifndef SUNDOWNDRIVE_SCENE_SCENE_H
#define SUNDOWNDRIVE_SCENE_SCENE_H

#include "scene/Terrain.h"
#include "scene/Road.h"
#include "rendering/Skybox.h"
#include "rendering/Renderer.h"
#include "rendering/Model.h"
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
    Model     streetLampModel_;
    std::vector<Transform> treeTransforms_;
    std::vector<Transform> streetLampTransforms_;
    std::vector<glm::vec3> lampLightPositions_;

    // Future (C1/C2): Car player_, vector<TrafficCar> traffic_;
};

#endif // SUNDOWNDRIVE_SCENE_SCENE_H
