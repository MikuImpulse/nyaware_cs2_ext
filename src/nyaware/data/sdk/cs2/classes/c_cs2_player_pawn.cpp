#include "c_cs2_player_pawn.hpp"

C_CSPlayerPawn* C_CSPlayerPawn::get(uintptr_t entity_list, uintptr_t handle) {
	const auto entry = mem.read<uintptr_t>(entity_list + 8 * ((handle & 0x7FFF) >> 9) + 16);
	if (entry)
		return mem.read<C_CSPlayerPawn*>(entry + 112 * (handle & 0x1FF));

	return nullptr;
}

C_CSPlayerPawn* C_CSPlayerPawn::get_fromCrosshairID(uintptr_t entity_list, uintptr_t crosshair_id) {
	const auto entry = mem.read<uintptr_t>(entity_list + 0x10);
	if (entry)
		return mem.read<C_CSPlayerPawn*>(entry + 112 * (crosshair_id & 0x1FF));

	return nullptr;
}