#include "modules.hpp"

#include <sstream>
#include <iomanip>
#include <windows.h>

const char* c_ui_modules::getDisplayLabel(const char* label) {
    if (!label)
        return "";

    const char* hash = strstr(label, "##");
    if (!hash)
        return label;

    static char buffer[256];
    size_t len = hash - label;
    if (len >= sizeof(buffer))
        len = sizeof(buffer) - 1;

    memcpy(buffer, label, len);
    buffer[len] = '\0';

    return buffer;
}

ImGuiID c_ui_modules::makeWidgetId(const char* label, const void* ptr) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    ImGuiID id = window->GetID(label ? label : "##widget");

    if (ptr)
        id = ImHashData(&ptr, sizeof(ptr), id);

    ImVec2 local_pos(window->DC.CursorPos.x - window->Pos.x, window->DC.CursorPos.y - window->Pos.y);

    id = ImHashData(&local_pos, sizeof(local_pos), id);
    return id;
}

ImGuiID c_ui_modules::makeRectWidgetId(const char* label, const void* ptr, const ImRect& bb) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    ImGuiID id = window->GetID(label ? label : "##widget");

    if (ptr)
        id = ImHashData(&ptr, sizeof(ptr), id);

    ImVec2 local_pos(bb.Min.x - window->Pos.x, bb.Min.y - window->Pos.y);

    id = ImHashData(&local_pos, sizeof(local_pos), id);
    return id;
}

ImGuiID c_ui_modules::makeSubId(ImGuiID id, ImGuiID seed) {
    return ImHashData(&seed, sizeof(seed), id);
}

bool c_ui_modules::uiCanUseButtonBehavior(ImGuiID id) {
    ImGuiContext& g = *GImGui;

    return g.ActiveId == 0 || g.ActiveId == id || g.ActiveIdPreviousFrame == id || g.DragDropActive;
}

bool c_ui_modules::uiHasForeignActiveId(ImGuiID id) {
    ImGuiContext& g = *GImGui;

    return g.ActiveId != 0 && g.ActiveId != id && g.ActiveIdPreviousFrame != id && !g.DragDropActive;
}

void c_ui_modules::markItemEditedSafe(ImGuiID id) {
    ImGuiContext& g = *GImGui;

    if (g.ActiveId == id || g.ActiveId == 0 || g.ActiveIdPreviousFrame == id || g.DragDropActive)
        ImGui::MarkItemEdited(id);
}

ImVec4 c_ui_modules::textColor(float hover_anim, float active_anim) {
    ImGuiStyle& style = ImGui::GetStyle();

    ImVec4 base = style.Colors[ImGuiCol_TextDisabled];
    ImVec4 hovered = ImLerp(style.Colors[ImGuiCol_TextDisabled], style.Colors[ImGuiCol_Text], 0.55f);
    ImVec4 active = style.Colors[ImGuiCol_Text];

    ImVec4 result = ImLerp(base, hovered, hover_anim);

    result = ImLerp(result, active, active_anim);
    return result;
}

std::string c_ui_modules::clippedPreview(const std::string& text, float max_width) {
    if (ImGui::CalcTextSize(text.c_str()).x <= max_width)
        return text;

    std::string result = text;

    while (!result.empty()) {
        std::string clipped = result;

        while (!clipped.empty() && clipped[clipped.size() - 1] == ' ')
            clipped.pop_back();

        if (!clipped.empty() && clipped[clipped.size() - 1] == ',') {
            clipped += " ...";
        }
        else if (clipped.size() >= 2 && clipped[clipped.size() - 2] == ',' && clipped[clipped.size() - 1] != ' ') {
            clipped.insert(clipped.size() - 1, " ");
            clipped += "...";
        }
        else {
            clipped += "...";
        }

        if (ImGui::CalcTextSize(clipped.c_str()).x <= max_width)
            return clipped;

        result.pop_back();
    }

    return "...";
}

float c_ui_modules::animFloat(ImGuiID id, float target, float speed) {
    ImGuiStorage* storage = ImGui::GetStateStorage();

    float current = storage->GetFloat(id, target);
    current += (target - current) * ImClamp(ImGui::GetIO().DeltaTime * speed, 0.f, 1.f);

    storage->SetFloat(id, current);
    return current;
}

float c_ui_modules::animFloatLinear(ImGuiID id, float target, float speed) {
    ImGuiStorage* storage = ImGui::GetStateStorage();

    float current = storage->GetFloat(id, target);
    float delta = target - current;
    float step = speed * ImGui::GetIO().DeltaTime;

    if (delta > step)
        current += step;
    else if (delta < -step)
        current -= step;
    else
        current = target;

    storage->SetFloat(id, current);
    return current;
}

bool c_ui_modules::colorEditPreview(const char* label, ImColor* color, const ImRect& bb) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems || !color)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;
    ImDrawList* draw = window->DrawList;

    ImGuiID id = makeRectWidgetId(label, color, bb);
    ImGuiStorage* storage = ImGui::GetStateStorage();

    bool popup_open = ImGui::IsPopupOpen(id, ImGuiPopupFlags_None);
    bool block_preview_item = g.ActiveId != 0 && g.ActiveId != id && g.ActiveIdPreviousFrame != id && !popup_open && !g.DragDropActive;

    bool hovered = ImGui::IsMouseHoveringRect(bb.Min, bb.Max);
    bool held = false;
    bool pressed = false;

    if (!block_preview_item) {
        if (!ImGui::ItemAdd(bb, id) && !popup_open)
            return false;

        if (g.ActiveId == 0 || g.ActiveId == id || g.ActiveIdPreviousFrame == id)
            pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
    }

    if (pressed) {
        ImGui::SetActiveID(id, window);
        ImGui::SetFocusID(id, window);
        ImGui::FocusWindow(window);

        ImGui::OpenPopupEx(id, ImGuiPopupFlags_None);

        popup_open = true;
    }

    if (g.ActiveId == id && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
        ImGui::ClearActiveID();

    float hover_anim = animFloat(makeSubId(id, 1), hovered ? 1.f : 0.f, 12.f);
    float active_anim = animFloat(makeSubId(id, 2), popup_open ? 1.f : 0.f, 14.f);

    ImVec4 border_col = ImLerp(style.Colors[ImGuiCol_Border], style.Colors[ImGuiCol_Text], hover_anim * 0.35f);
    border_col = ImLerp(border_col, style.Colors[ImGuiCol_ButtonActive], active_anim);

    draw->AddRectFilled(bb.Min, bb.Max, ImGui::GetColorU32(color->Value), style.FrameRounding);
    draw->AddRect(bb.Min, bb.Max, ImGui::GetColorU32(border_col), style.FrameRounding);

    if (block_preview_item)
        return false;

    bool changed = false;

    ImGui::SetNextWindowPos(ImVec2(bb.Min.x, bb.Max.y + 6.f));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6.f, 6.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, style.PopupRounding);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, style.FrameRounding);
    ImGui::PushStyleColor(ImGuiCol_PopupBg, style.Colors[ImGuiCol_PopupBg]);
    ImGui::PushStyleColor(ImGuiCol_Border, style.Colors[ImGuiCol_Border]);

    if (ImGui::BeginPopupEx(id, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar)) {
        ImGuiWindow* popup_window = ImGui::GetCurrentWindow();
        ImDrawList* popup_draw = popup_window->DrawList;

        ImVec4 color_value = color->Value;

        ImGuiID hsv_init_id = makeSubId(id, 100);
        ImGuiID h_id = makeSubId(id, 101);
        ImGuiID s_id = makeSubId(id, 102);
        ImGuiID v_id = makeSubId(id, 103);
        ImGuiID r_id = makeSubId(id, 104);
        ImGuiID g_id = makeSubId(id, 105);
        ImGuiID b_id = makeSubId(id, 106);
        ImGuiID drag_mode_id = makeSubId(id, 201);

        ImGuiID sv_cursor_s_id = makeSubId(id, 301);
        ImGuiID sv_cursor_v_id = makeSubId(id, 302);
        ImGuiID hue_cursor_id = makeSubId(id, 303);

        ImGuiID sv_picker_id = makeSubId(id, 401);
        ImGuiID hue_picker_id = makeSubId(id, 402);

        float converted_h = 0.f;
        float converted_s = 0.f;
        float converted_v = 0.f;

        ImGui::ColorConvertRGBtoHSV(color_value.x, color_value.y, color_value.z, converted_h, converted_s, converted_v);

        bool hsv_initialized = storage->GetBool(hsv_init_id, false);

        float h = storage->GetFloat(h_id, converted_h);
        float s = storage->GetFloat(s_id, converted_s);
        float v = storage->GetFloat(v_id, converted_v);

        if (!hsv_initialized) {
            h = converted_h;
            s = converted_s;
            v = converted_v;

            storage->SetBool(hsv_init_id, true);
        }

        float external_r = storage->GetFloat(r_id, color_value.x);
        float external_g = storage->GetFloat(g_id, color_value.y);
        float external_b = storage->GetFloat(b_id, color_value.z);

        bool external_changed =
            fabsf(external_r - color_value.x) > 0.001f ||
            fabsf(external_g - color_value.y) > 0.001f ||
            fabsf(external_b - color_value.z) > 0.001f;

        if (external_changed && !ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            h = converted_h;
            s = converted_s;
            v = converted_v;
        }

        float sv_size = 126.f;
        float hue_width = 10.f;
        float hue_gap = 6.f;
        float button_height = 22.f;
        float full_width = sv_size + hue_gap + hue_width;
        float button_width = (full_width - style.ItemSpacing.x) * 0.5f;

        ImVec2 picker_pos = ImGui::GetCursorScreenPos();

        ImRect sv_bb(picker_pos, ImVec2(picker_pos.x + sv_size, picker_pos.y + sv_size));
        ImRect hue_bb(ImVec2(sv_bb.Max.x + hue_gap, sv_bb.Min.y), ImVec2(sv_bb.Max.x + hue_gap + hue_width, sv_bb.Max.y));

        ImGui::Dummy(ImVec2(full_width, sv_size));

        float hue_r = 1.f;
        float hue_g = 1.f;
        float hue_b = 1.f;

        ImGui::ColorConvertHSVtoRGB(h, 1.f, 1.f, hue_r, hue_g, hue_b);

        popup_draw->AddRectFilledMultiColor(sv_bb.Min, sv_bb.Max, ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f)),
            ImGui::GetColorU32(ImVec4(hue_r, hue_g, hue_b, 1.f)), ImGui::GetColorU32(ImVec4(hue_r, hue_g, hue_b, 1.f)),
            ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f)));

        popup_draw->AddRectFilledMultiColor(sv_bb.Min, sv_bb.Max, ImGui::GetColorU32(ImVec4(0.f, 0.f, 0.f, 0.f)),
            ImGui::GetColorU32(ImVec4(0.f, 0.f, 0.f, 0.f)), ImGui::GetColorU32(ImVec4(0.f, 0.f, 0.f, 1.f)),
            ImGui::GetColorU32(ImVec4(0.f, 0.f, 0.f, 1.f)));

        popup_draw->AddRect(sv_bb.Min, sv_bb.Max, ImGui::GetColorU32(style.Colors[ImGuiCol_Border]), 0.f);

        bool sv_hovered = false;
        bool sv_held = false;
        bool sv_pressed = false;

        if (ImGui::ItemAdd(sv_bb, sv_picker_id))
            sv_pressed = ImGui::ButtonBehavior(sv_bb, sv_picker_id, &sv_hovered, &sv_held, ImGuiButtonFlags_PressedOnClick);

        const int hue_segments = 6;

        for (int i = 0; i < hue_segments; ++i) {
            float h0 = static_cast<float>(i) / static_cast<float>(hue_segments);
            float h1 = static_cast<float>(i + 1) / static_cast<float>(hue_segments);

            float r0 = 0.f;
            float g0 = 0.f;
            float b0 = 0.f;

            float r1 = 0.f;
            float g1 = 0.f;
            float b1 = 0.f;

            ImGui::ColorConvertHSVtoRGB(h0, 1.f, 1.f, r0, g0, b0);
            ImGui::ColorConvertHSVtoRGB(h1, 1.f, 1.f, r1, g1, b1);

            float y0 = hue_bb.Min.y + hue_bb.GetHeight() * h0;
            float y1 = hue_bb.Min.y + hue_bb.GetHeight() * h1;

            popup_draw->AddRectFilledMultiColor(ImVec2(hue_bb.Min.x, y0), ImVec2(hue_bb.Max.x, y1),
                ImGui::GetColorU32(ImVec4(r0, g0, b0, 1.f)), ImGui::GetColorU32(ImVec4(r0, g0, b0, 1.f)),
                ImGui::GetColorU32(ImVec4(r1, g1, b1, 1.f)), ImGui::GetColorU32(ImVec4(r1, g1, b1, 1.f)));
        }

        popup_draw->AddRect(hue_bb.Min, hue_bb.Max, ImGui::GetColorU32(style.Colors[ImGuiCol_Border]), 0.f);

        bool hue_hovered = false;
        bool hue_held = false;
        bool hue_pressed = false;

        if (ImGui::ItemAdd(hue_bb, hue_picker_id))
            hue_pressed = ImGui::ButtonBehavior(hue_bb, hue_picker_id, &hue_hovered, &hue_held, ImGuiButtonFlags_PressedOnClick);

        int drag_mode = storage->GetInt(drag_mode_id, 0);

        if (sv_pressed)
            drag_mode = 1;
        else if (hue_pressed)
            drag_mode = 2;

        storage->SetInt(drag_mode_id, drag_mode);

        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            drag_mode = 0;
            storage->SetInt(drag_mode_id, drag_mode);
        }

        if (drag_mode == 1 && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            s = ImClamp((g.IO.MousePos.x - sv_bb.Min.x) / sv_bb.GetWidth(), 0.f, 1.f);
            v = 1.f - ImClamp((g.IO.MousePos.y - sv_bb.Min.y) / sv_bb.GetHeight(), 0.f, 1.f);

            ImGui::ColorConvertHSVtoRGB(h, s, v, color_value.x, color_value.y, color_value.z);
            color->Value = color_value;

            changed = true;
        }

        if (drag_mode == 2 && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            h = ImClamp((g.IO.MousePos.y - hue_bb.Min.y) / hue_bb.GetHeight(), 0.f, 1.f);

            ImGui::ColorConvertHSVtoRGB(h, s, v, color_value.x, color_value.y, color_value.z);
            color->Value = color_value;

            changed = true;
        }

        float animated_s = animFloat(sv_cursor_s_id, s, 24.f);
        float animated_v = animFloat(sv_cursor_v_id, v, 24.f);
        float animated_h = animFloat(hue_cursor_id, h, 24.f);

        ImVec2 sv_cursor(sv_bb.Min.x + animated_s * sv_bb.GetWidth(), sv_bb.Min.y + (1.f - animated_v) * sv_bb.GetHeight());

        popup_draw->AddCircle(sv_cursor, 4.f, IM_COL32(0, 0, 0, 190), 24, 2.f);
        popup_draw->AddCircle(sv_cursor, 3.f, IM_COL32(255, 255, 255, 230), 24, 1.f);

        float hue_cursor_y = hue_bb.Min.y + animated_h * hue_bb.GetHeight();

        popup_draw->AddRectFilled(ImVec2(hue_bb.Min.x - 2.f, hue_cursor_y - 2.f), ImVec2(hue_bb.Max.x + 2.f, hue_cursor_y + 2.f),
            ImGui::GetColorU32(style.Colors[ImGuiCol_Text]), 2.f);

        ImGui::SetCursorScreenPos(ImVec2(sv_bb.Min.x, sv_bb.Max.y + 8.f));

        if (this->button("Copy", ImVec2(button_width, button_height))) {
            char buffer[128];

            ImFormatString(buffer, IM_ARRAYSIZE(buffer), "%.3f %.3f %.3f %.3f",
                color->Value.x, color->Value.y, color->Value.z, color->Value.w);

            ImGui::SetClipboardText(buffer);
        }

        ImGui::SameLine();

        if (this->button("Paste", ImVec2(button_width, button_height))) {
            const char* clipboard = ImGui::GetClipboardText();

            if (clipboard) {
                float r = 0.f;
                float g_col = 0.f;
                float b = 0.f;
                float a = 1.f;

                if (sscanf_s(clipboard, "%f %f %f %f", &r, &g_col, &b, &a) >= 3) {
                    color->Value.x = ImClamp(r, 0.f, 1.f);
                    color->Value.y = ImClamp(g_col, 0.f, 1.f);
                    color->Value.z = ImClamp(b, 0.f, 1.f);
                    color->Value.w = ImClamp(a, 0.f, 1.f);

                    ImGui::ColorConvertRGBtoHSV(color->Value.x, color->Value.y, color->Value.z, h, s, v);
                    changed = true;
                }
            }
        }

        storage->SetFloat(h_id, h);
        storage->SetFloat(s_id, s);
        storage->SetFloat(v_id, v);
        storage->SetFloat(r_id, color->Value.x);
        storage->SetFloat(g_id, color->Value.y);
        storage->SetFloat(b_id, color->Value.z);

        ImGui::EndPopup();
    }

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(4);

    return changed;
}

const char* c_ui_modules::keyName(int key) {
    static char buffer[64];

    if (key <= 0)
        return "None";

    switch (key) {
        case VK_LBUTTON: return "Mouse 1";
        case VK_RBUTTON: return "Mouse 2";
        case VK_MBUTTON: return "Mouse 3";
        case VK_XBUTTON1: return "Mouse 4";
        case VK_XBUTTON2: return "Mouse 5";
        case VK_BACK: return "Backspace";
        case VK_TAB: return "Tab";
        case VK_RETURN: return "Enter";
        case VK_SHIFT: return "Shift";
        case VK_CONTROL: return "Ctrl";
        case VK_MENU: return "Alt";
        case VK_PAUSE: return "Pause";
        case VK_CAPITAL: return "Caps Lock";
        case VK_ESCAPE: return "Escape";
        case VK_SPACE: return "Space";
        case VK_PRIOR: return "Page Up";
        case VK_NEXT: return "Page Down";
        case VK_END: return "End";
        case VK_HOME: return "Home";
        case VK_LEFT: return "Left";
        case VK_UP: return "Up";
        case VK_RIGHT: return "Right";
        case VK_DOWN: return "Down";
        case VK_INSERT: return "Insert";
        case VK_DELETE: return "Delete";
        case VK_LWIN: return "Left Win";
        case VK_RWIN: return "Right Win";
        case VK_NUMLOCK: return "Num Lock";
        case VK_SCROLL: return "Scroll Lock";
        case VK_LSHIFT: return "Left Shift";
        case VK_RSHIFT: return "Right Shift";
        case VK_LCONTROL: return "Left Ctrl";
        case VK_RCONTROL: return "Right Ctrl";
        case VK_LMENU: return "Left Alt";
        case VK_RMENU: return "Right Alt";
    }

    if (key >= VK_F1 && key <= VK_F24) {
        ImFormatString(buffer, IM_ARRAYSIZE(buffer), "F%d", key - VK_F1 + 1);
        return buffer;
    }

    UINT scan_code = MapVirtualKeyA(static_cast<UINT>(key), MAPVK_VK_TO_VSC);

    if (key == VK_LEFT || key == VK_UP || key == VK_RIGHT || key == VK_DOWN ||
        key == VK_PRIOR || key == VK_NEXT || key == VK_END || key == VK_HOME ||
        key == VK_INSERT || key == VK_DELETE || key == VK_DIVIDE || key == VK_NUMLOCK)
        scan_code |= 0x100;

    LONG lparam = static_cast<LONG>(scan_code << 16);

    if (GetKeyNameTextA(lparam, buffer, IM_ARRAYSIZE(buffer)) > 0)
        return buffer;

    ImFormatString(buffer, IM_ARRAYSIZE(buffer), "VK %d", key);
    return buffer;
}

bool c_ui_modules::button(const char* label, const ImVec2& size_arg) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;

    ImGuiID id = makeWidgetId(label);

    ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

    ImVec2 size = size_arg;
    if (size.x <= 0.f)
        size.x = label_size.x + style.FramePadding.x * 2.f;
    if (size.y <= 0.f)
        size.y = label_size.y + style.FramePadding.y * 2.f;

    ImVec2 pos = window->DC.CursorPos;
    ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));

    ImGui::ItemSize(bb, style.FramePadding.y);

    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool hovered = false;
    bool held = false;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

    float hover_anim = animFloat(makeSubId(id, 1), hovered ? 1.f : 0.f, 14.f);
    float active_anim = animFloat(makeSubId(id, 2), held ? 1.f : 0.f, 14.f);

    ImDrawList* draw = window->DrawList;

    ImVec4 col = style.Colors[ImGuiCol_Button];
    col = ImLerp(col, style.Colors[ImGuiCol_ButtonHovered], hover_anim);
    col = ImLerp(col, style.Colors[ImGuiCol_ButtonActive], active_anim);

    ImVec4 text_col = style.Colors[ImGuiCol_Text];
    text_col = ImLerp(text_col, style.Colors[ImGuiCol_Text], hover_anim);
    text_col = ImLerp(text_col, style.Colors[ImGuiCol_Text], active_anim);

    float rounding = style.FrameRounding;

    draw->AddRectFilled(bb.Min, bb.Max, ImGui::GetColorU32(col), rounding);

    if (style.FrameBorderSize > 0.f) {
        ImVec4 border_col = style.Colors[ImGuiCol_Border];
        border_col = ImLerp(border_col, style.Colors[ImGuiCol_Border], hover_anim);
        border_col = ImLerp(border_col, style.Colors[ImGuiCol_Border], active_anim);

        draw->AddRect(bb.Min, bb.Max, ImGui::GetColorU32(border_col), rounding, 0, style.FrameBorderSize);
    }

    const char* display_label = getDisplayLabel(label);
    ImVec2 text_pos(bb.Min.x + (size.x - label_size.x) * 0.5f, bb.Min.y + (size.y - label_size.y) * 0.5f);

    draw->AddText(text_pos, ImGui::GetColorU32(text_col), display_label);

    if (pressed)
        markItemEditedSafe(id);

    return pressed;
}

bool c_ui_modules::checkbox(const char* label, bool* value) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems || !value)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;

    ImGuiID id = makeWidgetId(label, value);

    float height = 34.f;
    float switch_width = 38.f;
    float switch_height = 19.f;
    float rounding = switch_height * 0.5f;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, height);
    ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));

    ImGui::ItemSize(bb, style.FramePadding.y);

    bool hovered = ImGui::IsMouseHoveringRect(bb.Min, bb.Max);
    bool held = false;
    bool pressed = false;

    if (!uiHasForeignActiveId(id)) {
        if (!ImGui::ItemAdd(bb, id))
            return false;

        if (uiCanUseButtonBehavior(id))
            pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
    }

    if (pressed) {
        *value = !*value;
        markItemEditedSafe(id);
    }

    float active_anim = animFloat(makeSubId(id, 1), *value ? 1.f : 0.f, 16.f);
    float hover_anim = animFloat(makeSubId(id, 2), hovered ? 1.f : 0.f, 12.f);

    ImDrawList* draw = window->DrawList;

    const char* display_label = getDisplayLabel(label);

    ImVec4 text_col_v = textColor(hover_anim, active_anim);

    ImVec4 bg_col_v = ImLerp(style.Colors[ImGuiCol_FrameBg], style.Colors[ImGuiCol_FrameBgHovered], hover_anim);
    bg_col_v = ImLerp(bg_col_v, style.Colors[ImGuiCol_FrameBgActive], active_anim);

    ImVec2 switch_pos(bb.Max.x - switch_width, bb.Min.y + height * 0.5f - switch_height * 0.5f);
    ImRect switch_bb(switch_pos, ImVec2(switch_pos.x + switch_width, switch_pos.y + switch_height));

    draw->AddText(ImVec2(bb.Min.x, bb.Min.y + height * 0.5f - ImGui::GetTextLineHeight() * 0.5f), ImGui::GetColorU32(text_col_v), display_label);
    draw->AddRectFilled(switch_bb.Min, switch_bb.Max, ImGui::GetColorU32(bg_col_v), rounding);

    float knob_radius = 7.f;
    float knob_x = ImLerp(switch_bb.Min.x + switch_height * 0.5f, switch_bb.Max.x - switch_height * 0.5f, active_anim);

    ImVec2 knob_center = ImVec2(knob_x, switch_bb.Min.y + switch_height * 0.5f);

    draw->AddCircleFilled(knob_center, knob_radius, ImGui::GetColorU32(style.Colors[ImGuiCol_CheckMark]), 32);

    return pressed;
}

bool c_ui_modules::checkboxColored(const char* label, bool* value, const std::vector<ImColor*>& colors) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems || !value)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;

    ImGuiID id = makeWidgetId(label, value);
    ImGuiID row_active_id = makeSubId(id, 9000);

    float height = 34.f;
    float switch_width = 38.f;
    float switch_height = 19.f;
    float switch_rounding = switch_height * 0.5f;

    float color_size = 18.f;
    float color_spacing = 6.f;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, height);
    ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));

    ImGui::ItemSize(bb, style.FramePadding.y);

    ImVec2 switch_pos(bb.Max.x - switch_width, bb.Min.y + height * 0.5f - switch_height * 0.5f);
    ImRect switch_bb(switch_pos, ImVec2(switch_pos.x + switch_width, switch_pos.y + switch_height));

    float colors_total_width = 0.f;
    if (!colors.empty())
        colors_total_width = static_cast<float>(colors.size()) * color_size + static_cast<float>(colors.size() - 1) * color_spacing;

    ImVec2 colors_start(switch_bb.Min.x - style.ItemSpacing.x - colors_total_width, bb.Min.y + height * 0.5f - color_size * 0.5f);

    bool color_hovered = false;
    bool color_popup_open = false;

    for (int i = 0; i < static_cast<int>(colors.size()); ++i) {
        if (!colors[i])
            continue;

        ImVec2 c_min(colors_start.x + static_cast<float>(i) * (color_size + color_spacing), colors_start.y);
        ImVec2 c_max(c_min.x + color_size, c_min.y + color_size);

        if (ImGui::IsMouseHoveringRect(c_min, c_max))
            color_hovered = true;

        ImRect color_bb(c_min, c_max);

        ImGuiID color_scope_id = makeSubId(id, static_cast<ImGuiID>(1000 + i));
        color_scope_id = ImHashData(&colors[i], sizeof(colors[i]), color_scope_id);

        ImGui::PushID(color_scope_id);

        char color_label[32];
        ImFormatString(color_label, IM_ARRAYSIZE(color_label), "color_%d", i);

        ImGuiID popup_id = makeRectWidgetId(color_label, colors[i], color_bb);
        if (ImGui::IsPopupOpen(popup_id, ImGuiPopupFlags_None))
            color_popup_open = true;

        ImGui::PopID();
    }

    bool popup_blocking = g.OpenPopupStack.Size > g.BeginPopupStack.Size && !color_popup_open;
    bool can_interact = !popup_blocking && !uiHasForeignActiveId(id);

    if (can_interact) {
        if (!ImGui::ItemAdd(bb, id))
            return false;
    }

    ImRect label_click_bb(bb.Min, ImVec2(switch_bb.Min.x - style.ItemSpacing.x, bb.Max.y));

    if (!colors.empty())
        label_click_bb.Max.x = colors_start.x - style.ItemSpacing.x;

    bool label_hovered = !popup_blocking && ImGui::IsMouseHoveringRect(label_click_bb.Min, label_click_bb.Max);
    bool switch_hovered = !popup_blocking && ImGui::IsMouseHoveringRect(switch_bb.Min, switch_bb.Max);

    bool pressed = false;

    if (!popup_blocking && !color_popup_open && (label_hovered || switch_hovered) && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        ImGui::SetActiveID(row_active_id, window);
        ImGui::SetFocusID(row_active_id, window);
        ImGui::FocusWindow(window);

        pressed = true;
        *value = !*value;
        markItemEditedSafe(id);
    }

    if (g.ActiveId == row_active_id && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
        ImGui::ClearActiveID();

    float active_anim = animFloat(makeSubId(id, 1), (*value || color_popup_open) ? 1.f : 0.f, 16.f);
    float hover_anim = animFloat(makeSubId(id, 2), (label_hovered || switch_hovered || color_hovered) ? 1.f : 0.f, 12.f);
    float checked_anim = animFloat(makeSubId(id, 3), *value ? 1.f : 0.f, 16.f);

    ImDrawList* draw = window->DrawList;

    const char* display_label = getDisplayLabel(label);

    ImVec4 text_col_v = textColor(hover_anim, active_anim);

    ImVec4 bg_col_v = ImLerp(style.Colors[ImGuiCol_FrameBg], style.Colors[ImGuiCol_FrameBgHovered], hover_anim);
    bg_col_v = ImLerp(bg_col_v, style.Colors[ImGuiCol_FrameBgActive], checked_anim);

    draw->AddText(ImVec2(bb.Min.x, bb.Min.y + height * 0.5f - ImGui::GetTextLineHeight() * 0.5f), ImGui::GetColorU32(text_col_v), display_label);
    draw->AddRectFilled(switch_bb.Min, switch_bb.Max, ImGui::GetColorU32(bg_col_v), switch_rounding);

    float knob_radius = 7.f;
    float knob_x = ImLerp(
        switch_bb.Min.x + switch_height * 0.5f,
        switch_bb.Max.x - switch_height * 0.5f,
        checked_anim
    );

    ImVec2 knob_center(knob_x, switch_bb.Min.y + switch_height * 0.5f);

    draw->AddCircleFilled(knob_center, knob_radius, ImGui::GetColorU32(style.Colors[ImGuiCol_CheckMark]), 32);

    bool changed = pressed;

    for (int i = 0; i < static_cast<int>(colors.size()); ++i) {
        if (!colors[i])
            continue;

        ImVec2 color_pos(colors_start.x + static_cast<float>(i) * (color_size + color_spacing), colors_start.y);

        ImVec2 color_min = color_pos;
        ImVec2 color_max = ImVec2(color_pos.x + color_size, color_pos.y + color_size);

        ImRect color_bb(color_min, color_max);

        ImGuiID color_scope_id = makeSubId(id, static_cast<ImGuiID>(1000 + i));
        color_scope_id = ImHashData(&colors[i], sizeof(colors[i]), color_scope_id);

        ImGui::PushID(color_scope_id);

        char color_label[32];
        ImFormatString(color_label, IM_ARRAYSIZE(color_label), "color_%d", i);

        ImGuiID popup_id = makeRectWidgetId(color_label, colors[i], color_bb);
        bool this_color_popup_open = ImGui::IsPopupOpen(popup_id, ImGuiPopupFlags_None);
        bool block_color_preview = g.OpenPopupStack.Size > g.BeginPopupStack.Size && !this_color_popup_open;

        if (block_color_preview) {
            draw->AddRectFilled(color_bb.Min, color_bb.Max, ImGui::GetColorU32(colors[i]->Value), style.FrameRounding);
            draw->AddRect(color_bb.Min, color_bb.Max, ImGui::GetColorU32(style.Colors[ImGuiCol_Border]), style.FrameRounding);
        }
        else if (colorEditPreview(color_label, colors[i], color_bb)) {
            changed = true;
        }

        ImGui::PopID();
    }

    return changed;
}

bool c_ui_modules::checkboxKeyBind(const char* label, bool* value, int* key) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems || !value || !key)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;

    ImGuiID id = makeWidgetId(label, value);
    ImGuiStorage* storage = ImGui::GetStateStorage();

    ImGuiID waiting_id = makeSubId(id, 100);
    ImGuiID row_active_id = makeSubId(id, 101);
    ImGuiID bind_active_id = makeSubId(id, 102);
    ImGuiID capture_ready_id = makeSubId(id, 103);
    ImGuiID capture_frame_id = makeSubId(id, 104);

    float height = 34.f;
    float switch_width = 38.f;
    float switch_height = 19.f;
    float switch_rounding = switch_height * 0.5f;

    float bind_width = 104.f;
    float bind_height = 24.f;
    float bind_gap = 8.f;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, height);
    ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));

    ImGui::ItemSize(bb, style.FramePadding.y);

    bool waiting = storage->GetBool(waiting_id, false);
    bool capture_ready = storage->GetBool(capture_ready_id, true);
    int capture_frame = storage->GetInt(capture_frame_id, -1);
    bool can_interact = !uiHasForeignActiveId(id) || g.ActiveId == row_active_id || g.ActiveId == bind_active_id;

    if (can_interact) {
        if (!ImGui::ItemAdd(bb, id))
            return false;
    }

    ImVec2 switch_pos(bb.Max.x - switch_width, bb.Min.y + height * 0.5f - switch_height * 0.5f);
    ImRect switch_bb(switch_pos, ImVec2(switch_pos.x + switch_width, switch_pos.y + switch_height));

    ImRect bind_bb(ImVec2(switch_bb.Min.x - bind_gap - bind_width, bb.Min.y + height * 0.5f - bind_height * 0.5f), ImVec2(switch_bb.Min.x - bind_gap, bb.Min.y + height * 0.5f + bind_height * 0.5f));
    ImRect label_click_bb(bb.Min, ImVec2(bind_bb.Min.x - style.ItemSpacing.x, bb.Max.y));

    bool label_hovered = can_interact && !waiting && ImGui::IsMouseHoveringRect(label_click_bb.Min, label_click_bb.Max);
    bool switch_hovered = can_interact && !waiting && ImGui::IsMouseHoveringRect(switch_bb.Min, switch_bb.Max);
    bool bind_hovered = can_interact && ImGui::IsMouseHoveringRect(bind_bb.Min, bind_bb.Max);

    bool changed = false;

    if (!waiting && (label_hovered || switch_hovered) && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        ImGui::SetActiveID(row_active_id, window);
        ImGui::SetFocusID(row_active_id, window);
        ImGui::FocusWindow(window);

        *value = !*value;
        changed = true;

        markItemEditedSafe(id);
    }

    if (!waiting && bind_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        waiting = true;
        capture_ready = false;
        capture_frame = g.FrameCount;

        storage->SetBool(waiting_id, true);
        storage->SetBool(capture_ready_id, false);
        storage->SetInt(capture_frame_id, capture_frame);

        for (int vk = 1; vk < 256; ++vk)
            GetAsyncKeyState(vk);

        ImGui::SetActiveID(bind_active_id, window);
        ImGui::SetFocusID(bind_active_id, window);
        ImGui::FocusWindow(window);
    }

    if (waiting) {
        bool start_frame = capture_frame == g.FrameCount;

        bool mouse_released =
            !ImGui::IsMouseDown(ImGuiMouseButton_Left) &&
            !ImGui::IsMouseDown(ImGuiMouseButton_Right) &&
            !ImGui::IsMouseDown(ImGuiMouseButton_Middle);

        if (!start_frame && !capture_ready && mouse_released) {
            capture_ready = true;
            storage->SetBool(capture_ready_id, true);

            for (int vk = 1; vk < 256; ++vk)
                GetAsyncKeyState(vk);
        }

        if (!start_frame && capture_ready) {
            for (int vk = 1; vk < 256; ++vk) {
                if (GetAsyncKeyState(vk) & 1) {
                    if (vk == VK_ESCAPE || vk == VK_BACK)
                        *key = 0;
                    else
                        *key = vk;

                    waiting = false;
                    storage->SetBool(waiting_id, false);
                    storage->SetBool(capture_ready_id, true);
                    storage->SetInt(capture_frame_id, -1);

                    if (g.ActiveId == bind_active_id)
                        ImGui::ClearActiveID();

                    changed = true;
                    markItemEditedSafe(id);
                    break;
                }
            }
        }

        if (!changed && g.ActiveId != bind_active_id)
            ImGui::SetActiveID(bind_active_id, window);
    }

    if (g.ActiveId == row_active_id && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
        ImGui::ClearActiveID();

    if (!waiting && g.ActiveId == bind_active_id && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
        ImGui::ClearActiveID();

    float checked_anim = animFloat(makeSubId(id, 1), *value ? 1.f : 0.f, 16.f);
    float hover_anim = animFloat(makeSubId(id, 2), (label_hovered || switch_hovered || bind_hovered || waiting) ? 1.f : 0.f, 12.f);
    float active_anim = animFloat(makeSubId(id, 3), (*value || waiting) ? 1.f : 0.f, 16.f);
    float bind_active_anim = animFloat(makeSubId(id, 4), waiting ? 1.f : 0.f, 16.f);

    ImDrawList* draw = window->DrawList;

    const char* display_label = getDisplayLabel(label);
    const char* bind_text = waiting ? "Press any key" : keyName(*key);

    ImVec4 text_col_v = textColor(hover_anim, active_anim);

    ImVec4 switch_col_v = ImLerp(style.Colors[ImGuiCol_FrameBg], style.Colors[ImGuiCol_FrameBgHovered], hover_anim);
    switch_col_v = ImLerp(switch_col_v, style.Colors[ImGuiCol_FrameBgActive], checked_anim);

    ImVec4 bind_col_v = ImLerp(style.Colors[ImGuiCol_FrameBg], style.Colors[ImGuiCol_FrameBgHovered], bind_hovered || waiting ? 1.f : 0.f);
    bind_col_v = ImLerp(bind_col_v, style.Colors[ImGuiCol_FrameBgActive], bind_active_anim);

    ImVec4 bind_text_col = ImLerp(style.Colors[ImGuiCol_Text], style.Colors[ImGuiCol_TextDisabled], waiting ? 0.35f : 0.f);
    bind_text_col = ImLerp(bind_text_col, style.Colors[ImGuiCol_Text], bind_active_anim * 0.35f);

    draw->AddText(ImVec2(bb.Min.x, bb.Min.y + height * 0.5f - ImGui::GetTextLineHeight() * 0.5f), ImGui::GetColorU32(text_col_v), display_label);
    draw->AddRectFilled(bind_bb.Min, bind_bb.Max, ImGui::GetColorU32(bind_col_v), style.FrameRounding);

    if (style.FrameBorderSize > 0.f)
        draw->AddRect(bind_bb.Min, bind_bb.Max, ImGui::GetColorU32(style.Colors[ImGuiCol_Border]), style.FrameRounding, 0, style.FrameBorderSize);

    ImVec2 bind_text_size = ImGui::CalcTextSize(bind_text);
    ImVec2 bind_text_pos(bind_bb.Min.x + (bind_bb.GetWidth() - bind_text_size.x) * 0.5f, bind_bb.Min.y + (bind_bb.GetHeight() - bind_text_size.y) * 0.5f);

    draw->PushClipRect(bind_bb.Min, bind_bb.Max, true);
    draw->AddText(bind_text_pos, ImGui::GetColorU32(bind_text_col), bind_text);
    draw->PopClipRect();

    draw->AddRectFilled(switch_bb.Min, switch_bb.Max, ImGui::GetColorU32(switch_col_v), switch_rounding);

    float knob_radius = 7.f;
    float knob_x = ImLerp(switch_bb.Min.x + switch_height * 0.5f, switch_bb.Max.x - switch_height * 0.5f, checked_anim);

    ImVec2 knob_center(knob_x, switch_bb.Min.y + switch_height * 0.5f);

    draw->AddCircleFilled(knob_center, knob_radius, ImGui::GetColorU32(style.Colors[ImGuiCol_CheckMark]), 32);

    return changed;
}

bool c_ui_modules::sliderFloat(const char* label, float* value, float min, float max, const char* format) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems || !value)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;

    ImGuiID id = makeWidgetId(label, value);

    float height = 38.f;
    float slider_width = 118.f;
    float track_height = 4.f;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, height);
    ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));

    ImGui::ItemSize(bb, style.FramePadding.y);

    bool can_interact = !uiHasForeignActiveId(id);

    if (can_interact) {
        if (!ImGui::ItemAdd(bb, id))
            return false;
    }

    ImRect slider_bb(ImVec2(bb.Max.x - slider_width, bb.Min.y + height * 0.5f - 8.f), ImVec2(bb.Max.x, bb.Min.y + height * 0.5f + 8.f));

    bool hovered = ImGui::IsMouseHoveringRect(slider_bb.Min, slider_bb.Max);
    bool held = false;
    bool pressed = false;

    if (can_interact && uiCanUseButtonBehavior(id))
        pressed = ImGui::ButtonBehavior(slider_bb, id, &hovered, &held);

    bool row_hovered = ImGui::IsMouseHoveringRect(bb.Min, bb.Max);
    bool changed = false;

    if (held && max > min) {
        float t = ImClamp((g.IO.MousePos.x - slider_bb.Min.x) / slider_width, 0.f, 1.f);
        float new_value = min + (max - min) * t;

        if (*value != new_value) {
            *value = new_value;
            changed = true;
            markItemEditedSafe(id);
        }
    }

    float target_t = 0.f;
    if (max > min)
        target_t = ImClamp((*value - min) / (max - min), 0.f, 1.f);

    float animated_t = animFloat(makeSubId(id, 1), target_t, 18.f);
    float hover_anim = animFloat(makeSubId(id, 2), row_hovered ? 1.f : 0.f, 12.f);
    float active_anim = animFloat(makeSubId(id, 3), held ? 1.f : 0.f, 16.f);

    ImDrawList* draw = window->DrawList;

    const char* display_label = getDisplayLabel(label);

    char value_buf[64];
    ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), format, *value);

    ImVec4 text_col_v = textColor(hover_anim, active_anim);
    ImVec4 value_col_v = textColor(hover_anim * 0.65f, active_anim);

    ImVec4 track_col_v = ImLerp(style.Colors[ImGuiCol_FrameBg], style.Colors[ImGuiCol_FrameBgHovered], hover_anim);
    track_col_v = ImLerp(track_col_v, style.Colors[ImGuiCol_FrameBgActive], active_anim);

    ImVec4 fill_col_v = ImLerp(style.Colors[ImGuiCol_SliderGrab], style.Colors[ImGuiCol_SliderGrabActive], active_anim);

    draw->AddText(ImVec2(bb.Min.x, bb.Min.y + height * 0.5f - ImGui::GetTextLineHeight() * 0.5f), ImGui::GetColorU32(text_col_v), display_label);

    ImVec2 value_size = ImGui::CalcTextSize(value_buf);

    draw->AddText(ImVec2(slider_bb.Min.x - value_size.x - 14.f, bb.Min.y + height * 0.5f - value_size.y * 0.5f), ImGui::GetColorU32(value_col_v), value_buf);

    float track_y = slider_bb.Min.y + 8.f;
    ImVec2 track_min = ImVec2(slider_bb.Min.x, track_y - track_height * 0.5f);
    ImVec2 track_max = ImVec2(slider_bb.Max.x, track_y + track_height * 0.5f);

    draw->AddRectFilled(track_min, track_max, ImGui::GetColorU32(track_col_v), style.GrabRounding);

    float fill_x = ImLerp(slider_bb.Min.x, slider_bb.Max.x, animated_t);

    if (fill_x > track_min.x)
        draw->AddRectFilled(track_min, ImVec2(fill_x, track_max.y), ImGui::GetColorU32(fill_col_v), style.GrabRounding);

    return changed || pressed;
}

bool c_ui_modules::sliderInt(const char* label, int* value, int min, int max, const char* format) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems || !value)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;

    ImGuiID id = makeWidgetId(label, value);

    float height = 38.f;
    float slider_width = 118.f;
    float track_height = 4.f;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, height);
    ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));

    ImGui::ItemSize(bb, style.FramePadding.y);

    bool can_interact = !uiHasForeignActiveId(id);

    if (can_interact) {
        if (!ImGui::ItemAdd(bb, id))
            return false;
    }

    ImRect slider_bb(ImVec2(bb.Max.x - slider_width, bb.Min.y + height * 0.5f - 8.f), ImVec2(bb.Max.x, bb.Min.y + height * 0.5f + 8.f));

    bool hovered = ImGui::IsMouseHoveringRect(slider_bb.Min, slider_bb.Max);
    bool held = false;
    bool pressed = false;

    if (can_interact && uiCanUseButtonBehavior(id))
        pressed = ImGui::ButtonBehavior(slider_bb, id, &hovered, &held);

    bool row_hovered = ImGui::IsMouseHoveringRect(bb.Min, bb.Max);
    bool changed = false;

    if (held && max > min) {
        float t = ImClamp((g.IO.MousePos.x - slider_bb.Min.x) / slider_width, 0.f, 1.f);
        int new_value = min + static_cast<int>((max - min) * t + 0.5f);
        new_value = ImClamp(new_value, min, max);

        if (*value != new_value) {
            *value = new_value;
            changed = true;
            markItemEditedSafe(id);
        }
    }

    float target_t = 0.f;
    if (max > min)
        target_t = ImClamp((static_cast<float>(*value) - static_cast<float>(min)) / static_cast<float>(max - min), 0.f, 1.f);

    float animated_t = animFloat(makeSubId(id, 1), target_t, 18.f);
    float hover_anim = animFloat(makeSubId(id, 2), row_hovered ? 1.f : 0.f, 12.f);
    float active_anim = animFloat(makeSubId(id, 3), held ? 1.f : 0.f, 16.f);

    ImDrawList* draw = window->DrawList;

    const char* display_label = getDisplayLabel(label);

    char value_buf[64];
    ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), format, *value);

    ImVec4 text_col_v = textColor(hover_anim, active_anim);
    ImVec4 value_col_v = textColor(hover_anim * 0.65f, active_anim);

    ImVec4 track_col_v = ImLerp(style.Colors[ImGuiCol_FrameBg], style.Colors[ImGuiCol_FrameBgHovered], hover_anim);
    track_col_v = ImLerp(track_col_v, style.Colors[ImGuiCol_FrameBgActive], active_anim);

    ImVec4 fill_col_v = ImLerp(style.Colors[ImGuiCol_SliderGrab], style.Colors[ImGuiCol_SliderGrabActive], active_anim);

    draw->AddText(ImVec2(bb.Min.x, bb.Min.y + height * 0.5f - ImGui::GetTextLineHeight() * 0.5f), ImGui::GetColorU32(text_col_v), display_label);

    ImVec2 value_size = ImGui::CalcTextSize(value_buf);

    draw->AddText(ImVec2(slider_bb.Min.x - value_size.x - 14.f, bb.Min.y + height * 0.5f - value_size.y * 0.5f), ImGui::GetColorU32(value_col_v), value_buf);

    float track_y = slider_bb.Min.y + 8.f;
    ImVec2 track_min = ImVec2(slider_bb.Min.x, track_y - track_height * 0.5f);
    ImVec2 track_max = ImVec2(slider_bb.Max.x, track_y + track_height * 0.5f);

    draw->AddRectFilled(track_min, track_max, ImGui::GetColorU32(track_col_v), style.GrabRounding);

    float fill_x = ImLerp(slider_bb.Min.x, slider_bb.Max.x, animated_t);

    if (fill_x > track_min.x)
        draw->AddRectFilled(track_min, ImVec2(fill_x, track_max.y), ImGui::GetColorU32(fill_col_v), style.GrabRounding);

    return changed || pressed;
}

bool c_ui_modules::colorEdit(const char* label, ImColor* color) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems || !color)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;

    ImGuiID id = makeWidgetId(label, color);

    float height = 34.f;
    float preview_size = 18.f;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, height);
    ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));

    ImGui::ItemSize(bb, style.FramePadding.y);

    bool block_row_item = g.ActiveId != 0 && g.ActiveId != id && g.ActiveIdPreviousFrame != id && !g.DragDropActive;
    if (!block_row_item) {
        if (!ImGui::ItemAdd(bb, id))
            return false;
    }

    ImRect preview_bb(ImVec2(bb.Max.x - preview_size, bb.Min.y + height * 0.5f - preview_size * 0.5f), ImVec2(bb.Max.x, bb.Min.y + height * 0.5f + preview_size * 0.5f));
    bool preview_hovered = ImGui::IsMouseHoveringRect(preview_bb.Min, preview_bb.Max);

    bool changed = colorEditPreview("preview", color, preview_bb);

    ImGuiID popup_id = makeRectWidgetId("preview", color, preview_bb);
    bool popup_open = ImGui::IsPopupOpen(popup_id, ImGuiPopupFlags_None);

    float hover_anim = animFloat(makeSubId(id, 1), preview_hovered ? 1.f : 0.f, 12.f);
    float active_anim = animFloat(makeSubId(id, 2), popup_open ? 1.f : 0.f, 16.f);

    const char* display_label = getDisplayLabel(label);
    ImVec4 text_col_v = textColor(hover_anim, active_anim);

    window->DrawList->AddText(ImVec2(bb.Min.x, bb.Min.y + height * 0.5f - ImGui::GetTextLineHeight() * 0.5f), ImGui::GetColorU32(text_col_v), display_label);
    return changed;
}

bool c_ui_modules::combo(const char* label, int* current_item, const std::vector<std::string>& items) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems || !current_item || items.empty())
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;

    ImGuiID id = makeWidgetId(label, current_item);

    float height = 34.f;
    float combo_width = 128.f;
    float combo_height = 24.f;
    float popup_item_height = 22.f;
    float popup_padding = 6.f;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, height);
    ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));

    ImGui::ItemSize(bb, style.FramePadding.y);

    bool can_interact = !uiHasForeignActiveId(id);
    if (can_interact) {
        if (!ImGui::ItemAdd(bb, id))
            return false;
    }

    ImRect frame_bb(ImVec2(bb.Max.x - combo_width, bb.Min.y + height * 0.5f - combo_height * 0.5f), ImVec2(bb.Max.x, bb.Min.y + height * 0.5f + combo_height * 0.5f));

    bool hovered = ImGui::IsMouseHoveringRect(frame_bb.Min, frame_bb.Max);
    bool held = false;
    bool pressed = false;

    if (can_interact && uiCanUseButtonBehavior(id))
        pressed = ImGui::ButtonBehavior(frame_bb, id, &hovered, &held);

    bool row_hovered = ImGui::IsMouseHoveringRect(bb.Min, bb.Max);

    if (pressed)
        ImGui::OpenPopupEx(id, ImGuiPopupFlags_None);

    bool popup_open = ImGui::IsPopupOpen(id, ImGuiPopupFlags_None);

    float open_anim = animFloat(makeSubId(id, 1), popup_open ? 1.f : 0.f, 14.f);
    float hover_anim = animFloat(makeSubId(id, 2), row_hovered || popup_open ? 1.f : 0.f, 12.f);
    float active_anim = animFloat(makeSubId(id, 3), held || popup_open ? 1.f : 0.f, 16.f);

    ImDrawList* draw = window->DrawList;

    const char* display_label = getDisplayLabel(label);

    ImVec4 text_col_v = textColor(hover_anim, active_anim);

    ImVec4 frame_col_v = ImLerp(style.Colors[ImGuiCol_FrameBg], style.Colors[ImGuiCol_FrameBgHovered], hover_anim);
    frame_col_v = ImLerp(frame_col_v, style.Colors[ImGuiCol_FrameBgActive], active_anim);

    draw->AddText(ImVec2(bb.Min.x, bb.Min.y + height * 0.5f - ImGui::GetTextLineHeight() * 0.5f), ImGui::GetColorU32(text_col_v), display_label);
    draw->AddRectFilled(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(frame_col_v), style.FrameRounding);

    std::string preview = "Unselected";
    if (*current_item >= 0 && *current_item < static_cast<int>(items.size()))
        preview = items[*current_item];

    preview = clippedPreview(preview, combo_width - 36.f);

    draw->PushClipRect(ImVec2(frame_bb.Min.x + 9.f, frame_bb.Min.y), ImVec2(frame_bb.Max.x - 25.f, frame_bb.Max.y), true);

    draw->AddText(ImVec2(frame_bb.Min.x + 9.f, frame_bb.Min.y + combo_height * 0.5f - ImGui::GetTextLineHeight() * 0.5f),
        ImGui::GetColorU32(style.Colors[ImGuiCol_Text]), preview.c_str());

    draw->PopClipRect();

    ImVec2 arrow_center = ImVec2(frame_bb.Max.x - 13.f, frame_bb.Min.y + combo_height * 0.5f);

    draw->AddTriangleFilled(ImVec2(arrow_center.x - 4.f, arrow_center.y - 2.f + open_anim * 4.f), ImVec2(arrow_center.x + 4.f, arrow_center.y - 2.f + open_anim * 4.f),
        ImVec2(arrow_center.x, arrow_center.y + 3.f - open_anim * 6.f), ImGui::GetColorU32(style.Colors[ImGuiCol_TextDisabled]));

    bool changed = false;

    float popup_item_spacing = 2.f;
    float popup_height = static_cast<float>(items.size()) * popup_item_height + popup_padding * 2.f;

    if (items.size() > 1)
        popup_height += static_cast<float>(items.size() - 1) * popup_item_spacing;

    popup_height = ImMin(popup_height, 170.f);

    float animated_popup_height = ImMax(1.f, popup_height * open_anim);

    ImGui::SetNextWindowPos(ImVec2(frame_bb.Min.x, frame_bb.Max.y + 5.f));
    ImGui::SetNextWindowSize(ImVec2(combo_width, animated_popup_height));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(popup_padding, popup_padding));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, popup_item_spacing));
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, style.PopupRounding);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, style.PopupRounding);
    ImGui::PushStyleColor(ImGuiCol_PopupBg, style.Colors[ImGuiCol_PopupBg]);
    ImGui::PushStyleColor(ImGuiCol_Border, style.Colors[ImGuiCol_Border]);

    if (ImGui::BeginPopupEx(id, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar)) {
        ImGuiWindow* popup_window = ImGui::GetCurrentWindow();
        ImDrawList* popup_draw = popup_window->DrawList;

        for (int i = 0; i < static_cast<int>(items.size()); ++i) {
            ImGui::PushID(i);

            ImGuiID item_id = ImGui::GetID("item");

            ImVec2 item_pos = ImGui::GetCursorScreenPos();
            ImVec2 item_size = ImVec2(combo_width - popup_padding * 2.f, popup_item_height);
            ImRect item_bb(item_pos, ImVec2(item_pos.x + item_size.x, item_pos.y + item_size.y));

            ImGui::InvisibleButton("item", item_size);

            bool item_hovered = ImGui::IsItemHovered();
            bool item_clicked = ImGui::IsItemClicked();
            bool selected_item = *current_item == i;

            float item_hover_anim = animFloat(makeSubId(item_id, 20), item_hovered ? 1.f : 0.f, 14.f);
            float selected_anim = animFloat(makeSubId(item_id, 21), selected_item ? 1.f : 0.f, 16.f);

            ImVec4 item_col = style.Colors[ImGuiCol_HeaderHovered];
            item_col.w *= item_hover_anim * 0.55f;

            if (item_hover_anim > 0.01f)
                popup_draw->AddRectFilled(item_bb.Min, item_bb.Max, ImGui::GetColorU32(item_col), style.FrameRounding);

            if (selected_anim > 0.01f) {
                ImVec4 selected_col = style.Colors[ImGuiCol_HeaderActive];
                selected_col.w *= selected_anim;

                popup_draw->AddRectFilled(item_bb.Min, item_bb.Max, ImGui::GetColorU32(selected_col), style.FrameRounding);
            }

            ImVec4 item_text_col = textColor(item_hover_anim, selected_anim);

            popup_draw->AddText(ImVec2(item_bb.Min.x + 8.f, item_bb.Min.y + popup_item_height * 0.5f - ImGui::GetTextLineHeight() * 0.5f),
                ImGui::GetColorU32(item_text_col), items[i].c_str());

            if (item_clicked) {
                *current_item = i;
                changed = true;
                ImGui::CloseCurrentPopup();
                ImGui::MarkItemEdited(item_id);
            }

            ImGui::PopID();
        }

        ImGui::EndPopup();
    }

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(4);

    return changed;
}

bool c_ui_modules::multiCombo(const char* label, const std::vector<std::string>& items, std::vector<bool>& selected) {
    if (selected.size() != items.size())
        selected.assign(items.size(), false);

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems || items.empty())
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;

    ImGuiID id = makeWidgetId(label, &selected);

    float height = 34.f;
    float combo_width = 128.f;
    float combo_height = 24.f;
    float popup_item_height = 22.f;
    float popup_padding = 6.f;

    std::string preview;
    bool any = false;

    for (size_t i = 0; i < items.size(); ++i) {
        if (selected[i]) {
            if (any)
                preview += ", ";

            preview += items[i];
            any = true;
        }
    }

    if (!any)
        preview = "Unselected";

    preview = clippedPreview(preview, combo_width - 36.f);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, height);
    ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));

    ImGui::ItemSize(bb, style.FramePadding.y);

    bool can_interact = !uiHasForeignActiveId(id);

    if (can_interact) {
        if (!ImGui::ItemAdd(bb, id))
            return false;
    }

    ImRect frame_bb(ImVec2(bb.Max.x - combo_width, bb.Min.y + height * 0.5f - combo_height * 0.5f), ImVec2(bb.Max.x, bb.Min.y + height * 0.5f + combo_height * 0.5f));

    bool hovered = ImGui::IsMouseHoveringRect(frame_bb.Min, frame_bb.Max);
    bool held = false;
    bool pressed = false;

    if (can_interact && uiCanUseButtonBehavior(id))
        pressed = ImGui::ButtonBehavior(frame_bb, id, &hovered, &held);

    bool row_hovered = ImGui::IsMouseHoveringRect(bb.Min, bb.Max);

    if (pressed)
        ImGui::OpenPopupEx(id, ImGuiPopupFlags_None);

    bool popup_open = ImGui::IsPopupOpen(id, ImGuiPopupFlags_None);

    float open_anim = animFloat(makeSubId(id, 1), popup_open ? 1.f : 0.f, 14.f);
    float hover_anim = animFloat(makeSubId(id, 2), row_hovered || popup_open ? 1.f : 0.f, 12.f);
    float active_anim = animFloat(makeSubId(id, 3), held || popup_open ? 1.f : 0.f, 16.f);

    ImDrawList* draw = window->DrawList;

    const char* display_label = getDisplayLabel(label);

    ImVec4 text_col_v = textColor(hover_anim, active_anim);

    ImVec4 frame_col_v = ImLerp(style.Colors[ImGuiCol_FrameBg], style.Colors[ImGuiCol_FrameBgHovered], hover_anim);
    frame_col_v = ImLerp(frame_col_v, style.Colors[ImGuiCol_FrameBgActive], active_anim);

    draw->AddText(ImVec2(bb.Min.x, bb.Min.y + height * 0.5f - ImGui::GetTextLineHeight() * 0.5f), ImGui::GetColorU32(text_col_v), display_label);
    draw->AddRectFilled(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(frame_col_v), style.FrameRounding);

    ImVec2 preview_pos = ImVec2(frame_bb.Min.x + 9.f, frame_bb.Min.y + combo_height * 0.5f - ImGui::GetTextLineHeight() * 0.5f);

    draw->PushClipRect(preview_pos, ImVec2(frame_bb.Max.x - 25.f, frame_bb.Max.y), true);
    draw->AddText(preview_pos, ImGui::GetColorU32(style.Colors[ImGuiCol_Text]), preview.c_str());
    draw->PopClipRect();

    ImVec2 arrow_center = ImVec2(frame_bb.Max.x - 13.f, frame_bb.Min.y + combo_height * 0.5f);

    draw->AddTriangleFilled(ImVec2(arrow_center.x - 4.f, arrow_center.y - 2.f + open_anim * 4.f), ImVec2(arrow_center.x + 4.f, arrow_center.y - 2.f + open_anim * 4.f),
        ImVec2(arrow_center.x, arrow_center.y + 3.f - open_anim * 6.f), ImGui::GetColorU32(style.Colors[ImGuiCol_TextDisabled]));

    bool changed = false;

    float popup_item_spacing = 2.f;
    float popup_height = static_cast<float>(items.size()) * popup_item_height + popup_padding * 2.f;

    if (items.size() > 1)
        popup_height += static_cast<float>(items.size() - 1) * popup_item_spacing;

    popup_height = ImMin(popup_height, 180.f);

    float animated_popup_height = ImMax(1.f, popup_height * open_anim);

    ImGui::SetNextWindowPos(ImVec2(frame_bb.Min.x, frame_bb.Max.y + 5.f));
    ImGui::SetNextWindowSize(ImVec2(combo_width, animated_popup_height));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(popup_padding, popup_padding));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, popup_item_spacing));
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, style.PopupRounding);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, style.PopupRounding);
    ImGui::PushStyleColor(ImGuiCol_PopupBg, style.Colors[ImGuiCol_PopupBg]);
    ImGui::PushStyleColor(ImGuiCol_Border, style.Colors[ImGuiCol_Border]);

    if (ImGui::BeginPopupEx(id, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar)) {
        ImGuiWindow* popup_window = ImGui::GetCurrentWindow();
        ImDrawList* popup_draw = popup_window->DrawList;

        for (int i = 0; i < static_cast<int>(items.size()); ++i) {
            ImGui::PushID(i);

            ImGuiID item_id = ImGui::GetID("item");

            ImVec2 item_pos = ImGui::GetCursorScreenPos();
            ImVec2 item_size = ImVec2(combo_width - popup_padding * 2.f, popup_item_height);
            ImRect item_bb(item_pos, ImVec2(item_pos.x + item_size.x, item_pos.y + item_size.y));

            ImGui::InvisibleButton("item", item_size);

            bool item_hovered = ImGui::IsItemHovered();
            bool item_clicked = ImGui::IsItemClicked();
            bool item_selected = selected[i];

            float item_hover_anim = animFloat(makeSubId(item_id, 20), item_hovered ? 1.f : 0.f, 14.f);
            float selected_anim = animFloat(makeSubId(item_id, 21), item_selected ? 1.f : 0.f, 16.f);
            float check_anim = animFloat(makeSubId(item_id, 22), item_selected ? 1.f : 0.f, 18.f);

            if (item_hover_anim > 0.01f) {
                ImVec4 hover_col = style.Colors[ImGuiCol_HeaderHovered];
                hover_col.w *= item_hover_anim * 0.55f;

                popup_draw->AddRectFilled(item_bb.Min, item_bb.Max, ImGui::GetColorU32(hover_col), style.FrameRounding);
            }

            if (selected_anim > 0.01f) {
                ImVec4 selected_col = style.Colors[ImGuiCol_HeaderActive];
                selected_col.w *= selected_anim;

                popup_draw->AddRectFilled(item_bb.Min, item_bb.Max, ImGui::GetColorU32(selected_col), style.FrameRounding);
            }

            ImVec4 item_text_col = textColor(item_hover_anim, selected_anim);

            popup_draw->AddText(ImVec2(item_bb.Min.x + 8.f, item_bb.Min.y + popup_item_height * 0.5f - ImGui::GetTextLineHeight() * 0.5f),
                ImGui::GetColorU32(item_text_col), items[i].c_str());

            ImVec2 check_center = ImVec2(item_bb.Max.x - 12.f, item_bb.Min.y + popup_item_height * 0.5f);

            popup_draw->AddCircle(check_center, 4.5f, ImGui::GetColorU32(style.Colors[ImGuiCol_TextDisabled]), 24, 1.25f);

            if (check_anim > 0.01f) {
                ImVec4 check_col = style.Colors[ImGuiCol_SliderGrab];
                check_col.w *= check_anim;

                ImVec4 check_dot_col = style.Colors[ImGuiCol_CheckMark];
                check_dot_col.w *= check_anim;

                popup_draw->AddCircleFilled(check_center, 4.5f * check_anim, ImGui::GetColorU32(check_col), 24);
                popup_draw->AddCircleFilled(check_center, 1.8f * check_anim, ImGui::GetColorU32(check_dot_col), 24);
            }

            if (item_clicked) {
                selected[i] = !selected[i];
                changed = true;
                ImGui::MarkItemEdited(item_id);
            }

            ImGui::PopID();
        }

        ImGui::EndPopup();
    }

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(4);

    return changed;
}

bool c_ui_modules::keyBind(const char* label, int* key) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems || !key)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;

    ImGuiID id = makeWidgetId(label, key);
    ImGuiStorage* storage = ImGui::GetStateStorage();

    ImGuiID waiting_id = makeSubId(id, 100);
    ImGuiID row_active_id = makeSubId(id, 101);
    ImGuiID capture_ready_id = makeSubId(id, 102);
    ImGuiID capture_frame_id = makeSubId(id, 103);

    float height = 34.f;
    float bind_width = 128.f;
    float bind_height = 24.f;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, height);
    ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));

    ImGui::ItemSize(bb, style.FramePadding.y);

    bool waiting = storage->GetBool(waiting_id, false);
    bool capture_ready = storage->GetBool(capture_ready_id, true);
    int capture_frame = storage->GetInt(capture_frame_id, -1);
    bool can_interact = !uiHasForeignActiveId(id) || g.ActiveId == row_active_id;

    if (can_interact) {
        if (!ImGui::ItemAdd(bb, id))
            return false;
    }

    ImRect bind_bb(ImVec2(bb.Max.x - bind_width, bb.Min.y + height * 0.5f - bind_height * 0.5f), ImVec2(bb.Max.x, bb.Min.y + height * 0.5f + bind_height * 0.5f));

    bool hovered = can_interact && ImGui::IsMouseHoveringRect(bind_bb.Min, bind_bb.Max);
    bool held = false;
    bool pressed = false;

    if (!waiting && can_interact && uiCanUseButtonBehavior(id))
        pressed = ImGui::ButtonBehavior(bind_bb, id, &hovered, &held);

    bool row_hovered = ImGui::IsMouseHoveringRect(bb.Min, bb.Max);
    bool changed = false;

    if (!waiting && pressed) {
        waiting = true;
        capture_ready = false;
        capture_frame = g.FrameCount;

        storage->SetBool(waiting_id, true);
        storage->SetBool(capture_ready_id, false);
        storage->SetInt(capture_frame_id, capture_frame);

        for (int vk = 1; vk < 256; ++vk)
            GetAsyncKeyState(vk);

        ImGui::SetActiveID(row_active_id, window);
        ImGui::SetFocusID(row_active_id, window);
        ImGui::FocusWindow(window);
    }

    if (waiting) {
        bool start_frame = capture_frame == g.FrameCount;

        bool mouse_released =
            !ImGui::IsMouseDown(ImGuiMouseButton_Left) &&
            !ImGui::IsMouseDown(ImGuiMouseButton_Right) &&
            !ImGui::IsMouseDown(ImGuiMouseButton_Middle);

        if (!start_frame && !capture_ready && mouse_released) {
            capture_ready = true;
            storage->SetBool(capture_ready_id, true);

            for (int vk = 1; vk < 256; ++vk)
                GetAsyncKeyState(vk);
        }

        if (!start_frame && capture_ready) {
            for (int vk = 1; vk < 256; ++vk) {
                if (GetAsyncKeyState(vk) & 1) {
                    if (vk == VK_ESCAPE || vk == VK_BACK)
                        *key = 0;
                    else
                        *key = vk;

                    waiting = false;
                    storage->SetBool(waiting_id, false);
                    storage->SetBool(capture_ready_id, true);
                    storage->SetInt(capture_frame_id, -1);

                    if (g.ActiveId == row_active_id)
                        ImGui::ClearActiveID();

                    changed = true;
                    markItemEditedSafe(id);
                    break;
                }
            }
        }

        if (!changed && g.ActiveId != row_active_id)
            ImGui::SetActiveID(row_active_id, window);
    }

    if (!waiting && g.ActiveId == row_active_id && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
        ImGui::ClearActiveID();

    float waiting_anim = animFloat(makeSubId(id, 1), waiting ? 1.f : 0.f, 16.f);
    float hover_anim = animFloat(makeSubId(id, 2), (row_hovered || hovered || waiting) ? 1.f : 0.f, 12.f);
    float active_anim = animFloat(makeSubId(id, 3), (held || waiting) ? 1.f : 0.f, 16.f);

    ImDrawList* draw = window->DrawList;

    const char* display_label = getDisplayLabel(label);
    const char* bind_text = waiting ? "Press any key" : this->keyName(*key);

    ImVec4 text_col_v = textColor(hover_anim, active_anim);

    ImVec4 frame_col_v = ImLerp(style.Colors[ImGuiCol_FrameBg], style.Colors[ImGuiCol_FrameBgHovered], hover_anim);
    frame_col_v = ImLerp(frame_col_v, style.Colors[ImGuiCol_FrameBgActive], active_anim);

    ImVec4 bind_text_col = ImLerp(style.Colors[ImGuiCol_Text], style.Colors[ImGuiCol_TextDisabled], waiting_anim * 0.35f);
    bind_text_col = ImLerp(bind_text_col, style.Colors[ImGuiCol_Text], active_anim);

    draw->AddText(ImVec2(bb.Min.x, bb.Min.y + height * 0.5f - ImGui::GetTextLineHeight() * 0.5f), ImGui::GetColorU32(text_col_v), display_label);
    draw->AddRectFilled(bind_bb.Min, bind_bb.Max, ImGui::GetColorU32(frame_col_v), style.FrameRounding);

    if (style.FrameBorderSize > 0.f)
        draw->AddRect(bind_bb.Min, bind_bb.Max, ImGui::GetColorU32(style.Colors[ImGuiCol_Border]), style.FrameRounding, 0, style.FrameBorderSize);

    ImVec2 text_size = ImGui::CalcTextSize(bind_text);
    ImVec2 text_pos(bind_bb.Min.x + (bind_bb.GetWidth() - text_size.x) * 0.5f, bind_bb.Min.y + (bind_bb.GetHeight() - text_size.y) * 0.5f);

    draw->PushClipRect(bind_bb.Min, bind_bb.Max, true);
    draw->AddText(text_pos, ImGui::GetColorU32(bind_text_col), bind_text);
    draw->PopClipRect();

    return changed;
}

bool c_ui_modules::beginFade(const char* str_id, bool visible, float speed) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        fade_stack.push_back({ 0, 0.f, ImVec2(0.f, 0.f), false });
        return false;
    }

    ImGuiID id = makeWidgetId(str_id);

    ImGuiStorage* storage = ImGui::GetStateStorage();

    ImGuiID alpha_id = makeSubId(id, 5000);
    ImGuiID height_id = makeSubId(id, 5001);

    float target = visible ? 1.f : 0.f;

    float alpha = storage->GetFloat(alpha_id, target);
    alpha += (target - alpha) * ImClamp(ImGui::GetIO().DeltaTime * speed, 0.f, 1.f);

    if (!visible && alpha <= 0.01f)
        alpha = 0.f;

    storage->SetFloat(alpha_id, alpha);

    bool should_render = visible || alpha > 0.01f;

    ImVec2 start_pos = ImGui::GetCursorScreenPos();

    fade_stack.push_back({ id, alpha, start_pos, should_render });

    if (!should_render)
        return false;

    float stored_height = storage->GetFloat(height_id, 0.f);
    float clipped_height = stored_height > 0.f ? stored_height * alpha : FLT_MAX;

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * alpha);

    ImGui::PushClipRect(start_pos, ImVec2(start_pos.x + ImGui::GetContentRegionAvail().x, start_pos.y + clipped_height), true);
    ImGui::BeginGroup();

    return true;
}

void c_ui_modules::endFade() {
    if (fade_stack.empty())
        return;

    fade_data_t data = fade_stack.back();
    fade_stack.pop_back();

    if (!data.should_render)
        return;

    ImGui::EndGroup();

    ImVec2 group_size = ImGui::GetItemRectSize();

    ImGuiStorage* storage = ImGui::GetStateStorage();

    if (group_size.y > 0.f)
        storage->SetFloat(makeSubId(data.id, 5001), group_size.y);

    ImGui::PopClipRect();
    ImGui::PopStyleVar();

    float visible_height = group_size.y * data.alpha;

    if (visible_height > 0.f)
        ImGui::Dummy(ImVec2(0.f, visible_height - group_size.y));
}

bool c_ui_modules::beginChild(const char* str_id, const ImVec2& size, ImGuiChildFlags flags, ImGuiWindowFlags window_flags) {
    ImGuiWindow* parent_window = ImGui::GetCurrentWindow();
    if (parent_window->SkipItems)
        return false;

    ImGuiStyle& style = ImGui::GetStyle();

    ImGuiID id = parent_window->GetID(str_id);

    const char* display_label = getDisplayLabel(str_id);
    bool has_visible_label = display_label[0] != '\0';

    ImVec2 cursor_before_label = ImGui::GetCursorPos();

    float title_offset = 0.f;

    if (has_visible_label) {
        ImVec2 text_size = ImGui::CalcTextSize(display_label);
        ImGui::TextUnformatted(display_label);
        title_offset = text_size.y + style.ItemSpacing.y;
    }

    float fallback_child_height = size.y;
    if (fallback_child_height > 0.f)
        fallback_child_height -= title_offset;

    if (fallback_child_height <= 0.f)
        fallback_child_height = 40.f;

    ImGuiStorage* storage = ImGui::GetStateStorage();

    ImGuiID target_height_id = makeSubId(id, 1001);
    ImGuiID animated_height_id = makeSubId(id, 1002);

    float target_height = storage->GetFloat(target_height_id, fallback_child_height);
    target_height = ImMax(target_height, 1.f);

    float animated_height = animFloatLinear(animated_height_id, target_height, 650.f);

    ImGui::SetCursorPos(ImVec2(cursor_before_label.x, cursor_before_label.y + title_offset));

    flags |= ImGuiChildFlags_AlwaysUseWindowPadding;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.WindowPadding.x, 6.f));

    bool opened = ImGui::BeginChild(str_id, ImVec2(size.x, animated_height), flags, window_flags);

    child_id_stack.push_back(id);

    return opened;
}

void c_ui_modules::endChild() {
    ImGuiID id = 0;

    if (!child_id_stack.empty()) {
        id = child_id_stack.back();
        child_id_stack.pop_back();
    }

    ImGuiWindow* child_window = ImGui::GetCurrentWindow();

    float content_height = 1.f;

    if (child_window) {
        float top_padding = child_window->DC.CursorStartPos.y - child_window->Pos.y;
        float content_bottom = child_window->DC.CursorMaxPos.y - child_window->Pos.y;
        float bottom_padding = child_window->WindowPadding.y;

        if (content_bottom > top_padding)
            content_height = content_bottom + bottom_padding;
        else
            content_height = top_padding + bottom_padding;
    }

    content_height = ImMax(content_height, 1.f);

    ImGui::EndChild();
    ImGui::PopStyleVar();

    if (id != 0) {
        ImGuiStorage* storage = ImGui::GetStateStorage();
        storage->SetFloat(makeSubId(id, 1001), content_height);
    }
}