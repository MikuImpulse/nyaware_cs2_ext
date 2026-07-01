#pragma once

#include "data/sdk/sdk.hpp"

class c_visuals {
public:
	void draw_speed(ImDrawList* draw, const player_t& local_player);
	void anti_flash(const player_t& local_player);
	void change_fov(const player_t& local_player);
};