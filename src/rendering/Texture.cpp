#include "rendering/Texture.h"

// stb_image implementation — compiled only once, here.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

Texture::~Texture() {
    if (id_) glDeleteTextures(1, &id_);
}

bool Texture::load2D(const std::string& path, bool flipY) {
    type_ = GL_TEXTURE_2D;
    stbi_set_flip_vertically_on_load(flipY);

    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "[Texture] Failed to load: " << path << std::endl;
        return false;
    }

    GLenum format = GL_RGB;
    if (channels == 1) format = GL_RED;
    else if (channels == 4) format = GL_RGBA;

    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return true;
}

bool Texture::loadCubemap(const std::vector<std::string>& facePaths) {
    type_ = GL_TEXTURE_CUBE_MAP;
    stbi_set_flip_vertically_on_load(false);  // cubemaps should NOT be flipped

    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id_);

    for (unsigned int i = 0; i < facePaths.size(); ++i) {
        int width, height, channels;
        unsigned char* data = stbi_load(facePaths[i].c_str(), &width, &height, &channels, 0);
        if (!data) {
            std::cerr << "[Texture] Cubemap face failed: " << facePaths[i] << std::endl;
            return false;
        }
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format,
                     width, height, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    // No mipmaps for cubemaps: mip generation is per-face and creates seams at boundaries.
    // Skyboxes are always at a fixed apparent scale so mipmaps provide no benefit.
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return true;
}

void Texture::bind(GLenum unit) const {
    glActiveTexture(unit);
    glBindTexture(type_, id_);
}
