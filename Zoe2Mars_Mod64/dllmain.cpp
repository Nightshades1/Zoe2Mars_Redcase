#include "pch.h"
#include "Mod.h"
#include "GameOffsets.h"
#include "Hooks.h"
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//LoadLibrary(L"C:\\Program Files\\RenderDoc\\renderdoc.dll");
		//LoadLibrary(L"C:\\Program Files\\RenderDoc\\dbghelp.dll");
		DetourRestoreAfterWith();
		DisableThreadLibraryCalls(hModule);
		Mod::Init();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
	default:
		break;
	}
	return TRUE;
}

