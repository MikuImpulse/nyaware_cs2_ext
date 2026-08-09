#pragma once

#include <vector>

#include "data/sdk/sdk.hpp"

#include "visuals/esp.hpp"
#include "visuals/visuals.hpp"

#include "legitbot/legitbot.hpp"

#include "functions/functions.hpp"

struct va_offsets_t {
	uintptr_t entity_list{};
	uintptr_t view_matrix{};
	uintptr_t game_rules{};
	uintptr_t global_vars{};
	uintptr_t planted_c4{};
	uintptr_t key_buttons{};
};

struct spectator_info_t {
	uint32_t ping{};
};

struct local_player_info {
	uint32_t ping{};
	vector3_t velocity{};
};

struct spec_player_info_t {
	team_t team{};
	vector3_t top_position{};
};

class c_runtime_manager {
private:
	player_t local_player{};
	std::vector<player_t> players{};
	std::vector<spectator_t> spectators{};

	c_esp esp{};
	c_visuals visuals{};

	c_legitbot legitbot{};

	c_functions functions{};

	va_offsets_t find_offsets(const dll_t& client_module);
	void find_buttons(uintptr_t key_buttons_header_ptr);
public:
	std::array<uintptr_t, (size_t)input_buttons::count> buttons_cache{};

	local_player_info local_info{};
	matrix_t view_matrix{};

	c_visible_check visible_check_daemon{};
	std::string current_map{};

	void update_map(uintptr_t global_vars);
	void update_ui_cfg(c_user_interface* ui);
	void update();
};