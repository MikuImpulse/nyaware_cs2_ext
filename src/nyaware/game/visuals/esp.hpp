#pragma once

#include <cstdint>
#include <vector>

#include "data/sdk/sdk.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

struct esp_player_t {
    ImDrawList* draw{};

    ImRect bounds{};

    ImVec2 rect_size{};
    ImRect rect_bounds{};

    void tracer() const;
    void rectangle(const vector3_t& player_pos, const vector3_t& player_top, const matrix_t& view_matrix) const;
    void health(int health, int health_max, float font_size) const;
    void nickName(const std::string& name, float font_size) const;
    void skeleton(const C_CSPlayerPawn* pawn, const vector3_t& local_position, const matrix_t& view_matrix) const;
    void weapon(const std::string& weapon_icon, const std::string& weapon_name, int ammo, int ammo_max, bool reloading, float font_size) const;
    void flags(bool is_planting, bool is_defusing, bool is_scoped, float flashbang_time, uint32_t ping, float current_time, float font_size) const;

    esp_player_t(ImDrawList* draw, ImRect bounds);
};

struct esp_world_t {
    ImDrawList* draw{};

    void bomb(C_PlantedC4* c4, C_CSGameRules* game_rules, const player_t& local_player, const matrix_t& view_matrix, float font_size) const;

    esp_world_t(ImDrawList* draw);
};

class c_esp {
public:
    static ImVec2 calc_rect(ImVec2 top, ImVec2 bottom);
    static float calc_size_by_distance(float size_min, float size_max, float distance);

    static void draw_outlined_text(ImDrawList* draw, ImFont* font, float font_size, ImVec2 position, ImColor color, const char* text);

    static inline std::vector<std::pair<bones_t, bones_t>> bone_connections = {
        {bones_t::head, bones_t::neck},

        {bones_t::neck, bones_t::left_shoulder},
        {bones_t::left_shoulder, bones_t::left_arm},
        {bones_t::left_arm, bones_t::left_hand},

        {bones_t::neck, bones_t::right_shoulder},
        {bones_t::right_shoulder, bones_t::right_arm},
        {bones_t::right_arm, bones_t::right_hand},

        {bones_t::neck, bones_t::chest},
        {bones_t::chest, bones_t::stomach},
        {bones_t::stomach, bones_t::pelvis},

        {bones_t::pelvis, bones_t::left_hip},
        {bones_t::left_hip, bones_t::left_knee},
        {bones_t::left_knee, bones_t::left_foot},

        {bones_t::pelvis, bones_t::right_hip},
        {bones_t::right_hip, bones_t::right_knee},
        {bones_t::right_knee, bones_t::right_foot},
    };

    void process_player(ImDrawList* draw, float current_time, const player_t& player, const player_t& local_player, const vector3_t& top_spec_pos, const matrix_t& view_matrix) const;
    void process_world(ImDrawList* draw, C_CSGameRules* game_rules, C_PlantedC4* bomb, const player_t& local_player, const matrix_t& view_matrix) const;
};