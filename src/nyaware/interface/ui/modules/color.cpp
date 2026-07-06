#include "color.hpp"

#include <cmath>
#include <algorithm>

void c_color_helper::RGBtoHSV(float r, float g, float b, float& h, float& s, float& v) {
    float cmax = std::max({ r, g, b });
    float cmin = std::min({ r, g, b });
    float delta = cmax - cmin;

    if (delta == 0.0f)
        h = 0.0f;
    else if (cmax == r)
        h = 60.0f * std::fmodf((g - b) / delta, 6.0f);
    else if (cmax == g)
        h = 60.0f * ((b - r) / delta + 2.0f);
    else
        h = 60.0f * ((r - g) / delta + 4.0f);

    if (h < 0.0f)
        h += 360.0f;

    s = (cmax == 0.0f) ? 0.0f : (delta / cmax);
    v = cmax;
}

void c_color_helper::HSVtoRGB(float h, float s, float v, float& r, float& g, float& b) {
    if (s == 0.0f) {
        r = g = b = v;
        return;
    }

    h = std::fmodf(h, 360.0f);
    if (h < 0.0f) h += 360.0f;

    h /= 60.0f;
    int i = static_cast<int>(std::floor(h)) % 6;
    float f = h - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));

    switch (i) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
    }
}

ImColor c_color_helper::make_darkened(ImColor color, float darkness_factor) {
    float r = color.Value.x;
    float g = color.Value.y;
    float b = color.Value.z;

    float h, s, v;
    RGBtoHSV(r, g, b, h, s, v);

    v = std::clamp(v * darkness_factor, 0.0f, 1.0f);

    HSVtoRGB(h, s, v, r, g, b);

    return ImColor(r, g, b, color.Value.w);
}