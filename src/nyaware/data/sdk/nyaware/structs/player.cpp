#include "player.hpp"
#include "utils/log.hpp"

player_t::player_t(uintptr_t entity_list, CCSPlayerController* controller, C_CSPlayerPawn* pawn) {
	this->controller = controller;
	this->pawn = pawn;

	this->team = pawn->m_iTeamNum();

	this->ping = controller->m_iPing();
	this->nickname = controller->m_iszPlayerName();

	this->position = pawn->m_vOldOrigin();
	this->top_position = { position.x, position.y, position.z + pawn->m_vecViewOffset().z };

	this->health.value = pawn->m_iHealth();
	this->health.max = pawn->m_iMaxHealth();

	this->armor.value = pawn->m_ArmorValue();

	this->weapon.base = C_CSWeaponBase::get(entity_list, pawn->m_pWeaponServices()->m_hActiveWeapon());
	this->weapon.data = weapon.base->m_nSubclass<CCSWeaponBaseVData>();

	auto value = weaponDefinitions.find(weapon.data->m_szName());
	if (value != weaponDefinitions.end()) {
		this->weapon.name = value->second.first;
		this->weapon.icon = value->second.second;
	}
}

spectator_t::spectator_t(uintptr_t entity_list, CCSPlayerController* controller, C_CSPlayerPawn* pawn) {
	this->controller = controller;
	this->pawn = pawn;

	this->team = pawn->m_iTeamNum();
	this->nickname = controller->m_iszPlayerName();
}

crosshair_target_t::crosshair_target_t(C_CSPlayerPawn* pawn) {
	this->pawn = pawn;
	this->team = pawn->m_iTeamNum();
};