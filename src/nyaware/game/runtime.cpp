#include "runtime.hpp"

#include <format>
#include <vector>
#include <string>
#include <thread>
#include <filesystem>

#include "imgui/imgui.h"

#include "data/patterns.hpp"
#include "data/globals.hpp"

#include "utils/memory.hpp"
#include "utils/log.hpp"

uintptr_t c_runtime_manager::get_jumpButton(uintptr_t key_buttons_header_ptr) {
    static uintptr_t ret_addr = 0;
    static bool was_found = false;

    if (was_found) {
        return ret_addr;
    }

    uintptr_t current_addr = mem.read<uintptr_t>(key_buttons_header_ptr);
    while (current_addr) {
        key_button_t current = mem.read<key_button_t>(current_addr);

        std::string name = mem.read_str(current.name_ptr);
        if (name.empty()) {
            current_addr = current.next_ptr;
            continue;
        }

        if (name == "jump") {
            ret_addr = current_addr + 0x30;
            was_found = true;
            
            break;
        }

        current_addr = current.next_ptr;
    }
}

void c_runtime_manager::update_map() {
    static const auto load_maps = []() -> std::vector<std::string> {
        std::vector<std::string> result{};

        const std::string maps_path = "C:\\nyaware\\maps_cache";
        if (!std::filesystem::exists(maps_path)) return result;

        for (const auto& entry : std::filesystem::directory_iterator(maps_path)) {
            if (entry.is_regular_file() && entry.path().extension() == ".opt") {
                result.push_back(entry.path().stem().string());
            }
        }

        return result;
    };

    static std::vector<std::string> maps = load_maps();

    uintptr_t global_vars = mem.read<uintptr_t>(sig2offset.global_vars);
    if (global_vars) {
        std::string map_name = mem.read_str(mem.read<uintptr_t>(global_vars + 0x188));

        if (std::find(maps.begin(), maps.end(), map_name) != maps.end() && current_map != map_name) {
            std::thread([this, map_name]() {
                g.uinterface.ui.is_map_updating = true;

                this->current_map = map_name;
                this->visible_check_daemon = c_visible_check(std::format("C:\\nyaware\\maps_cache\\{}.opt", map_name));

                g.uinterface.ui.is_map_updating = false;
            }).detach();
        }
    }
}

void c_runtime_manager::update_ui_cfg(c_user_interface* ui) {
    switch (local_player.weapon.data->m_WeaponType()) {
        case CSWeaponType::WEAPONTYPE_PISTOL:
            ui->weapon_selected = 0;
            break;
        case CSWeaponType::WEAPONTYPE_SUBMACHINEGUN:
            ui->weapon_selected = 1;
            break;
        case CSWeaponType::WEAPONTYPE_RIFLE:
            ui->weapon_selected = 2;
            break;
        case CSWeaponType::WEAPONTYPE_SHOTGUN:
            ui->weapon_selected = 3;
            break;
        case CSWeaponType::WEAPONTYPE_SNIPER_RIFLE:
            ui->weapon_selected = 4;
            break;
        case CSWeaponType::WEAPONTYPE_MACHINEGUN:
            ui->weapon_selected = 5;
            break;
    }
}

void c_runtime_manager::update() {
    this->local_player = {};
	this->players.clear();
    this->spectators.clear();

    static bool was_updated = false;
    if (!was_updated) {
        this->sig2offset.entity_list = mem.resolve_pattern(g.modules.client.base, g.modules.client.size, signatures::dwEntityList);

        this->sig2offset.view_matrix = mem.resolve_pattern(g.modules.client.base, g.modules.client.size, signatures::dwViewMatrix);

        this->sig2offset.game_rules = mem.resolve_pattern(g.modules.client.base, g.modules.client.size, signatures::dwGameRules);
        this->sig2offset.global_vars = mem.resolve_pattern(g.modules.client.base, g.modules.client.size, signatures::dwGlobalVars);

        this->sig2offset.planted_c4 = mem.resolve_pattern(g.modules.client.base, g.modules.client.size, signatures::dwPlantedC4);

        this->sig2offset.key_buttons = mem.resolve_pattern(g.modules.client.base, g.modules.client.size, signatures::dwKeyButtons);

        was_updated = true;
    }

    this->update_map();

    uintptr_t entity_list = mem.read<uintptr_t>(sig2offset.entity_list);
    if (entity_list) {
        this->view_matrix = mem.read<matrix_t>(sig2offset.view_matrix);

        for (int i = 0; i < 64; i++) {
            CCSPlayerController* player_controller = CCSPlayerController::get(entity_list, i);
            if (!player_controller) continue;

            C_CSPlayerPawn* player_pawn = C_CSPlayerPawn::get(entity_list, player_controller->m_hPawn());
            if (!player_pawn) continue;

            player_t pl = player_t(entity_list, player_controller, player_pawn);
            if (!pl.isValid()) continue;

            if (pl.controller->m_bIsLocalPlayerController()) {
                this->local_player = pl;
            }
            else {
                //CPlayer_ObserverServices* observer = pl.pawn->m_pObserverServices();
                //LOGD("observer: %p", observer);
                //if (observer) {
                //    C_CSPlayerPawn* target_pawn = C_CSPlayerPawn::get(entity_list, observer->m_hObserverTarget());
                //    LOGD("target pawn: %p", target_pawn);
                //    if (target_pawn && target_pawn == local_player.pawn) {
                //        LOGD("player name: %s", pl.nickname.c_str());
                //        this->spectators.push_back(pl.nickname);
                //    }
                //}

                this->players.push_back(pl);
            }
        }

        ImDrawList* draw = ImGui::GetBackgroundDrawList();
        weapon_config_t* weapon_cfg{};

        if (local_player.isAlive()) {
            this->local_team = local_player.team;
            weapon_cfg = legitbot.get_weaponConfig(local_player.weapon.data->m_WeaponType());
        }

        for (auto player : players) {
            if (player.isAlive() && player.team != local_team) {
                this->esp.process_player(draw, player, local_player, view_matrix);
                if (local_player.isAlive() && weapon_cfg) this->legitbot.find_target(player, local_player, view_matrix, weapon_cfg);
            }
        }

        for (auto name : spectators) {
            LOGD("%s spectating you", name.c_str());
        }

        this->esp.process_world(draw, mem.read<C_CSGameRules*>(sig2offset.game_rules), mem.read<C_PlantedC4*>(mem.read<uintptr_t>(sig2offset.planted_c4)), local_player, view_matrix);
        
        this->visuals.draw_speed(draw, local_player);
        this->visuals.anti_flash(local_player);
        this->visuals.change_fov(local_player);

        if (local_player.isAlive()) {
            if (weapon_cfg) {
                this->legitbot.draw_fov(draw, weapon_cfg);
                this->legitbot.auto_aim(local_player, weapon_cfg);
            }

            this->legitbot.auto_fire(entity_list, local_team, local_player, weapon_cfg);
        }

        this->legitbot.target = {};
        this->legitbot.target_bone_screen = {};
        this->legitbot.metric.best = FLT_MAX;

        this->functions.bunny_hop(local_player, get_jumpButton(sig2offset.key_buttons));
    }
}