#pragma once

#include <vector>

#include "imgui/imgui.h"

struct weapon_config_t {
    bool aimbot = false;
    bool lmb_check = false;
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
		bool panic = false;
	} client;

    struct {
        struct {
            bool enable = false;

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
                    ImColor color = { 135, 255, 135, 255 };
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

                    std::vector<bool> modes = { true, true };
                    std::vector<ImColor> colors = { {255, 255, 255, 255}, {255, 255, 255, 255} };
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

        struct {
            bool enable = false;
            int value = 90;
        } fov;

        bool anti_flash = false;

        struct {
            bool draw = false;
            int mode = 0;
        } speedometer;
    } visuals;

    struct {
        bool enable = false;
        bool scope_check = false;
        bool early_shot = false;

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
        bool bunny_hop = false;
    } misc;
} inline cfg;

class c_config_manager {
public:

};