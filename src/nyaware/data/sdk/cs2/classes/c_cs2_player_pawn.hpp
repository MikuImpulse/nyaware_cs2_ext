#pragma once

#include "c_base_model_entity.hpp"
#include "c_cs2_player_weapon.hpp"

#include "data/sdk/schema_dumper.hpp"
#include "data/sdk/source2/include.hpp"

#include "utils/memory.hpp"

struct CPlayer_ObserverServices {
	inline uintptr_t m_hObserverTarget() const {
		return mem.read<uintptr_t>(this_cast + c_schema_dumper::schema_offset["CPlayer_ObserverServices"]["m_hObserverTarget"]);
	}
};

struct C_BasePlayerPawn : C_BaseCombatCharacter {
	inline CPlayer_WeaponServices* m_pWeaponServices() const {
		return mem.read<CPlayer_WeaponServices*>(this_cast + c_schema_dumper::schema_offset["C_BasePlayerPawn"]["m_pWeaponServices"]);
	}

	inline CPlayer_ObserverServices* m_pObserverServices() const {
		return mem.read<CPlayer_ObserverServices*>(this_cast + c_schema_dumper::schema_offset["C_BasePlayerPawn"]["m_pObserverServices"]);
	}

	inline vector3_t m_vOldOrigin() const {
		return mem.read<vector3_t>(this_cast + c_schema_dumper::schema_offset["C_BasePlayerPawn"]["m_vOldOrigin"]);
	}
};

struct C_CSPlayerPawnBase : C_BasePlayerPawn {
	inline float m_flFlashBangTime() const {
		return mem.read<float>(this_cast + c_schema_dumper::schema_offset["C_CSPlayerPawnBase"]["m_flFlashBangTime"]);
	}

	inline float m_flFlashDuration() const {
		return mem.read<float>(this_cast + c_schema_dumper::schema_offset["C_CSPlayerPawnBase"]["m_flFlashDuration"]);
	}

	inline void m_flFlashDuration_set(float value) {
		mem.write<float>(this_cast + c_schema_dumper::schema_offset["C_CSPlayerPawnBase"]["m_flFlashDuration"], value);
	}
};

struct C_CSPlayerPawn : C_CSPlayerPawnBase {
	inline CCSPlayer_AimPunchServices* m_pAimPunchServices() const {
		return mem.read<CCSPlayer_AimPunchServices*>(this_cast + c_schema_dumper::schema_offset["C_CSPlayerPawn"]["m_pAimPunchServices"]);
	}

	inline bool m_bIsScoped() const {
		return mem.read<bool>(this_cast + c_schema_dumper::schema_offset["C_CSPlayerPawn"]["m_bIsScoped"]);
	}

	inline bool m_bIsDefusing() const {
		return mem.read<bool>(this_cast + c_schema_dumper::schema_offset["C_CSPlayerPawn"]["m_bIsDefusing"]);
	}

	inline int m_iShotsFired() const {
		return mem.read<int>(this_cast + c_schema_dumper::schema_offset["C_CSPlayerPawn"]["m_iShotsFired"]);
	}

	inline int m_ArmorValue() const {
		return mem.read<int>(this_cast + c_schema_dumper::schema_offset["C_CSPlayerPawn"]["m_ArmorValue"]);
	}

	inline int m_iIDEntIndex() const {
		return mem.read<int>(this_cast + c_schema_dumper::schema_offset["C_CSPlayerPawn"]["m_iIDEntIndex"]);
	}

	static C_CSPlayerPawn* get(uintptr_t entity_list, uintptr_t handle);
	static C_CSPlayerPawn* get_fromCrosshairID(uintptr_t entity_list, uintptr_t crosshair_id);
};