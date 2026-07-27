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

struct CGameSceneNode {
	inline vector3_t m_vecAbsOrigin() const {
		return mem.read<vector3_t>(this_cast + c_schema_dumper::schema_offset["CGameSceneNode"]["m_vecAbsOrigin"]);
	}
};

struct CSkeletonInstance : CGameSceneNode {
	inline boneArray* m_pBoneList() const {
		return mem.read<boneArray*>(this_cast + c_schema_dumper::schema_offset["CSkeletonInstance"]["m_modelState"] + 0x80);
	}
};

struct C_BaseEntity {
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

	inline uint8_t m_iTeamNum() const {
		return mem.read<uint8_t>(this_cast + c_schema_dumper::schema_offset["C_BaseEntity"]["m_iTeamNum"]);
	}

	inline uint32_t m_fFlags() const {
		return mem.read<uint32_t>(this_cast + c_schema_dumper::schema_offset["C_BaseEntity"]["m_fFlags"]);
	}

	inline vector3_t m_vecAbsVelocity() const {
		return mem.read<vector3_t>(this_cast + c_schema_dumper::schema_offset["C_BaseEntity"]["m_vecAbsVelocity"]);
	}
};

struct C_BaseModelEntity : C_BaseEntity {
	inline vector3_t m_vecViewOffset() const {
		return mem.read<vector3_t>(this_cast + c_schema_dumper::schema_offset["C_BaseModelEntity"]["m_vecViewOffset"]);
	}
};

struct CBaseAnimGraph : C_BaseModelEntity {

};

struct C_BaseCombatCharacter : CBaseAnimGraph {

};

struct C_EconEntity : CBaseAnimGraph {
	
};