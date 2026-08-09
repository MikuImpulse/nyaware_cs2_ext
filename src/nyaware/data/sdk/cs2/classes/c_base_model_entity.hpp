#pragma once

#include "c_base_model_skeleton.hpp"

#include "data/sdk/schema_dumper.hpp"
#include "utils/memory.hpp"

enum class flags_t {
	onground = 1 << 0,
	ducking = 1 << 1,
	waterjump = 1 << 2,
	bot = 1 << 4,
	frozen = 1 << 5,
	atcontrols = 1 << 6,
	client = 1 << 7,
	fakeclient = 1 << 8,
	fly = 1 << 10,
	suppress_save = 1 << 11,
	in_vehicle = 1 << 12,
	godmode = 1 << 14,
	no_target = 1 << 15,
	aimtarget = 1 << 16,
	grenade = 1 << 20,
	donttouch = 1 << 22,
	basevelocity = 1 << 23,
	conveyor = 1 << 24,
	object = 1 << 25,
	onfire = 1 << 27,
	dissolving = 1 << 28,
	transragdoll = 1 << 29,
	unblockable_by_player = 1 << 30
};

enum class team_t : uint8_t {
	none = 0,
	spectator = 1,
	terrorist = 2,
	counter_terrorist = 3
};

class CGameSceneNode {
public:
	inline vector3_t m_vecAbsOrigin() const {
		return mem.read<vector3_t>(this_cast + c_schema_dumper::schema_offset["CGameSceneNode"]["m_vecAbsOrigin"]);
	}
};

class CSkeletonInstance : public CGameSceneNode {
public:
	inline CModelState m_modelState() const {
		return mem.read<CModelState>(this_cast + c_schema_dumper::schema_offset["CSkeletonInstance"]["m_modelState"]);
	}
};

class C_BaseEntity {
public:
	inline CGameSceneNode* m_pGameSceneNode() const {
		return mem.read<CGameSceneNode*>(this_cast + c_schema_dumper::schema_offset["C_BaseEntity"]["m_pGameSceneNode"]);
	}

	inline CSkeletonInstance* m_pGameSceneNodeParent() const {
		return (CSkeletonInstance*) this->m_pGameSceneNode();
	}

	inline int m_iMaxHealth() const {
		return mem.read<int>(this_cast + c_schema_dumper::schema_offset["C_BaseEntity"]["m_iMaxHealth"]);
	}

	inline int m_iHealth() const {
		return mem.read<int>(this_cast + c_schema_dumper::schema_offset["C_BaseEntity"]["m_iHealth"]);
	}

	template<typename T>
	inline T* m_nSubclass() const {
		return mem.read<T*>(this_cast + c_schema_dumper::schema_offset["C_BaseEntity"]["m_nSubclassID"] + 8); // 2nd class
	}

	inline team_t m_iTeamNum() const {
		return mem.read<team_t>(this_cast + c_schema_dumper::schema_offset["C_BaseEntity"]["m_iTeamNum"]);
	}

	inline uint32_t m_fFlags() const {
		return mem.read<uint32_t>(this_cast + c_schema_dumper::schema_offset["C_BaseEntity"]["m_fFlags"]);
	}

	inline vector3_t m_vecAbsVelocity() const {
		return mem.read<vector3_t>(this_cast + c_schema_dumper::schema_offset["C_BaseEntity"]["m_vecAbsVelocity"]);
	}
};

class C_BaseModelEntity : public C_BaseEntity {
public:
	inline vector3_t m_vecViewOffset() const {
		return mem.read<vector3_t>(this_cast + c_schema_dumper::schema_offset["C_BaseModelEntity"]["m_vecViewOffset"]);
	}
};

class CBaseAnimGraph : public C_BaseModelEntity {
public:

};

class C_BaseCombatCharacter : public CBaseAnimGraph {
public:

};

class C_EconEntity : public CBaseAnimGraph {
public:
	
};