#include "legitbot.hpp"

#include <winuser.h>
#include <random>

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
        vector3_t punch = local_player.pawn->m_pAimPunchServices()->m_aimPunchCache();
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

float c_legitbot::calc_hitchance(const player_t& local_player) {
    C_CSWeaponBase* weapon_base = local_player.weapon.base;
    CCSWeaponBaseVData* weapon_data = local_player.weapon.data;

    if (!weapon_base || !weapon_data)
        return 0.f;

    const float spread = weapon_data->m_flSpread();
    const float penalty = weapon_base->m_fAccuracyPenalty();
    const float turning = weapon_base->m_flTurningInaccuracy();

    const float max_speed = std::max<float>(weapon_data->m_flMaxSpeed(), 1.0f);

    const float speed = std::min<float>(local_player.pawn->m_vecAbsVelocity().length2d(), max_speed);
    const float movement_penalty = (speed / max_speed) * 0.09f;

    const float inaccuracy = spread + penalty + turning + movement_penalty;
    const float chance = 100.0f / (1.0f + inaccuracy * 35.0f);

    return std::clamp(chance, 0.0f, 100.0f);
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

    CModelState model_state = player.pawn->m_pGameSceneNodeParent()->m_modelState();
    C_BoneArray* bone_array = model_state.m_boneArray;
    if (!bone_array) return;

    vector3_t bone_pos = bone_array->position(bone);

    if (g.runtime.visible_check_daemon.was_init && !g.uinterface.ui.is_map_updating && 
        !g.runtime.visible_check_daemon.is_point_visible(local_player.top_position, bone_pos)) return;

    vector3_t boneScreen = view_matrix.worldToScreenPoint(g.screen, bone_pos);
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

void c_legitbot::auto_aim(const player_t& local_player, weapon_config_t* weapon_cfg) { // i dont mind writing chatgpt code with static vars here
    if (!cfg.legitbot.enable || !weapon_cfg->aimbot) return;

    fov_screen_pos = this->calc_fov(local_player, weapon_cfg);

    static vector2_t velocity{}, curve{}, target_curve{}, overshoot{}, remainder{};
    static float speed_factor = 1.0f, target_speed_factor = 1.0f;
    static float curve_timer = 0.0f, speed_timer = 0.0f;
    static bool overshoot_generated = false;
    static float previous_distance = 0.0f;

    static std::mt19937 rng(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));

    if (!weapon_cfg->triggerbot.enable && weapon_cfg->aim_bind != 0 && !GetAsyncKeyState(weapon_cfg->aim_bind)) {
        velocity = {};
        curve = {};
        target_curve = {};
        overshoot = {};
        remainder = {};

        speed_factor = 1.0f;
        target_speed_factor = 1.0f;
        curve_timer = 0.0f;
        speed_timer = 0.0f;

        overshoot_generated = false;
        previous_distance = 0.0f;

        return;
    }

    if (!this->target.isValid() || g.uinterface.ui.is_opened) {
        velocity = {};
        curve = {};
        target_curve = {};
        overshoot = {};
        remainder = {};

        speed_factor = 1.0f;
        target_speed_factor = 1.0f;
        curve_timer = 0.0f;
        speed_timer = 0.0f;

        overshoot_generated = false;
        previous_distance = 0.0f;

        return;
    }

    const float center_x = g.screen.width * 0.5f;
    const float center_y = g.screen.height * 0.5f;

    vector2_t delta = { target_bone_screen.x - center_x, target_bone_screen.y - center_y };

    float dt = std::clamp(ImGui::GetIO().DeltaTime, 0.001f, 0.05f);
    float scale = dt * 60.0f;
    float smooth = std::max<float>(weapon_cfg->smooth, 1.0f);

    if (local_player.pawn->m_iShotsFired() > 1 && weapon_cfg->rcs_strength > 0) {
        vector3_t punch = local_player.pawn->m_pAimPunchServices()->m_aimPunchCache();
        punch *= weapon_cfg->rcs_strength;

        float fov_rad = local_player.controller->m_iDesiredFOV() * (3.1415926535f / 180.0f) * 0.5f;
        float fov_scale = tanf(45.0f * 3.1415926535f / 180.0f) / tanf(fov_rad);

        delta = { target_bone_screen.x - (center_x - punch.y * fov_scale), target_bone_screen.y - (center_y + punch.x * fov_scale) };
    }

    if (!cfg.legitbot.humanize) {
        int move_x = static_cast<int>(std::round((delta.x / smooth) * scale));
        int move_y = static_cast<int>(std::round((delta.y / smooth) * scale));

        mouse_event(MOUSEEVENTF_MOVE, move_x, move_y, 0, 0);
        return;
    }

    float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    std::uniform_real_distribution<float> random01(0.0f, 1.0f);

    curve_timer -= dt;
    speed_timer -= dt;

    if (curve_timer <= 0.0f) {
        curve_timer = 0.10f + random01(rng) * 0.12f;

        float length = std::max<float>(distance, 0.001f);
        float curve_strength = std::clamp(distance * 0.010f, 0.10f, 2.25f);

        vector2_t perpendicular = { -delta.y / length, delta.x / length };

        float side = random01(rng) * 2.0f - 1.0f;
        target_curve = perpendicular * (curve_strength * side);
    }

    if (distance < 30.0f) {
        float accuracy = std::clamp(distance / 30.0f, 0.0f, 1.0f);
        target_curve *= accuracy;
    }

    float curve_lerp = std::clamp(dt * 6.0f, 0.0f, 1.0f);
    curve += (target_curve - curve) * curve_lerp;

    if (speed_timer <= 0.0f) {
        speed_timer = 0.10f + random01(rng) * 0.15f;
        target_speed_factor = 0.88f + random01(rng) * 0.24f;
    }

    float speed_lerp = std::clamp(dt * 5.0f, 0.0f, 1.0f);
    speed_factor += (target_speed_factor - speed_factor) * speed_lerp;

    bool new_movement = previous_distance < 2.0f && distance > 12.0f;

    if (new_movement) {
        velocity *= 0.20f;
        curve = {};
        target_curve = {};
        overshoot = {};
        remainder = {};
        overshoot_generated = false;
    }

    if (!overshoot_generated && distance > 45.0f) {
        if (random01(rng) < 0.18f) {
            float length = std::max<float>(distance, 0.001f);
            float overshoot_amount = 0.30f + random01(rng) * 0.90f;

            vector2_t direction = delta / length;
            overshoot = direction * overshoot_amount;
        }

        overshoot_generated = true;
    }

    if (distance < 25.0f) {
        float decay = std::clamp(dt * 10.0f, 0.0f, 1.0f);
        overshoot *= (1.0f - decay);
    }

    if (distance < 12.0f) {
        float curve_decay = std::clamp(dt * 10.0f, 0.0f, 1.0f);
        target_curve *= (1.0f - curve_decay);
        curve *= (1.0f - curve_decay * 0.5f);
    }

    vector2_t desired = delta + curve + overshoot;
    desired = (desired / smooth) * speed_factor * scale;

    float acceleration = 11.0f + random01(rng) * 3.0f;

    if (distance > 100.0f)
        acceleration += 5.0f;
    else if (distance > 50.0f)
        acceleration += 2.5f;

    float velocity_lerp = std::clamp(acceleration * dt, 0.0f, 1.0f);
    velocity += (desired - velocity) * velocity_lerp;

    float velocity_noise = 0.997f + random01(rng) * 0.006f;
    velocity *= velocity_noise;

    if (distance < 0.75f) {
        velocity *= 0.55f;
        curve *= 0.4f;
        target_curve = {};
        overshoot *= 0.3f;

        if (std::fabs(velocity.x) < 0.05f)
            velocity.x = 0.0f;

        if (std::fabs(velocity.y) < 0.05f)
            velocity.y = 0.0f;

        overshoot_generated = false;
    }

    vector2_t movement = velocity + remainder;

    int move_x = static_cast<int>(std::round(movement.x));
    int move_y = static_cast<int>(std::round(movement.y));

    remainder = { movement.x - static_cast<float>(move_x), movement.y - static_cast<float>(move_y) };

    mouse_event(MOUSEEVENTF_MOVE, move_x, move_y, 0, 0);
    previous_distance = distance;
}

#include "utils/log.hpp"

void c_legitbot::auto_fire(uintptr_t entity_list, const player_t& local_player, weapon_config_t* weapon_cfg) { // and here
    static float delay_timer = 0.f;
    static float hold_timer = 0.f;
    static float revolver_hold_timer = 0.f;
    static bool is_holding = false;
    static bool first_shot = true;
    static bool revolver_holding = false;

    CSWeaponType weapon_type = local_player.weapon.data->m_WeaponType();
    bool is_revolver = local_player.weapon.data->m_bIsRevolver();
    bool is_scoped = local_player.pawn->m_bIsScoped();
    float hit_chance = this->calc_hitchance(local_player);

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
                crosshair_target = C_CSPlayerPawn::get(entity_list, crosshair_id);
                if (crosshair_target.team != local_player.team) {
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
            static int last_tap_target_id = 0;

            int crosshair_id = local_player.pawn->m_iIDEntIndex();
            if (weapon_type == CSWeaponType::WEAPONTYPE_SNIPER_RIFLE && cfg.legitbot.scope_check && !is_scoped) return;

            if (crosshair_id > 0) {
                crosshair_target = C_CSPlayerPawn::get(entity_list, crosshair_id);

                const bool valid_target = crosshair_target.team != local_player.team && crosshair_target.pawn->m_iHealth() > 0;
                if (valid_target) {
                    if (last_tap_target_id != crosshair_id) {
                        last_tap_target_id = crosshair_id;
                        first_shot = true;
                        delay_timer = 0.f;
                    }

                    if (first_shot) {
                        delay_timer += ImGui::GetIO().DeltaTime;

                        if (delay_timer >= weapon_cfg->triggerbot.delay && hit_chance >= weapon_cfg->triggerbot.hit_chance) {
                            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

                            first_shot = false;
                            delay_timer = 0.f;
                        }
                    }
                    else if (hit_chance >= weapon_cfg->triggerbot.hit_chance) {
                        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                    }
                }
                else {
                    last_tap_target_id = 0;
                    first_shot = true;
                    delay_timer = 0.f;
                }
            }
            else {
                last_tap_target_id = 0;
                first_shot = true;
                delay_timer = 0.f;
            }
        }
        else {
            if (!weapon_cfg->aimbot) {
                int crosshair_id = local_player.pawn->m_iIDEntIndex();

                if (crosshair_id > 0) {
                    crosshair_target = C_CSPlayerPawn::get(entity_list, crosshair_id);

                    if (crosshair_target.team != local_player.team && crosshair_target.pawn->m_iHealth() > 0) {
                        if (first_shot) {
                            delay_timer += ImGui::GetIO().DeltaTime;

                            if (delay_timer >= weapon_cfg->triggerbot.delay && hit_chance >= weapon_cfg->triggerbot.hit_chance) {
                                mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                                is_holding = true;
                                hold_timer = 0.f;
                                first_shot = false;
                                delay_timer = 0.f;
                            }
                        }
                        else if (hit_chance >= weapon_cfg->triggerbot.hit_chance) {
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

                        if (delay_timer >= weapon_cfg->triggerbot.delay && hit_chance >= weapon_cfg->triggerbot.hit_chance) {
                            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);

                            is_holding = true;
                            hold_timer = 0.f;
                            first_shot = false;
                            delay_timer = 0.f;
                        }
                    }
                    else if (hit_chance >= weapon_cfg->triggerbot.hit_chance) {
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