#pragma once

#include "data/sdk/schema_dumper.hpp"
#include "c_base_model_entity.hpp"

struct C_BasePlayerWeapon : C_EconEntity {
	inline int m_iClip1() const {
		return mem.read<int>(this_cast + c_schema_dumper::schema_offset["C_BasePlayerWeapon"]["m_iClip1"]);
	}

	inline int m_pReserveAmmo() const {
		return mem.read<int>(this_cast + c_schema_dumper::schema_offset["C_BasePlayerWeapon"]["m_pReserveAmmo"]);
	}
};

struct C_CSWeaponBase : C_BasePlayerWeapon {
	inline bool m_bInReload() const {
		return mem.read<bool>(this_cast + c_schema_dumper::schema_offset["C_CSWeaponBase"]["m_bInReload"]);
	}

	inline bool m_bSilencerOn() const {
		return mem.read<bool>(this_cast + c_schema_dumper::schema_offset["C_CSWeaponBase"]["m_bSilencerOn"]);
	}

	inline float m_fLastShotTime() const {
		return mem.read<float>(this_cast + c_schema_dumper::schema_offset["C_CSWeaponBase"]["m_fLastShotTime"]);
	}
	
	static C_CSWeaponBase* get(uintptr_t entity_list, uintptr_t handle);
};