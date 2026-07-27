#pragma once

#include "imgui/imgui.h"

#include "data/config.hpp"
#include "data/sdk/sdk.hpp"

class c_legitbot {
private:
	bool isInFOV(const ImVec2& point, const ImVec2& center, float radius);
    ImVec2 calc_fov(const player_t& local_player, weapon_config_t* weapon_cfg);
public:
	static weapon_config_t* get_weaponConfig(CSWeaponType weapon_type);

	player_t target{};
	crosshair_target_t crosshair_target{};
	vector3_t target_bone_screen{};

	struct {
		float best = FLT_MAX;
		float temp = FLT_MAX;
	} metric;

	ImVec2 fov_screen_pos{};

    void draw_fov(ImDrawList* draw, weapon_config_t* weapon_cfg);

	float calc_hitchance(const player_t& local_player);
    void find_target(const player_t& player, const player_t& local_player, const matrix_t& view_matrix, weapon_config_t* weapon_cfg);

    void auto_aim(const player_t& local_player, weapon_config_t* weapon_cfg);
	void auto_fire(uintptr_t entity_list, uint8_t local_team, float current_time, const player_t& local_player, weapon_config_t* weapon_cfg);
};