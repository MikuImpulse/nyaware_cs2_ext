#pragma once

#include <vector>
#include <string>

class c_ui_modules {
public:
	bool multiCombo(const char* label, const std::vector<std::string>& items, std::vector<bool>& selected);
};