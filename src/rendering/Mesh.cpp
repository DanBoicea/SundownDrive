#include "rendering/Mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    setup(vertices, indices);
}

Mesh::~Mesh() { cleanup(); }

Mesh::Mesh(Mesh&& other) noexcept
    : VAO_(other.VAO_), VBO_(other.VBO_), EBO_(other.EBO_), indexCount_(other.indexCount_) {
    other.VAO_ = other.VBO_ = other.EBO_ = 0;
    other.indexCount_ = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        cleanup();
        VAO_ = other.VAO_; VBO_ = other.VBO_; EBO_ = other.EBO_;
        indexCount_ = other.indexCount_;
        other.VAO_ = other.VBO_ = other.EBO_ = 0;
        other.indexCount_ = 0;
    }
    return *this;
}

void Mesh::draw() const {
    if (!VAO_) return;
    glBindVertexArray(VAO_);
    glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::setup(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    indexCount_ = static_cast<unsigned int>(indices.size());

    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &EBO_);

    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position — layout(location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

    // Normal — layout(location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // TexCoords — layout(location = 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    glBindVertexArray(0);
}

void Mesh::cleanup() {
    if (EBO_) glDeleteBuffers(1, &EBO_);
    if (VBO_) glDeleteBuffers(1, &VBO_);
    if (VAO_) glDeleteVertexArrays(1, &VAO_);
    VAO_ = VBO_ = EBO_ = 0;
}
