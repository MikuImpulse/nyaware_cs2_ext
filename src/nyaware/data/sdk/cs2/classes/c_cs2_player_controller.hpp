#pragma once

#include <cstdint>

#include "data/sdk/schema_dumper.hpp"
#include "utils/memory.hpp"

#include "c_base_model_entity.hpp"

struct CBasePlayerController : C_BaseEntity {
	inline uintptr_t m_hPawn() const {
		return mem.read<uintptr_t>(this_cast + c_schema_dumper::schema_offset["CBasePlayerController"]["m_hPawn"]);
	}

	inline std::string m_iszPlayerName() const {
		return mem.read_str(this_cast + c_schema_dumper::schema_offset["CBasePlayerController"]["m_iszPlayerName"]);
	}

	inline bool m_bIsLocalPlayerController() const {
		return mem.read<bool>(this_cast + c_schema_dumper::schema_offset["CBasePlayerController"]["m_bIsLocalPlayerController"]);
	}

	inline uint32_t m_iDesiredFOV() const {
		return mem.read<uint32_t>(this_cast + c_schema_dumper::schema_offset["CBasePlayerController"]["m_iDesiredFOV"]);
	}

	inline void m_iDesiredFOV_set(uint32_t value) const {
		mem.write<uint32_t>(this_cast + c_schema_dumper::schema_offset["CBasePlayerController"]["m_iDesiredFOV"], value);
	}
};

struct CCSPlayerController : CBasePlayerController {
	inline uint32_t m_iPing() const {
		return mem.read<uint32_t>(this_cast + c_schema_dumper::schema_offset["CCSPlayerController"]["m_iPing"]);
	}

	inline uintptr_t m_hPlayerPawn() const {
		return mem.read<uintptr_t>(this_cast + c_schema_dumper::schema_offset["CCSPlayerController"]["m_hPlayerPawn"]);
	}

	inline uintptr_t m_hObserverPawn() const {
		return mem.read<uintptr_t>(this_cast + c_schema_dumper::schema_offset["CCSPlayerController"]["m_hObserverPawn"]);
	}

	inline bool m_bPawnIsAlive() const {
		return mem.read<bool>(this_cast + c_schema_dumper::schema_offset["CCSPlayerController"]["m_bPawnIsAlive"]);
	}

	static CCSPlayerController* get(uintptr_t entity_list, int index);
};