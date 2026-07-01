#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "imgui/imgui.h"
#include "modules/modules.hpp"

struct weapon_config_t;

class c_user_interface {
private:
    ImVec2 window_size = { 700, 405 };
    short tab = 1;

    c_ui_modules ui_widgets{};
    ImFont* nyaware_logo_font{};

    weapon_config_t* get_weaponConfig(int index);
    void render_weapon_config(weapon_config_t* config, const char* weapon_name);
    void render_weapon_trigger_config(weapon_config_t* config, const char* weapon_name);
public:
    int weapon_selected = 0;

    bool is_map_updating = false;
    bool is_opened = true;

    void render();
    void render_text();

    void style();

    void init();
    void update();
};