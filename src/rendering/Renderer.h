#ifndef SUNDOWNDRIVE_RENDERING_RENDERER_H
#define SUNDOWNDRIVE_RENDERING_RENDERER_H

#include "core/Shader.h"
#include "core/Camera.h"

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

    // Access shaders by name
    Shader& getBasicShader()   { return basicShader_; }
    Shader& getSkyboxShader()  { return skyboxShader_; }

private:
    Shader basicShader_;
    Shader skyboxShader_;
    // Future: Shader terrainShader_, shadowShader_;
};

#endif // SUNDOWNDRIVE_RENDERING_RENDERER_H
