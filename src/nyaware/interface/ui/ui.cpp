#include "ui.hpp"

#include <format>

#include "interface/data/fonts/include.hpp"
#include "data/globals.hpp"

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
    ImGui::Checkbox("aimbot##weapon", &config->aimbot);
    ImGui::Checkbox("LMB check", &config->lmb_check);
    ImGui::Checkbox("draw fov", &config->fov.draw);
    ImGui::SliderFloat("fov##slider", &config->fov.value, 0.1f, 360.f);
    ImGui::SliderFloat("smooth##slider", &config->smooth, 1.f, 2.5f);
    ImGui::SliderInt("rcs power##slider", &config->rcs_strength, 5, 25);

    static const char* bones[] = { "head", "neck", "chest" };

    ImGui::Combo("bone##combo", &config->bone, bones, IM_ARRAYSIZE(bones));
}

void c_user_interface::render_weapon_trigger_config(weapon_config_t* config, const char* weapon_name) {
    ImGui::Checkbox("triggerbot##weapon", &config->triggerbot.enable);

    if (!strcmp(weapon_name, "sniper_rifle")) {
        ImGui::Checkbox("scope check##sniper", &cfg.legitbot.scope_check);
    }

    if (!strcmp(weapon_name, "pistol")) {
        ImGui::Checkbox("early shot (revolver)", &cfg.legitbot.early_shot);
    }

    ImGui::SliderFloat("delay (sec)##trigger", &config->triggerbot.delay, 0.f, 0.5f);
}

void c_user_interface::render() {
    if (this->is_opened) {
        ImGui::SetNextWindowSize(window_size, ImGuiCond_Once);

        ImGui::Begin("##nyaware", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

        ImGuiStyle& style = ImGui::GetStyle();
        ImDrawList* draw = ImGui::GetWindowDrawList();
        ImVec2 window_pos = ImGui::GetWindowPos();

        ImGui::BeginChild("##tabs", ImVec2(window_size.x * 0.2f - style.WindowPadding.x * 2, window_size.y - style.WindowPadding.y * 2), ImGuiChildFlags_Borders);

        ImVec2 logo_size = nyaware_logo_font->CalcTextSizeA(25, FLT_MAX, 0.f, "A");
        draw->AddText(nyaware_logo_font, 25, ImVec2(ImGui::GetCursorScreenPos().x + (ImGui::GetContentRegionAvail().x * 0.5f - logo_size.x * 0.5f), ImGui::GetCursorScreenPos().y), ImColor(255, 255, 255), "A");

        ImGui::SetCursorPosY(ImGui::GetCursorPos().y + logo_size.y + style.WindowPadding.y);
        ImGui::Separator();

        if (ImGui::Button(std::format("{} visuals", ICON_FA_EYE).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 30))) {
            tab = 1;
        }

        if (ImGui::Button(std::format("{} legitbot", ICON_FA_CROSSHAIRS).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 30))) {
            tab = 2;
        }

        if (ImGui::Button(std::format("{} misc", ICON_FA_WAND_SPARKLES).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 30))) {
            tab = 3;
        }

        if (ImGui::Button(std::format("{} settings", ICON_FA_GEAR).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 30))) {
            tab = 4;
        }

        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("##funcs", ImGui::GetContentRegionAvail(), ImGuiChildFlags_Borders);

        if (tab == 1) {
            ImGui::Checkbox("esp", &cfg.visuals.esp.enable);

            if (cfg.visuals.esp.enable) {
                ImGui::Separator();

                ImGui::Text("player");

                ImGui::Checkbox("tracer", &cfg.visuals.esp.player.tracer.draw);
                ImGui::SameLine();
                ImGui::ColorEdit4("##tracercol", (float*)&cfg.visuals.esp.player.tracer.color, ImGuiColorEditFlags_NoInputs);

                ImGui::Checkbox("rectangle", &cfg.visuals.esp.player.rect.draw);
                ImGui::SameLine();
                ImGui::ColorEdit4("##rectcol", (float*)&cfg.visuals.esp.player.rect.color, ImGuiColorEditFlags_NoInputs);

                static const char* rect_modes[] = { "default", "corners", "3d box" };

                if (cfg.visuals.esp.player.rect.draw) {
                    ImGui::SameLine();

                    ImGui::SetNextItemWidth(200.f);
                    ImGui::Combo("##rectmode", &cfg.visuals.esp.player.rect.mode, rect_modes, IM_ARRAYSIZE(rect_modes));
                }

                ImGui::Checkbox("health", &cfg.visuals.esp.player.health.draw);
                ImGui::SameLine();
                ImGui::ColorEdit4("##healthcol", (float*)&cfg.visuals.esp.player.health.color, ImGuiColorEditFlags_NoInputs);
                ImGui::SameLine();
                ImGui::ColorEdit4("##healthtextcol", (float*)&cfg.visuals.esp.player.health.text_color, ImGuiColorEditFlags_NoInputs);

                ImGui::Checkbox("name", &cfg.visuals.esp.player.nickName.draw);
                ImGui::SameLine();
                ImGui::ColorEdit4("##namecol", (float*)&cfg.visuals.esp.player.nickName.color, ImGuiColorEditFlags_NoInputs);

                ImGui::Checkbox("skeleton", &cfg.visuals.esp.player.skeleton.draw);
                ImGui::SameLine();
                ImGui::ColorEdit4("##skeletoncol", (float*)&cfg.visuals.esp.player.skeleton.color, ImGuiColorEditFlags_NoInputs);

                if (cfg.visuals.esp.player.skeleton.draw) {
                    ImGui::SameLine();
                    ImGui::Checkbox("visible check", &cfg.visuals.esp.player.skeleton.visible_check);
                    ImGui::SameLine();
                    ImGui::ColorEdit4("##boneviscol", (float*)&cfg.visuals.esp.player.skeleton.visible_color, ImGuiColorEditFlags_NoInputs);
                    ImGui::SameLine();
                    ImGui::ColorEdit4("##boneinviscol", (float*)&cfg.visuals.esp.player.skeleton.invisible_color, ImGuiColorEditFlags_NoInputs);
                }

                ImGui::Checkbox("weapon", &cfg.visuals.esp.player.weapon.draw);
                ImGui::SameLine();
                ImGui::ColorEdit4("##weaponcol1", (float*)&cfg.visuals.esp.player.weapon.colors[0], ImGuiColorEditFlags_NoInputs);
                ImGui::SameLine();
                ImGui::ColorEdit4("##weaponcol2", (float*)&cfg.visuals.esp.player.weapon.colors[1], ImGuiColorEditFlags_NoInputs);
                ImGui::SameLine();
                ImGui::ColorEdit4("##weaponcol3", (float*)&cfg.visuals.esp.player.weapon.colors[2], ImGuiColorEditFlags_NoInputs);

                if (cfg.visuals.esp.player.weapon.draw) {
                    ImGui::SameLine();

                    ImGui::SetNextItemWidth(200.f);
                    ui_widgets.multiCombo("modes##wp", { "icon", "name", "ammo" }, cfg.visuals.esp.player.weapon.modes);
                }

                ImGui::Checkbox("flags", &cfg.visuals.esp.player.flags.draw);
                ImGui::SameLine();
                ImGui::ColorEdit4("##flagscol1", (float*)&cfg.visuals.esp.player.flags.colors[0], ImGuiColorEditFlags_NoInputs);
                ImGui::SameLine();
                ImGui::ColorEdit4("##flagscol2", (float*)&cfg.visuals.esp.player.flags.colors[1], ImGuiColorEditFlags_NoInputs);

                if (cfg.visuals.esp.player.flags.draw) {
                    ImGui::SameLine();

                    ImGui::SetNextItemWidth(200.f);
                    ui_widgets.multiCombo("modes##fl", { "defuse", "scope" }, cfg.visuals.esp.player.flags.modes);
                }

                ImGui::Spacing();

                ImGui::Text("world");

                ImGui::Checkbox("bomb", &cfg.visuals.esp.world.bomb.draw);
            }

            ImGui::Separator();

            ImGui::Text("visuals");
            ImGui::Checkbox("camera fov", &cfg.visuals.fov.enable);

            ImGui::SameLine();

            ImGui::Checkbox("anti flash", &cfg.visuals.anti_flash);

            ImGui::SameLine();

            float speedo_cursor_x = ImGui::GetCursorPosX();

            ImGui::Checkbox("speedometer", &cfg.visuals.speedometer.draw);

            if (cfg.visuals.fov.enable) {
                ImGui::SetNextItemWidth(105.f);
                ImGui::SliderInt("##desfov", &cfg.visuals.fov.value, 30, 120);

                ImGui::SameLine();
            }

            static const char* speedo_modes[] = { "meters", "units" };

            if (cfg.visuals.speedometer.draw) {
                ImGui::SetCursorPosX(speedo_cursor_x);

                ImGui::SetNextItemWidth(120.f);
                ImGui::Combo("##speedomode", &cfg.visuals.speedometer.mode, speedo_modes, IM_ARRAYSIZE(speedo_modes));
            }
        }
        else if (tab == 2) {
            weapon_config_t* current_weapon = get_weaponConfig(weapon_selected);

            ImGui::BeginChild("##weapon_types", ImVec2(ImGui::GetContentRegionAvail().x, 40), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar);

            float button_width = (ImGui::GetContentRegionAvail().x - style.ItemSpacing.x * 5) / 6;

            ImGui::PushFont(g.fonts.weapon);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));

            const char* icons[] = { "`", "w", "A", "~", "C", "y" };

            for (int i = 0; i < 6; i++) {
                if (i > 0) ImGui::SameLine();

                ImVec2 text_size = ImGui::CalcTextSize(icons[i]);

                ImVec2 button_size(button_width, ImGui::GetContentRegionAvail().y);
                ImVec2 text_pos = ImGui::GetCursorScreenPos();
                text_pos.x += (button_size.x - text_size.x) * 0.5f;
                text_pos.y += (button_size.y - text_size.y) * 0.5f - 2;

                int pushed_count = 0;
                if (weapon_selected == i) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.9f, 1.f));
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

            ImGui::BeginChild("##weapon_cfg", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f - style.FramePadding.x, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_Borders);

            ImGui::Checkbox("master switch##legitbot", &cfg.legitbot.enable);

            ImGui::Separator();

            static const char* weapons[] = { "pistol", "submachine_gun", "rifle", "shotgun", "sniper_rifle", "machine_gun" };

            if (current_weapon) {
                render_weapon_config(current_weapon, weapons[weapon_selected]);
            }

            ImGui::EndChild();

            ImGui::SameLine();

            ImGui::BeginChild("##weapon_trigger_cfg", ImGui::GetContentRegionAvail(), ImGuiChildFlags_Borders);

            if (current_weapon) {
                render_weapon_trigger_config(current_weapon, weapons[weapon_selected]);
            }

            ImGui::EndChild();
        }
        else if (tab == 3) {
            ImGui::Checkbox("bunny hop (space)", &cfg.misc.bunny_hop);
        }
        else if (tab == 4) {
            ImGui::Text(std::format("build date: {} {}", __DATE__, __TIME__).c_str());

            if (ImGui::Button("panic")) {
                cfg.client.panic = true;
            }
        }

        ImGui::EndChild();

        ImGui::End();
    }
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

    style.WindowPadding = ImVec2(10.f, 10.f);
    style.WindowRounding = 8.f;
    style.ChildRounding = 6.f;
    style.FramePadding = ImVec2(4.f, 4.f);
    style.FrameRounding = 8.f;
    style.ItemSpacing = ImVec2(8.f, 4.f);
    style.ItemInnerSpacing = ImVec2(8.f, 4.f);
    style.IndentSpacing = 25.f;
    style.ScrollbarSize = 13.f;
    style.ScrollbarRounding = 12.f;
    style.GrabMinSize = 10.f;
    style.GrabRounding = 6.f;
    style.PopupRounding = 8.f;
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);

    style.Colors[ImGuiCol_Text] = ImVec4(0.9f, 0.9f, 0.93f, 1.f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4f, 0.4f, 0.45f, 1.f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.14f, 1.f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.18f, 0.2f, 0.22f, 0.3f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.13f, 0.15f, 1.f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.3f, 0.3f, 0.35f, 1.f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.18f, 0.2f, 1.f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.28f, 1.f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.3f, 0.3f, 0.34f, 1.f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.15f, 0.17f, 1.f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1f, 0.1f, 0.12f, 1.f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.17f, 1.f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.14f, 1.f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.12f, 0.12f, 0.14f, 1.f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3f, 0.3f, 0.35f, 1.f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4f, 0.4f, 0.45f, 1.f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5f, 0.5f, 0.55f, 1.f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.7f, 0.7f, 0.9f, 1.f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.7f, 0.7f, 0.9f, 1.f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.8f, 0.8f, 0.9f, 1.f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.18f, 0.18f, 0.2f, 1.f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.6f, 0.6f, 0.9f, 1.f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.8f, 0.8f, 1.f, 1.f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.2f, 0.23f, 1.f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.28f, 1.f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.3f, 0.3f, 0.34f, 1.f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.4f, 0.4f, 0.45f, 1.f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.5f, 0.5f, 0.55f, 1.f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.6f, 0.6f, 0.65f, 1.f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.2f, 0.2f, 0.23f, 1.f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.25f, 0.25f, 0.28f, 1.f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.3f, 0.3f, 0.34f, 1.f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.64f, 1.f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.7f, 0.7f, 0.75f, 1.f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.61f, 0.61f, 0.64f, 1.f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.7f, 0.7f, 0.75f, 1.f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.3f, 0.3f, 0.34f, 1.f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.1f, 0.1f, 0.12f, 0.8f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.2f, 0.22f, 1.f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.6f, 0.6f, 0.9f, 1.f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.56f, 0.96f, 1.f);
}

void c_user_interface::init() {
    this->style();

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->ClearFonts();

    ImFontConfig main_cfg{};
    main_cfg.SizePixels = 15.f;

    io.Fonts->AddFontFromMemoryTTF(eurostile_font, sizeof(eurostile_font), main_cfg.SizePixels, &main_cfg);
    
    ImFontConfig fawesome_cfg{};
    fawesome_cfg.SizePixels = 15.f;
    fawesome_cfg.MergeMode = true;
    fawesome_cfg.PixelSnapH = true;
    fawesome_cfg.OversampleH = 3;
    fawesome_cfg.OversampleV = 3;
    fawesome_cfg.GlyphOffset.y = 1.5f;
    static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0x0 };

    io.Fonts->AddFontFromMemoryTTF(font_awesome_binary, sizeof(font_awesome_binary), fawesome_cfg.SizePixels, &fawesome_cfg, icon_ranges);

    /* --------------------------------------------------------------------------------------- */
    nyaware_logo_font = io.Fonts->AddFontFromMemoryTTF(nyaware_logo, sizeof(nyaware_logo), 15.f);
    /* --------------------------------------------------------------------------------------- */

    ImFontConfig esp_cfg{};
    esp_cfg.SizePixels = 20.f;
    esp_cfg.GlyphRanges = io.Fonts->GetGlyphRangesCyrillic();

    g.fonts.jacobs = io.Fonts->AddFontFromMemoryTTF(jacobs_font, sizeof(jacobs_font), esp_cfg.SizePixels, &esp_cfg);
    g.fonts.weapon = io.Fonts->AddFontFromMemoryTTF(weapon_font, sizeof(weapon_font), esp_cfg.SizePixels, &esp_cfg);
}

void c_user_interface::update() {
    this->render();
    this->render_text();
}