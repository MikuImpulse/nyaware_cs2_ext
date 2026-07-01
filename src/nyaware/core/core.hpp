#pragma once

class c_cheat_core {
private:
	int processID{};
public:
	bool process_focused();

	void update();
	void init();
};