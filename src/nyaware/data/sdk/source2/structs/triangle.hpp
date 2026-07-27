#pragma once

#include "vector3.hpp"

struct triangle_t {
    int a{}, b{}, c{};

    triangle_t() : a(0), b(0), c(0) {}
    triangle_t(int a_, int b_, int c_) : a(a_), b(b_), c(c_) {}
};

struct aaBB_t {
    vector3_t min{};
    vector3_t max{};

    inline bool ray_intersects(const vector3_t& ray_origin, const vector3_t& ray_dir) const noexcept {
        float tmin = FLT_MIN;
        float tmax = FLT_MAX;

        const float* rayOriginArr = &ray_origin.x;
        const float* rayDirArr = &ray_dir.x;
        const float* minArr = &min.x;
        const float* maxArr = &max.x;

        for (int i = 0; i < 3; ++i) {
            float invDir = 1.0f / rayDirArr[i];
            float t0 = (minArr[i] - rayOriginArr[i]) * invDir;
            float t1 = (maxArr[i] - rayOriginArr[i]) * invDir;

            if (invDir < 0.0f) std::swap(t0, t1);

            tmin = (tmin > t0) ? tmin : t0;
            tmax = (tmax < t1) ? tmax : t1;
        }

        return tmax >= tmin && tmax >= 0;
    }
};

struct triangleCombined_t {
    vector3_t v0{}, v1{}, v2{};

    triangleCombined_t() = default;
    triangleCombined_t(const vector3_t& v0_, const vector3_t& v1_, const vector3_t& v2_) : v0(v0_), v1(v1_), v2(v2_) {}

    inline aaBB_t compute_aaBB() const noexcept {
        vector3_t min_point, max_point;

        min_point.x = v0.x;
        if (v1.x < min_point.x) min_point.x = v1.x;
        if (v2.x < min_point.x) min_point.x = v2.x;

        max_point.x = v0.x;
        if (v1.x > max_point.x) max_point.x = v1.x;
        if (v2.x > max_point.x) max_point.x = v2.x;

        min_point.y = v0.y;
        if (v1.y < min_point.y) min_point.y = v1.y;
        if (v2.y < min_point.y) min_point.y = v2.y;

        max_point.y = v0.y;
        if (v1.y > max_point.y) max_point.y = v1.y;
        if (v2.y > max_point.y) max_point.y = v2.y;

        min_point.z = v0.z;
        if (v1.z < min_point.z) min_point.z = v1.z;
        if (v2.z < min_point.z) min_point.z = v2.z;

        max_point.z = v0.z;
        if (v1.z > max_point.z) max_point.z = v1.z;
        if (v2.z > max_point.z) max_point.z = v2.z;

        return { min_point, max_point };
    }
};