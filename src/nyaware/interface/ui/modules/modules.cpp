#include "modules.hpp"

#include "imgui/imgui.h"

bool c_ui_modules::multiCombo(const char* label, const std::vector<std::string>& items, std::vector<bool>& selected) {
    if (selected.size() != items.size())
        selected.assign(items.size(), false);

    std::string preview;
    bool any = false;
    for (size_t i = 0; i < items.size(); ++i) {
        if (selected[i]) {
            if (any) preview += ", ";
            preview += items[i];
            any = true;
        }
    }
    if (!any) preview = "none";

    bool changed = false;
    if (ImGui::BeginCombo(label, preview.c_str())) {
        for (int i = 0; i < (int)items.size(); ++i) {
            bool was = selected[i];
            if (ImGui::Selectable(items[i].c_str(), was, ImGuiSelectableFlags_DontClosePopups)) {
                selected[i] = !was;
                changed = true;
            }
            if (selected[i])
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    return changed;
}