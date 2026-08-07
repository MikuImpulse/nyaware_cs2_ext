#include "visuals.hpp"

#include <string>
#include <format>

#include "data/globals.hpp"
#include "data/config.hpp"

#include "utils/log.hpp"

void c_visuals::spectator_list(bool is_ui_opened, const std::vector<spectator_t>& spectators) {
    if (spectators.empty() && !(is_ui_opened && cfg.visuals.spectator_list.enable)) return;

    ImGuiStyle& style = ImGui::GetStyle();
    int spectators_count = static_cast<int>(spectators.size());

    const float title_height = ImGui::GetFontSize() + style.FramePadding.y * 2.0f;
    const float content_height = style.WindowPadding.y * 2.0f + spectators_count * ImGui::GetTextLineHeight()
        + std::max<int>(0, spectators_count - 1) * style.ItemSpacing.y;

    const float height = title_height + content_height;

    ImGui::SetNextWindowSize(ImVec2(200.0f, height), ImGuiCond_Always);

    if (cfg.visuals.spectator_list.position_update) {
        ImGui::SetNextWindowPos(cfg.visuals.spectator_list.position, ImGuiCond_Always);
        cfg.visuals.spectator_list.position_update = false;
    }

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImColor(0, 0, 0, 100).Value);
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImColor(0, 0, 0, 255).Value);
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImColor(0, 0, 0, 255).Value);

    ImGui::Begin("Spectators", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    for (const spectator_t& spectator : spectators) {
        if (!spectator.isValid())
            continue;

        ImGui::TextUnformatted(spectator.nickname.c_str());
    }

    cfg.visuals.spectator_list.position = ImGui::GetWindowPos();
    ImGui::End();

    ImGui::PopStyleColor(3);
}

void c_visuals::anti_flash(const player_t& local_player) {
	if (!cfg.visuals.anti_flash) return;

	if (local_player.pawn->m_flFlashDuration() > 0.f)
		local_player.pawn->m_flFlashDuration_set(0.f);
}

void c_visuals::change_fov(const player_t& local_player) {
	int current_fov = local_player.controller->m_iDesiredFOV();

	if (!local_player.pawn->m_bIsScoped()) {
		if (cfg.visuals.fov.enable) {
			if (current_fov != cfg.visuals.fov.value)
				local_player.controller->m_iDesiredFOV_set((uint32_t)cfg.visuals.fov.value);
		}
		else
			local_player.controller->m_iDesiredFOV_set((uint32_t)90);
	}
}