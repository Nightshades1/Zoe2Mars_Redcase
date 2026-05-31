#include "pch.h"
#include "Mod.h"
#include "GameOffsets.h"
#include "Hooks.h"
#include "GameTypes/Jehuty/Jehuty.h"
namespace Mod
{
	void RemapMemory()
	{
		GameOffsets::BaseAddress = (std::uintptr_t)GetModuleHandleA(nullptr);
		MEMORY_BASIC_INFORMATION mbi{ 0 };
		int64_t currentAddress = GameOffsets::BaseAddress;
		while (VirtualQuery((void*)(currentAddress), &mbi, sizeof(mbi)) && mbi.State != MEM_FREE)
		{
			DWORD oldProtect = 0;
			if (mbi.Protect != PAGE_NOACCESS)
			{
				VirtualProtect((void*)(currentAddress), mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldProtect);
			}
			currentAddress += mbi.RegionSize;
		}
	}
	void Init()
	{
		RemapMemory();
		std::thread([]() {
			do
			{
#ifdef DEMO
				GameOffsets::g_pSceneManager = *(std::uintptr_t**)(GameOffsets::BaseAddress + 0x151E228);
#else
				GameOffsets::g_pSceneManager = *(std::uintptr_t**)(GameOffsets::BaseAddress + 0x15441D8);
#endif
				if (GameOffsets::g_pSceneManager)
				{
					std::cout << "[Scene Manager] constructed." << std::endl;
					Mod::SetupPointers();
				}
			} while (!GameOffsets::g_pSceneManager);
			}).detach();
		Disable_LCG_Decryption();
		Hooks::SetupHooks();
	}

	void SetupPointers()
	{
		GameOffsets::g_nStoryFlag = (int*)((char*)GameOffsets::g_pSceneManager + 0x5C);
		GameOffsets::Jehuty::pSubWeapons = (GameTypes::Jehuty::SubWeapons*)((char*)GameOffsets::g_pSceneManager + 0x61BC);
		GameOffsets::Jehuty::g_pCharacterFlags = (int*)((char*)GameOffsets::g_pSceneManager + 0x6194);
		GameOffsets::Jehuty::g_CurrentExp = (int*)((char*)GameOffsets::g_pSceneManager + 0x653C);
#ifdef DEMO
		GameOffsets::g_CurrentStage = (char*)(GameOffsets::BaseAddress + 0x150F670);
		GameOffsets::g_nLoadRequest = (int*)(GameOffsets::BaseAddress + 0x150F6A0);
		GameOffsets::g_MainResident_ResourceName = (char*)(GameOffsets::BaseAddress + 0x150F680);
#else
		GameOffsets::g_CurrentStage = (char*)(GameOffsets::BaseAddress + 0x1535830);
		GameOffsets::g_nLoadRequest = (int*)(GameOffsets::BaseAddress + 0x1535844);
		GameOffsets::g_MainResident_ResourceName = (char*)(GameOffsets::BaseAddress + 0x1535820);
#endif
	}

	// Disable all XOR Decryption stream to play
	// The game with a fully decrypted STAGE.DAT
	void Disable_LCG_Decryption()
	{
#ifdef DEMO
		// Pass 1 - Decrypt the Header Metadata.
		Nop_Patch(GameOffsets::BaseAddress + 0x1A90D0, 21);
		// Pass 2 - Decrypt the Table Of Content.
		Nop_Patch(GameOffsets::BaseAddress + 0x1A9630, 21);
		// Pass 3 - Decrypt the DataConfig Metadata.
		Nop_Patch(GameOffsets::BaseAddress + 0x1A9900, 20); 
		Nop_Patch(GameOffsets::BaseAddress + 0x1A99D0, 20);
		// Pass 4 - Decrypt and decompress each section (compressed folder)
		Nop_Patch(GameOffsets::BaseAddress + 0x1A8BF0, 21); 
		Nop_Patch(GameOffsets::BaseAddress + 0x1A8C40, 20);
#else
		// Due to Denuvo that virtualize/obfuscate,some patch may be different compared to the Demo version
		// Pass 1 - Decrypt the Header Metadata.
		Nop_Patch(GameOffsets::BaseAddress + 0x5454C23, 21);
		// Pass 2 - Decrypt the Table Of Content.
		Nop_Patch(GameOffsets::BaseAddress + 0x5454D00, 21);
		// Pass 3 - Decrypt the DataConfig Metadata.
		Nop_Patch(GameOffsets::BaseAddress + 0x5456780, 20);
		Nop_Patch(GameOffsets::BaseAddress + 0x5456850, 20);
		// Pass 4 - Decrypt and decompress each section (compressed folder)
		Nop_Patch(GameOffsets::BaseAddress + 0x544B6D0, 21);
		Nop_Patch(GameOffsets::BaseAddress + 0x544B720, 20);
#endif
	}
}