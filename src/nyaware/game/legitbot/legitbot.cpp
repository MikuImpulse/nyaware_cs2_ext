#include "legitbot.hpp"

#include <winuser.h>

#include "data/globals.hpp"

bool c_legitbot::isInFOV(const ImVec2& point, const ImVec2& center, float radius) {
    float dx = point.x - center.x;
    float dy = point.y - center.y;
    float distanceSquared = dx * dx + dy * dy;

    return distanceSquared <= (radius * radius);
}

ImVec2 c_legitbot::calc_fov(const player_t& local_player, weapon_config_t* weapon_cfg) {
    ImVec2 center = g.screen.centerToImVec2();

    if (local_player.pawn->m_iShotsFired() > 1 && weapon_cfg->rcs_strength > 0) {
        vector3_t punch = local_player.pawn->m_pAimPunchServices()->m_predictableBaseAngle();
        punch *= weapon_cfg->rcs_strength;

        float fov_rad = local_player.controller->m_iDesiredFOV() * (3.14159f / 180.0f) / 2.0f;
        float fov_scale = tanf(45.0f * 3.14159f / 180.0f) / tanf(fov_rad);

        center.x -= punch.y * fov_scale;
        center.y += punch.x * fov_scale;
    }

    return center;
}

weapon_config_t* c_legitbot::get_weaponConfig(CSWeaponType weapon_type) {
    switch (weapon_type) {
        case CSWeaponType::WEAPONTYPE_PISTOL:
            return &cfg.legitbot.configs.pistol;
        case CSWeaponType::WEAPONTYPE_SUBMACHINEGUN:
            return &cfg.legitbot.configs.submachine_gun;
        case CSWeaponType::WEAPONTYPE_RIFLE:
            return &cfg.legitbot.configs.rifle;
        case CSWeaponType::WEAPONTYPE_SHOTGUN:
            return &cfg.legitbot.configs.shotgun;
        case CSWeaponType::WEAPONTYPE_SNIPER_RIFLE:
            return &cfg.legitbot.configs.sniper_rifle;
        case CSWeaponType::WEAPONTYPE_MACHINEGUN:
            return &cfg.legitbot.configs.machine_gun;
        default:
            break;
    }

    return nullptr;
}

void c_legitbot::draw_fov(ImDrawList* draw, weapon_config_t* weapon_cfg) {
    if (!cfg.legitbot.enable || !weapon_cfg->aimbot || !weapon_cfg->fov.draw) return;

    ImColor color{};
    if (this->target.isValid())
        color = { 0, 255, 0, 200 };
    else
        color = { 255, 0, 0, 200 };

    draw->AddCircle(fov_screen_pos, weapon_cfg->fov.value, color, INT_MAX, 1.f);
}

void c_legitbot::find_target(const player_t& player, const player_t& local_player, const matrix_t& view_matrix, weapon_config_t* weapon_cfg) {
    if (!cfg.legitbot.enable || !weapon_cfg->aimbot) return;

    bones_t bone{};
    switch (weapon_cfg->bone) {
        case 0:
            bone = bones_t::head;
            break;
        case 1:
            bone = bones_t::neck;
            break;
        case 2:
            bone = bones_t::chest;
            break;
    }

    vector3_t bonePos = player.pawn->m_pGameSceneNodeParent()->m_pBoneList()->position(bone);
    if (!g.uinterface.ui.is_map_updating && !g.runtime.visible_check_daemon.is_point_visible(local_player.top_position, bonePos)) return;

    vector3_t boneScreen = view_matrix.worldToScreenPoint(g.screen, bonePos);
    if (boneScreen.z < 0.f) return;

    if (isInFOV(ImVec2(boneScreen.x, boneScreen.y), fov_screen_pos, weapon_cfg->fov.value)) {
        metric.temp = vector2_t::distance({ boneScreen.x, boneScreen.y }, g.screen.centerToVec2());

        if (metric.temp < metric.best) {
            this->metric.best = metric.temp;
            this->target = player;
            this->target_bone_screen = boneScreen;
        }
    }
}

void c_legitbot::auto_aim(const player_t& local_player, weapon_config_t* weapon_cfg) {
    if (!cfg.legitbot.enable || !weapon_cfg->aimbot) return;

    fov_screen_pos = this->calc_fov(local_player, weapon_cfg);
    if (weapon_cfg->lmb_check && !GetAsyncKeyState(VK_LBUTTON)) return;

    if (this->target.isValid() && !g.uinterface.ui.is_opened) {
        vector2_t delta = { target_bone_screen.x - g.screen.width * 0.5f, target_bone_screen.y - g.screen.height * 0.5f };

        float scale = ImGui::GetIO().DeltaTime * 60.0f;
        float smooth = weapon_cfg->smooth;

        if (local_player.pawn->m_iShotsFired() > 1 && weapon_cfg->rcs_strength > 0) {
            vector3_t punch = local_player.pawn->m_pAimPunchServices()->m_predictableBaseAngle();
            punch *= weapon_cfg->rcs_strength;

            float fov_rad = local_player.controller->m_iDesiredFOV() * (3.14159f / 180.0f) / 2.0f;
            float fov_scale = tanf(45.0f * 3.14159f / 180.0f) / tanf(fov_rad);

            delta = { target_bone_screen.x - (g.screen.width * 0.5f - punch.y * fov_scale), target_bone_screen.y - (g.screen.height * 0.5f + punch.x * fov_scale) };
        }

        int move_x = (int)((delta.x / smooth) * scale);
        int move_y = (int)((delta.y / smooth) * scale);

        mouse_event(MOUSEEVENTF_MOVE, move_x, move_y, 0, 0);
    }
}

#include "utils/log.hpp"

void c_legitbot::auto_fire(uintptr_t entity_list, uint8_t local_team, const player_t& local_player, weapon_config_t* weapon_cfg) {
    static float delay_timer = 0.f;
    static float hold_timer = 0.f;
    static float revolver_hold_timer = 0.f;
    static bool is_holding = false;
    static bool first_shot = true;
    static bool revolver_holding = false;

    CSWeaponType weapon_type = local_player.weapon.data->m_WeaponType();
    bool is_revolver = local_player.weapon.data->m_bIsRevolver();
    bool is_scoped = local_player.pawn->m_bIsScoped();

    if (!is_revolver && revolver_holding) {
        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        revolver_holding = false;
        revolver_hold_timer = 0.f;

        return;
    }

    if (weapon_cfg) {
        if (!cfg.legitbot.enable || !weapon_cfg->triggerbot.enable) return;

        bool is_tap_gun = (weapon_type == CSWeaponType::WEAPONTYPE_PISTOL ||
            weapon_type == CSWeaponType::WEAPONTYPE_SHOTGUN ||
            weapon_type == CSWeaponType::WEAPONTYPE_SNIPER_RIFLE) && !is_revolver;

        if (is_revolver && cfg.legitbot.early_shot) {
            if (g.uinterface.ui.is_opened) {
                if (revolver_holding) {
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                    revolver_holding = false;
                    revolver_hold_timer = 0.f;
                }
                return;
            }

            revolver_hold_timer += ImGui::GetIO().DeltaTime;

            if (revolver_hold_timer >= 0.21f) {
                if (!revolver_holding) {
                    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                    revolver_holding = true;
                }
            }

            if (revolver_hold_timer >= 0.4f) {
                if (revolver_holding) {
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                    revolver_holding = false;
                }
                revolver_hold_timer = 0.f;
            }

            int crosshair_id = local_player.pawn->m_iIDEntIndex();
            if (crosshair_id > 0) {
                crosshair_target = C_CSPlayerPawn::get_fromCrosshairID(entity_list, crosshair_id);
                if (crosshair_target.team != local_team) {
                    if (!revolver_holding) {
                        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                        revolver_holding = true;
                    }
                    revolver_hold_timer = 0.f;
                }
            }

            return;
        }

        if (is_tap_gun) {
            int crosshair_id = local_player.pawn->m_iIDEntIndex();
            if (weapon_type == CSWeaponType::WEAPONTYPE_SNIPER_RIFLE && cfg.legitbot.scope_check && !is_scoped) return;

            if (crosshair_id > 0) {
                crosshair_target = C_CSPlayerPawn::get_fromCrosshairID(entity_list, crosshair_id);
                if (crosshair_target.team != local_team && crosshair_target.pawn->m_iHealth() > 0) {
                    if (first_shot) {
                        delay_timer += ImGui::GetIO().DeltaTime;
                        if (delay_timer >= weapon_cfg->triggerbot.delay) {
                            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                            first_shot = false;
                            delay_timer = 0.f;
                        }
                    }
                    else {
                        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                    }
                }
            }
            else {
                first_shot = true;
                delay_timer = 0.f;
            }
        }
        else {
            if (!weapon_cfg->aimbot) {
                int crosshair_id = local_player.pawn->m_iIDEntIndex();
                if (crosshair_id > 0) {
                    crosshair_target = C_CSPlayerPawn::get_fromCrosshairID(entity_list, crosshair_id);
                    if (crosshair_target.team != local_team && crosshair_target.pawn->m_iHealth() > 0) {
                        if (first_shot) {
                            delay_timer += ImGui::GetIO().DeltaTime;
                            if (delay_timer >= weapon_cfg->triggerbot.delay) {
                                mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                                is_holding = true;
                                hold_timer = 0.f;
                                first_shot = false;
                                delay_timer = 0.f;
                            }
                        }
                        else {
                            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                            is_holding = true;
                            hold_timer = 0.f;
                        }
                    }
                }
                else {
                    first_shot = true;
                    delay_timer = 0.f;
                }
            }
            else if (weapon_cfg->aimbot) {
                static bool was_trigger = false;
                if (this->target.isValid()) {
                    if (first_shot) {
                        delay_timer += ImGui::GetIO().DeltaTime;
                        if (delay_timer >= weapon_cfg->triggerbot.delay) {
                            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                            is_holding = true;
                            hold_timer = 0.f;
                            first_shot = false;
                            delay_timer = 0.f;
                        }
                    }
                    else {
                        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                        is_holding = true;
                        hold_timer = 0.f;
                    }
                    was_trigger = true;
                }
                else {
                    if (was_trigger) {
                        if (is_holding) {
                            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                            is_holding = false;
                        }
                        was_trigger = false;
                        first_shot = true;
                        delay_timer = 0.f;
                    }
                }
            }

            if (is_holding) {
                hold_timer += ImGui::GetIO().DeltaTime;
                if (hold_timer >= 0.25f) {
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                    is_holding = false;
                }
            }
        }
    }
}