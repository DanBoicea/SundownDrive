#ifndef SUNDOWNDRIVE_CORE_SHADER_H
#define SUNDOWNDRIVE_CORE_SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

// ── Shader ───────────────────────────────────────────────────────────────────
// Loads, compiles, and links a vertex + fragment shader program.
// Provides helpers to set uniform values.
class Shader {
public:
    Shader() = default;
    ~Shader();

    // Load shaders from file paths, compile, and link. Returns true on success.
    bool load(const std::string& vertexPath, const std::string& fragmentPath);

    // Activate this shader program.
    void use() const;

    // Uniform setters (call use() first)
    void setBool (const std::string& name, bool  value) const;
    void setInt  (const std::string& name, int   value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3 (const std::string& name, const glm::vec3& v) const;
    void setVec4 (const std::string& name, const glm::vec4& v) const;
    void setMat4 (const std::string& name, const glm::mat4& m) const;

    GLuint getId() const { return programId_; }

private:
    GLuint programId_ = 0;

    GLuint compileShader(GLenum type, const std::string& source);
    std::string readFile(const std::string& path);
};

#endif // SUNDOWNDRIVE_CORE_SHADER_H
