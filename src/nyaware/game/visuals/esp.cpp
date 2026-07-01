#include "esp.hpp"

#include <format>
#include <algorithm>

#include "data/config.hpp"
#include "data/globals.hpp"

#include "utils/log.hpp"

ImVec2 c_esp::calc_rect(ImVec2 top, ImVec2 bottom) {
    float height = top.y - bottom.y;
    float width = height * 0.6f;

    return { width, height };
}

float c_esp::calc_size_by_distance(float size_min, float size_max, float distance) {
    float distanceSize = (size_max / distance) * (size_max / 2.0f);

    return std::clamp(distanceSize, size_min, size_max);
}

std::pair<int, int> c_esp::calc_bomb(const std::string& map) {
    static const auto fnv1a = [](const char* str, uint32_t hash = 2166136261UL) {
        auto impl = [](auto& self, const char* s, uint32_t h) -> uint32_t {
            return *s ? self(self, s + 1, (h ^ *s) * 16777619ULL) : h;
            };

        return impl(impl, str, hash);
    };

    switch (fnv1a(map.c_str())) {
    case fnv1a("de_anubis"):
        return { 450, 1575 };
    case fnv1a("de_overpass"):
        return { 650, 2275 };
    case fnv1a("de_inferno"):
        return { 600, 2100 };
    case fnv1a("de_mirage"):
        return { 650, 2275 };
    case fnv1a("de_dust2"):
        return { 700, 2450 };
    case fnv1a("de_nuke"):
        return { 650, 2275 };
    case fnv1a("de_ancient"):
        return { 650, 2275 };
    case fnv1a("de_ancient_night"):
        return { 650, 2275 };
    case fnv1a("de_train"):
        return { 500, 1750 };
    case fnv1a("de_vertigo"):
        return { 500, 1750 };
    case fnv1a("de_cache"):
        return { 600, 2100 };
    case fnv1a("de_warden"):
        return { 500, 1750 };
    case fnv1a("de_stronghold"):
        return { 650, 2275 };
    case fnv1a("cs_alpine"):
        return { 500, 1750 };
    case fnv1a("cs_office"):
        return { 500, 1750 };
    case fnv1a("cs_italy"):
        return { 500, 1750 };
    }

    return {};
}

int c_esp::calc_bomb_damage(vector3_t player_pos, vector3_t bomb_pos, int armor, const std::string& map) {
    const std::pair<int, int> bomb_calculations = c_esp::calc_bomb(map);
    const int bomb_damage = bomb_calculations.first;
    const int bomb_radius = bomb_calculations.second;

    const double c = bomb_radius / 3.0;

    auto armor_modifier = [](float damage, int armor) -> float {
        if (armor > 0) {
            const float armor_ratio = 0.5f;
            const float armor_bonus = 0.5f;
            float armor_ratio_multiply = damage * armor_ratio;
            float actual = (damage - armor_ratio_multiply) * armor_bonus;

            if (actual > static_cast<float>(armor)) {
                actual = static_cast<float>(armor) * (1.f / armor_bonus);
                armor_ratio_multiply = damage - actual;
            }

            damage = armor_ratio_multiply;
        }

        return damage;
    };

    const float damage = bomb_damage * std::exp(-std::pow(vector3_t::distance(player_pos, bomb_pos), 2) / (2 * std::pow(c, 2)));
    const float damage_armor = armor_modifier(damage, armor);

    return static_cast<int>(std::floor(damage_armor));
}

void c_esp::draw_outlined_text(ImDrawList* draw, ImFont* font, float font_size, ImVec2 position, ImColor color, const char* text) {
    if (cfg.visuals.esp.outline.draw) {
        draw->AddText(font, font_size, ImVec2(position.x + 1.f, position.y + 1.f), cfg.visuals.esp.outline.color, text);
        draw->AddText(font, font_size, ImVec2(position.x - 1.f, position.y - 1.f), cfg.visuals.esp.outline.color, text);
        draw->AddText(font, font_size, ImVec2(position.x + 1.f, position.y - 1.f), cfg.visuals.esp.outline.color, text);
        draw->AddText(font, font_size, ImVec2(position.x - 1.f, position.y + 1.f), cfg.visuals.esp.outline.color, text);
    }

    draw->AddText(font, font_size, position, color, text);
}

void c_esp::process_player(ImDrawList* draw, const player_t& player, const player_t& local_player, const matrix_t& view_matrix) const {
    if (!cfg.visuals.esp.enable || !player.isAlive()) return;

    vector3_t player_root = player.position;
    vector3_t player_top = { player.top_position.x, player.top_position.y, player.top_position.z + 10.f };

    vector3_t root_screen = view_matrix.worldToScreenPoint(g.screen, player_root);
    vector3_t top_screen = view_matrix.worldToScreenPoint(g.screen, player_top);

    esp_player_t esp_p(draw, ImRect(top_screen.x, top_screen.y, root_screen.x, root_screen.y));

    if (root_screen.z > 0.f && top_screen.z > 0.f) {
        float distance_toPlayer = vector3_t::distance(local_player.position, player_root);
        float distance_meters = distance_toPlayer * 0.01f;

        if (cfg.visuals.esp.player.tracer.draw) esp_p.tracer();
        if (cfg.visuals.esp.player.rect.draw) esp_p.rectangle(player_root, player_top, view_matrix);
        if (cfg.visuals.esp.player.health.draw) esp_p.health(player.health.value, player.health.max, 13.f);
        if (cfg.visuals.esp.player.nickName.draw) esp_p.nickName(player.nickname, 13.f);
        if (cfg.visuals.esp.player.skeleton.draw) esp_p.skeleton(player.pawn, local_player.isAlive(), local_player.top_position, view_matrix);
        if (cfg.visuals.esp.player.weapon.draw) esp_p.weapon(player.weapon.icon, player.weapon.name, player.weapon.base->m_iClip1(), player.weapon.data->m_iMaxClip1(), player.weapon.base->m_bInReload(), 13.f);
        if (cfg.visuals.esp.player.flags.draw) esp_p.flags(player.pawn->m_bIsDefusing(), player.pawn->m_bIsScoped(), 13.f);
    }
    else {
        //esp_p.out_screen(player.position, player.pawn->m_vecViewOffset(), view_matrix);
    }
}

void c_esp::process_world(ImDrawList* draw, C_CSGameRules* game_rules, C_PlantedC4* bomb, const player_t& local_player, const matrix_t& view_matrix) const {
    if (!game_rules) return;

    esp_world_t esp_w(draw);

    esp_w.bomb(bomb, game_rules, local_player, view_matrix, 13.f);
}

void esp_player_t::tracer() const {
    if (cfg.visuals.esp.outline.draw)
        draw->AddLine(ImVec2(g.screen.width * 0.5f, 0), ImVec2(this->rect_bounds.GetCenter().x, this->rect_bounds.Min.y), cfg.visuals.esp.outline.color, 1.5f * 1.8f);

    draw->AddLine(ImVec2(g.screen.width * 0.5f, 0), ImVec2(this->rect_bounds.GetCenter().x, this->rect_bounds.Min.y), cfg.visuals.esp.player.tracer.color, 1.5f);
}

void esp_player_t::rectangle(const vector3_t& player_pos, const vector3_t& player_top, const matrix_t& view_matrix) const {
    switch (cfg.visuals.esp.player.rect.mode) {
        case 0: {
            if (cfg.visuals.esp.outline.draw)
                draw->AddRect(rect_bounds.Min, rect_bounds.Max, cfg.visuals.esp.outline.color, 0, ImDrawFlags_None, 3.f);

            draw->AddRect(rect_bounds.Min, rect_bounds.Max, cfg.visuals.esp.player.rect.color, 0, ImDrawFlags_None, 1.f);

            break;
        }
        
        case 1: { // chatgpt
            float clamped_width = std::clamp((rect_bounds.Max.x - rect_bounds.Min.x) * 0.25f, 2.f, 8.f);
            float clamped_height = std::clamp((rect_bounds.Max.y - rect_bounds.Min.y) * 0.25f, 2.f, 8.f);

            if (cfg.visuals.esp.outline.draw) {
                // top left
                draw->AddLine({ rect_bounds.Min.x, rect_bounds.Min.y }, { rect_bounds.Min.x + clamped_width, rect_bounds.Min.y }, cfg.visuals.esp.outline.color, 3.f);
                draw->AddLine({ rect_bounds.Min.x, rect_bounds.Min.y }, { rect_bounds.Min.x, rect_bounds.Min.y + clamped_height }, cfg.visuals.esp.outline.color, 3.f);

                // top right
                draw->AddLine({ rect_bounds.Max.x, rect_bounds.Min.y }, { rect_bounds.Max.x - clamped_width, rect_bounds.Min.y }, cfg.visuals.esp.outline.color, 3.f);
                draw->AddLine({ rect_bounds.Max.x, rect_bounds.Min.y }, { rect_bounds.Max.x, rect_bounds.Min.y + clamped_height }, cfg.visuals.esp.outline.color, 3.f);

                // bottom left
                draw->AddLine({ rect_bounds.Min.x, rect_bounds.Max.y }, { rect_bounds.Min.x + clamped_width, rect_bounds.Max.y }, cfg.visuals.esp.outline.color, 3.f);
                draw->AddLine({ rect_bounds.Min.x, rect_bounds.Max.y }, { rect_bounds.Min.x, rect_bounds.Max.y - clamped_height }, cfg.visuals.esp.outline.color, 3.f);

                // bottom right
                draw->AddLine({ rect_bounds.Max.x, rect_bounds.Max.y }, { rect_bounds.Max.x - clamped_width, rect_bounds.Max.y }, cfg.visuals.esp.outline.color, 3.f);
                draw->AddLine({ rect_bounds.Max.x, rect_bounds.Max.y }, { rect_bounds.Max.x, rect_bounds.Max.y - clamped_height }, cfg.visuals.esp.outline.color, 3.f);
            }

            // top left
            draw->AddLine({ rect_bounds.Min.x, rect_bounds.Min.y }, { rect_bounds.Min.x + clamped_width, rect_bounds.Min.y }, cfg.visuals.esp.player.rect.color, 1.f);
            draw->AddLine({ rect_bounds.Min.x, rect_bounds.Min.y }, { rect_bounds.Min.x, rect_bounds.Min.y + clamped_height }, cfg.visuals.esp.player.rect.color, 1.f);

            // top right
            draw->AddLine({ rect_bounds.Max.x, rect_bounds.Min.y }, { rect_bounds.Max.x - clamped_width, rect_bounds.Min.y }, cfg.visuals.esp.player.rect.color, 1.f);
            draw->AddLine({ rect_bounds.Max.x, rect_bounds.Min.y }, { rect_bounds.Max.x, rect_bounds.Min.y + clamped_height }, cfg.visuals.esp.player.rect.color, 1.f);

            // bottom left
            draw->AddLine({ rect_bounds.Min.x, rect_bounds.Max.y }, { rect_bounds.Min.x + clamped_width, rect_bounds.Max.y }, cfg.visuals.esp.player.rect.color, 1.f);
            draw->AddLine({ rect_bounds.Min.x, rect_bounds.Max.y }, { rect_bounds.Min.x, rect_bounds.Max.y - clamped_height }, cfg.visuals.esp.player.rect.color, 1.f);

            // bottom right
            draw->AddLine({ rect_bounds.Max.x, rect_bounds.Max.y }, { rect_bounds.Max.x - clamped_width, rect_bounds.Max.y }, cfg.visuals.esp.player.rect.color, 1.f);
            draw->AddLine({ rect_bounds.Max.x, rect_bounds.Max.y }, { rect_bounds.Max.x, rect_bounds.Max.y - clamped_height }, cfg.visuals.esp.player.rect.color, 1.f);

            break;
        }

        case 2: { // chatgpt
            constexpr float width = 16.f;

            vector3_t box[8] = {
                { player_pos.x - width, player_pos.y - width, player_pos.z },
                { player_pos.x + width, player_pos.y - width, player_pos.z },
                { player_pos.x + width, player_pos.y + width, player_pos.z },
                { player_pos.x - width, player_pos.y + width, player_pos.z },

                { player_top.x - width, player_top.y - width, player_top.z },
                { player_top.x + width, player_top.y - width, player_top.z },
                { player_top.x + width, player_top.y + width, player_top.z },
                { player_top.x - width, player_top.y + width, player_top.z }
            };

            vector3_t screen[8];

            for (int i = 0; i < 8; i++)
                screen[i] = view_matrix.worldToScreenPoint(g.screen, box[i]);

            if (cfg.visuals.esp.outline.draw) {
                // bottom
                draw->AddLine(ImVec2(screen[0].x, screen[0].y), ImVec2(screen[1].x, screen[1].y), cfg.visuals.esp.outline.color, 3.f);
                draw->AddLine(ImVec2(screen[1].x, screen[1].y), ImVec2(screen[2].x, screen[2].y), cfg.visuals.esp.outline.color, 3.f);
                draw->AddLine(ImVec2(screen[2].x, screen[2].y), ImVec2(screen[3].x, screen[3].y), cfg.visuals.esp.outline.color, 3.f);
                draw->AddLine(ImVec2(screen[3].x, screen[3].y), ImVec2(screen[0].x, screen[0].y), cfg.visuals.esp.outline.color, 3.f);

                // top
                draw->AddLine(ImVec2(screen[4].x, screen[4].y), ImVec2(screen[5].x, screen[5].y), cfg.visuals.esp.outline.color, 3.f);
                draw->AddLine(ImVec2(screen[5].x, screen[5].y), ImVec2(screen[6].x, screen[6].y), cfg.visuals.esp.outline.color, 3.f);
                draw->AddLine(ImVec2(screen[6].x, screen[6].y), ImVec2(screen[7].x, screen[7].y), cfg.visuals.esp.outline.color, 3.f);
                draw->AddLine(ImVec2(screen[7].x, screen[7].y), ImVec2(screen[4].x, screen[4].y), cfg.visuals.esp.outline.color, 3.f);

                // verticals
                draw->AddLine(ImVec2(screen[0].x, screen[0].y), ImVec2(screen[4].x, screen[4].y), cfg.visuals.esp.outline.color, 3.f);
                draw->AddLine(ImVec2(screen[1].x, screen[1].y), ImVec2(screen[5].x, screen[5].y), cfg.visuals.esp.outline.color, 3.f);
                draw->AddLine(ImVec2(screen[2].x, screen[2].y), ImVec2(screen[6].x, screen[6].y), cfg.visuals.esp.outline.color, 3.f);
                draw->AddLine(ImVec2(screen[3].x, screen[3].y), ImVec2(screen[7].x, screen[7].y), cfg.visuals.esp.outline.color, 3.f);
            }

            // bottom
            draw->AddLine(ImVec2(screen[0].x, screen[0].y), ImVec2(screen[1].x, screen[1].y), cfg.visuals.esp.player.rect.color, 1.f);
            draw->AddLine(ImVec2(screen[1].x, screen[1].y), ImVec2(screen[2].x, screen[2].y), cfg.visuals.esp.player.rect.color, 1.f);
            draw->AddLine(ImVec2(screen[2].x, screen[2].y), ImVec2(screen[3].x, screen[3].y), cfg.visuals.esp.player.rect.color, 1.f);
            draw->AddLine(ImVec2(screen[3].x, screen[3].y), ImVec2(screen[0].x, screen[0].y), cfg.visuals.esp.player.rect.color, 1.f);

            // top
            draw->AddLine(ImVec2(screen[4].x, screen[4].y), ImVec2(screen[5].x, screen[5].y), cfg.visuals.esp.player.rect.color, 1.f);
            draw->AddLine(ImVec2(screen[5].x, screen[5].y), ImVec2(screen[6].x, screen[6].y), cfg.visuals.esp.player.rect.color, 1.f);
            draw->AddLine(ImVec2(screen[6].x, screen[6].y), ImVec2(screen[7].x, screen[7].y), cfg.visuals.esp.player.rect.color, 1.f);
            draw->AddLine(ImVec2(screen[7].x, screen[7].y), ImVec2(screen[4].x, screen[4].y), cfg.visuals.esp.player.rect.color, 1.f);

            // verticals
            draw->AddLine(ImVec2(screen[0].x, screen[0].y), ImVec2(screen[4].x, screen[4].y), cfg.visuals.esp.player.rect.color, 1.f);
            draw->AddLine(ImVec2(screen[1].x, screen[1].y), ImVec2(screen[5].x, screen[5].y), cfg.visuals.esp.player.rect.color, 1.f);
            draw->AddLine(ImVec2(screen[2].x, screen[2].y), ImVec2(screen[6].x, screen[6].y), cfg.visuals.esp.player.rect.color, 1.f);
            draw->AddLine(ImVec2(screen[3].x, screen[3].y), ImVec2(screen[7].x, screen[7].y), cfg.visuals.esp.player.rect.color, 1.f);

            break;
        }
    }
}

void esp_player_t::health(int health, int health_max, float font_size) const {
    float health_clamped = std::clamp<float>(static_cast<float>(health), 0, static_cast<float>(health_max));

    ImRect health_bounds{};
    float health_clamp_pos{};

    std::string health_str = std::to_string(health);

    ImVec2 text_size = g.fonts.jacobs->CalcTextSizeA(font_size, FLT_MAX, 0.f, health_str.c_str());
    ImVec2 text_pos{};

    switch (cfg.visuals.esp.player.rect.mode) {
        case 0: case 1:
            health_bounds = { rect_bounds.Min.x - 6.f, rect_bounds.Min.y, rect_bounds.Min.x - 4.f, rect_bounds.Max.y };
            health_clamp_pos = std::lerp(health_bounds.Max.y, health_bounds.Min.y, health_clamped / static_cast<float>(health_max));

            text_pos = { health_bounds.GetCenter().x - text_size.x * 0.5f, health_clamp_pos - text_size.y * 0.5f };

            break;
        case 2:
            health_bounds = { bounds.Min.x - 12.f, bounds.Min.y - 6.f, bounds.Min.x + 12.f, bounds.Min.y - 4.f };
            health_clamp_pos = std::lerp(health_bounds.Min.x, health_bounds.Max.x, health_clamped / static_cast<float>(health_max));

            text_pos = { health_clamp_pos - text_size.x * 0.5f, health_bounds.GetCenter().y - text_size.y * 0.5f };

            break;
    }

    if (cfg.visuals.esp.outline.draw)
        draw->AddRectFilled(ImVec2(health_bounds.Min.x - 1.f, health_bounds.Min.y - 1.f), ImVec2(health_bounds.Max.x + 1.f, health_bounds.Max.y + 1.f), cfg.visuals.esp.outline.color, 0.f);

    draw->AddRectFilled(cfg.visuals.esp.player.rect.mode == 2 ? health_bounds.Min : ImVec2(health_bounds.Min.x, health_clamp_pos), cfg.visuals.esp.player.rect.mode == 2 ? ImVec2(health_clamp_pos, health_bounds.Max.y) : health_bounds.Max, cfg.visuals.esp.player.health.color, 0.f);

    if (health < health_max)
        c_esp::draw_outlined_text(draw, g.fonts.jacobs, font_size, text_pos, cfg.visuals.esp.player.health.text_color, health_str.c_str());
}

void esp_player_t::nickName(const std::string& name, float font_size) const {
    ImVec2 text_size = g.fonts.jacobs->CalcTextSizeA(font_size, FLT_MAX, 0.f, name.c_str());
    ImVec2 text_pos{};

    switch (cfg.visuals.esp.player.rect.mode) {
        case 0: case 1:
            text_pos = { rect_bounds.GetCenter().x - text_size.x * 0.5f, rect_bounds.Min.y - text_size.y - 2.f };
            break;
        case 2:
            text_pos = { bounds.Min.x - text_size.x * 0.5f, bounds.Min.y - text_size.y - (cfg.visuals.esp.player.health.draw ? 8.f : 2.f) };
            break;
    }

    c_esp::draw_outlined_text(draw, g.fonts.jacobs, font_size, text_pos, cfg.visuals.esp.player.nickName.color, name.c_str());
}

void esp_player_t::skeleton(const C_CSPlayerPawn* pawn, bool is_local_alive, const vector3_t& local_position, const matrix_t& view_matrix) const {
    CSkeletonInstance* node_skeleton = pawn->m_pGameSceneNodeParent();

    if (node_skeleton) {
        boneArray* bone_list = node_skeleton->m_pBoneList();

        for (const auto& connection : c_esp::bone_connections) {
            vector3_t bonePos1 = bone_list->position(connection.first);
            vector3_t bonePos2 = bone_list->position(connection.second);

            vector3_t boneScreen1 = view_matrix.worldToScreenPoint(g.screen, bonePos1);
            vector3_t boneScreen2 = view_matrix.worldToScreenPoint(g.screen, bonePos2);

            ImColor boneColor{};
            if (cfg.visuals.esp.player.skeleton.visible_check && is_local_alive && !g.uinterface.ui.is_map_updating) {
                if (g.runtime.visible_check_daemon.is_point_visible(local_position, bonePos1) ||
                    g.runtime.visible_check_daemon.is_point_visible(local_position, bonePos2))

                    boneColor = cfg.visuals.esp.player.skeleton.visible_color;
                else
                    boneColor = cfg.visuals.esp.player.skeleton.invisible_color;
            }
            else {
                boneColor = cfg.visuals.esp.player.skeleton.color;
            }

            draw->AddLine(ImVec2(boneScreen1.x, boneScreen1.y), ImVec2(boneScreen2.x, boneScreen2.y), boneColor, 1.f);
        }
    }
}

void esp_player_t::weapon(const std::string& weapon_icon, const std::string& weapon_name, int ammo, int ammo_max, bool reloading, float font_size) const {
    ImVec2 icon_size = g.fonts.weapon->CalcTextSizeA(font_size, FLT_MAX, 0.f, weapon_icon.c_str());
    ImVec2 icon_pos{};

    static float spacing{}, spacing2{};

    const char* reload_str = "reloading..";
    std::string ammo_str = std::format("{}/{}", std::to_string(ammo), std::to_string(ammo_max)).c_str();

    ImVec2 text_size = g.fonts.jacobs->CalcTextSizeA(font_size, FLT_MAX, 0.f, weapon_name.c_str());
    ImVec2 text_pos{};

    ImVec2 reloading_size = g.fonts.jacobs->CalcTextSizeA(font_size, FLT_MAX, 0.f, reload_str);
    ImVec2 reloading_pos{};

    ImVec2 ammo_size = g.fonts.jacobs->CalcTextSizeA(font_size, FLT_MAX, 0.f, ammo_str.c_str());
    ImVec2 ammo_pos{};

    switch (cfg.visuals.esp.player.rect.mode) {
        case 0: case 1: {
            icon_pos = { rect_bounds.GetCenter().x - icon_size.x * 0.5f, rect_bounds.Max.y + 1.f };

            if (cfg.visuals.esp.player.weapon.modes[0])
                spacing = icon_size.y;
            else
                spacing = 0;

            text_pos = { rect_bounds.GetCenter().x - text_size.x * 0.5f, rect_bounds.Max.y + spacing + 1.f };

            if (cfg.visuals.esp.player.weapon.modes[1])
                spacing2 = text_size.y;
            else
                spacing2 = 0;

            reloading_pos = { rect_bounds.GetCenter().x - reloading_size.x * 0.5f, rect_bounds.Max.y + spacing + spacing2 + 1.f };
            ammo_pos = { rect_bounds.GetCenter().x - ammo_size.x * 0.5f, rect_bounds.Max.y + spacing + spacing2 + 1.f };

            break;
        }
        case 2: { // there was no point at all because my default rectangle stays on the bottom of the player
            icon_pos = { bounds.Max.x - icon_size.x * 0.5f, bounds.Max.y + 1.f };

            if (cfg.visuals.esp.player.weapon.modes[0])
                spacing = icon_size.y;
            else
                spacing = 0;

            text_pos = { bounds.Max.x - text_size.x * 0.5f, bounds.Max.y + spacing + 1.f };

            if (cfg.visuals.esp.player.weapon.modes[1])
                spacing2 = text_size.y;
            else
                spacing2 = 0;

            reloading_pos = { bounds.Max.x - reloading_size.x * 0.5f, bounds.Max.y + spacing + spacing2 + 1.f };
            ammo_pos = { bounds.Max.x - ammo_size.x * 0.5f, bounds.Max.y + spacing + spacing2 + 1.f };

            break;
        }
    }

    if (cfg.visuals.esp.player.weapon.modes[0])
        c_esp::draw_outlined_text(draw, g.fonts.weapon, font_size, icon_pos, cfg.visuals.esp.player.weapon.colors[0], weapon_icon.c_str());

    if (cfg.visuals.esp.player.weapon.modes[1])
        c_esp::draw_outlined_text(draw, g.fonts.jacobs, font_size, text_pos, cfg.visuals.esp.player.weapon.colors[1], weapon_name.c_str());

    if (cfg.visuals.esp.player.weapon.modes[2] && ammo >= 0) {
        if (reloading)
            c_esp::draw_outlined_text(draw, g.fonts.jacobs, font_size, reloading_pos, cfg.visuals.esp.player.weapon.colors[2], reload_str);
        else
            c_esp::draw_outlined_text(draw, g.fonts.jacobs, font_size, ammo_pos, cfg.visuals.esp.player.weapon.colors[2], ammo_str.c_str());
    }
}

void esp_player_t::flags(bool is_defusing, bool is_scoped, float font_size) const {
    const char* defusing_str = "defusing";
    const char* scoped_str = "in scope";

    ImVec2 defusing_size = g.fonts.jacobs->CalcTextSizeA(font_size, FLT_MAX, 0.f, defusing_str);
    ImVec2 defusing_pos{};

    ImVec2 scoped_size = g.fonts.jacobs->CalcTextSizeA(font_size, FLT_MAX, 0.f, scoped_str);
    ImVec2 scoped_pos{};

    switch (cfg.visuals.esp.player.rect.mode) {
        case 0: case 1:
            defusing_pos = { rect_bounds.Max.x + 3.f, rect_bounds.Min.y };
            scoped_pos = { rect_bounds.Max.x + 3.f, rect_bounds.Min.y + (cfg.visuals.esp.player.flags.modes[0] && is_defusing ? defusing_size.y : 0) };

            break;
        case 2:
            defusing_pos = { bounds.Min.x - defusing_size.x * 0.5f, bounds.Min.y - 30.f - defusing_size.y };
            scoped_pos = { bounds.Min.x - scoped_size.x * 0.5f, bounds.Min.y - 30.f - (cfg.visuals.esp.player.flags.modes[0] && is_defusing ? defusing_size.y : 0) - scoped_size.y };

            break;
    }

    if (cfg.visuals.esp.player.flags.modes[0] && is_defusing)
        c_esp::draw_outlined_text(draw, g.fonts.jacobs, font_size, defusing_pos, cfg.visuals.esp.player.flags.colors[0], defusing_str);

    if (cfg.visuals.esp.player.flags.modes[1] && is_scoped)
        c_esp::draw_outlined_text(draw, g.fonts.jacobs, font_size, scoped_pos, cfg.visuals.esp.player.flags.colors[1], scoped_str);
}

esp_player_t::esp_player_t(ImDrawList* draw, ImRect bounds) {
    this->draw = draw;

    this->bounds = bounds;
    this->rect_size = c_esp::calc_rect(bounds.Min, bounds.Max);

    this->rect_bounds = { (float)static_cast<int>(bounds.Max.x + this->rect_size.x * 0.5f), (float)static_cast<int>(bounds.Min.y), (float)static_cast<int>(bounds.Max.x - this->rect_size.x * 0.5f), (float)static_cast<int>(bounds.Min.y - this->rect_size.y) };
}

void esp_world_t::bomb(C_PlantedC4* c4, C_CSGameRules* game_rules, const player_t& local_player, const matrix_t& view_matrix, float font_size) const {
    if (!c4) return;

    static float bomb_timer = 0.f;
    static bool was_active = false;

    if (game_rules->m_bBombPlanted()) {
        CGameSceneNode* bomb_node = c4->m_pGameSceneNode();
        if (!bomb_node) return;

        if (!was_active) {
            bomb_timer = c4->m_flTimerLength();
            was_active = true;
        }

        bomb_timer -= ImGui::GetIO().DeltaTime;
        if (bomb_timer < 0.f)
            bomb_timer = 0.f;

        std::string bomb_site{};

        switch (c4->m_nBombSite()) {
            case 0:
                bomb_site = "A";
                break;
            case 1:
                bomb_site = "B";
                break;
            default:
                bomb_site = "unknown";
                break;
        }

        vector3_t bomb_pos = bomb_node->m_vecAbsOrigin();
        vector3_t bomb_scr = view_matrix.worldToScreenPoint(g.screen, bomb_pos);

        int damage = 99999;
        if (!g.runtime.current_map.empty())
            damage = c_esp::calc_bomb_damage(local_player.position, bomb_pos, local_player.armor.value, g.runtime.current_map);

        if (cfg.visuals.esp.world.bomb.draw && bomb_scr.z > 0.f) {
            ImVec2 icon_size = g.fonts.weapon->CalcTextSizeA(font_size, FLT_MAX, 0.f, "E");
            ImVec2 icon_pos = { bomb_scr.x - icon_size.x * 0.5f, bomb_scr.y - icon_size.y * 0.5f };

            std::string damage_str = std::format("damage: {}", damage == 99999 ? "invalid map!" : (local_player.isAlive() ? (local_player.health.value <= damage ? "LETHAL" : std::to_string(damage)) : "you're already dead bruh"));

            ImVec2 damage_text_size = g.fonts.jacobs->CalcTextSizeA(font_size, FLT_MAX, 0.f, damage_str.c_str());
            ImVec2 damage_text_pos = { bomb_scr.x - damage_text_size.x * 0.5f, bomb_scr.y - icon_size.y - damage_text_size.y * 0.5f };

            std::string timer_str = std::format("time left: {:.1f}s", bomb_timer);

            ImVec2 timer_text_size = g.fonts.jacobs->CalcTextSizeA(font_size, FLT_MAX, 0.f, timer_str.c_str());
            ImVec2 timer_text_pos = { bomb_scr.x - timer_text_size.x * 0.5f, bomb_scr.y - icon_size.y - damage_text_size.y - timer_text_size.y * 0.5f };

            std::string site_str = std::format("site: {}", bomb_site);

            ImVec2 site_text_size = g.fonts.jacobs->CalcTextSizeA(font_size, FLT_MAX, 0.f, site_str.c_str());
            ImVec2 site_text_pos = { bomb_scr.x - site_text_size.x * 0.5f, bomb_scr.y - icon_size.y - damage_text_size.y - timer_text_size.y - site_text_size.y * 0.5f };

            c_esp::draw_outlined_text(draw, g.fonts.weapon, font_size, icon_pos, ImColor(255, 255, 255), "E");
            c_esp::draw_outlined_text(draw, g.fonts.jacobs, font_size, damage_text_pos, ImColor(255, 255, 255), damage_str.c_str());
            c_esp::draw_outlined_text(draw, g.fonts.jacobs, font_size, timer_text_pos, ImColor(255, 255, 255), timer_str.c_str());
            c_esp::draw_outlined_text(draw, g.fonts.jacobs, font_size, site_text_pos, ImColor(255, 255, 255), site_str.c_str());
        }
    }
    else {
        was_active = false;
    }
}

esp_world_t::esp_world_t(ImDrawList* draw) {
    this->draw = draw;
}