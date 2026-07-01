#pragma once

#include "imgui/imgui.h"

#include "utils/memory.hpp"
#include "sdk/schema_dumper.hpp"

#include "interface/interface.hpp"

#include "core/core.hpp"
#include "game/runtime.hpp"

struct globals_t {
	bool initialized{};

	struct {
		dll_t client{};
		dll_t schemasystem{};

		bool isValid() { return client.isValid() && schemasystem.isValid(); }
	} modules;

	c_schema_dumper schema_dumper{};

	c_interface uinterface{};
	screen_t screen{};

	struct {
		ImFont* jacobs{};
		ImFont* weapon{};
	} fonts;

	c_cheat_core core{};
	c_runtime_manager runtime{};
} inline g;