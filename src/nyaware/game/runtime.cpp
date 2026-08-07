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

void c_runtime_manager::find_buttons(uintptr_t key_buttons_header_ptr) {
    uintptr_t current = mem.read<uintptr_t>(key_buttons_header_ptr);

    while (current) {
        key_button_t key = mem.read<key_button_t>(current);

        std::string name = mem.read_str(key.name_ptr);

        for (size_t i = 0; i < (size_t) input_buttons::count; i++) {
            if (name == button_names[i]) {
                buttons_cache[i] = current + 0x30;
                break;
            }
        }

        current = key.next_ptr;
    }
}

void c_runtime_manager::update_map(uintptr_t global_vars) {
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

    if (global_vars) {
        std::string map_name = mem.read_str(mem.read<uintptr_t>(global_vars + 0x188));

        if (current_map != map_name) {
            this->current_map = map_name;

            if (std::find(maps.begin(), maps.end(), map_name) != maps.end()) {
                std::thread([this, map_name]() {
                    g.uinterface.ui.is_map_updating = true;

                    this->visible_check_daemon = c_visible_check(std::format("C:\\nyaware\\maps_cache\\{}.opt", map_name));

                    g.uinterface.ui.is_map_updating = false;
                }).detach();
            }
            else {
                this->visible_check_daemon = {};
            }
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

    dll_t& client_dll = g.modules.client;

    static bool was_updated = false;
    if (!was_updated) {
        this->sig2offset.entity_list = mem.resolve_pattern(client_dll.base, client_dll.size, signatures::dwEntityList);

        this->sig2offset.view_matrix = mem.resolve_pattern(client_dll.base, client_dll.size, signatures::dwViewMatrix);

        this->sig2offset.game_rules = mem.resolve_pattern(client_dll.base, client_dll.size, signatures::dwGameRules);
        this->sig2offset.global_vars = mem.resolve_pattern(client_dll.base, client_dll.size, signatures::dwGlobalVars);

        this->sig2offset.planted_c4 = mem.resolve_pattern(client_dll.base, client_dll.size, signatures::dwPlantedC4);

        this->find_buttons(mem.resolve_pattern(client_dll.base, client_dll.size, signatures::dwKeyButtons));
        was_updated = true;
    }

    team_t spec_target_team = team_t::none;
    uint32_t local_spec_ping{};

    uintptr_t global_vars = mem.read<uintptr_t>(sig2offset.global_vars);
    this->update_map(global_vars);

    uintptr_t entity_list = mem.read<uintptr_t>(sig2offset.entity_list);
    if (entity_list) {
        this->view_matrix = mem.read<matrix_t>(sig2offset.view_matrix);

        for (int i = 0; i < 64; i++) {
            CCSPlayerController* player_controller = CCSPlayerController::get(entity_list, i);
            if (!player_controller) continue;

            C_CSPlayerPawn* observer_pawn = C_CSPlayerPawn::get(entity_list, player_controller->m_hObserverPawn());
            if (observer_pawn && !player_controller->m_bPawnIsAlive()) {
                CPlayer_ObserverServices* observer = observer_pawn->m_pObserverServices();

                if (observer) {
                    C_CSPlayerPawn* target_pawn = C_CSPlayerPawn::get(entity_list, observer->m_hObserverTarget());

                    if (target_pawn) {
                        if (!player_controller->m_bIsLocalPlayerController() && target_pawn == local_player.pawn) {
                            spectator_t spectator = spectator_t(entity_list, player_controller, observer_pawn);
                            if (!spectator.isValid()) continue;

                            this->spectators.push_back(spectator);
                        }

                        if (player_controller->m_bIsLocalPlayerController()) {
                            spec_target_team = target_pawn->m_iTeamNum();
                            local_spec_ping = player_controller->m_iPing();
                        }
                    }
                }
            }

            C_CSPlayerPawn* player_pawn = C_CSPlayerPawn::get(entity_list, player_controller->m_hPawn());
            if (!player_pawn) continue;

            player_t pl = player_t(entity_list, player_controller, player_pawn);
            if (!pl.isValid()) continue;

            if (pl.controller->m_bIsLocalPlayerController()) {
                this->local_player = pl;
            }
            else {
                this->players.push_back(pl);
            }
        }

        ImDrawList* draw = ImGui::GetBackgroundDrawList();
        weapon_config_t* weapon_cfg{};

        if (local_player.isAlive())
            weapon_cfg = legitbot.get_weaponConfig(local_player.weapon.data->m_WeaponType());

        this->local_ping = local_player.isAlive() ? local_player.ping : local_spec_ping;
        this->local_velocity = local_player.pawn->m_vecAbsVelocity();

        float current_time = mem.read<float>(global_vars + 0x30);

        for (auto player : players) {
            team_t enemy_team = local_player.isAlive() ? local_player.team : spec_target_team;

            if (player.isAlive() && player.team != enemy_team) {
                this->esp.process_player(draw, current_time, player, local_player, view_matrix);
                if (local_player.isAlive() && weapon_cfg) this->legitbot.find_target(player, local_player, view_matrix, weapon_cfg);
            }
        }

        this->esp.process_world(draw, mem.read<C_CSGameRules*>(sig2offset.game_rules), mem.read<C_PlantedC4*>(sig2offset.planted_c4), local_player, view_matrix);
        
        this->visuals.spectator_list(g.uinterface.ui.is_opened, spectators);
        this->visuals.anti_flash(local_player);
        this->visuals.change_fov(local_player);

        if (local_player.isAlive()) {
            if (weapon_cfg) {
                this->legitbot.draw_fov(draw, weapon_cfg);
                this->legitbot.auto_aim(local_player, weapon_cfg);
            }

            this->legitbot.auto_fire(entity_list, local_player, weapon_cfg);
        }

        this->legitbot.target = {};
        this->legitbot.target_bone_screen = {};
        this->legitbot.metric.best = FLT_MAX;

        this->functions.force_crosshair();
        this->functions.bunny_hop(local_player, buttons_cache[(size_t) input_buttons::jump]);
    }
}