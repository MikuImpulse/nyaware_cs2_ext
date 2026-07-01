#include "visuals.hpp"

#include <string>
#include <format>

#include "data/globals.hpp"
#include "data/config.hpp"

#include "utils/log.hpp"

void c_visuals::draw_speed(ImDrawList* draw, const player_t& local_player) {
	if (!cfg.visuals.speedometer.draw) return;

	vector3_t velocity = local_player.pawn->m_vecAbsVelocity();

	float speed_units = std::sqrt((velocity.x * velocity.x + velocity.y * velocity.y));
	float speed_meters = speed_units / 39.37f;

	std::string speed_str = std::format("{:.1f} {}/s", cfg.visuals.speedometer.mode == 0 ? speed_meters : speed_units, cfg.visuals.speedometer.mode == 0 ? "m" : "u");
	
	ImVec2 text_size = ImGui::GetFont()->CalcTextSizeA(25.f, FLT_MAX, 0.f, speed_str.c_str());
	ImVec2 text_pos = { g.screen.width * 0.5f - text_size.x * 0.5f, g.screen.height * 0.8f - text_size.y * 0.5f };

	c_esp::draw_outlined_text(draw, ImGui::GetFont(), 25.f, text_pos, ImColor(255, 255, 255), speed_str.c_str());
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