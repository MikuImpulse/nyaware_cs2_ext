#include "interface.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_dx11.h"
#include "imgui/backends/imgui_impl_win32.h"

#include "data/globals.hpp"
#include "utils/log.hpp"

void c_interface::render() {
	this->ui.update();
	g.runtime.update();
}

void c_interface::setup() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();

	io.IniFilename = nullptr;
	io.LogFilename = nullptr;
	io.DisplaySize = ImVec2((float) this->screen->width, (float)this->screen->height);
	io.FontGlobalScale = 1.0f;

	this->ui.init();

	ImGui_ImplWin32_Init(this->window);
	ImGui_ImplDX11_Init(device, device_context);
}

void c_interface::newFrame() {
	MSG msg;

	while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	if (GetAsyncKeyState(cfg.ui.menu_keybind) & 1) {
		ui.is_opened = !ui.is_opened;

		if (!ui.is_opened)
			SetWindowLong(window, GWL_EXSTYLE,
				WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
		else
			SetWindowLong(window, GWL_EXSTYLE,
				WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
	}

	if (!ui.is_opened)
		g.runtime.update_ui_cfg(&ui);
}

void c_interface::endFrame() {
	ImGui::Render();

	float color[4]{ 0, 0, 0, 0 };
	device_context->OMSetRenderTargets(1, &render_targetview, nullptr);
	device_context->ClearRenderTargetView(render_targetview, color);

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	swap_chain->Present(1U, 0U);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam))
		return true;

	switch (msg) {
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU)
				return 0;
			break;

		case WM_DESTROY:
			g.uinterface.shutdown();
			PostQuitMessage(0);
			return 0;

		case WM_CLOSE:
			g.uinterface.shutdown();
			return 0;
	}

	return DefWindowProc(window, msg, wParam, lParam);
}

void c_interface::init(screen_t* screenInfo) {
	this->screen = screenInfo;
	screenInfo->update(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

	window_class.cbSize = sizeof(window_class);
	window_class.style = CS_CLASSDC;
	window_class.lpfnWndProc = wndProc;
	window_class.hInstance = GetModuleHandleA(0);
	window_class.lpszClassName = TEXT("RC7");

	RegisterClassEx(&window_class);

	this->window = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW, window_class.lpszClassName, TEXT("ImGui"), WS_POPUP, 0, 0, screen->width, screen->height, NULL, NULL, window_class.hInstance, NULL);
	SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

	RECT client_area{}, window_area{};
	POINT diff{};

	GetClientRect(this->window, &client_area); GetWindowRect(this->window, &window_area);
	ClientToScreen(window, &diff);

	const MARGINS margins {
		window_area.left + (diff.x - window_area.left),
		window_area.top + (diff.y - window_area.top),
		client_area.right,
		client_area.bottom
	};

	DwmExtendFrameIntoClientArea(window, &margins);
	ShowWindow(window, SW_SHOW);
	UpdateWindow(window);

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 360;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = this->window;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

	HRESULT result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0U, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &swap_chain, &device, &featureLevel, &device_context);
	if (result == S_OK) {
		ID3D11Texture2D* back_buffer{};
		swap_chain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));

		if (back_buffer) {
			device->CreateRenderTargetView(back_buffer, nullptr, &render_targetview);
			back_buffer->Release();
		}
	}
	else {
		LOGE("unable to create device. please restart program");
	}

	this->setup();
}

void c_interface::shutdown() {
	device->Release();
	device_context->Release();
	swap_chain->Release();
	render_targetview->Release();

	DestroyWindow(this->window);
	UnregisterClass(window_class.lpszClassName, window_class.hInstance);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}