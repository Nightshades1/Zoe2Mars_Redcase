#pragma once
namespace Hooks
{
	void SetupHooks();
}
namespace Hooks::DX11
{
	typedef HRESULT(STDMETHODCALLTYPE* Present_t)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	inline Present_t oPresent = nullptr;
	HRESULT STDMETHODCALLTYPE Hook_Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

	typedef HRESULT(STDMETHODCALLTYPE* ResizeBuffers_t)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
	HRESULT STDMETHODCALLTYPE Hook_ResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
	inline ResizeBuffers_t oResizeBuffers = nullptr;

	bool Init_DX11_VtableHooks();
}
namespace Hooks::Game
{
	typedef void(__fastcall* Pass4_Decrypt_Files_Decompress_t)(unsigned __int64 a1);

	typedef int64_t(*Cygames_DecompressFiles_t)(int64_t a1, void* a2, size_t a3);
	int64_t Cygames_DecompressFiles_Hook(int64_t a1, void* a2, size_t a3);

	inline Cygames_DecompressFiles_t oCygames_DecompressFiles = nullptr;
}
namespace Hooks::WinAPI
{
	LRESULT WINAPI Hook_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	inline WNDPROC oWndProc = nullptr;
}