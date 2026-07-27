#pragma once

#include "data/sdk/source2/structs/vector3.hpp"

#include "data/sdk/schema_dumper.hpp"
#include "utils/memory.hpp"

enum class bones_t : int {
    head = 7,
    neck = 6,

    left_shoulder = 9,
    left_arm = 10,
    left_hand = 11,

    right_shoulder = 13,
    right_arm = 14,
    right_hand = 15,

    chest = 23,
    stomach = 3,
    pelvis = 1,

    left_hip = 20,
    left_knee = 21,
    left_foot = 22,

    right_hip = 17,
    right_knee = 18,
    right_foot = 19
};

struct boneArray {
	inline vector3_t position(bones_t index) const {
		return mem.read<vector3_t>(this_cast + (int) index * 0x20);
	}
};