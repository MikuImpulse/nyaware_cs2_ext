#pragma once

#include <vector>

#include "data/sdk/sdk.hpp"

class c_visuals {
public:
	void spectator_list(bool is_ui_opened, const std::vector<spectator_t>& spectators);

	void anti_flash(const player_t& local_player);
	void change_fov(const player_t& local_player);
};