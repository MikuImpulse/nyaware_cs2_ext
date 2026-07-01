#pragma once

#include <vector>

#include "data/sdk/sdk.hpp"

#include "visuals/esp.hpp"
#include "visuals/visuals.hpp"

#include "legitbot/legitbot.hpp"

#include "functions/functions.hpp"

class c_runtime_manager {
private:
	struct {
		uintptr_t entity_list{};

		uintptr_t view_matrix{};

		uintptr_t game_rules{};
		uintptr_t global_vars{};

		uintptr_t planted_c4{};

		uintptr_t key_buttons{};
	} sig2offset;

	player_t local_player{};
	std::vector<player_t> players{};
	std::vector<std::string> spectators{};

	uint8_t local_team{};

	c_esp esp{};
	c_visuals visuals{};

	c_legitbot legitbot{};

	c_functions functions{};
public:
	matrix_t view_matrix{};

	c_visible_check visible_check_daemon{};
	std::string current_map{};

	uintptr_t get_jumpButton(uintptr_t key_buttons_header_ptr);

	void update_map();
	void update_ui_cfg(c_user_interface* ui);
	void update();
};