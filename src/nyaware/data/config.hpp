#pragma once

#include <vector>
#include <string>
#include <windows.h>

#include "imgui/imgui.h"

#include "json.hpp"
using json = nlohmann::json;

struct weapon_config_t {
    bool aimbot = false;
    int aim_bind = 0;
    float smooth = 1.5f;
    int rcs_strength = 14;

    struct {
        bool draw = false;
        float value = 40.f;
    } fov;

    struct {
        bool enable = false;
        float delay = 0.f;
    } triggerbot;

    int bone = 0;
};

struct config {
    struct {
        ImColor accent_color = { 185, 145, 255 };
        int menu_keybind = VK_INSERT;
        
        struct {
            bool draw = false;
            int position_num = 0;

            std::vector<bool> elements = { true, true, true, true, true };
        } watermark;
    } ui;

    struct {
        int font_num = 0;

        struct {
            struct {
                struct {
                    bool draw = false;
                    ImColor color = { 255, 255, 255, 255 };
                } tracer;

                struct {
                    bool draw = false;
                    int mode = 0;
                    ImColor color = { 255, 255, 255, 255 };
                } rect;

                struct {
                    bool draw = false;
                    ImColor bar_color = { 135, 255, 135, 255 };
                    ImColor text_color = { 255, 255, 255, 255 };
                } health;

                struct {
                    bool draw = false;
                    ImColor color = { 255, 255, 255, 255 };
                } nickName;

                struct {
                    bool draw = false;
                    bool visible_check = false;

                    ImColor color = { 255, 255, 255, 255 };

                    ImColor visible_color = { 0, 255, 0, 255 };
                    ImColor invisible_color = { 255, 0, 0, 255 };
                } skeleton;

                struct {
                    bool draw = false;

                    std::vector<bool> modes = { true, true, true };
                    std::vector<ImColor> colors = { {255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255} };
                } weapon;

                struct {
                    bool draw = false;

                    std::vector<bool> modes = { true, true, true, true, true };
                    std::vector<ImColor> colors = { {255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255} };
                } flags;
            } player;

            struct {
                struct {
                    bool draw = false;
                } bomb;
            } world;

            struct {
                bool draw = true;
                ImColor color = { 0, 0, 0, 200 };
            } outline;
        } esp;

        bool anti_flash = false;
        bool force_crosshair = false;

        struct {
            bool enable = false;
            int value = 90;
        } fov;
    } visuals;

    struct {
        bool enable = false;
        bool early_shot = false;
        bool scope_check = false;

        struct {
            weapon_config_t pistol{};
            weapon_config_t submachine_gun{};
            weapon_config_t rifle{};
            weapon_config_t shotgun{};
            weapon_config_t sniper_rifle{};
            weapon_config_t machine_gun{};
        } configs;
    } legitbot;

    struct {
        struct {
            bool enable = false;
            int bind = VK_SPACE;
        } bunny_hop;
    } misc;
} inline cfg;

class c_config_manager {
private:
    static inline const std::string m_config_path = "C:\\nyaware\\configs";

    json save_weapon_cfg(const weapon_config_t& weapon_cfg);
    weapon_config_t load_weapon_cfg(const json& object);

    json save_color(const ImColor& color);
    ImColor load_color(const json& array);

    template <typename T>
    inline json save_array(const std::vector<T>& array) {
        json new_array = json::array();

        if constexpr (std::is_same_v<T, ImColor>) {
            for (const ImColor& color : array) {
                new_array.push_back(save_color(color));
            }
        }
        else {
            for (const T& item : array) {
                new_array.push_back(item);
            }
        }

        return new_array;
    }

    template <typename T>
    inline std::vector<T> load_array(const json& array) {
        std::vector<T> new_array{};

        if (array.is_array()) {
            new_array.reserve(array.size());

            if constexpr (std::is_same_v<T, ImColor>) {
                for (const auto& item : array) {
                    new_array.push_back(load_color(item));
                }
            }
            else {
                for (const auto& item : array) {
                    new_array.push_back(item.get<T>());
                }
            }
        }
        
        return new_array;
    }
public:
    std::vector<std::string> get_configList();

    bool save(const std::string& cfg_name);
    bool load(const std::string& cfg_name);

    bool create(const std::string& cfg_name);
    bool remove(const std::string& cfg_name);
};