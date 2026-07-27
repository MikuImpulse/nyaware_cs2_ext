#pragma once

#include <cstdint>

struct key_button_t {
    uint8_t pad_0[0x8];
    uintptr_t name_ptr;

    uint8_t pad_1[0x20];
    uint32_t state;

    uint8_t pad_2[0x54];
    uintptr_t next_ptr;
};

enum class input_buttons {
	attack,
	attack2,
	back,
	duck,
	forward,
	jump,
	left,
	lookatweapon,
	reload,
	right,
	showscores,
	sprint,
	turnleft,
	turnright,
	use,
	zoom,

	count
};

static inline const char* button_names[] = {
    "attack",
    "attack2",
    "back",
    "duck",
    "forward",
    "jump",
    "left",
    "lookatweapon",
    "reload",
    "right",
    "showscores",
    "sprint",
    "turnleft",
    "turnright",
    "use",
    "zoom"
};