#include "rendering/Model.h"

// tinyobjloader implementation — compiled only once, here.
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <map>
#include <unordered_map>
#include <limits>

bool Model::load(const std::string& objPath) {
    meshes_.clear();
    subMeshes_.clear();
    materialTextures_.clear();
    materialHasTexture_.clear();
    materialAlphaCutout_.clear();
    materialDiffuse_.clear();

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    // Extract directory for material search path
    std::string dir = objPath.substr(0, objPath.find_last_of("/\\") + 1);

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objPath.c_str(), dir.c_str())) {
        std::cerr << "[Model] Failed to load: " << objPath << "\n" << err << std::endl;
        return false;
    }
    if (!warn.empty()) {
        std::cerr << "[Model] Warning: " << warn << std::endl;
    }

    boundsMin_ = glm::vec3(std::numeric_limits<float>::max());
    boundsMax_ = glm::vec3(std::numeric_limits<float>::lowest());
    for (size_t i = 0; i + 2 < attrib.vertices.size(); i += 3) {
        glm::vec3 p(attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2]);
        boundsMin_.x = std::min(boundsMin_.x, p.x);
        boundsMin_.y = std::min(boundsMin_.y, p.y);
        boundsMin_.z = std::min(boundsMin_.z, p.z);
        boundsMax_.x = std::max(boundsMax_.x, p.x);
        boundsMax_.y = std::max(boundsMax_.y, p.y);
        boundsMax_.z = std::max(boundsMax_.z, p.z);
    }

    materialTextures_.resize(materials.size());
    materialHasTexture_.assign(materials.size(), false);
    materialAlphaCutout_.assign(materials.size(), false);
    materialDiffuse_.resize(materials.size(), glm::vec3(1.0f));

    for (size_t i = 0; i < materials.size(); ++i) {
        materialDiffuse_[i] = glm::vec3(materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2]);
        const std::string& texName = materials[i].diffuse_texname;
        if (texName.empty()) {
            continue;
        }

        std::string texPath = dir + texName;
        if (materialTextures_[i].load2D(texPath)) {
            materialHasTexture_[i] = true;
            std::string lowerName = texName;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
                           [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            materialAlphaCutout_[i] = (lowerName.find(".png") != std::string::npos);
        } else {
            std::cerr << "[Model] Failed material texture: " << texPath << std::endl;
        }
    }

    for (const auto& shape : shapes) {
        std::map<int, std::vector<tinyobj::index_t>> indicesByMaterial;

        size_t indexOffset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
            int fv = shape.mesh.num_face_vertices[f];
            int matId = -1;
            if (f < shape.mesh.material_ids.size()) {
                matId = shape.mesh.material_ids[f];
            }

            auto& group = indicesByMaterial[matId];
            for (int v = 0; v < fv; ++v) {
                group.push_back(shape.mesh.indices[indexOffset + v]);
            }
            indexOffset += static_cast<size_t>(fv);
        }

        for (const auto& group : indicesByMaterial) {
            int matId = group.first;
            const auto& shapeIndices = group.second;

            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::unordered_map<std::string, unsigned int> uniqueVerts;

            for (const auto& index : shapeIndices) {
                Vertex v{};
                v.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2],
                };
                if (index.normal_index >= 0) {
                    v.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2],
                    };
                }
                if (index.texcoord_index >= 0) {
                    v.texCoords = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1],
                    };
                }

                std::string key = std::to_string(index.vertex_index) + "/"
                                + std::to_string(index.normal_index) + "/"
                                + std::to_string(index.texcoord_index);

                auto it = uniqueVerts.find(key);
                if (it == uniqueVerts.end()) {
                    unsigned int newIdx = static_cast<unsigned int>(vertices.size());
                    uniqueVerts[key] = newIdx;
                    vertices.push_back(v);
                    indices.push_back(newIdx);
                } else {
                    indices.push_back(it->second);
                }
            }

            if (!indices.empty()) {
                Mesh mesh(vertices, indices);
                meshes_.push_back(Mesh(vertices, indices));
                subMeshes_.push_back(SubMesh{std::move(mesh), matId});
            }
        }
    }

    return !subMeshes_.empty();
}

void Model::draw() const {
    for (const auto& mesh : meshes_) {
        mesh.draw();
    }
}

void Model::draw(Shader& shader) const {
    int fallbackMaterial = -1;
    for (size_t i = 0; i < materialHasTexture_.size(); ++i) {
        if (materialHasTexture_[i]) {
            fallbackMaterial = static_cast<int>(i);
            break;
        }
    }

    for (const auto& sub : subMeshes_) {
        int matId = sub.materialId;
        if (matId < 0 || matId >= static_cast<int>(materialHasTexture_.size())) {
            matId = -1;
        }

        bool useAlphaCutout = false;
        bool useTexture = false;
        glm::vec3 materialColor(1.0f);
        if (matId >= 0) {
            if (materialHasTexture_[matId]) {
                materialTextures_[matId].bind(GL_TEXTURE0);
                shader.setInt("textureSampler", 0);
                useAlphaCutout = materialAlphaCutout_[matId];
                useTexture = true;
            } else if (matId < static_cast<int>(materialDiffuse_.size())) {
                materialColor = materialDiffuse_[matId];
            }
        } else if (fallbackMaterial >= 0) {
            materialTextures_[fallbackMaterial].bind(GL_TEXTURE0);
            shader.setInt("textureSampler", 0);
            useAlphaCutout = materialAlphaCutout_[fallbackMaterial];
            useTexture = true;
        }
        shader.setBool("useTexture", useTexture);
        shader.setVec3("materialColor", materialColor);
        shader.setBool("useAlphaCutout", useAlphaCutout);
        sub.mesh.draw();
    }

    shader.setBool("useAlphaCutout", false);
    shader.setBool("useTexture", true);
}
