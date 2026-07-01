#pragma once

#include <vector>

#include "data/sdk/source2/include.hpp"

class c_parser {
private:
    std::string data_path{};

    std::vector<std::vector<triangle_t>> triangles_list{};
    std::vector<std::vector<vector3_t>> vertices_list{};
    std::vector<std::vector<triangleCombined_t>> combined_list{};

    std::vector<std::vector<triangle_t>> get_triangles();
    std::vector<std::vector<vector3_t>> get_vertices();

    template<typename T>
    std::vector<T> parse_elements(const unsigned char* data, size_t dataSize);

    template<typename T>
    std::vector<std::vector<T>> parse_section(const unsigned char* fileData, size_t fileSize, const std::string& sectionName);

    inline void fetch_triangles() {
        triangles_list = get_triangles();
    }

    inline void fetch_vertices() {
        vertices_list = get_vertices();
    }

public:
    c_parser(const std::string& path);

    const std::vector<std::vector<triangleCombined_t>>& get_combinedList() const {
        return combined_list;
    }
};

class c_optimized_geometry {
public:
    std::vector<std::vector<triangleCombined_t>> meshes;

    bool load_from_file(const std::string& optimized_file);
    bool create_optimized_file(const std::string& raw_file, const std::string& optimized_file);
};

struct c_bvh_node {
    aaBB_t bounds{};

    std::unique_ptr<c_bvh_node> left{};
    std::unique_ptr<c_bvh_node> right{};

    std::vector<triangleCombined_t> triangles{};

    bool isLeaf() const {
        return left == nullptr && right == nullptr;
    }
};

class c_visible_check {
public:
    static inline bool updating = false;

    c_visible_check() = default;
    c_visible_check(const std::string& file);

    bool is_point_visible(const vector3_t& point1, const vector3_t& point2);
    bool ray_intersects_triangle(const vector3_t& ray_origin, const vector3_t& ray_dir, const triangleCombined_t& triangle, float& t);
private:
    c_optimized_geometry geometry{};
    std::vector<std::unique_ptr<c_bvh_node>> bvhNodes{};

    std::unique_ptr<c_bvh_node> build_bvh(const std::vector<triangleCombined_t>& tris);
    bool intersect_bvh(const c_bvh_node* node, const vector3_t& ray_origin, const vector3_t& ray_dir, float max_distance, float& hit_distance);
};