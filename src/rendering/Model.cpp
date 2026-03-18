#include "rendering/Model.h"

// tinyobjloader implementation — compiled only once, here.
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <iostream>
#include <unordered_map>

bool Model::load(const std::string& objPath) {
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
    if (!warn.empty()) std::cout << "[Model] Warning: " << warn << std::endl;

    for (const auto& shape : shapes) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::unordered_map<std::string, unsigned int> uniqueVerts;

        for (const auto& index : shape.mesh.indices) {
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

            // Simple dedup key
            std::string key = std::to_string(index.vertex_index) + "/"
                            + std::to_string(index.normal_index) + "/"
                            + std::to_string(index.texcoord_index);

            if (uniqueVerts.count(key) == 0) {
                uniqueVerts[key] = static_cast<unsigned int>(vertices.size());
                vertices.push_back(v);
            }
            indices.push_back(uniqueVerts[key]);
        }

        meshes_.emplace_back(vertices, indices);
    }

    return true;
}

void Model::draw() const {
    for (const auto& mesh : meshes_) {
        mesh.draw();
    }
}
