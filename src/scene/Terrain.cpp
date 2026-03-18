#include "scene/Terrain.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

float Terrain::rawHeight(float wx, float wz) {
    // layer 1: large rolling hills
    float h  = std::sin(wx * 0.08f) * std::cos(wz * 0.06f) * 2.5f;
    // layer 2: medium bumps
    h += std::sin(wx * 0.18f + 1.3f) * std::sin(wz * 0.22f + 0.7f) * 1.2f;
    // layer 3: small detail
    h += std::sin(wx * 0.45f + 3.0f) * std::cos(wz * 0.50f + 2.0f) * 0.4f;
    // layer 4: micro detail
    h += std::sin(wx * 1.1f) * std::sin(wz * 0.9f + 1.5f) * 0.15f;
    return h;
}

// float Terrain::roadMask(float wx, float wz) {
//     // oval center-line: (x/a)^2 + (z/b)^2 = 1 with a=30, b=20
//     const float a = 30.0f;
//     const float b = 20.0f;
//     const float roadHalf  = 5.0f;  // flat zone half-width (will contain the road)
//     const float blendZone = 5.0f;  // smooth transition width

//     // Distance from the oval in "oval-normalised" space:
//     // For a point P, the closest point on the ellipse is along the radial direction.
//     // Simplified: compute radial distance from ellipse.
//     float ex = wx / a;
//     float ez = wz / b;
//     float ovalDist = std::sqrt(ex * ex + ez * ez);  // <1 inside, >1 outside

//     // Distance from the actual oval perimeter (in world units, approximate)
//     // Radial unit vector in oval space → scale back to world for distance
//     float perimeterWorld;
//     if (ovalDist < 0.001f) {
//         // At center of oval — far from the road
//         perimeterWorld = std::min(a, b);
//     } else {
//         // Direction to closest point on unit circle in oval space
//         float nx = ex / ovalDist;
//         float nz = ez / ovalDist;
//         // Closest point on ellipse in world space
//         float cx = nx * a;
//         float cz = nz * b;
//         float dx = wx - cx;
//         float dz = wz - cz;
//         perimeterWorld = std::sqrt(dx * dx + dz * dz);
//     }

//     // perimeterWorld is ~0 on the road, grows away from it
//     if (perimeterWorld < roadHalf) return 0.0f;
//     if (perimeterWorld < roadHalf + blendZone) {
//         float t = (perimeterWorld - roadHalf) / blendZone;
//         // smoothstep for nice blend
//         return t * t * (3.0f - 2.0f * t);
//     }
//     return 1.0f;
// }

glm::vec3 Terrain::computeNormal(float wx, float wz, float step) const {
    float hL = rawHeight(wx - step, wz) /* * roadMask(wx - step, wz) */;
    float hR = rawHeight(wx + step, wz) /* * roadMask(wx + step, wz) */;
    float hD = rawHeight(wx, wz - step) /* * roadMask(wx, wz - step) */;
    float hU = rawHeight(wx, wz + step) /* * roadMask(wx, wz + step) */;
    // cross product of tangent vectors gives normal
    glm::vec3 n(hL - hR, 2.0f * step, hD - hU);
    return glm::normalize(n);
}

bool Terrain::init(float halfSize, int resolution, const std::string& texturePath) {
    halfSize_   = halfSize;
    resolution_ = resolution;

    int vertsPerRow = resolution + 1;
    heights_.resize(vertsPerRow * vertsPerRow);

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    vertices.reserve(vertsPerRow * vertsPerRow);

    float step = (2.0f * halfSize) / resolution;
    float uvScale = 10.0f; // tile the texture 10x across the terrain

    // First pass: compute positions (need heights stored for normal calc)
    for (int z = 0; z <= resolution; ++z) {
        for (int x = 0; x <= resolution; ++x) {
            float wx = -halfSize + x * step;
            float wz = -halfSize + z * step;

            float height = rawHeight(wx, wz) /* * roadMask(wx, wz) */;
            heights_[z * vertsPerRow + x] = height;

            Vertex v;
            v.position  = glm::vec3(wx, height, wz);
            v.normal    = glm::vec3(0.0f, 1.0f, 0.0f); // placeholder, computed below
            v.texCoords = glm::vec2(static_cast<float>(x) / resolution * uvScale,
                                     static_cast<float>(z) / resolution * uvScale);
            vertices.push_back(v);
        }
    }

    // Second pass: compute smooth normals via central differences
    for (int z = 0; z <= resolution; ++z) {
        for (int x = 0; x <= resolution; ++x) {
            float wx = -halfSize + x * step;
            float wz = -halfSize + z * step;
            vertices[z * vertsPerRow + x].normal = computeNormal(wx, wz, step);
        }
    }

    // generate triangle indices
    indices.reserve(resolution * resolution * 6);
    for (int z = 0; z < resolution; ++z) {
        for (int x = 0; x < resolution; ++x) {
            unsigned int topLeft  = z * vertsPerRow + x;
            unsigned int topRight = topLeft + 1;
            unsigned int botLeft  = (z + 1) * vertsPerRow + x;
            unsigned int botRight = botLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(botLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(botLeft);
            indices.push_back(botRight);
        }
    }

    mesh_ = Mesh(vertices, indices);

    // load terrain texture
    if (!texturePath.empty()) {
        if (!texture_.load2D(texturePath)) {
            std::cerr << "[Terrain] Texture load failed, continuing without texture.\n";
        }
    }

    return true;
}

void Terrain::draw(Shader& shader) {
    shader.use();
    shader.setMat4("model", glm::mat4(1.0f)); // terrain sits at origin
    texture_.bind(GL_TEXTURE0);
    shader.setInt("textureSampler", 0);
    mesh_.draw();
}

float Terrain::getHeightAt(float x, float z) const {
    int vertsPerRow = resolution_ + 1;

    // map world coords to grid-local [0..resolution]
    float gx = (x + halfSize_) / (2.0f * halfSize_) * resolution_;
    float gz = (z + halfSize_) / (2.0f * halfSize_) * resolution_;

    // clamp to valid grid range
    gx = std::max(0.0f, std::min(gx, static_cast<float>(resolution_)));
    gz = std::max(0.0f, std::min(gz, static_cast<float>(resolution_)));

    int x0 = static_cast<int>(gx);
    int z0 = static_cast<int>(gz);
    int x1 = std::min(x0 + 1, resolution_);
    int z1 = std::min(z0 + 1, resolution_);

    float fx = gx - x0;
    float fz = gz - z0;

    float h00 = heights_[z0 * vertsPerRow + x0];
    float h10 = heights_[z0 * vertsPerRow + x1];
    float h01 = heights_[z1 * vertsPerRow + x0];
    float h11 = heights_[z1 * vertsPerRow + x1];

    // bilinear interpolation
    float h0 = h00 + (h10 - h00) * fx;
    float h1 = h01 + (h11 - h01) * fx;
    return h0 + (h1 - h0) * fz;
}
