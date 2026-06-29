#include "pch.h"
#include "PtrTypedefs.h"
#include <ModPanel.h>
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		DetourRestoreAfterWith();
		DisableThreadLibraryCalls(hModule);
		ModPanel::Settings_Renderdoc = GetPrivateProfileIntW(L"Settings", L"Renderdoc", 0, ModPanel::Settings_FilePath);
		if (ModPanel::Settings_Renderdoc)
		{
			LoadLibraryW(L".\\renderdoc.dll");
		}
		Mod::Init();
	}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
	default:
		break;
	}
	return TRUE;
}

