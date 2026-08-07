#include "ui.hpp"

#include <format>
#include <iterator>

#include "interface/fonts/include.hpp"
#include "data/globals.hpp"

#include <psapi.h>

weapon_config_t* c_user_interface::get_weaponConfig(int index) {
    switch (index) {
        case 0:
            return &cfg.legitbot.configs.pistol;
        case 1:
            return &cfg.legitbot.configs.submachine_gun;
        case 2:
            return &cfg.legitbot.configs.rifle;
        case 3:
            return &cfg.legitbot.configs.shotgun;
        case 4:
            return &cfg.legitbot.configs.sniper_rifle;
        case 5:
            return &cfg.legitbot.configs.machine_gun;
        default:
            return nullptr;
    }
}

void c_user_interface::render_weapon_config(weapon_config_t* config, const char* weapon_name) {
    ui_widgets.checkboxKeyBind("Aimbot", &config->aimbot, &config->aim_bind);
    ImGui::Separator();
    ui_widgets.checkbox("Humanization (shared)", &cfg.legitbot.humanize);
    ImGui::Separator();
    ui_widgets.checkbox("Draw FOV", &config->fov.draw);
    ImGui::Separator();
    ui_widgets.sliderFloat("FOV value", &config->fov.value, 0.1f, 360.f, "%.1f");
    ImGui::Separator();
    ui_widgets.sliderFloat("Smooth", &config->smooth, 1.f, 5.f, "%.1f");
    ImGui::Separator();
    ui_widgets.sliderInt("RCS power", &config->rcs_strength, 5, 25);
    ImGui::Separator();
    ui_widgets.combo("Bone", &config->bone, { "Head", "Neck", "Chest" });
}

void c_user_interface::render_weapon_trigger_config(weapon_config_t* config, const char* weapon_name) {
    ui_widgets.checkbox("Triggerbot", &config->triggerbot.enable);

    if (!strcmp(weapon_name, "sniper_rifle")) {
        ImGui::Separator();
        ui_widgets.checkbox("Scope check", &cfg.legitbot.scope_check);
    }

    if (!strcmp(weapon_name, "pistol")) {
        ImGui::Separator();
        ui_widgets.checkbox("Early shot (revolver)", &cfg.legitbot.early_shot);
    }

    ImGui::Separator();
    ui_widgets.sliderInt("Hit chance", &config->triggerbot.hit_chance, 0, 100);
    ImGui::Separator();
    ui_widgets.sliderFloat("Delay (sec)##trigger", &config->triggerbot.delay, 0.f, 0.5f, "%.2f");
}

void c_user_interface::watermark() {
    if (!cfg.ui.watermark.draw) return;

    ImVec2 water_pos{};

    switch (cfg.ui.watermark.position_num) {
        case 0:
            water_pos = { g.screen.width * 0.5f, 25 };
            break;
        case 1:
            water_pos = { g.screen.width * 0.5f, g.screen.height - 25.f };
            break;
    }

    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();

    auto get_memory_mb = []() -> size_t {
        HANDLE hProcess = GetCurrentProcess();
        PROCESS_MEMORY_COUNTERS pmc;

        if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
            return pmc.WorkingSetSize / (1024 * 1024);
        }
        return 0;
    };

    SYSTEMTIME lt{};
    GetLocalTime(&lt);

    static float timer = 0.f;
    static int fps = 0;

    timer += io.DeltaTime;

    if (timer >= 1.0f) {
        fps = (int)io.Framerate;
        timer = 0.f;
    }

    float velocity = g.runtime.local_velocity.length2d();

    std::string watermark_elements = std::format("{}{}{}{}{}{}{}", NYAWARE_LOGO, (std::find(cfg.ui.watermark.elements.begin(), cfg.ui.watermark.elements.end(), true) != cfg.ui.watermark.elements.end() ? " " : ""),
        cfg.ui.watermark.elements[0] ? std::format("  {} {}:{:02d}:{:02d}", ICON_CLOCK, lt.wHour, lt.wMinute, lt.wSecond) : "",
        cfg.ui.watermark.elements[1] ? std::format("  {} {} FPS", ICON_WINDOW, fps) : "",
        cfg.ui.watermark.elements[2] ? std::format("  {} {} MB", ICON_RAM, get_memory_mb()) : "",
        cfg.ui.watermark.elements[3] ? std::format("  {} {} u/s", ICON_METER, static_cast<int>(velocity)) : "",
        cfg.ui.watermark.elements[4] ? std::format("  {} {} ms", ICON_GLOBE, g.runtime.local_ping) : "");

    ImVec2 elements_size = ImGui::CalcTextSize(watermark_elements.c_str());
    ImVec2 elements_pos = ImVec2(water_pos.x - elements_size.x * 0.5f, water_pos.y - elements_size.y * 0.5f);

    ImVec4 window_color = style.Colors[ImGuiCol_WindowBg];
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.f, 12.f));

    draw->AddRectFilled(ImVec2(water_pos.x - style.WindowPadding.x - elements_size.x * 0.5f, water_pos.y - style.WindowPadding.y - elements_size.y * 0.5f),
        ImVec2(water_pos.x + style.WindowPadding.x + elements_size.x * 0.5f, water_pos.y + style.WindowPadding.y + elements_size.y * 0.5f),
        ImGui::ColorConvertFloat4ToU32(ImVec4(window_color.x, window_color.y, window_color.z, 0.7f)), style.WindowRounding);

    draw->AddText(nyaware_logo_font, 13.f, elements_pos, ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Text]), "A");
    draw->AddText(elements_pos, ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Text]), watermark_elements.c_str());

    ImGui::PopStyleVar();
}

void c_user_interface::render() {
    static float menu_alpha = 0.f;
    static bool panic_triggered = false;
    static float panic_close_timer = 0.f;

    float target_alpha = this->is_opened ? 1.f : 0.f;

    if (panic_triggered)
        target_alpha = 0.f;

    float delta_time = ImGui::GetIO().DeltaTime;
    delta_time = ImClamp(delta_time, 0.f, 1.f / 60.f);

    menu_alpha += (target_alpha - menu_alpha) * ImClamp(delta_time * 12.f, 0.f, 1.f);

    if (panic_triggered) {
        if (menu_alpha <= 0.01f) {
            menu_alpha = 0.f;
            panic_triggered = false;
            cfg = {};
            g.panic = true;
            return;
        }
    }

    if (!this->is_opened && menu_alpha <= 0.01f && !panic_triggered) {
        menu_alpha = 0.f;
        return;
    }

    ImGui::SetNextWindowSize(window_size, ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(g.screen.width * 0.5f - window_size.x * 0.5f, g.screen.height * 0.5f - window_size.y * 0.5f), ImGuiCond_Once);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, menu_alpha);

    ImGui::Begin("##nyaware", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground);

    ImGuiStyle& style = ImGui::GetStyle();
    ImDrawList* draw = ImGui::GetWindowDrawList();

    ImVec4 accent = cfg.ui.accent_color.Value;

    auto hash_float = [](float seed) {
        float value = sinf(seed * 12.9898f) * 43758.5453f;
        return value - floorf(value);
    };

    auto point_in_rounded_rect = [](const ImVec2& point, const ImVec2& min, const ImVec2& max, float rounding) {
        if (point.x < min.x || point.y < min.y || point.x > max.x || point.y > max.y)
            return false;

        ImVec2 corner_center = point;

        if (point.x < min.x + rounding && point.y < min.y + rounding)
            corner_center = ImVec2(min.x + rounding, min.y + rounding);
        else if (point.x > max.x - rounding && point.y < min.y + rounding)
            corner_center = ImVec2(max.x - rounding, min.y + rounding);
        else if (point.x < min.x + rounding && point.y > max.y - rounding)
            corner_center = ImVec2(min.x + rounding, max.y - rounding);
        else if (point.x > max.x - rounding && point.y > max.y - rounding)
            corner_center = ImVec2(max.x - rounding, max.y - rounding);
        else
            return true;

        float dx = point.x - corner_center.x;
        float dy = point.y - corner_center.y;

        return dx * dx + dy * dy <= rounding * rounding;
    };

    ImVec2 main_window_pos = ImGui::GetWindowPos();
    ImVec2 main_window_size = ImGui::GetWindowSize();
    ImVec2 main_window_min = main_window_pos;
    ImVec2 main_window_max = ImVec2(main_window_pos.x + main_window_size.x, main_window_pos.y + main_window_size.y);

    float window_rounding = 8.f;

    ImVec4 menu_bg = ImVec4(0.02f, 0.02f, 0.028f, 1.f);

    draw->AddRectFilled(main_window_min, main_window_max, ImGui::GetColorU32(ImVec4(menu_bg.x, menu_bg.y, menu_bg.z, menu_bg.w * menu_alpha)), window_rounding);

    static menu_particle_t particles[112];
    static bool particles_initialized = false;

    if (!particles_initialized) {
        for (int i = 0; i < IM_ARRAYSIZE(particles); ++i) {
            float seed = static_cast<float>(i + 1);

            particles[i].pos = ImVec2(hash_float(seed * 15.31f) * window_size.x, hash_float(seed * 42.77f) * window_size.y);

            float angle = hash_float(seed * 91.13f) * 6.28318530718f;
            float speed = 16.f + hash_float(seed * 33.49f) * 26.f;

            particles[i].velocity = ImVec2(cosf(angle) * speed, sinf(angle) * speed);
            particles[i].radius = 1.05f + hash_float(seed * 57.21f) * 2.15f;
            particles[i].alpha = 0.11f + hash_float(seed * 12.93f) * 0.26f;
        }

        particles_initialized = true;
    }

    draw->PushClipRect(main_window_min, main_window_max, true);

    for (int i = 0; i < IM_ARRAYSIZE(particles); ++i) {
        menu_particle_t& particle = particles[i];

        particle.pos.x += particle.velocity.x * delta_time;
        particle.pos.y += particle.velocity.y * delta_time;

        if (particle.pos.x < 0.f)
            particle.pos.x += main_window_size.x;

        if (particle.pos.x > main_window_size.x)
            particle.pos.x -= main_window_size.x;

        if (particle.pos.y < 0.f)
            particle.pos.y += main_window_size.y;

        if (particle.pos.y > main_window_size.y)
            particle.pos.y -= main_window_size.y;

        ImVec2 particle_screen_pos = ImVec2(main_window_pos.x + particle.pos.x, main_window_pos.y + particle.pos.y);
        if (!point_in_rounded_rect(particle_screen_pos, main_window_min, main_window_max, window_rounding))
            continue;

        float pulse = 0.75f + 0.25f * sinf(static_cast<float>(ImGui::GetTime()) * 1.4f + i * 0.61f);
        float particle_alpha = particle.alpha * pulse * menu_alpha;

        draw->AddCircleFilled(particle_screen_pos, particle.radius, ImGui::GetColorU32(ImVec4(accent.x, accent.y, accent.z, particle_alpha)), 12);

        for (int j = i + 1; j < IM_ARRAYSIZE(particles); ++j) {
            ImVec2 other_particle_screen_pos = ImVec2(main_window_pos.x + particles[j].pos.x, main_window_pos.y + particles[j].pos.y);
            if (!point_in_rounded_rect(other_particle_screen_pos, main_window_min, main_window_max, window_rounding))
                continue;

            ImVec2 distance = ImVec2(other_particle_screen_pos.x - particle_screen_pos.x, other_particle_screen_pos.y - particle_screen_pos.y);
            float distance_sq = distance.x * distance.x + distance.y * distance.y;
            float max_distance = 64.f;

            if (distance_sq < max_distance * max_distance) {
                float distance_alpha = 1.f - distance_sq / (max_distance * max_distance);

                draw->AddLine(particle_screen_pos, other_particle_screen_pos,
                    ImGui::GetColorU32(ImVec4(accent.x, accent.y, accent.z, distance_alpha * 0.026f * menu_alpha)), 1.f);
            }
        }
    }

    draw->PopClipRect();

    float child_tabs_size_width = window_size.x * 0.09f;

    ImGui::SetCursorPos(ImVec2(0.f, 0.f));

    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.f);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_ChildBg]);

    ImGui::BeginChild(1337, ImVec2(child_tabs_size_width + style.WindowPadding.x, ImGui::GetContentRegionAvail().y + style.WindowPadding.y), ImGuiChildFlags_None);

    ImVec2 window_pos = ImGui::GetWindowPos();
    ImVec2 cursor_childtabs = ImGui::GetCursorPos();
    ImVec2 cursor_screen_childtabs = ImGui::GetCursorScreenPos();

    ImVec2 logo_size = nyaware_logo_font->CalcTextSizeA(25.f, FLT_MAX, 0.f, "A");
    ImVec2 logo_pos = ImVec2(window_pos.x + cursor_childtabs.x * 0.5f + logo_size.x * 0.5f, child_tabs_size_width * 0.05f + cursor_screen_childtabs.y + logo_size.y * 0.5f);

    ImU32 logo_glow_far = ImGui::GetColorU32(ImVec4(accent.x, accent.y, accent.z, 0.16f * menu_alpha));
    ImU32 logo_glow_mid = ImGui::GetColorU32(ImVec4(accent.x, accent.y, accent.z, 0.30f * menu_alpha));
    ImU32 logo_outline = ImGui::GetColorU32(ImVec4(accent.x, accent.y, accent.z, 0.95f * menu_alpha));
    ImU32 logo_main = ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, menu_alpha));

    draw->AddText(nyaware_logo_font, 25.f, ImVec2(logo_pos.x - 4.f, logo_pos.y), logo_glow_far, "A");
    draw->AddText(nyaware_logo_font, 25.f, ImVec2(logo_pos.x + 4.f, logo_pos.y), logo_glow_far, "A");
    draw->AddText(nyaware_logo_font, 25.f, ImVec2(logo_pos.x, logo_pos.y - 4.f), logo_glow_far, "A");
    draw->AddText(nyaware_logo_font, 25.f, ImVec2(logo_pos.x, logo_pos.y + 4.f), logo_glow_far, "A");

    draw->AddText(nyaware_logo_font, 25.f, ImVec2(logo_pos.x - 3.f, logo_pos.y - 3.f), logo_glow_far, "A");
    draw->AddText(nyaware_logo_font, 25.f, ImVec2(logo_pos.x + 3.f, logo_pos.y - 3.f), logo_glow_far, "A");
    draw->AddText(nyaware_logo_font, 25.f, ImVec2(logo_pos.x - 3.f, logo_pos.y + 3.f), logo_glow_far, "A");
    draw->AddText(nyaware_logo_font, 25.f, ImVec2(logo_pos.x + 3.f, logo_pos.y + 3.f), logo_glow_far, "A");

    draw->AddText(nyaware_logo_font, 25.f, ImVec2(logo_pos.x - 2.f, logo_pos.y), logo_glow_mid, "A");
    draw->AddText(nyaware_logo_font, 25.f, ImVec2(logo_pos.x + 2.f, logo_pos.y), logo_glow_mid, "A");
    draw->AddText(nyaware_logo_font, 25.f, ImVec2(logo_pos.x, logo_pos.y - 2.f), logo_glow_mid, "A");
    draw->AddText(nyaware_logo_font, 25.f, ImVec2(logo_pos.x, logo_pos.y + 2.f), logo_glow_mid, "A");

    draw->AddText(nyaware_logo_font, 25.f, ImVec2(logo_pos.x - 1.f, logo_pos.y), logo_outline, "A");
    draw->AddText(nyaware_logo_font, 25.f, ImVec2(logo_pos.x + 1.f, logo_pos.y), logo_outline, "A");
    draw->AddText(nyaware_logo_font, 25.f, ImVec2(logo_pos.x, logo_pos.y - 1.f), logo_outline, "A");
    draw->AddText(nyaware_logo_font, 25.f, ImVec2(logo_pos.x, logo_pos.y + 1.f), logo_outline, "A");

    draw->AddText(nyaware_logo_font, 25.f, logo_pos, logo_main, "A");

    float button_child_width = child_tabs_size_width * 0.4f;
    float tab_pill_size = button_child_width * 1.45f;
    float tab_pill_rounding = tab_pill_size * 0.5f;

    static float animated_pill_local_y = 0.f;
    static bool animated_pill_initialized = false;

    float tab_button_spacing = 16.f;
    ImGui::SetCursorPosY(ImGui::GetContentRegionAvail().y * 0.5f - (button_child_width * 4 + tab_button_spacing * 3) * 0.5f);

    auto anim_float = [](ImGuiID id, float target, float speed = 14.f) {
        ImGuiStorage* storage = ImGui::GetStateStorage();

        float current = storage->GetFloat(id, target);
        current += (target - current) * ImClamp(ImGui::GetIO().DeltaTime * speed, 0.f, 1.f);

        storage->SetFloat(id, current);
        return current;
    };

    auto lerp_color = [](const ImVec4& a, const ImVec4& b, float t) {
        return ImVec4(ImLerp(a.x, b.x, t), ImLerp(a.y, b.y, t), ImLerp(a.z, b.z, t), ImLerp(a.w, b.w, t));
    };

    ImGui::PushFont(menu_ico_font);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x, tab_button_spacing));

    ImVec2 tab_positions[4];
    bool tab_hovered[4] = { false, false, false, false };

    auto tab_button = [&](const char* icon, int tab_id) {
        ImGui::SetCursorPosX(child_tabs_size_width * 0.5f - button_child_width * 0.5f + style.WindowPadding.x * 0.5f);

        ImVec2 button_local_pos = ImGui::GetCursorPos();
        ImVec2 button_screen_pos = ImGui::GetCursorScreenPos();
        ImVec2 button_size = ImVec2(button_child_width, button_child_width);

        tab_positions[tab_id - 1] = ImVec2(button_screen_pos.x + button_size.x * 0.5f, button_local_pos.y + button_size.y * 0.5f);

        ImGui::InvisibleButton(icon, button_size);

        tab_hovered[tab_id - 1] = ImGui::IsItemHovered();

        if (ImGui::IsItemClicked())
            tab = tab_id;
    };

    tab_button("A", 1);
    tab_button("B", 2);
    tab_button("C", 3);
    tab_button("D", 4);

    int active_tab_index = tab - 1;

    if (active_tab_index < 0)
        active_tab_index = 0;

    if (active_tab_index > 3)
        active_tab_index = 3;

    float target_pill_local_y = tab_positions[active_tab_index].y;

    if (!animated_pill_initialized) {
        animated_pill_local_y = target_pill_local_y;
        animated_pill_initialized = true;
    }

    float animation_speed = 14.f;
    animated_pill_local_y += (target_pill_local_y - animated_pill_local_y) * ImClamp(delta_time * animation_speed, 0.f, 1.f);

    ImVec2 child_screen_pos = ImGui::GetWindowPos();

    ImVec2 pill_center = ImVec2(tab_positions[active_tab_index].x, child_screen_pos.y + animated_pill_local_y);
    ImVec2 pill_min = ImVec2(pill_center.x - tab_pill_size * 0.5f, pill_center.y - tab_pill_size * 0.5f);
    ImVec2 pill_max = ImVec2(pill_center.x + tab_pill_size * 0.5f, pill_center.y + tab_pill_size * 0.5f);

    draw->AddRectFilled(pill_min, pill_max, ImGui::GetColorU32(ImVec4(accent.x, accent.y, accent.z, 0.42f * menu_alpha)), tab_pill_rounding);
    draw->AddRectFilled(ImVec2(pill_min.x + tab_pill_size * 0.18f, pill_min.y + tab_pill_size * 0.18f),
        ImVec2(pill_max.x - tab_pill_size * 0.18f, pill_max.y - tab_pill_size * 0.18f),
        ImGui::GetColorU32(ImVec4(accent.x, accent.y, accent.z, 0.16f * menu_alpha)), tab_pill_rounding);

    const char* tab_icons[4] = { "A", "B", "C", "D" };

    ImVec4 tab_text_base = ImVec4(0.49f, 0.49f, 0.53f, 1.f);
    ImVec4 tab_text_hovered = lerp_color(tab_text_base, accent, 0.3f);
    ImVec4 tab_text_active = accent;

    for (int i = 0; i < 4; ++i) {
        ImGuiID tab_anim_id = ImGui::GetID(tab_icons[i]);

        float active_anim = anim_float(tab_anim_id + 100, active_tab_index == i ? 1.f : 0.f, 16.f);
        float hover_anim = anim_float(tab_anim_id + 200, tab_hovered[i] ? 1.f : 0.f, 12.f);

        ImVec4 text_col = lerp_color(tab_text_base, tab_text_hovered, hover_anim);
        text_col = lerp_color(text_col, tab_text_active, active_anim);
        text_col.w *= menu_alpha;

        ImVec2 text_size = menu_ico_font->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.f, tab_icons[i]);
        ImVec2 text_pos = ImVec2(tab_positions[i].x - text_size.x * 0.5f, child_screen_pos.y + tab_positions[i].y - text_size.y * 0.5f);

        float hover_circle_anim = hover_anim * (1.f - active_anim);

        if (hover_circle_anim > 0.01f) {
            ImVec2 icon_center = ImVec2(tab_positions[i].x, child_screen_pos.y + tab_positions[i].y);

            draw->AddCircleFilled(icon_center, 17.f + hover_circle_anim * 5.f,
                ImGui::GetColorU32(ImVec4(accent.x, accent.y, accent.z, 0.12f * hover_circle_anim * menu_alpha)), 32);
        }

        draw->AddText(menu_ico_font, ImGui::GetFontSize(), text_pos, ImGui::GetColorU32(text_col), tab_icons[i]);
    }

    ImGui::PopStyleVar();
    ImGui::PopFont();

    ImGui::Dummy(ImVec2(0.0f, 0.0f));
    ImGui::EndChild();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    static int content_tab = 1;
    static float content_alpha = 1.f;
    static float content_slide = 0.f;
    static bool content_transitioning = false;

    if (content_tab != tab && !content_transitioning) {
        content_transitioning = true;
    }

    if (content_transitioning) {
        content_alpha += (0.f - content_alpha) * ImClamp(delta_time * 18.f, 0.f, 1.f);
        content_slide += (15.f - content_slide) * ImClamp(delta_time * 18.f, 0.f, 1.f);

        if (content_alpha <= 0.05f) {
            content_tab = tab;
            content_alpha = 0.f;
            content_slide = -15.f;

            if (content_tab == tab) {
                content_transitioning = false;
            }
        }
    }

    if (!content_transitioning) {
        content_alpha += (1.f - content_alpha) * ImClamp(delta_time * 16.f, 0.f, 1.f);
        content_slide += (0.f - content_slide) * ImClamp(delta_time * 16.f, 0.f, 1.f);
    }

    if (!content_transitioning && fabsf(content_slide) < 0.5f) {
        content_slide = 0.f;
    }

    float content_offset_x = content_slide;

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, content_alpha * menu_alpha);

    if (content_tab == 1) {
        ImGui::SetCursorPos(ImVec2(child_tabs_size_width + style.WindowPadding.x * 2 + content_offset_x, style.WindowPadding.y));

        float p_esp_child_width = ImGui::GetContentRegionAvail().x * 0.5f;
        ui_widgets.beginChild("Player ESP", ImVec2(p_esp_child_width, 0.f), ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ui_widgets.checkboxColored("Tracer", &cfg.visuals.esp.player.tracer.draw, { &cfg.visuals.esp.player.tracer.color });
        ImGui::Separator();
        ui_widgets.checkboxColored("Rectangle", &cfg.visuals.esp.player.rect.draw, { &cfg.visuals.esp.player.rect.color });
        ImGui::Separator();
        ui_widgets.checkboxColored("Health bar", &cfg.visuals.esp.player.health.draw, { &cfg.visuals.esp.player.health.bar_color, &cfg.visuals.esp.player.health.text_color });
        ImGui::Separator();
        ui_widgets.checkboxColored("Name tags", &cfg.visuals.esp.player.nickName.draw, { &cfg.visuals.esp.player.nickName.color });
        ImGui::Separator();
        ui_widgets.checkboxColored("Skeleton", &cfg.visuals.esp.player.skeleton.draw, { &cfg.visuals.esp.player.skeleton.color });
        ImGui::Separator();
        ui_widgets.checkboxColored("Weapon", &cfg.visuals.esp.player.weapon.draw, { &cfg.visuals.esp.player.weapon.colors[0], &cfg.visuals.esp.player.weapon.colors[1], &cfg.visuals.esp.player.weapon.colors[2] });
        ImGui::Separator();
        ui_widgets.checkboxColored("Flags", &cfg.visuals.esp.player.flags.draw, { &cfg.visuals.esp.player.flags.colors[0], &cfg.visuals.esp.player.flags.colors[1], &cfg.visuals.esp.player.flags.colors[2], &cfg.visuals.esp.player.flags.colors[3], &cfg.visuals.esp.player.flags.colors[4] });

        ui_widgets.endChild();

        ImGui::SetCursorPosX(child_tabs_size_width + style.WindowPadding.x * 2 + content_offset_x);
        ui_widgets.beginChild("World ESP", ImVec2(p_esp_child_width, 0.f), ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ui_widgets.checkbox("Bomb", &cfg.visuals.esp.world.bomb.draw);

        ui_widgets.endChild();

        ImGui::SetCursorPos(ImVec2(child_tabs_size_width + style.WindowPadding.x * 2 + p_esp_child_width + style.ItemSpacing.x + content_offset_x, style.WindowPadding.y));
        ui_widgets.beginChild("ESP Appearance", ImVec2(ImGui::GetContentRegionAvail().x, 0.f), ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ui_widgets.checkboxColored("Visible check per bone", &cfg.visuals.esp.player.skeleton.visible_check, { &cfg.visuals.esp.player.skeleton.visible_color, &cfg.visuals.esp.player.skeleton.invisible_color });
        ImGui::Separator();
        if (ui_widgets.combo("Text font", &cfg.visuals.font_num, { "Jacobs", "Pixel", "Gotham", "Montserrat" }))
            this->fonts();
        ImGui::Separator();
        ui_widgets.combo("Rect mode", &cfg.visuals.esp.player.rect.mode, { "Bounding box", "Corners", "3D box" });
        ImGui::Separator();
        ui_widgets.multiCombo("Weapon elements", { "Icon", "Name", "Ammo" }, cfg.visuals.esp.player.weapon.modes);
        ImGui::Separator();
        ui_widgets.multiCombo("Player flags", { "Plant", "Defuse", "Scope", "Blind", "Ping"}, cfg.visuals.esp.player.flags.modes);

        ui_widgets.endChild();

        ImGui::SetCursorPosX(child_tabs_size_width + style.WindowPadding.x * 2 + p_esp_child_width + style.ItemSpacing.x + content_offset_x);
        ui_widgets.beginChild("Visuals", ImVec2(ImGui::GetContentRegionAvail().x, 0.f), ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ui_widgets.checkbox("Spectator list", &cfg.visuals.spectator_list.enable);
        ImGui::Separator();
        ui_widgets.checkbox("Anti flash", &cfg.visuals.anti_flash);
        ImGui::Separator();
        ui_widgets.checkbox("Force crosshair", &cfg.visuals.force_crosshair);
        ImGui::Separator();
        ui_widgets.checkbox("Camera FOV", &cfg.visuals.fov.enable);
        ImGui::Separator();
        ui_widgets.sliderInt("FOV value", &cfg.visuals.fov.value, 30, 120);

        ui_widgets.endChild();
    }
    else if (content_tab == 2) {
        weapon_config_t* current_weapon = get_weaponConfig(weapon_selected);
        static const char* weapons[] = { "pistol", "submachine_gun", "rifle", "shotgun", "sniper_rifle", "machine_gun" };

        ImGui::SetCursorPos(ImVec2(child_tabs_size_width + style.WindowPadding.x * 2 + content_offset_x, style.WindowPadding.y));
        ImGui::BeginChild("##weapon_types", ImVec2(ImGui::GetContentRegionAvail().x, 40), ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ImGui::PushFont(g.fonts.weapon);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));

        ImGui::SetCursorPosY(0.f);

        float button_width = (ImGui::GetContentRegionAvail().x - style.ItemSpacing.x * 5) / 6;
        static const char* icons[] = { "`", "w", "A", "~", "C", "y" };

        for (int i = 0; i < 6; i++) {
            if (i > 0) ImGui::SameLine();

            ImVec2 text_size = ImGui::CalcTextSize(icons[i]);

            ImVec2 button_size(button_width, 40);
            ImVec2 text_pos = ImGui::GetCursorScreenPos();
            text_pos.x += (button_size.x - text_size.x) * 0.5f;
            text_pos.y += (button_size.y - text_size.y) * 0.5f - 2;

            int pushed_count = 0;
            if (weapon_selected == i) {
                ImGui::PushStyleColor(ImGuiCol_Text, color_helper.make_darkened(cfg.ui.accent_color, 0.9f).Value);
                pushed_count = 1;
            }

            ImGui::PushFont(g.fonts.weapon);

            ImGui::PushID(i);
            if (ImGui::Button("##weapon", button_size)) {
                weapon_selected = i;
            }
            ImGui::PopID();

            ImGui::GetWindowDrawList()->AddText(text_pos, ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)), icons[i]);

            ImGui::PopFont();

            if (pushed_count > 0) {
                ImGui::PopStyleColor(pushed_count);
            }
        }

        ImGui::PopStyleColor(3);
        ImGui::PopFont();

        ImGui::EndChild();

        ImGui::SetCursorPosX(child_tabs_size_width + style.WindowPadding.x * 2 + content_offset_x);

        float p_lbot_child_width = ImGui::GetContentRegionAvail().x * 0.5f;
        float p_lbot_cursor_pos_y = ImGui::GetCursorPosY();
        ui_widgets.beginChild("##lbot_switchh", ImVec2(p_lbot_child_width, 0.f), ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ui_widgets.checkbox("Master switch##legitbot", &cfg.legitbot.enable);

        ui_widgets.endChild();

        ImGui::SetCursorPosX(child_tabs_size_width + style.WindowPadding.x * 2 + content_offset_x);
        ui_widgets.beginChild("##weapon_cfg", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0.f), ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoScrollbar);

        if (current_weapon) {
            this->render_weapon_config(current_weapon, weapons[weapon_selected]);
        }

        ui_widgets.endChild();

        ImGui::SetCursorPos(ImVec2(child_tabs_size_width + style.WindowPadding.x * 2 + p_lbot_child_width + style.ItemSpacing.x + content_offset_x, style.WindowPadding.y + 40 + style.ItemSpacing.y));
        ui_widgets.beginChild("##trigger_cfg", ImVec2(ImGui::GetContentRegionAvail().x, 0.f), ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoScrollbar);

        if (current_weapon) {
            render_weapon_trigger_config(current_weapon, weapons[weapon_selected]);
        }

        ui_widgets.endChild();
    }
    else if (content_tab == 3) {
        ImGui::SetCursorPos(ImVec2(child_tabs_size_width + style.WindowPadding.x * 2 + content_offset_x, style.WindowPadding.y));

        float p_misc_child_width = ImGui::GetContentRegionAvail().x * 0.5f;
        ui_widgets.beginChild("Miscellaneous", ImVec2(p_misc_child_width, 0.f), ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ui_widgets.checkboxKeyBind("Bunny hop", &cfg.misc.bunny_hop.enable, &cfg.misc.bunny_hop.bind);

        ui_widgets.endChild();
    }
    else if (content_tab == 4) {
        ImGui::SetCursorPos(ImVec2(child_tabs_size_width + style.WindowPadding.x * 2 + content_offset_x, style.WindowPadding.y));

        float p_cfg_child_width = ImGui::GetContentRegionAvail().x * 0.5f;
        ui_widgets.beginChild("Configurations", ImVec2(p_cfg_child_width, 0.f), ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        static int file_selected = 0;
        auto config_list = cfg_manager.get_configList();

        ui_widgets.combo("Files", &file_selected, config_list);

        if (ui_widgets.button("Save", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 30.f))) {
            if (!config_list.empty() && (file_selected + 1) <= config_list.size())
                ImGui::OpenPopup("##upd_cfg");
        }

        static const char* cfg_update_alert = "Are you really want to update this config?";
        static ImVec2 update_text_size = ImGui::CalcTextSize(cfg_update_alert);

        static ImVec2 cfg_update_window_size = { update_text_size.x + style.WindowPadding.x * 2.f, update_text_size.y + 30.f + style.WindowPadding.y * 2.f + style.ItemSpacing.y };

        ImGui::SetNextWindowSize(cfg_update_window_size, ImGuiCond_Once);
        ImGui::SetNextWindowPos(ImVec2(g.screen.width * 0.5f - cfg_update_window_size.x * 0.5f, g.screen.height * 0.5f - cfg_update_window_size.y * 0.5f), ImGuiCond_Once);

        if (ImGui::BeginPopupModal("##upd_cfg", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
            ImGui::Text(cfg_update_alert);

            if (ui_widgets.button("Update", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 30.f))) {
                cfg_manager.save(config_list.at(file_selected));
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ui_widgets.button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x, 30.f))) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::SameLine();

        if (ui_widgets.button("Load", ImVec2(ImGui::GetContentRegionAvail().x, 30.f))) {
            if (!config_list.empty() && (file_selected + 1) <= config_list.size())
                ImGui::OpenPopup("##load_cfg");
        }

        static const char* cfg_load_alert = "Are you really want to load this config?";
        static ImVec2 load_text_size = ImGui::CalcTextSize(cfg_load_alert);

        static ImVec2 cfg_load_window_size = { load_text_size.x + style.WindowPadding.x * 2.f, load_text_size.y + 30.f + style.WindowPadding.y * 2.f + style.ItemSpacing.y };

        ImGui::SetNextWindowSize(cfg_load_window_size, ImGuiCond_Once);
        ImGui::SetNextWindowPos(ImVec2(g.screen.width * 0.5f - cfg_load_window_size.x * 0.5f, g.screen.height * 0.5f - cfg_load_window_size.y * 0.5f), ImGuiCond_Once);

        if (ImGui::BeginPopupModal("##load_cfg", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
            ImGui::Text(cfg_load_alert);

            if (ui_widgets.button("Load", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 30.f))) {
                cfg_manager.load(config_list.at(file_selected));

                this->colors();
                this->fonts();

                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ui_widgets.button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x, 30.f))) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        if (ui_widgets.button("Create", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 30.f))) {
            ImGui::OpenPopup("##create_cfg");
        }

        if (ImGui::BeginPopup("##create_cfg", ImGuiWindowFlags_AlwaysAutoResize)) {
            static char name[13]{};

            ImGui::InputTextWithHint("##cfg_input", "Enter new config name", name, sizeof(name));

            if (ui_widgets.button("Confirm", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 30.f))) {
                std::string config_str = std::string(name);
                if (!config_str.empty()) {
                    if (cfg_manager.create(config_str))
                        cfg_manager.save(config_str);
                }

                memset(name, 0, sizeof(name));
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ui_widgets.button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x, 30.f))) {
                memset(name, 0, sizeof(name));
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::SameLine();

        if (ui_widgets.button("Remove", ImVec2(ImGui::GetContentRegionAvail().x, 30.f))) {
            if (!config_list.empty() && (file_selected + 1) <= config_list.size())
                ImGui::OpenPopup("##remove_cfg");
        }

        static const char* cfg_remove_alert = "Are you really want to REMOVE this config?";
        static ImVec2 remove_text_size = ImGui::CalcTextSize(cfg_remove_alert);

        static ImVec2 cfg_remove_window_size = { remove_text_size.x + style.WindowPadding.x * 2.f, remove_text_size.y + 30.f + style.WindowPadding.y * 2.f + style.ItemSpacing.y };

        ImGui::SetNextWindowSize(cfg_remove_window_size, ImGuiCond_Once);
        ImGui::SetNextWindowPos(ImVec2(g.screen.width * 0.5f - cfg_remove_window_size.x * 0.5f, g.screen.height * 0.5f - cfg_remove_window_size.y * 0.5f), ImGuiCond_Once);

        if (ImGui::BeginPopupModal("##remove_cfg", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
            ImGui::Text(cfg_remove_alert);

            if (ui_widgets.button("Remove", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 30.f))) {
                cfg_manager.remove(config_list.at(file_selected));
                file_selected = 0;

                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ui_widgets.button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x, 30.f))) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ui_widgets.endChild();

        ImGui::SetCursorPosX(child_tabs_size_width + style.WindowPadding.x * 2 + content_offset_x);
        ui_widgets.beginChild("Menu appearance", ImVec2(p_cfg_child_width, 0.f), ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        if (ui_widgets.colorEdit("Accent color", &cfg.ui.accent_color)) {
            this->colors();
        }
        ImGui::Separator();
        ui_widgets.checkbox("Watermark", &cfg.ui.watermark.draw);
        ImGui::Separator();
        ui_widgets.combo("Mark position", &cfg.ui.watermark.position_num, { "Top", "Bottom" });
        ImGui::Separator();
        ui_widgets.multiCombo("Mark elements", { "Time", "Render FPS", "Memory", "Velocity", "Ping"}, cfg.ui.watermark.elements);

        ui_widgets.endChild();

        ImGui::SetCursorPos(ImVec2(child_tabs_size_width + style.WindowPadding.x * 2 + p_cfg_child_width + style.ItemSpacing.x + content_offset_x, style.WindowPadding.y));
        ui_widgets.beginChild("Settings", ImVec2(ImGui::GetContentRegionAvail().x, 0.f), ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ui_widgets.keyBind("Menu key", &cfg.ui.menu_keybind);
        ImGui::Separator();
        if (ui_widgets.button("Panic!", ImVec2(ImGui::GetContentRegionAvail().x, 30.f))) {
            if (!panic_triggered) {
                panic_triggered = true;
                this->is_opened = false;
            }
        }

        ui_widgets.endChild();
    }

    ImGui::PopStyleVar();

    ImGui::End();

    ImGui::PopStyleVar();
}

void c_user_interface::render_text() {
    ImDrawList* draw = ImGui::GetBackgroundDrawList();

    if (this->is_map_updating) {
        static const char* updating_text = "updating map for visible check\nthis may take some time..";

        static ImVec2 text_pos = { 20, (float) g.screen.height / 3 };
        c_esp::draw_outlined_text(draw, ImGui::GetFont(), 25.f, text_pos, ImColor(255, 255, 255), updating_text);
    }
}

void c_user_interface::style() {
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();

    style.ScaleAllSizes(1.f);

    style.WindowRounding = 8.f;
    style.ChildRounding = 8.f;
    style.FrameRounding = 5.f;
    style.PopupRounding = 7.f;
    style.GrabRounding = 5.f;
    style.ScrollbarRounding = 8.f;

	style.WindowPadding = ImVec2(16.f, 16.f);
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
}

void c_user_interface::colors() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImColor& accent_color = cfg.ui.accent_color;

    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.11f, 0.13f, 1.f);

    style.Colors[ImGuiCol_Text] = ImVec4(0.96f, 0.96f, 0.98f, 1.f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.55f, 0.55f, 0.62f, 1.f);

    style.Colors[ImGuiCol_FrameBg] = color_helper.make_darkened(accent_color, 0.2f);
    style.Colors[ImGuiCol_FrameBgHovered] = color_helper.make_darkened(accent_color, 0.3f);
    style.Colors[ImGuiCol_FrameBgActive] = color_helper.make_darkened(accent_color, 0.7f);

    style.Colors[ImGuiCol_SliderGrab] = color_helper.make_darkened(accent_color, 0.6f);
    style.Colors[ImGuiCol_SliderGrabActive] = accent_color;

    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.86f, 0.84f, 1.f, 1.f);

    style.Colors[ImGuiCol_Button] = color_helper.make_darkened(accent_color, 0.2f);
    style.Colors[ImGuiCol_ButtonHovered] = color_helper.make_darkened(accent_color, 0.3f);
    style.Colors[ImGuiCol_ButtonActive] = color_helper.make_darkened(accent_color, 0.7f);

    style.Colors[ImGuiCol_Header] = color_helper.make_darkened(accent_color, 0.2f);
    style.Colors[ImGuiCol_HeaderHovered] = color_helper.make_darkened(accent_color, 0.3f);
    style.Colors[ImGuiCol_HeaderActive] = color_helper.make_darkened(accent_color, 0.7f);

    style.Colors[ImGuiCol_PopupBg] = color_helper.make_darkened(accent_color, 0.1f);
    style.Colors[ImGuiCol_Border] = color_helper.make_darkened(accent_color, 0.2f);
    style.Colors[ImGuiCol_Separator] = ImVec4(1.f, 1.f, 1.f, 0.055f);

    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.f, 0.f, 0.f, 0.35f);
}

void c_user_interface::fonts() {
    static bool was_updated = false;

    ImGuiIO& io = ImGui::GetIO();

    if (!was_updated) {
        io.Fonts->ClearFonts();

        ImFontConfig main_cfg{};
        main_cfg.SizePixels = 18.f;
        main_cfg.GlyphRanges = io.Fonts->GetGlyphRangesCyrillic();

        io.Fonts->AddFontFromMemoryTTF(google_sans_font, sizeof(google_sans_font), main_cfg.SizePixels, &main_cfg);

        ImFontConfig icons_cfg{};
        icons_cfg.MergeMode = true;
        icons_cfg.PixelSnapH = true;
        icons_cfg.FontDataOwnedByAtlas = false;
        icons_cfg.GlyphOffset.y = 1.0f;
        icons_cfg.SizePixels = 13.f;

        static const ImWchar icon_ranges[] = { 0xE100, 0xE109, 0 };

        io.Fonts->AddFontFromMemoryTTF(watermark_icons_font, sizeof(watermark_icons_font), icons_cfg.SizePixels, &icons_cfg, icon_ranges);

        /* --------------------------------------------------------------------------------------- */
        nyaware_logo_font = io.Fonts->AddFontFromMemoryTTF(nyaware_logo, sizeof(nyaware_logo), 25.f, nullptr);
        menu_ico_font = io.Fonts->AddFontFromMemoryTTF(menu_icons_font, sizeof(menu_icons_font), 28.f, nullptr);
        /* --------------------------------------------------------------------------------------- */

        was_updated = true;
    }

    ImFontConfig esp_cfg{};
    esp_cfg.SizePixels = 20.f;
    esp_cfg.GlyphRanges = io.Fonts->GetGlyphRangesCyrillic();

    struct {
        unsigned char* bytes{};
        size_t size{};
    } font_data;

    switch (cfg.visuals.font_num) {
        case 0:
            font_data = { jacobs_font, sizeof(jacobs_font) };
            break;
        case 1:
            font_data = { pixel_font, sizeof(pixel_font) };
            break;
        case 2:
            font_data = { gotham_font, sizeof(gotham_font) };
            break;
        case 3:
            font_data = { montserrat_font, sizeof(montserrat_font) };
            break;
    }

    g.fonts.visuals = io.Fonts->AddFontFromMemoryTTF(font_data.bytes, font_data.size, esp_cfg.SizePixels, &esp_cfg);
    g.fonts.weapon = io.Fonts->AddFontFromMemoryTTF(weapon_font, sizeof(weapon_font), esp_cfg.SizePixels, &esp_cfg);
}

void c_user_interface::init() {
    this->style();
    this->colors();
    this->fonts();
}

void c_user_interface::update() {
    this->watermark();
    this->render();
    this->render_text();
}