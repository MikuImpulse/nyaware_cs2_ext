#include "physics.hpp"

#include <thread>
#include <fstream>
#include <iostream>
#include <unordered_map>

static std::vector<unsigned char> hex_str_to_bytes(const std::string& hex) {
    std::string hexCleaned;
    hexCleaned.reserve(hex.length());

    std::copy_if(hex.begin(), hex.end(), std::back_inserter(hexCleaned), [](char c) {
        return !std::isspace(c);
    });

    std::vector<unsigned char> bytes;
    bytes.reserve(hexCleaned.length() / 2);

    for (size_t i = 0; i < hexCleaned.length(); i += 2) {
        std::string byteString = hexCleaned.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(std::stoul(byteString, nullptr, 16));
        bytes.push_back(byte);
    }

    return bytes;
}

c_parser::c_parser(const std::string& path) : data_path(path) {
    std::thread trianglesThread(&c_parser::fetch_triangles, this);
    std::thread verticesThread(&c_parser::fetch_vertices, this);

    trianglesThread.join();
    verticesThread.join();

    for (size_t i = 0; i < triangles_list.size(); ++i) {
        const std::vector<triangle_t>& triangles = triangles_list[i];
        const std::vector<vector3_t>& vertices = vertices_list[i];

        std::vector<triangleCombined_t> Combined;

        for (const triangle_t& triangle : triangles) {
            triangleCombined_t t;
            t.v0 = vertices[triangle.a];
            t.v1 = vertices[triangle.b];
            t.v2 = vertices[triangle.c];
            Combined.push_back(t);
        }

        combined_list.push_back(Combined);
    }
}

template<typename T>
std::vector<T> c_parser::parse_elements(const unsigned char* data, size_t dataSize) {
    std::vector<T> elements;
    size_t elementSize = sizeof(T);
    elements.reserve(dataSize / elementSize);

    for (size_t i = 0; i < dataSize; i += elementSize) {
        T element;
        std::memcpy(&element, data + i, elementSize);
        elements.push_back(element);
    }

    return elements;
}

template<typename T>
std::vector<std::vector<T>> c_parser::parse_section(const unsigned char* fileData, size_t fileSize, const std::string& sectionName) {
    std::vector<std::vector<T>> elementsLists;
    std::istringstream fileStream(std::string(reinterpret_cast<const char*>(fileData), fileSize));
    std::string line;
    bool inMeshSection = false;

    while (std::getline(fileStream, line)) {
        if (line.find("m_meshes") != std::string::npos) {
            inMeshSection = true;
        }

        if (inMeshSection && line.find(sectionName) != std::string::npos) {
            std::getline(fileStream, line);
            if (line.find("#[") != std::string::npos) {
                std::string hexString;
                while (std::getline(fileStream, line) && line.find("]") == std::string::npos) {
                    hexString += line;
                }
                auto bytes = hex_str_to_bytes(hexString);
                auto parsedElements = parse_elements<T>(bytes.data(), bytes.size());
                elementsLists.push_back(std::move(parsedElements));
            }
        }
    }

    return elementsLists;
}

std::vector<std::vector<triangle_t>> c_parser::get_triangles() {
    std::ifstream file(data_path, std::ios::binary);
    if (!file.is_open()) {
        return {};
    }

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> fileData(fileSize);
    file.read(reinterpret_cast<char*>(fileData.data()), fileSize);

    return parse_section<triangle_t>(fileData.data(), fileSize, "m_Triangles");
}

std::vector<std::vector<vector3_t>> c_parser::get_vertices() {
    std::ifstream file(data_path, std::ios::binary);
    if (!file.is_open()) {
        return {};
    }

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> fileData(fileSize);
    file.read(reinterpret_cast<char*>(fileData.data()), fileSize);

    return parse_section<vector3_t>(fileData.data(), fileSize, "m_Vertices");
}

bool c_optimized_geometry::load_from_file(const std::string& optimized_file) {
    std::ifstream in(optimized_file, std::ios::binary);

    meshes.clear();

    size_t numMeshes;
    in.read(reinterpret_cast<char*>(&numMeshes), sizeof(size_t));

    for (size_t i = 0; i < numMeshes; ++i) {
        size_t numTris;
        in.read(reinterpret_cast<char*>(&numTris), sizeof(size_t));
        std::vector<triangleCombined_t> mesh;
        mesh.resize(numTris);
        for (size_t j = 0; j < numTris; ++j) {
            in.read(reinterpret_cast<char*>(&mesh[j].v0), sizeof(vector3_t));
            in.read(reinterpret_cast<char*>(&mesh[j].v1), sizeof(vector3_t));
            in.read(reinterpret_cast<char*>(&mesh[j].v2), sizeof(vector3_t));
        }
        meshes.push_back(mesh);
    }

    in.close();
    return true;
}

bool c_optimized_geometry::create_optimized_file(const std::string& raw_file, const std::string& optimized_file) {
    c_parser parser(raw_file);
    meshes = parser.get_combinedList();

    std::ofstream out(optimized_file, std::ios::binary);
    size_t numMeshes = meshes.size();

    out.write(reinterpret_cast<const char*>(&numMeshes), sizeof(size_t));

    for (const auto& mesh : meshes) {
        size_t numTris = mesh.size();
        out.write(reinterpret_cast<const char*>(&numTris), sizeof(size_t));
        for (const auto& tri : mesh) {
            out.write(reinterpret_cast<const char*>(&tri.v0), sizeof(vector3_t));
            out.write(reinterpret_cast<const char*>(&tri.v1), sizeof(vector3_t));
            out.write(reinterpret_cast<const char*>(&tri.v2), sizeof(vector3_t));
        }
    }

    out.close();
    return true;
}

c_visible_check::c_visible_check(const std::string& file) {
    updating = true;

    geometry.load_from_file(file);

    for (const auto& mesh : geometry.meshes) {
        bvhNodes.push_back(build_bvh(mesh));
    }

    updating = false;
}

std::unique_ptr<c_bvh_node> c_visible_check::build_bvh(const std::vector<triangleCombined_t>& tris) {
    auto node = std::make_unique<c_bvh_node>();
    if (tris.empty()) return node;

    aaBB_t bounds = tris[0].compute_aaBB();
    for (size_t i = 1; i < tris.size(); ++i) {
        aaBB_t triAABB = tris[i].compute_aaBB();
        bounds.min.x = (bounds.min.x < triAABB.min.x) ? bounds.min.x : triAABB.min.x;
        bounds.min.y = (bounds.min.y < triAABB.min.y) ? bounds.min.y : triAABB.min.y;
        bounds.min.z = (bounds.min.z < triAABB.min.z) ? bounds.min.z : triAABB.min.z;
        bounds.max.x = (bounds.max.x > triAABB.max.x) ? bounds.max.x : triAABB.max.x;
        bounds.max.y = (bounds.max.y > triAABB.max.y) ? bounds.max.y : triAABB.max.y;
        bounds.max.z = (bounds.max.z > triAABB.max.z) ? bounds.max.z : triAABB.max.z;
    }

    node->bounds = bounds;

    if (tris.size() <= 4) {
        node->triangles = tris;
        return node;
    }

    vector3_t diff = bounds.max - bounds.min;
    int axis = (diff.x > diff.y && diff.x > diff.z) ? 0 : ((diff.y > diff.z) ? 1 : 2);

    std::vector<triangleCombined_t> sortedTris = tris;

    std::sort(sortedTris.begin(), sortedTris.end(), [axis](const triangleCombined_t& a, const triangleCombined_t& b) {
        aaBB_t aabbA = a.compute_aaBB();
        aaBB_t aabbB = b.compute_aaBB();
        float centerA, centerB;
        if (axis == 0) {
            centerA = (aabbA.min.x + aabbA.max.x) / 2.0f;
            centerB = (aabbB.min.x + aabbB.max.x) / 2.0f;
        }
        else if (axis == 1) {
            centerA = (aabbA.min.y + aabbA.max.y) / 2.0f;
            centerB = (aabbB.min.y + aabbB.max.y) / 2.0f;
        }
        else {
            centerA = (aabbA.min.z + aabbA.max.z) / 2.0f;
            centerB = (aabbB.min.z + aabbB.max.z) / 2.0f;
        }
        return centerA < centerB;
    });

    size_t mid = sortedTris.size() / 2;

    std::vector<triangleCombined_t> leftTris(sortedTris.begin(), sortedTris.begin() + mid);
    std::vector<triangleCombined_t> rightTris(sortedTris.begin() + mid, sortedTris.end());

    node->left = build_bvh(leftTris);
    node->right = build_bvh(rightTris);

    return node;
}

bool c_visible_check::intersect_bvh(const c_bvh_node* node, const vector3_t& ray_origin, const vector3_t& ray_dir, float max_distance, float& hit_distance) {
    if (!node->bounds.ray_intersects(ray_origin, ray_dir)) {
        return false;
    }

    bool hit = false;
    if (node->isLeaf()) {
        for (const auto& tri : node->triangles) {
            float t;
            if (ray_intersects_triangle(ray_origin, ray_dir, tri, t)) {
                if (t < max_distance && t < hit_distance) {
                    hit_distance = t;
                    hit = true;
                }
            }
        }
    }
    else {
        if (node->left) {
            hit |= intersect_bvh(node->left.get(), ray_origin, ray_dir, max_distance, hit_distance);
        }
        if (node->right) {
            hit |= intersect_bvh(node->right.get(), ray_origin, ray_dir, max_distance, hit_distance);
        }
    }
    return hit;
}

bool c_visible_check::is_point_visible(const vector3_t& point1, const vector3_t& point2) {
    if (updating) return false;

    vector3_t rayDir = { point2.x - point1.x, point2.y - point1.y, point2.z - point1.z };
    float distance = sqrtf(rayDir.x * rayDir.x + rayDir.y * rayDir.y + rayDir.z * rayDir.z);

    rayDir = { rayDir.x / distance, rayDir.y / distance, rayDir.z / distance };
    float hitDistance = FLT_MAX;

    for (const auto& bvhRoot : bvhNodes) {
        if (intersect_bvh(bvhRoot.get(), point1, rayDir, distance, hitDistance)) {
            if (hitDistance < distance)
                return false;
        }
    }

    return true;
}

bool c_visible_check::ray_intersects_triangle(const vector3_t& rayOrigin, const vector3_t& rayDir, const triangleCombined_t& triangle, float& t) {
    const float EPSILON = 1e-7f;

    vector3_t edge1 = triangle.v1 - triangle.v0;
    vector3_t edge2 = triangle.v2 - triangle.v0;
    vector3_t h = rayDir.cross(edge2);
    float a = edge1.dot(h);

    if (a > -EPSILON && a < EPSILON)
        return false;

    float f = 1.0f / a;
    vector3_t s = rayOrigin - triangle.v0;
    float u = f * s.dot(h);

    if (u < 0.0f || u > 1.0f)
        return false;

    vector3_t q = s.cross(edge1);
    float v = f * rayDir.dot(q);

    if (v < 0.0f || u + v > 1.0f)
        return false;

    t = f * edge2.dot(q);

    return (t > EPSILON);
}