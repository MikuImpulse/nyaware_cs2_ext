#include "visuals.hpp"

#include <string>
#include <format>

#include "data/globals.hpp"
#include "data/config.hpp"

#include "utils/log.hpp"

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