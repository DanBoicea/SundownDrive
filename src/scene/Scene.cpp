#include "scene/Scene.h"
#include "core/Input.h"
#include "utils/Constants.h"

bool Scene::init() {
    if (!renderer_.init()) return false;

    terrain_.init(Constants::WorldHalfSize, 128, "assets/textures/grass.jpg"); // 128 = smooth hills

    skybox_.init("assets/textures/skybox");

    return true;
}

void Scene::update(float deltaTime) {
    if (Input::isKeyDown('w')) camera_.processKeyboard(0, deltaTime);
    if (Input::isKeyDown('s')) camera_.processKeyboard(1, deltaTime);
    if (Input::isKeyDown('a')) camera_.processKeyboard(2, deltaTime);
    if (Input::isKeyDown('d')) camera_.processKeyboard(3, deltaTime);

    int dx = Input::getMouseDeltaX();
    int dy = Input::getMouseDeltaY();
    if (dx || dy) camera_.processMouse(static_cast<float>(dx), static_cast<float>(dy));

    if (Input::isKeyPressed(27)) {
        Input::setMouseCaptured(!Input::isMouseCaptured());
    }

    Input::resetMouseDelta();

    camera_.update(deltaTime);
}

void Scene::draw() {
    renderer_.beginFrame();

    float aspect = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);
    glm::mat4 view = camera_.getViewMatrix();
    glm::mat4 proj = camera_.getProjectionMatrix(aspect);

    // Set VP matrices on the basic shader
    Shader& basic = renderer_.getBasicShader();
    basic.use();
    basic.setMat4("view", view);
    basic.setMat4("projection", proj);

    // Draw scene objects
    terrain_.draw(basic);
    road_.draw(basic);

    for (auto& obj : staticObjects_) {
        obj.draw(basic);
    }

    // Skybox last (depth trick)
    skybox_.draw(renderer_.getSkyboxShader(), view, proj);
}
