#ifndef SUNDOWNDRIVE_RENDERING_TEXTURE_H
#define SUNDOWNDRIVE_RENDERING_TEXTURE_H

#include <GL/glew.h>
#include <string>
#include <vector>

// Wraps a GL texture object.  Supports 2D textures and cubemaps.
class Texture {
public:
    Texture() = default;
    ~Texture();

    bool load2D(const std::string& path, bool flipY = true);

    bool loadCubemap(const std::vector<std::string>& facePaths);

    void bind(GLenum unit = GL_TEXTURE0) const;
    GLuint getId() const { return id_; }

private:
    GLuint id_   = 0;
    GLenum type_ = GL_TEXTURE_2D;   // or GL_TEXTURE_CUBE_MAP
};

#endif
