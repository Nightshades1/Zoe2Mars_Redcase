#include "pch.h"
#include "Mod.h"
#include "Hooks.h"
#include "GameOffsets.h"
#include "ModPanel.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace Hooks
{
	void SetupHooks()
	{
		// Spawn a thread and poll until the game window is created
		// Ensure that the game window exists before hooking DX11
		// It also fix the full version due to Denuvo startup creating fake D3D11Device.
		std::thread DX11_HookThread([]()
			{
				while (true)
				{
					std::this_thread::sleep_for(std::chrono::seconds(1));
					HWND GameWindow = FindWindowW(L"ZONE_OF_THE_ENDERS_THE_2nd_RUNNER_MARS", nullptr);
					if (GameWindow && IsWindowVisible(GameWindow))
					{
						Hooks::DX11::Init_DX11_VtableHooks();
						break;
					}

				}
			});
		DX11_HookThread.detach();
#ifdef DEMO
		Hooks::Game::oCygames_DecompressFiles = (Hooks::Game::Cygames_DecompressFiles_t)(GameOffsets::BaseAddress + 0x853A0);
#else
		Hooks::Game::oCygames_DecompressFiles = (Hooks::Game::Cygames_DecompressFiles_t)(GameOffsets::BaseAddress + 0x418E9C0);
#endif
		DetourTransactionBegin();
		DetourAttach(&(PVOID&)Hooks::Game::oCygames_DecompressFiles, Hooks::Game::Cygames_DecompressFiles_Hook);
		DetourTransactionCommit();
	}
}
namespace Hooks::DX11
{
	using Microsoft::WRL::ComPtr;
	ComPtr<ID3D11Device> pDevice = nullptr;
	ComPtr<ID3D11DeviceContext> pDeviceContext = nullptr;
	ComPtr<ID3D11RenderTargetView> pTempRTV = nullptr;
	static const char* ConfigPath = "ModAssets\\imgui.ini";
	HRESULT STDMETHODCALLTYPE Hook_Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		if (!ModPanel::Is_Initalized)
		{
			pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)pDevice.GetAddressOf());
			pDevice->GetImmediateContext(pDeviceContext.GetAddressOf());
			DXGI_SWAP_CHAIN_DESC swapChainDesc;
			pSwapChain->GetDesc(&swapChainDesc);
			IMGUI_CHECKVERSION();
			ModPanel::pCtx = ImGui::CreateContext();
			ModPanel::MainStyle = &ImGui::GetStyle();
			ModPanel::pIO = &ImGui::GetIO();
			ModPanel::pIO->IniFilename = ConfigPath;
			ModPanel::pIO->MouseDrawCursor = true;
			ModPanel::pFont.push_back(ModPanel::pIO->Fonts->AddFontFromFileTTF("ModAssets\\arial.ttf", 12.0f));
			ModPanel::pFont.push_back(ModPanel::pIO->Fonts->AddFontFromFileTTF("ModAssets\\arial.ttf", 18.0f));
			ModPanel::pFont.push_back(ModPanel::pIO->Fonts->AddFontFromFileTTF("ModAssets\\arial.ttf", 24.0f));
			ModPanel::pFont.push_back(ModPanel::pIO->Fonts->AddFontFromFileTTF("ModAssets\\arial.ttf", 36.0f));
			ModPanel::pFont.push_back(ModPanel::pIO->Fonts->AddFontFromFileTTF("ModAssets\\arial.ttf", 48.0f));
			ModPanel::pFont.push_back(ModPanel::pIO->Fonts->AddFontFromFileTTF("ModAssets\\arial.ttf", 60.0f));
			ModPanel::pFont.push_back(ModPanel::pIO->Fonts->AddFontFromFileTTF("ModAssets\\arial.ttf", 72.0f));
			ModPanel::LoadSettings();
			ImGui_ImplWin32_Init(swapChainDesc.OutputWindow);
			Hooks::WinAPI::oWndProc = (WNDPROC)SetWindowLongPtr(swapChainDesc.OutputWindow, GWLP_WNDPROC, (LONG_PTR)Hooks::WinAPI::Hook_WndProc);
			ImGui_ImplDX11_Init(pDevice.Get(), pDeviceContext.Get());
			ModPanel::Is_Initalized = true;
		}
		if (!ModPanel::Is_Hidden)
		{
			// CACHING LOGIC 
			if (!pTempRTV)
			{
				ComPtr<ID3D11Texture2D> pBackBuffer = nullptr;
				if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)pBackBuffer.GetAddressOf())))
				{
					pDevice->CreateRenderTargetView(pBackBuffer.Get(), NULL, pTempRTV.GetAddressOf());
				}
			}
			if (pTempRTV)
			{
				pDeviceContext->OMSetRenderTargets(1, pTempRTV.GetAddressOf(), nullptr);
				if (ModPanel::Is_FontResized)
				{
					ModPanel::pIO->FontDefault = ModPanel::pFont.at(ModPanel::Settings_FontSize);
					ModPanel::Is_FontResized = false;
				}
				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();
				ModPanel::DrawMainWindow();
#ifdef _DEBUG
				ImGui::ShowDemoWindow();
#endif
				ImGui::Render();
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			}
		}
		return oPresent(pSwapChain, SyncInterval, Flags);
	}

	HRESULT STDMETHODCALLTYPE Hook_ResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
	{
		if (pTempRTV)
		{
			pTempRTV.Reset();
		}
		return oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
	}

	bool Init_DX11_VtableHooks()
	{
		DXGI_SWAP_CHAIN_DESC sd = { 0 };
		sd.BufferCount = 1;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = GetForegroundWindow(); // Temporary handle
		sd.SampleDesc.Count = 1;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		ID3D11Device* pDevice = nullptr;
		ID3D11DeviceContext* pContext = nullptr;
		IDXGISwapChain* pSwapChain = nullptr;
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

		if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &sd, &pSwapChain, &pDevice, NULL, &pContext))) {
			return false;
		}

		void** vtable = *(void***)pSwapChain;
		void* presentAddr = vtable[8];

		Hooks::DX11::oPresent = (DX11::Present_t)vtable[8];
		Hooks::DX11::oResizeBuffers = (DX11::ResizeBuffers_t)vtable[13];

		DWORD oldProtect;
		VirtualProtect(vtable, sizeof(LPVOID) * 14, PAGE_EXECUTE_READWRITE, &oldProtect);

		vtable[8] = &DX11::Hook_Present;
		vtable[13] = &DX11::Hook_ResizeBuffers;

		pSwapChain->Release();
		pDevice->Release();
		pContext->Release();
		return true;
	}
}
namespace Hooks::Game
{
	int64_t Cygames_DecompressFiles_Hook(int64_t a1, void* a2, size_t a3)
	{
		if (ModPanel::Is_CygamesFiles_Intercepted)
		{
			std::filesystem::path FilePath((char*)a1 + 128);
			// If our file exist in the game directory then
			// Hotswap the file with the one decompressed
			if (std::filesystem::exists(FilePath))
			{
				std::ifstream TempFile(FilePath, std::ios::binary | std::ios::ate);
				size_t FileSize = TempFile.tellg();
				TempFile.seekg(0, std::ios::beg);
				auto result = oCygames_DecompressFiles(a1, a2, FileSize);
				TempFile.read((char*)a2, FileSize);
				return result;
			}
			auto result = oCygames_DecompressFiles(a1, a2, a3);
			std::filesystem::path Name = FilePath.filename();
			std::filesystem::path ParentPath = FilePath.parent_path();
			if (!std::filesystem::exists(ParentPath))
			{
				std::filesystem::create_directories(ParentPath);
			}
			if (!std::filesystem::exists(FilePath))
			{
				std::ofstream File(FilePath, std::ios::binary);
				File.write((char*)a2, a3);
			}
			return result;
		}
		return oCygames_DecompressFiles(a1, a2, a3);
	}
}
namespace Hooks::WinAPI
{
	LRESULT WINAPI Hook_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		
		if ((Msg == WM_KEYUP && wParam == VK_INSERT))
		{
			ModPanel::Is_Hidden ? ModPanel::pIO->MouseDrawCursor = true : ModPanel::pIO->MouseDrawCursor = false;
			ModPanel::Is_Hidden = !ModPanel::Is_Hidden;
		}
		if (!ModPanel::Is_Hidden)
		{
			if (ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam))
			{
				return true;
			}
		}
		return CallWindowProc(oWndProc, hWnd, Msg, wParam, lParam);
	}
}