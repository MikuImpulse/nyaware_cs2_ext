#pragma once

#include <vector>
#include <string>

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

class c_ui_modules {
private:
    struct fade_data_t {
        ImGuiID id;
        float alpha;
        ImVec2 start_pos;
        bool should_render;
    };

    std::vector<ImGuiID> child_id_stack;
    std::vector<fade_data_t> fade_stack;

    const char* getDisplayLabel(const char* label);

    ImGuiID makeWidgetId(const char* label, const void* ptr = nullptr);
    ImGuiID makeRectWidgetId(const char* label, const void* ptr, const ImRect& bb);
    ImGuiID makeSubId(ImGuiID id, ImGuiID seed);

    bool uiCanUseButtonBehavior(ImGuiID id);
    bool uiHasForeignActiveId(ImGuiID id);

    void markItemEditedSafe(ImGuiID id);

    ImVec4 textColor(float hover_anim, float active_anim);
    std::string clippedPreview(const std::string& text, float max_width);

    float animFloat(ImGuiID id, float target, float speed = 14.f);
    float animFloatLinear(ImGuiID id, float target, float speed);

    bool colorEditPreview(const char* label, ImColor* color, const ImRect& bb);
    const char* keyName(int key);
public:
    bool button(const char* label, const ImVec2& size_arg = ImVec2(0.f, 0.f));
    bool checkbox(const char* label, bool* value);
    bool checkboxColored(const char* label, bool* value, const std::vector<ImColor*>& colors);
    bool checkboxKeyBind(const char* label, bool* value, int* key);
    bool sliderFloat(const char* label, float* value, float min, float max, const char* format = "%.2f");
    bool sliderInt(const char* label, int* value, int min, int max, const char* format = "%d");
    bool colorEdit(const char* label, ImColor* color);
    bool combo(const char* label, int* current_item, const std::vector<std::string>& items);
    bool multiCombo(const char* label, const std::vector<std::string>& items, std::vector<bool>& selected);
    bool keyBind(const char* label, int* key);

    bool beginFade(const char* str_id, bool visible, float speed = 14.f);
    void endFade();

    bool beginChild(const char* str_id, const ImVec2& size = ImVec2(0.f, 0.f), ImGuiChildFlags flags = 0, ImGuiWindowFlags window_flags = 0);
    void endChild();
};