#include "scene/Road.h"
#include <iostream>

bool Road::init(float roadWidth, const std::string& texturePath) {
    roadWidth_ = roadWidth;

    // TODO (P2): Generate closed circuit geometry (oval or rectangular path).
    //   1. Define control/path points around the circuit center-line
    //   2. Extrude a quad strip along the path at ±roadWidth/2
    //   3. Assign UV coordinates so the texture tiles along the road length

    if (!texturePath.empty()) {
        if (!texture_.load2D(texturePath)) {
            std::cerr << "[Road] Texture load failed." << std::endl;
        }
    }

    return true;
}

void Road::draw(Shader& shader) {
    // TODO (P2): bind texture, set model matrix, draw mesh
    (void)shader; // suppress unused warning
}
