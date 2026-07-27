#pragma once

namespace signatures {
    // schemasystem.dll
    inline const char* dwSchemaSystemInterface = "48 89 05 ? ? ? ? 4C 8D 0D ? ? ? ? 33 C0";

    // client.dll
    inline const char* dwEntityList = "48 89 0D ? ? ? ? E9 ? ? ? ? CC";
    inline const char* dwGameRules = "4C 8B 05 ? ? ? ? 4C 8D 7F";
    inline const char* dwGlobalVars = "48 89 15 ? ? ? ? 48 89 42";
    inline const char* dwPlantedC4 = "48 8B 1D ? ? ? ? 45 32 F6";
    inline const char* dwViewMatrix = "48 8D 0D ? ? ? ? 48 C1 E0 06";
    inline const char* dwKeyButtons = "48 8B 15 ? ? ? ? 48 85 D2 74 ? 48 8B 02 48 85 C0";
    inline const char* dwDrawCrosshair = "83 38 ?? 74 ?? B0 ?? 48 8B 5C 24";
}