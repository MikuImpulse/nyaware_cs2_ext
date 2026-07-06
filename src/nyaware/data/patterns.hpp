#pragma once

namespace signatures {
    // schemasystem.dll
    inline const char* dwSchemaSystemInterface = "48 89 05 ? ? ? ? 4C 8D 0D ? ? ? ? 33 C0";

    // client.dll
    inline const char* dwEntityList = "48 8B 05 ? ? ? ? 4C 8D 2D ? ? ? ? 48 89 BC 24";
    inline const char* dwGameRules = "48 8B 05 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? ? ? ? 48 8B CE 48 8B 98";
    inline const char* dwGlobalVars = "48 8B 05 ? ? ? ? ? ? ? ? F3 0F 5C C8";
    inline const char* dwLocalPlayerController = "48 8D 0D ? ? ? ? 33 F6 ? ? ? ? 41 C6 86";
    inline const char* dwPlantedC4 = "48 8B 0D ? ? ? ? 49 C1 E1 ? F7 05 ? ? ? ? ? ? ? ? 4C 63 C3 0F 94 C2 49 C1 E0 ? FF 15 ? ? ? ? 48 8B 74 24";
    inline const char* dwViewMatrix = "48 8D 0D ? ? ? ? 48 89 44 24 ? 48 89 4C 24 ? 4C 8D 0D";
    inline const char* dwKeyButtons = "48 8B 15 ? ? ? ? 48 85 D2 74 ? 48 8B 02 48 85 C0";
    inline const char* dwDrawCrosshair = "83 38 ?? 74 ?? b0 ?? 48 8b 5c 24";
}