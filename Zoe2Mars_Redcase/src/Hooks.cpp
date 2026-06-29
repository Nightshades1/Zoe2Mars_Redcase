#include "pch.h"
#include "ModPanel.h"
#include "AudioManager.h"
#include "PtrTypedefs.h"
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
						PtrTypedefs::Hooks::DX11::Init_DX11_VtableHooks();
						break;
					}

				}
			});
		DX11_HookThread.detach();

		//Assigned from SetupPointers() in Mod.cpp
		PtrTypedefs::Hooks::WWISE::oPostEvent = (PtrTypedefs::WWISE::SoundEngine::PostEvent_t)(PtrTypedefs::WWISE::SoundEngine::PostEvent);
		if (Mod::Is_Demo)
		{
			PtrTypedefs::Hooks::Game::oCygames_DecompressFiles = (PtrTypedefs::Hooks::Game::Cygames_DecompressFiles_t)(PtrTypedefs::GameOffsets::BaseAddress + 0x853A0);
			PtrTypedefs::Hooks::Game::SdxSoundPack_Lookup = (PtrTypedefs::Hooks::Game::SdxSoundPack_Lookup_t)(PtrTypedefs::GameOffsets::BaseAddress + 0x12A330);
			// Wwise Bank Loading at game startup, load all banks after the game finished to load the startup one.
			PtrTypedefs::Hooks::WWISE::oGameStartup_WWISE_INIT = (PtrTypedefs::Hooks::WWISE::GameStartup_WWISE_INIT_t)(PtrTypedefs::GameOffsets::BaseAddress + 0x128CF0);
		}
		else
		{
			PtrTypedefs::Hooks::Game::oCygames_DecompressFiles = (PtrTypedefs::Hooks::Game::Cygames_DecompressFiles_t)(PtrTypedefs::GameOffsets::BaseAddress + 0x418E9C0);
			PtrTypedefs::Hooks::Game::SdxSoundPack_Lookup = (PtrTypedefs::Hooks::Game::SdxSoundPack_Lookup_t)(PtrTypedefs::GameOffsets::BaseAddress + 0x4CAAC30);
			PtrTypedefs::Hooks::WWISE::oGameStartup_WWISE_INIT = (PtrTypedefs::Hooks::WWISE::GameStartup_WWISE_INIT_t)(PtrTypedefs::GameOffsets::BaseAddress + 0x4CA1D00);
		}
		PtrTypedefs::Hooks::WinAPI::oCreateFileA = CreateFileA;
		DetourTransactionBegin();
		DetourAttach(&(PVOID&)PtrTypedefs::Hooks::WinAPI::oCreateFileA, PtrTypedefs::Hooks::WinAPI::CreateFileA_Hook);
		DetourAttach(&(PVOID&)PtrTypedefs::Hooks::Game::oCygames_DecompressFiles, PtrTypedefs::Hooks::Game::Cygames_DecompressFiles_Hook);
		DetourAttach(&(PVOID&)PtrTypedefs::Hooks::Game::SdxSoundPack_Lookup, PtrTypedefs::Hooks::Game::SdxSoundPack_Lookup_Hook);
		DetourAttach(&(PVOID&)PtrTypedefs::Hooks::WWISE::oGameStartup_WWISE_INIT, PtrTypedefs::Hooks::WWISE::GameStartup_WWISE_INIT_Hook);
		DetourAttach(&(PVOID&)PtrTypedefs::Hooks::WWISE::oPostEvent, PtrTypedefs::Hooks::WWISE::PostEvent_Hook);
		DetourTransactionCommit();
	}
}
namespace PtrTypedefs::Hooks::DX11
{
	using Microsoft::WRL::ComPtr;
	ComPtr<ID3D11Device> pDevice = nullptr;
	ComPtr<ID3D11DeviceContext> pDeviceContext = nullptr;
	ComPtr<ID3D11RenderTargetView> pTempRTV = nullptr;
	static const char* ConfigPath = "ModAssets\\imgui.ini";
	HRESULT STDMETHODCALLTYPE Present_Hook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
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
			PtrTypedefs::Hooks::WinAPI::oWndProc = (WNDPROC)SetWindowLongPtr(swapChainDesc.OutputWindow, GWLP_WNDPROC, (LONG_PTR)Hooks::WinAPI::WndProc_Hook);
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

	HRESULT STDMETHODCALLTYPE ResizeBuffers_Hook(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
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
		D3D_FEATURE_LEVEL   featureLevel = D3D_FEATURE_LEVEL_11_0;

		if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &sd, &pSwapChain, &pDevice, NULL, &pContext))) {
			return false;
		}

		void** vtable = *(void***)pSwapChain;
		void* presentAddr = vtable[8];

		Hooks::DX11::oPresent = (DX11::Present_t)vtable[8];
		Hooks::DX11::oResizeBuffers = (DX11::ResizeBuffers_t)vtable[13];

		DWORD oldProtect;
		VirtualProtect(vtable, sizeof(LPVOID) * 14, PAGE_EXECUTE_READWRITE, &oldProtect);

		vtable[8] = &DX11::Present_Hook;
		vtable[13] = &DX11::ResizeBuffers_Hook;

		pSwapChain->Release();
		pDevice->Release();
		pContext->Release();
		return true;
	}
}
namespace PtrTypedefs::Hooks::Game
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
	void SdxSoundPack_Lookup_Hook(__int64 a1, int a2)
	{
		std::string StageName = reinterpret_cast<const char*>(GameOffsets::g_CurrentStage);
		auto& OriginalStages = AudioManager::JSON_OriginalSoundPacks.SoundPacks;
		auto& NewSoundPacks = AudioManager::JSON_NewSoundPacks.SoundPacks;
		auto& OverrideSounds = AudioManager::JSON_Overrides_Sounds.SoundPacks;
		bool Override_Found = false;
		for (int SoundPackIndex = 0; SoundPackIndex < NewSoundPacks[StageName].size(); SoundPackIndex++)
		{
			uint32_t New_Sector_Identifier = std::stoul(NewSoundPacks[StageName].at(SoundPackIndex).ID, nullptr, 16);
			uint32_t Original_Sector_Identifier = std::stoul(OriginalStages[StageName].at(SoundPackIndex).ID, nullptr, 16);
			if (New_Sector_Identifier == a2)
			{
				for (auto& Stage : OverrideSounds)
				{
					if (Stage.first == StageName) {
						for (auto& SoundPack : Stage.second)
						{
							if (!SoundPack.Override_ID.empty() && SoundPack.SoundPack_Index == SoundPackIndex)
							{
								Override_Found = true;
								a2 = std::stoul(SoundPack.Override_ID, nullptr, 16);
							}
						}
					}
				}
				if (!Override_Found) { a2 = Original_Sector_Identifier; }
			}
		}
		return SdxSoundPack_Lookup(a1, a2);
	}
}
namespace PtrTypedefs::Hooks::WinAPI
{
	LRESULT WINAPI WndProc_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
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

	HANDLE WINAPI CreateFileA_Hook(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
	{
		return oCreateFileA(lpFileName, dwDesiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}
}

namespace PtrTypedefs::Hooks::WWISE
{
	void GameStartup_WWISE_INIT_Hook(__int64 a1)
	{
		AkBankID       ModBankID = 0;
		AkMemPoolId    Custom_Pool = 0;
		PtrTypedefs::Hooks::WWISE::oGameStartup_WWISE_INIT(a1);
		// NOTE: If creating a memory pool, Match the game exact pool allocation attributes (0x40u, 1, 0)
		// This stops the memory manager from corrupting and breaking game audio!
		// 
		// WWISE Works based on the exact sound bank filename even if we operate in another sound pool ...
		// Using hard link allow us to make a dummy copy which point to the original file
		// Allowing us to use all music/sounds of the game at any time, while letting the game manage his own Load/Unload
		// bank stuff based on the stage we are at.
		Custom_Pool = PtrTypedefs::WWISE::MemoryMgr::CreatePool(nullptr, 0x20000000, 0x40, 1, 0);

		// Register our own permanent game object, for the ModPanel sound test
		PtrTypedefs::WWISE::SoundEngine::RegisterGameObj(Mod::Custom_AkGameObj);

		for (const auto& Dir : std::filesystem::directory_iterator(".\\sound\\Windows"))
		{
			const auto& Path = Dir.path();
			const auto& FileName = Path.filename().stem().string();
			const auto& Extension = Path.extension().stem().string();
			if (FileName != "Init" && FileName != "global_sfx")
			{
				if (Extension == ".bnk")
				{
					if (Dir.hard_link_count() == 1)
					{
						std::filesystem::create_hard_link(Path, std::format(".\\sound\\Windows\\{}_mod.bnk", FileName));
					}
					PtrTypedefs::WWISE::SoundEngine::LoadBank((char*)std::format("{}_mod", FileName).c_str(), Custom_Pool, ModBankID);
				}
			}
		}
		PtrTypedefs::WWISE::SoundEngine::LoadBank("mod_zoe1.bnk", Custom_Pool, ModBankID);
	}
	uint32_t PostEvent_Hook(const char* in_pszEventName, uint64_t in_gameObjectID, uint32_t in_uFlags, void(*in_pfnCallback)(enum AkCallbackType, struct AkCallbackInfo*), void* in_pCookie, uint32_t in_cExternals, struct AkExternalSourceInfo* in_pExternalSources, uint32_t in_PlayingID)
	{
		const std::string& Event(in_pszEventName);
		if (ModPanel::Is_LogEnabled) 
		{
			ModPanel::Log_String << "[WWISE POST EVENT] : " << Event << std::endl; 
		}
		if (strcmp("ntitle",PtrTypedefs::GameOffsets::g_CurrentStage) == 0) {
			if (Event.find("Stop_Music_S") != std::string::npos)
			{
				PtrTypedefs::Hooks::WWISE::oPostEvent("Mod_Stop_Music_S", 0xC8, in_uFlags, in_pfnCallback, in_pCookie, in_cExternals, in_pExternalSources, in_PlayingID);
			}
			if (ModPanel::Settings_MainMenu_Soundtrack > 0)
			{
				if (Event.find("Play_Music_0004b2_1") != std::string::npos)
				{
					switch (ModPanel::Settings_MainMenu_Soundtrack)
					{
					case 1:
						return PtrTypedefs::Hooks::WWISE::oPostEvent("play_zoe1_intro", 0xC8, in_uFlags, in_pfnCallback, in_pCookie, in_cExternals, in_pExternalSources, in_PlayingID);
						break;
					case 2:
						return PtrTypedefs::Hooks::WWISE::oPostEvent("play_zoe1_menu", 0xC8, in_uFlags, in_pfnCallback, in_pCookie, in_cExternals, in_pExternalSources, in_PlayingID);
						break;
					default:
						break;
					}
				}
			}
		}
		return PtrTypedefs::Hooks::WWISE::oPostEvent(in_pszEventName, in_gameObjectID, in_uFlags, in_pfnCallback, in_pCookie, in_cExternals, in_pExternalSources, in_PlayingID);
	}
}