#pragma once

#include <unordered_map>
#include <string>

#include "c_base_player_weapon.hpp"

#include "data/sdk/schema_dumper.hpp"
#include "data/sdk/source2/structs/utl_vector.hpp"

#include "utils/memory.hpp"

class CPlayer_WeaponServices {
public:
	inline uintptr_t m_hActiveWeapon() const {
		return mem.read<uintptr_t>(this_cast + c_schema_dumper::schema_offset["CPlayer_WeaponServices"]["m_hActiveWeapon"]);
	}
};

class CCSPlayer_AimPunchServices {
public:
	inline vector3_t m_aimPunchCache() const {
		return mem.read<c_utl_vector>(this_cast + 0x88).to_vector3(); // no in schema
	}
};

enum class CSWeaponType : uint32_t {
    WEAPONTYPE_KNIFE = 0,
    WEAPONTYPE_PISTOL = 1,
    WEAPONTYPE_SUBMACHINEGUN = 2,
    WEAPONTYPE_RIFLE = 3,
    WEAPONTYPE_SHOTGUN = 4,
    WEAPONTYPE_SNIPER_RIFLE = 5,
    WEAPONTYPE_MACHINEGUN = 6,
    WEAPONTYPE_C4 = 7,
    WEAPONTYPE_TASER = 8,
    WEAPONTYPE_GRENADE = 9,
    WEAPONTYPE_EQUIPMENT = 10,
    WEAPONTYPE_STACKABLEITEM = 11,
    WEAPONTYPE_UNKNOWN = 12
};

inline std::unordered_map<std::string, std::pair<std::string, std::string>> weaponDefinitions = {
	{"weapon_ak47",						{"AK47",			"A"}},
	{"weapon_aug",						{"AUG",				"B"}},
	{"weapon_awp",						{"AWP",				"C"}},
	{"weapon_bizon",					{"PP-Bizon",		"D"}},
	{"weapon_c4",						{"C4",				"E"}},
	{"weapon_cz75a",					{"CZ75-Auto",		"F"}},
	{"weapon_deagle",					{"Desert Eagle",	"G"}},
	{"weapon_decoy",					{"Decoy",			"H"}},
	{"weapon_elite",					{"Dual Berettas",	"I"}},
	{"weapon_famas",					{"FAMAS",			"J"}},
	{"weapon_fiveseven",				{"Five-SeveN",		"K"}},
	{"weapon_flashbang",				{"Flashbang",		"L"}},
	{"weapon_g3sg1",					{"G3SG1",			"M"}},
	{"weapon_galilar",					{"Galil",			"N"}},
	{"weapon_glock",					{"Glock",			"O"}},
	{"weapon_healthshot",				{"Medi-Shot",		"P"}},
	{"weapon_hegrenade",				{"HE",				"Q"}},
	{"weapon_hkp2000",					{"P2000",			"R"}},
	{"weapon_incgrenade",				{"Incendiary",		"S"}},
	{"weapon_knife",					{"Knife",			"T"}},
	{"weapon_knife_t",					{"Knife",			"U"}},
	{"weapon_knife_bayonet",			{"Bayonet",			"V"}},
	{"weapon_knife_css",				{"Classic knife",	"W"}},
	{"weapon_knife_flip",				{"Flip knife",		"X"}},
	{"weapon_knife_gut",				{"Gut knife",		"Y"}},
	{"weapon_knife_karambit",			{"Karambit",		"Z"}},
	{"weapon_knife_m9_bayonet",			{"M9 Bayonet",		"a"}},
	{"weapon_knife_tactical",			{"Huntsman knife",	"b"}},
	{"weapon_knife_falchion",			{"Falchion knife",	"c"}},
	{"weapon_knife_survival_bowie",		{"Bowie knife",		"d"}},
	{"weapon_knife_butterfly",			{"Butterfly knife", "e"}},
	{"weapon_knife_push",				{"Shadow daggers",	"f"}},
	{"weapon_knife_cord",				{"Paracord knife",	"g"}},
	{"weapon_knife_canis",				{"Survival knife",	"h"}},
	{"weapon_knife_ursus",				{"Ursus knife",		"j"}},
	{"weapon_knife_gypsy_jackknife",	{"Navaja",			"k"}},
	{"weapon_knife_outdoor",			{"Nomad knife",		"l"}},
	{"weapon_knife_stiletto",			{"Stiletto",		"m"}},
	{"weapon_knife_widowmaker",			{"Talon knife",		"n"}},
	{"weapon_knife_skeleton",			{"Skeleton knife",	"o"}},
	{"weapon_m249",						{"M249",			"p"}},
	{"weapon_m4a1",						{"M4A4",			"q"}},
	{"weapon_m4a1_silencer",			{"M4A1-S",			"r"}},
	{"weapon_mac10",					{"MAC-10",			"s"}},
	{"weapon_mag7",						{"MAG-7",			"t"}},
	{"weapon_molotov",					{"Molotov",			"u"}},
	{"weapon_mp5sd",					{"MP5-SD",			"v"}},
	{"weapon_mp7",						{"MP7",				"w"}},
	{"weapon_mp9",						{"MP9",				"x"}},
	{"weapon_negev",					{"Negev",			"y"}},
	{"weapon_nova",						{"Nova",			"z"}},
	{"weapon_p250",						{"P250",			"1"}},
	{"weapon_p90",						{"P90",				"2"}},
	{"weapon_revolver",					{"R8 Revolver",		"3"}},
	{"weapon_sawedoff",					{"Sawed-Off",		"4"}},
	{"weapon_scar20",					{"SCAR-20",			"5"}},
	{"weapon_sg556",					{"SG 553",			"6"}},
	{"weapon_smokegrenade",				{"Smoke",			"7"}},
	{"weapon_ssg08",					{"SSG 08",			"8"}},
	{"weapon_taser",					{"Zeus x27",		"9"}},
	{"weapon_tec9",						{"Tec-9",			"["}},
	{"weapon_ump45",					{"UMP-45",			"]"}},
	{"weapon_usp_silencer",				{"USP-S",			"`"}},
	{"weapon_xm1014",					{"XM1014",			"~"}}
};

class CCSWeaponBaseVData {
public:
	inline int m_iMaxClip1() const {
		return mem.read<int>(this_cast + c_schema_dumper::schema_offset["CBasePlayerWeaponVData"]["m_iMaxClip1"]);
	}

    inline CSWeaponType m_WeaponType() const {
        return mem.read<CSWeaponType>(this_cast + c_schema_dumper::schema_offset["CCSWeaponBaseVData"]["m_WeaponType"]);
    }

    inline bool m_bIsRevolver() const {
        return mem.read<bool>(this_cast + c_schema_dumper::schema_offset["CCSWeaponBaseVData"]["m_bIsRevolver"]);
    }

    inline std::string m_szName() const {
        return mem.read_str(mem.read<uintptr_t>(this_cast + c_schema_dumper::schema_offset["CCSWeaponBaseVData"]["m_szName"]));
    }

	inline float m_flMaxSpeed() const {
		return mem.read<float>(this_cast + c_schema_dumper::schema_offset["CCSWeaponBaseVData"]["m_flMaxSpeed"]);
	}

	inline float m_flSpread() const {
		return mem.read<float>(this_cast + c_schema_dumper::schema_offset["CCSWeaponBaseVData"]["m_flSpread"]);
	}
};