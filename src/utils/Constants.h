#ifndef SUNDOWNDRIVE_UTILS_CONSTANTS_H
#define SUNDOWNDRIVE_UTILS_CONSTANTS_H

namespace Constants {

    // window defaults
    constexpr int   DefaultWidth  = 1280;
    constexpr int   DefaultHeight = 720;

    // world dimensions
    constexpr float WorldHalfSize = 100.0f;
    constexpr float WorldHeight   = 100.0f;

    // camera defaults
    constexpr float CameraNear    = 0.1f;
    constexpr float CameraFar     = 200.0f;
    constexpr float CameraFov     = 45.0f;

    // frame timing target
    constexpr int   TargetFPS     = 60;
    constexpr float FixedDeltaTime = 1.0f / TargetFPS;

}

#endif
