#ifndef SUNDOWNDRIVE_RENDERING_MESH_H
#define SUNDOWNDRIVE_RENDERING_MESH_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

// ── Vertex layout used throughout the engine ─────────────────────────────────
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

// ── Mesh ─────────────────────────────────────────────────────────────────────
// Wraps a VAO/VBO/EBO.  Constructed from a vector of Vertices + indices.
// Call draw() to issue a single glDrawElements.
class Mesh {
public:
    Mesh() = default;
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    // Move-only (GPU resources)
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void draw() const;
    unsigned int indexCount() const { return indexCount_; }

private:
    GLuint VAO_ = 0, VBO_ = 0, EBO_ = 0;
    unsigned int indexCount_ = 0;

    void setup(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void cleanup();
};

#endif // SUNDOWNDRIVE_RENDERING_MESH_H
