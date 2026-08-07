#pragma once

#include <cstdint>
#include "data/sdk/cs2/include.hpp"
#include "data/sdk/source2/include.hpp"

struct player_t {
	CCSPlayerController* controller{};
	C_CSPlayerPawn* pawn{};

	team_t team{};

	uint32_t ping{};
	std::string nickname{};

	vector3_t position{};
	vector3_t top_position{};

	struct {
		int value{};
		int max{};
	} health;

	struct {
		int value{};
		int max = 100;
	} armor;

	struct {
		C_CSWeaponBase* base{};
		CCSWeaponBaseVData* data{};

		std::string name{};
		std::string icon{};
	} weapon;

	player_t() = default;
	player_t(uintptr_t entity_list, CCSPlayerController* controller, C_CSPlayerPawn* pawn);

	inline bool isValid() const {
		return controller && pawn && weapon.base && weapon.data;
	}

	inline bool isAlive() const {
		if (this->isValid())
			return health.value > 0;

		return false;
	}
};

struct spectator_t {
	CCSPlayerController* controller{};
	C_CSPlayerPawn* pawn{};

	team_t team{};
	std::string nickname{};

	inline bool isValid() const {
		return controller && pawn;
	}

	spectator_t() = default;
	spectator_t(uintptr_t entity_list, CCSPlayerController* controller, C_CSPlayerPawn* pawn);
};

struct crosshair_target_t {
	C_CSPlayerPawn* pawn{};

	team_t team{};

	crosshair_target_t() = default;
	crosshair_target_t(C_CSPlayerPawn* pawn);
};