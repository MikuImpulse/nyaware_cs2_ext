#include "c_cs2_player_controller.hpp"

CCSPlayerController* CCSPlayerController::get(uintptr_t entity_list, int index) {
	const auto list_entry = mem.read<uintptr_t>(entity_list + (8 * (index & 0x7FFF) >> 9) + 16);
	if (list_entry)
		return mem.read<CCSPlayerController*>(list_entry + 112 * (index & 0x7FFF));

	return nullptr;
}