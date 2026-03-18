#include "core/Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::~Shader() {
    if (programId_) glDeleteProgram(programId_);
}

bool Shader::load(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertSrc = readFile(vertexPath);
    std::string fragSrc = readFile(fragmentPath);
    if (vertSrc.empty() || fragSrc.empty()) return false;

    GLuint vert = compileShader(GL_VERTEX_SHADER,   vertSrc);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragSrc);
    if (!vert || !frag) return false;

    programId_ = glCreateProgram();
    glAttachShader(programId_, vert);
    glAttachShader(programId_, frag);
    glLinkProgram(programId_);

    GLint success;
    glGetProgramiv(programId_, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(programId_, 512, nullptr, log);
        std::cerr << "[Shader] Link error:\n" << log << std::endl;
        return false;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
    return true;
}

void Shader::use() const {
    glUseProgram(programId_);
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(programId_, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(programId_, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(programId_, name.c_str()), value);
}
void Shader::setVec3(const std::string& name, const glm::vec3& v) const {
    glUniform3fv(glGetUniformLocation(programId_, name.c_str()), 1, glm::value_ptr(v));
}
void Shader::setVec4(const std::string& name, const glm::vec4& v) const {
    glUniform4fv(glGetUniformLocation(programId_, name.c_str()), 1, glm::value_ptr(v));
}
void Shader::setMat4(const std::string& name, const glm::mat4& m) const {
    glUniformMatrix4fv(glGetUniformLocation(programId_, name.c_str()), 1, GL_FALSE, glm::value_ptr(m));
}

GLuint Shader::compileShader(GLenum type, const std::string& source) {
    GLuint id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    GLint success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(id, 512, nullptr, log);
        std::cerr << "[Shader] Compile error ("
                  << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT")
                  << "):\n" << log << std::endl;
        return 0;
    }
    return id;
}

std::string Shader::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[Shader] Cannot open file: " << path << std::endl;
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}
