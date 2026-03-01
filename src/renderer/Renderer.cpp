#include "Renderer.hpp"

#include "font_awesome.hpp"

#include <Uxtheme.h>
#include <d3dcompiler.h>
#include <dwmapi.h>
#include <ranges>
#include <winnt.h>

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	if (!g_renderer)
		return ::DefWindowProcW(hWnd, msg, wParam, lParam);

	switch (msg) {
	case WM_NCHITTEST:
	{
		POINT pt = {static_cast<LONG>(static_cast<short>(LOWORD(lParam))), static_cast<LONG>(static_cast<short>(HIWORD(lParam)))};
		::ScreenToClient(hWnd, &pt);
		const int border_width = 8;

		if (pt.y < border_width) {
			if (pt.x < border_width)
				return HTTOPLEFT;
			if (pt.x >= g_renderer->m_width - border_width)
				return HTTOPRIGHT;
			return HTTOP;
		}
		if (pt.y >= g_renderer->m_height - border_width) {
			if (pt.x < border_width)
				return HTBOTTOMLEFT;
			if (pt.x >= g_renderer->m_width - border_width)
				return HTBOTTOMRIGHT;
			return HTBOTTOM;
		}
		if (pt.x < border_width)
			return HTLEFT;
		if (pt.x >= g_renderer->m_width - border_width)
			return HTRIGHT;

		if (ImGui::GetCurrentContext() && !ImGui::IsAnyItemHovered() && !ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopup))
			return HTCAPTION;

		return HTCLIENT;
	}
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;

		g_renderer->m_resize_width = g_renderer->m_width = (UINT)LOWORD(lParam);
		g_renderer->m_resize_height = g_renderer->m_height = (UINT)HIWORD(lParam);
		g_renderer->renderFrame();
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_ERASEBKGND: return 1;
	case WM_DESTROY: ::PostQuitMessage(0); return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

Renderer::Renderer() {
	ImGui_ImplWin32_EnableDpiAwareness();
	auto icon = LoadIconW(GetModuleHandle(nullptr), MAKEINTRESOURCEW(101));
	m_wc = {sizeof(m_wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), icon, nullptr, nullptr, nullptr, reinterpret_cast<LPCWSTR>("Todo list"), icon};
	RegisterClassExW(&m_wc);

	m_hwnd = ::CreateWindowW(m_wc.lpszClassName, L"Todo list", WS_POPUP, (GetSystemMetrics(SM_CXSCREEN) - m_width) / 2, (GetSystemMetrics(SM_CYSCREEN) - m_height) / 2, m_width, m_height, nullptr, nullptr, m_wc.hInstance, nullptr);

	SetWindowLongA(m_hwnd, GWL_EXSTYLE, GetWindowLong(m_hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(m_hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);

	MARGINS margins = {-1};
	DwmExtendFrameIntoClientArea(m_hwnd, &margins);

	rc = {0};
	GetWindowRect(m_hwnd, &rc);

	if (!createDeviceD3D()) {
		cleanupDeviceD3D();
		::UnregisterClassW(m_wc.lpszClassName, m_wc.hInstance);
		g_renderer.reset();
		throw std::runtime_error("Failed to create device");
	}

	::ShowWindow(m_hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(m_hwnd);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.IniFilename = NULL;

	ImFontConfig config;
	config.FontDataOwnedByAtlas = false;
	m_title_font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\ARIBLK.ttf",
	                                                        18.f,
	                                                        &config,
	                                                        io.Fonts->GetGlyphRangesDefault());
	m_desc_font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf",
	                                                       14.f,
	                                                       &config,
	                                                       io.Fonts->GetGlyphRangesDefault());

	config.FontDataOwnedByAtlas   = true;
	const ImWchar icons_ranges [] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
	m_font_awesome = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(fa6_solid_compressed_data, fa6_solid_compressed_size, 14.f, &config, icons_ranges);

	ImGui_ImplWin32_Init(m_hwnd);
	ImGui_ImplDX11_Init(m_d3d_device.Get(), m_d3d_device_context.Get());
}

void Renderer::destroy() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	cleanupDeviceD3D();
	::DestroyWindow(m_hwnd);
	::UnregisterClassW(m_wc.lpszClassName, m_wc.hInstance);
}

bool Renderer::createDeviceD3D() {
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount                        = 2;
	sd.BufferDesc.Width                   = 0;
	sd.BufferDesc.Height                  = 0;
	sd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator   = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow       = m_hwnd;
	sd.SampleDesc.Count   = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed           = TRUE;
	sd.SwapEffect         = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray [ 2 ] = {
	    D3D_FEATURE_LEVEL_11_0,
	    D3D_FEATURE_LEVEL_10_0,
	};

	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr,
	                                            D3D_DRIVER_TYPE_HARDWARE,
	                                            nullptr,
	                                            createDeviceFlags,
	                                            featureLevelArray,
	                                            2,
	                                            D3D11_SDK_VERSION,
	                                            &sd,
	                                            m_dxgi_swapchain.GetAddressOf(),
	                                            m_d3d_device.GetAddressOf(),
	                                            &featureLevel,
	                                            m_d3d_device_context.GetAddressOf());

	if (res == DXGI_ERROR_UNSUPPORTED)
		res = D3D11CreateDeviceAndSwapChain(nullptr,
		                                    D3D_DRIVER_TYPE_WARP,
		                                    nullptr,
		                                    createDeviceFlags,
		                                    featureLevelArray,
		                                    2,
		                                    D3D11_SDK_VERSION,
		                                    &sd,
		                                    m_dxgi_swapchain.GetAddressOf(),
		                                    m_d3d_device.GetAddressOf(),
		                                    &featureLevel,
		                                    m_d3d_device_context.GetAddressOf());

	if (res != S_OK)
		return false;

	createRenderTarget();
	return true;
}

void Renderer::createRenderTarget() {
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
	m_dxgi_swapchain->GetBuffer(0, IID_PPV_ARGS(pBackBuffer.GetAddressOf()));
	m_d3d_device->CreateRenderTargetView(pBackBuffer.Get(),
	                                     nullptr,
	                                     m_main_render_target_view.GetAddressOf());
}

void Renderer::cleanupDeviceD3D() {
	cleanupRenderTarget();
}

void Renderer::cleanupRenderTarget() {
	m_main_render_target_view.Reset();
}

bool Renderer::add_dx_callback(dx_callback callback, uint32_t priority) {
	auto it = std::lower_bound(m_dx_callbacks.begin(), m_dx_callbacks.end(), priority, [](const auto& pair, uint32_t p) {
		return pair.first < p;
	});

	if (it != m_dx_callbacks.end() && it->first == priority)
		return false;

	m_dx_callbacks.insert(it, {priority, callback});
	return true;
}

void Renderer::remove_dx_callback(uint32_t priority) {
	auto it = std::lower_bound(m_dx_callbacks.begin(), m_dx_callbacks.end(), priority, [](const auto& pair, uint32_t p) {
		return pair.first < p;
	});
	if (it != m_dx_callbacks.end() && it->first == priority)
		m_dx_callbacks.erase(it);
}

void Renderer::add_wndproc_callback(wndproc_callback callback) {
	m_wndproc_callbacks.emplace_back(callback);
}

void Renderer::OnPresent() {
	MSG msg;
	while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
			g_running = false;
	}

	renderFrame();
}

void Renderer::renderFrame() {
	if (::IsIconic(m_hwnd))
		return;

	if (m_is_rendering)
		return;

	m_is_rendering = true;

	if (m_resize_width != 0 && m_resize_height != 0) {
		cleanupRenderTarget();

		if (m_dxgi_swapchain)
			m_dxgi_swapchain->ResizeBuffers(0, m_resize_width, m_resize_height, DXGI_FORMAT_UNKNOWN, 0);

		m_resize_width = m_resize_height = 0;
		createRenderTarget();
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	{
		for (const auto& cb : m_dx_callbacks | std::views::values) {
			cb();
		}
	}

	ImGui::EndFrame();
	ImGui::Render();
	m_d3d_device_context->OMSetRenderTargets(1, m_main_render_target_view.GetAddressOf(), nullptr);
	const float background_color [ 4 ] = {0.f, 0.f, 0.f, 0.f};
	m_d3d_device_context->ClearRenderTargetView(m_main_render_target_view.Get(), background_color);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (m_dxgi_swapchain)
		m_dxgi_swapchain->Present(1, 0);

	m_is_rendering = false;
}

void Renderer::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	for (const auto& cb : m_wndproc_callbacks)
		cb(hwnd, msg, wparam, lparam);
}