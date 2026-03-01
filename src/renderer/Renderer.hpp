#pragma once
#define STB_IMAGE_IMPLEMENTATION

#include <WinUser.h>
#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>

using dx_callback      = std::function<void()>;
using wndproc_callback = std::function<void(HWND, UINT, WPARAM, LPARAM)>;

class Renderer final {
public:
	Renderer();
	~Renderer() = default;
	void destroy();

	bool add_dx_callback(dx_callback callback, uint32_t priority);
	void remove_dx_callback(uint32_t priority);

	void OnPresent();
	void renderFrame();

	void add_wndproc_callback(wndproc_callback callback);
	void wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	bool m_is_rendering  = false;
	UINT m_resize_height = 0;
	UINT m_resize_width  = 0;
	UINT m_width         = 1000;
	UINT m_height        = 700;
	HWND m_hwnd          = nullptr;
	RECT rc;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3d_device_context;

	ImFont* m_title_font   = nullptr;
	ImFont* m_desc_font    = nullptr;
	ImFont* m_font_awesome = nullptr;

private:
	bool createDeviceD3D();
	void cleanupDeviceD3D();
	void createRenderTarget();
	void cleanupRenderTarget();

	WNDCLASSEXW m_wc = {0};
	Microsoft::WRL::ComPtr<ID3D11Device> m_d3d_device;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_dxgi_swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_main_render_target_view;

	std::vector<std::pair<uint32_t, dx_callback>> m_dx_callbacks;
	std::vector<wndproc_callback> m_wndproc_callbacks;
};

inline std::unique_ptr<Renderer> g_renderer = nullptr;