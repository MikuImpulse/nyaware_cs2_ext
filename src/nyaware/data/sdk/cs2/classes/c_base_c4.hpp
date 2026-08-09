#pragma once

#include "data/sdk/schema_dumper.hpp"
#include "c_base_model_entity.hpp"

class C_PlantedC4 : public CBaseAnimGraph {
public:
	inline bool m_bBombTicking() const {
		return mem.read<bool>(this_cast + c_schema_dumper::schema_offset["C_PlantedC4"]["m_bBombTicking"]);
	}

	inline int m_nBombSite() const {
		return mem.read<int>(this_cast + c_schema_dumper::schema_offset["C_PlantedC4"]["m_nBombSite"]);
	}
	
	inline float m_flTimerLength() const {
		return mem.read<float>(this_cast + c_schema_dumper::schema_offset["C_PlantedC4"]["m_flTimerLength"]);
	}

	inline bool m_bC4Activated() const {
		return mem.read<bool>(this_cast + c_schema_dumper::schema_offset["C_PlantedC4"]["m_bC4Activated"]);
	}

	inline float m_flDefuseLength() const {
		return mem.read<float>(this_cast + c_schema_dumper::schema_offset["C_PlantedC4"]["m_flDefuseLength"]);
	}
};