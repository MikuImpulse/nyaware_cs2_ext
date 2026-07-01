#pragma once

#include "data/sdk/sdk.hpp"

class c_functions {
private:
	uintptr_t keybtns{};
public:
	void bunny_hop(const player_t& local_player, uintptr_t jump_button);
};