#ifndef SUNDOWNDRIVE_RENDERING_RENDERER_H
#define SUNDOWNDRIVE_RENDERING_RENDERER_H

#include "core/Shader.h"
#include "core/Camera.h"
#include <glm/glm.hpp>
#include <vector>

// ── Renderer ─────────────────────────────────────────────────────────────────
// Owns all shader programs and orchestrates the draw pipeline:
//   1) Shadow pass (future — P3)
//   2) Main geometry pass
//   3) Skybox pass
// Scene objects call back into Renderer to get the active shader.
class Renderer {
public:
    bool init();

    // Call once per frame before drawing anything
    void beginFrame();

    // Shadow pass control (P3)
    void beginShadowPass();
    void endShadowPass();

    // Point light shadow pass control (cubemap)
    bool initPointLightShadows(int count);
    void beginPointLightShadowPass(int lightIndex, int faceIndex);
    void endPointLightShadowPass();

    // Access shaders by name
    Shader& getBasicShader()   { return basicShader_; }
    Shader& getSkyboxShader()  { return skyboxShader_; }
    Shader& getShadowShader()  { return shadowShader_; }
    Shader& getPointShadowShader() { return pointShadowShader_; }
    Shader& getEmissiveShader() { return emissiveShader_; }

    GLuint getShadowMap() const { return shadowMap_; }
    const glm::mat4& getLightSpaceMatrix() const { return lightSpaceMatrix_; }
    const glm::vec3& getSunLightDir() const { return sunLightDir_; }

    GLuint getPointShadowMap(int index) const { return pointShadowMaps_[index]; }
    int getPointShadowCount() const { return static_cast<int>(pointShadowMaps_.size()); }
    float getPointShadowFarPlane() const { return pointShadowFarPlane_; }

private:
    Shader basicShader_;
    Shader skyboxShader_;
    Shader shadowShader_;
    Shader pointShadowShader_;
    Shader emissiveShader_;

    GLuint shadowFBO_ = 0;
    GLuint shadowMap_ = 0;
    int shadowMapSize_ = 2048;

    GLuint pointShadowFBO_ = 0;
    std::vector<GLuint> pointShadowMaps_;
    int pointShadowMapSize_ = 512;
    float pointShadowFarPlane_ = 60.0f;

    glm::mat4 lightSpaceMatrix_{1.0f};
    glm::vec3 sunLightDir_{-0.45f, -0.9f, -0.25f};
};

#endif // SUNDOWNDRIVE_RENDERING_RENDERER_H
