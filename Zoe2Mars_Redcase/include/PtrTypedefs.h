#pragma once
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/SoundEngine/Common/AkTypes.h>
namespace PtrTypedefs
{
	namespace GameOffsets
	{
#include "GameTypes/Jehuty/Jehuty.h"
		inline std::uintptr_t BaseAddress = 0;
		inline std::uintptr_t* g_pSceneManager = nullptr;
		inline char* g_CurrentStage = nullptr;
		inline char* g_MainResident_ResourceName = nullptr;
		inline int* g_nLoadRequest = nullptr;
		inline int* g_nStoryFlag = nullptr;
		inline int* g_Difficulty = nullptr;
		namespace Jehuty
		{
			inline int* g_pCharacterFlags = nullptr;
			inline int* g_CurrentExp = nullptr;
			inline GameTypes::Jehuty::SubWeapons* pSubWeapons = nullptr;
		}
	}
	namespace Hooks::DX11
	{
		typedef HRESULT(STDMETHODCALLTYPE* Present_t)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
		inline Present_t oPresent = nullptr;
		HRESULT STDMETHODCALLTYPE Present_Hook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

		typedef HRESULT(STDMETHODCALLTYPE* ResizeBuffers_t)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
		HRESULT STDMETHODCALLTYPE ResizeBuffers_Hook(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
		inline ResizeBuffers_t oResizeBuffers = nullptr;

		bool Init_DX11_VtableHooks();
	}
	namespace Hooks::Game
	{
		typedef void(__fastcall* Pass4_Decrypt_Files_Decompress_t)(unsigned __int64 a1);

		typedef int64_t(*Cygames_DecompressFiles_t)(int64_t a1, void* a2, size_t a3);
		int64_t Cygames_DecompressFiles_Hook(int64_t a1, void* a2, size_t a3);
		inline Cygames_DecompressFiles_t oCygames_DecompressFiles = nullptr;

		typedef void (*SdxSoundPack_Lookup_t)(__int64 a1, int a2);
		void SdxSoundPack_Lookup_Hook(__int64 a1, int a2);
		inline SdxSoundPack_Lookup_t SdxSoundPack_Lookup = nullptr;
	}
	namespace Hooks::WinAPI
	{
		LRESULT WINAPI WndProc_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
		inline WNDPROC oWndProc = nullptr;

		typedef HANDLE(WINAPI* Hook_CreateFileA_t)(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
		HANDLE WINAPI CreateFileA_Hook(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
		inline Hook_CreateFileA_t oCreateFileA = nullptr;
	}
	namespace WWISE
	{
		// Since the SDK won't ever change it's best to use typedefs over decltype and static_cast
		namespace MemoryMgr
		{
			typedef AkMemPoolId(*CreatePool_t)(void* in_pMemAddress, AkUInt32 in_uMemSize, AkUInt32 in_uBlockSize, AkUInt32 in_eAttributes, AkUInt32 in_uBlockAlign);
			inline CreatePool_t CreatePool = nullptr;
		}
		namespace SoundEngine
		{
			typedef uint32_t(*PostEvent_t)(const char* in_pszEventName, uint64_t in_gameObjectID, uint32_t in_uFlags, void(*in_pfnCallback)(enum AkCallbackType, struct AkCallbackInfo*), void* in_pCookie, uint32_t in_cExternals, struct AkExternalSourceInfo* in_pExternalSources, uint32_t in_PlayingID);
			inline PostEvent_t PostEvent = nullptr;

			typedef AKRESULT(*LoadBank_t)(const void* in_pInMemoryBankPtr, AkUInt32 in_uInMemoryBankSize, AkBankID& out_bankID);
			inline LoadBank_t LoadBank = nullptr;

			typedef AKRESULT(*RegisterGameObj_t)(AkGameObjectID in_gameObjectID);
			inline RegisterGameObj_t RegisterGameObj = nullptr;

			typedef void (*StopPlayingID_t)(AkPlayingID in_playingID,AkTimeMs in_uTransitionDuration, AkCurveInterpolation in_eFadeCurve);
			inline StopPlayingID_t StopPlayingID = nullptr;
		}
	}
	namespace Hooks::WWISE
	{
		typedef void (*GameStartup_WWISE_INIT_t)(__int64 a1);

		void GameStartup_WWISE_INIT_Hook(__int64 a1);
		inline GameStartup_WWISE_INIT_t oGameStartup_WWISE_INIT = nullptr;

		uint32_t PostEvent_Hook(const char* in_pszEventName, uint64_t in_gameObjectID, uint32_t in_uFlags, void(*in_pfnCallback)(enum AkCallbackType, struct AkCallbackInfo*), void* in_pCookie, uint32_t in_cExternals, struct AkExternalSourceInfo* in_pExternalSources, uint32_t in_PlayingID);
		inline PtrTypedefs::WWISE::SoundEngine::PostEvent_t oPostEvent = nullptr;
	}
}

namespace Mod
{
	inline AkGameObjectID Custom_AkGameObj  = 99999;
	inline AkGameObjectID Ntitle_SfxObject  = 0u;
	inline AkPlayingID Zoe1_Theme_PlayingID = 0u;
	inline bool Is_Demo = false;
	inline void Nop_Patch(uintptr_t at, size_t len)
	{
		memset((void*)at, 0x90, len);
	}
	void RemapMemory();
	void Init();
	void SetupGamePointers();
	void SetupPointers();
	void Disable_LCG_Decryption();
	void ApplyPatches();
}

namespace Hooks
{
	void SetupHooks();
}