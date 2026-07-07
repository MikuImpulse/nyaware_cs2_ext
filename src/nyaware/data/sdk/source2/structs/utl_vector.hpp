#pragma once

#include <cstdint>

#include "vector3.hpp"
#include "utils/memory.hpp"

struct c_utl_vector {
    int32_t m_Size{};
    uintptr_t m_pMemory{};
    int32_t m_nAllocationCount{};
    int32_t m_nGrowSize{};

    vector3_t to_vector3() {
        if (m_Size > 0) {
            return mem.read<vector3_t>(m_pMemory + (m_Size - 1) * sizeof(vector3_t));
        }

        return {};
    }
};