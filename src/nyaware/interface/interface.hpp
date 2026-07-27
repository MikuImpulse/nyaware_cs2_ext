#pragma once

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <dwmapi.h>

#include "ui/ui.hpp"
#include "imgui/imgui.h"
#include "data/sdk/source2/structs/vector2.hpp"

struct screen_t {
	int width{}, height{};

	inline void update(int x, int y) {
		this->width = x;
		this->height = y;
	}

	screen_t() = default;
	screen_t(int x, int y) : width(x), height(y) {}

	inline ImVec2 toImVec2() const {
		return ImVec2(width, height);
	}

	inline ImVec2 centerToImVec2() const {
		return ImVec2(width * 0.5f, height * 0.5f);
	}

	inline vector2_t toVec2() const {
		return vector2_t(width, height);
	}

	inline vector2_t centerToVec2() const {
		return vector2_t(width * 0.5f, height * 0.5f);
	}
};

class c_interface {
public:
	HWND window{};
	WNDCLASSEX window_class{};

	IDXGISwapChain* swap_chain{};
	ID3D11Device* device{};
	ID3D11DeviceContext* device_context{};
	ID3D11RenderTargetView* render_targetview{};

	screen_t* screen{};
	c_user_interface ui{};

	void render();
	void setup();

	void newFrame();
	void endFrame();

	void init(screen_t* screenInfo);
	void shutdown();
};