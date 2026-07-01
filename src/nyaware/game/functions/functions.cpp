#include "functions.hpp"

#include "data/globals.hpp"
#include "data/config.hpp"
#include "utils/memory.hpp"

void c_functions::bunny_hop(const player_t& local_player, uintptr_t jump_button) {
    if (!cfg.misc.bunny_hop) return;

    bool on_ground = local_player.pawn->m_fFlags() & (int)flags_t::onground;
    bool space_pressed = GetAsyncKeyState(VK_SPACE);

    int current_state = mem.read<int>(jump_button);

    if (space_pressed && on_ground) {
        static float sleep_timer = 0.f;
        sleep_timer += ImGui::GetIO().DeltaTime;

        if (sleep_timer > 0.005f) {
            mem.write<int>(jump_button, 65537);
            sleep_timer = 0.f;
        }
    }
    else if (space_pressed && !on_ground) {
        mem.write<int>(jump_button, 256);
    }
    else if (!space_pressed && current_state == 65537) {
        mem.write<int>(jump_button, 256);
    }
}