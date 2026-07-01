#include "c_base_player_weapon.hpp"

C_CSWeaponBase* C_CSWeaponBase::get(uintptr_t entity_list, uintptr_t handle) {
	const auto entry = mem.read<uintptr_t>(entity_list + 8 * ((handle & 0x7FFF) >> 9) + 16);
	if (entry)
		return mem.read<C_CSWeaponBase*>(entry + 112 * (handle & 0x1FF));

	return nullptr;
}