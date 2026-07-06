#pragma once

#include "imgui/imgui.h"

class c_color_helper {
public:
    void RGBtoHSV(float r, float g, float b, float& h, float& s, float& v);
    void HSVtoRGB(float h, float s, float v, float& r, float& g, float& b);

    ImColor make_darkened(ImColor color, float darkness_factor);
};