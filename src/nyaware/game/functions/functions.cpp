#include "functions.hpp"

#include "data/globals.hpp"
#include "data/config.hpp"
#include "utils/memory.hpp"

void c_functions::force_crosshair() {
    dll_t& client_dll = g.modules.client;
    static int crosshair_patch = -1;

    if (cfg.visuals.force_crosshair && crosshair_patch == -1) {
        crosshair_patch = mem.patch(mem.find_pattern(client_dll.base, client_dll.size, "83 38 ?? 74 ?? b0 ?? 48 8b 5c 24"),
            std::vector<uint8_t>(3, 0x90), client_dll.base);
    }
    else if (!cfg.visuals.force_crosshair && crosshair_patch != -1) {
        if (mem.restore(crosshair_patch))
            crosshair_patch = -1;
    }
}

void c_functions::bunny_hop(const player_t& local_player, uintptr_t jump_button) {
    if (!cfg.misc.bunny_hop.enable) return;

    bool on_ground = local_player.pawn->m_fFlags() & (int)flags_t::onground;
    bool bind_pressed = GetAsyncKeyState(cfg.misc.bunny_hop.bind);

    int current_state = mem.read<int>(jump_button);

    if (bind_pressed && on_ground) {
        static float sleep_timer = 0.f;
        sleep_timer += ImGui::GetIO().DeltaTime;

        if (sleep_timer > 0.005f) {
            mem.write<int>(jump_button, 65537);
            sleep_timer = 0.f;
        }
    }
    else if (bind_pressed && !on_ground) {
        mem.write<int>(jump_button, 256);
    }
    else if (!bind_pressed && current_state == 65537) {
        mem.write<int>(jump_button, 256);
    }
}