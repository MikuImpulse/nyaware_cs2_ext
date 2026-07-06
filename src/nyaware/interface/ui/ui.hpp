#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "imgui/imgui.h"

#include "modules/modules.hpp"
#include "modules/color.hpp"

#include "data/config.hpp"

struct weapon_config_t;

class c_user_interface {
private:
    ImVec2 window_size = { 800, 500 };
    short tab = 1;

    struct menu_particle_t {
        ImVec2 pos{};
        ImVec2 velocity{};
        float radius{};
        float alpha{};
    };

    c_ui_modules ui_widgets{};
    c_color_helper color_helper{};
    c_config_manager cfg_manager{};

    ImFont* nyaware_logo_font{};
    ImFont* menu_ico_font{};

    weapon_config_t* get_weaponConfig(int index);
    void render_weapon_config(weapon_config_t* config, const char* weapon_name);
    void render_weapon_trigger_config(weapon_config_t* config, const char* weapon_name);
public:
    int weapon_selected = 0;

    bool is_map_updating = false;
    bool is_opened = true;

    void watermark();
    void render();
    void render_text();

    void style();
    void colors();
    void fonts();

    void init();
    void update();
};