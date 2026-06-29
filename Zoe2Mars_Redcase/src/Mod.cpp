#include "pch.h"
#include "GameTypes/Jehuty/Jehuty.h"
#include "AudioManager.h"
#include "PtrTypedefs.h"
namespace Mod
{
	void RemapMemory()
	{
		PtrTypedefs::GameOffsets::BaseAddress = (std::uintptr_t)GetModuleHandleA(nullptr);
		MEMORY_BASIC_INFORMATION mbi{ 0 };
		int64_t currentAddress = PtrTypedefs::GameOffsets::BaseAddress;
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
		std::filesystem::file_size(".\\Zoe2Mars.exe") == 280924160 ? Is_Demo = false : Is_Demo = true;
		RemapMemory();
		ApplyPatches();
		AudioManager::SetupTables("ModAssets");
		std::thread([]() {
			do
			{
				if (Is_Demo)
				{
					PtrTypedefs::GameOffsets::g_pSceneManager = *(std::uintptr_t**)(PtrTypedefs::GameOffsets::BaseAddress + 0x151E228);
				}
				else {
					PtrTypedefs::GameOffsets::g_pSceneManager = *(std::uintptr_t**)(PtrTypedefs::GameOffsets::BaseAddress + 0x15441D8);
				}
				if (PtrTypedefs::GameOffsets::g_pSceneManager)
				{
					std::cout << "[Scene Manager] constructed." << std::endl;
					Mod::SetupGamePointers();
				}
			} while (!PtrTypedefs::GameOffsets::g_pSceneManager);
			}).detach();
		Disable_LCG_Decryption();
		SetupPointers();
		Hooks::SetupHooks();
	}

	void SetupGamePointers()
	{
		PtrTypedefs::GameOffsets::g_nStoryFlag = (int*)((char*)PtrTypedefs::GameOffsets::g_pSceneManager + 0x5C);
		PtrTypedefs::GameOffsets::Jehuty::pSubWeapons = (GameTypes::Jehuty::SubWeapons*)((char*)PtrTypedefs::GameOffsets::g_pSceneManager + 0x61BC);
		PtrTypedefs::GameOffsets::Jehuty::g_pCharacterFlags = (int*)((char*)PtrTypedefs::GameOffsets::g_pSceneManager + 0x6194);
		PtrTypedefs::GameOffsets::Jehuty::g_CurrentExp = (int*)((char*)PtrTypedefs::GameOffsets::g_pSceneManager + 0x653C);
		PtrTypedefs::GameOffsets::g_Difficulty = (int*)((char*)PtrTypedefs::GameOffsets::g_pSceneManager + 0x2C);
		if (Is_Demo)
		{
			PtrTypedefs::GameOffsets::g_CurrentStage = (char*)(PtrTypedefs::GameOffsets::BaseAddress + 0x150F670);
			PtrTypedefs::GameOffsets::g_nLoadRequest = (int*)(PtrTypedefs::GameOffsets::BaseAddress + 0x150F6A0);
			PtrTypedefs::GameOffsets::g_MainResident_ResourceName = (char*)(PtrTypedefs::GameOffsets::BaseAddress + 0x150F680);
		}
		else
		{
			PtrTypedefs::GameOffsets::g_CurrentStage = (char*)(PtrTypedefs::GameOffsets::BaseAddress + 0x1535830);
			PtrTypedefs::GameOffsets::g_nLoadRequest = (int*)(PtrTypedefs::GameOffsets::BaseAddress + 0x1535844);
			PtrTypedefs::GameOffsets::g_MainResident_ResourceName = (char*)(PtrTypedefs::GameOffsets::BaseAddress + 0x1535820);
		}
	}

	void SetupPointers()
	{
		if (Is_Demo)
		{
			PtrTypedefs::WWISE::SoundEngine::LoadBank = (PtrTypedefs::WWISE::SoundEngine::LoadBank_t)(PtrTypedefs::GameOffsets::BaseAddress + 0x869760);
			PtrTypedefs::WWISE::MemoryMgr::CreatePool = (PtrTypedefs::WWISE::MemoryMgr::CreatePool_t)(PtrTypedefs::GameOffsets::BaseAddress + 0x91A1D0);
			PtrTypedefs::WWISE::SoundEngine::PostEvent = (PtrTypedefs::WWISE::SoundEngine::PostEvent_t)(PtrTypedefs::GameOffsets::BaseAddress + 0x86A6E0);
			PtrTypedefs::WWISE::SoundEngine::RegisterGameObj = (PtrTypedefs::WWISE::SoundEngine::RegisterGameObj_t)(PtrTypedefs::GameOffsets::BaseAddress + 0x86C9F0);
		}
		else 
		{
			PtrTypedefs::WWISE::SoundEngine::PostEvent = (PtrTypedefs::WWISE::SoundEngine::PostEvent_t)(PtrTypedefs::GameOffsets::BaseAddress + 0xC21F900);
			PtrTypedefs::WWISE::SoundEngine::RegisterGameObj = (PtrTypedefs::WWISE::SoundEngine::RegisterGameObj_t)(PtrTypedefs::GameOffsets::BaseAddress + 0xC242F70);
			PtrTypedefs::WWISE::SoundEngine::LoadBank = (PtrTypedefs::WWISE::SoundEngine::LoadBank_t)(PtrTypedefs::GameOffsets::BaseAddress + 0xC2068C0);
			PtrTypedefs::WWISE::MemoryMgr::CreatePool = (PtrTypedefs::WWISE::MemoryMgr::CreatePool_t)(PtrTypedefs::GameOffsets::BaseAddress + 0xD964FC0);
		}
	}

	// Disable all XOR Decryption stream to play
	// The game with a fully decrypted STAGE.DAT
	void Disable_LCG_Decryption()
	{
		if (Is_Demo)
		{
			// Pass 1 - Decrypt the Header Metadata.
			Nop_Patch(PtrTypedefs::GameOffsets::BaseAddress + 0x1A90D0, 21);
			// Pass 2 - Decrypt the Table Of Content.
			Nop_Patch(PtrTypedefs::GameOffsets::BaseAddress + 0x1A9630, 21);
			// Pass 3 - Decrypt the DataConfig Metadata.
			Nop_Patch(PtrTypedefs::GameOffsets::BaseAddress + 0x1A9900, 20);
			Nop_Patch(PtrTypedefs::GameOffsets::BaseAddress + 0x1A99D0, 20);
			// Pass 4 - Decrypt and decompress each section (compressed folder)
			Nop_Patch(PtrTypedefs::GameOffsets::BaseAddress + 0x1A8BF0, 21);
			Nop_Patch(PtrTypedefs::GameOffsets::BaseAddress + 0x1A8C40, 20);
		}
		else
		{
			// Due to Denuvo the 1st patch can't happen and the header (16bytes) must be re-encrypted.
			// Pass 2 - Decrypt the Table Of Content.
			Nop_Patch(PtrTypedefs::GameOffsets::BaseAddress + 0x5454D00, 21);
			// Pass 3 - Decrypt the DataConfig Metadata.
			Nop_Patch(PtrTypedefs::GameOffsets::BaseAddress + 0x5456780, 20);
			Nop_Patch(PtrTypedefs::GameOffsets::BaseAddress + 0x5456850, 20);
			// Pass 4 - Decrypt and decompress each section (compressed folder)
			Nop_Patch(PtrTypedefs::GameOffsets::BaseAddress + 0x544B6D0, 21);
			Nop_Patch(PtrTypedefs::GameOffsets::BaseAddress + 0x544B720, 20);
		}
	}
	void ApplyPatches()
	{
		if (Is_Demo)
		{
			// Skip trial ending post Anubis encounter
			const BYTE TrialEnd_Patch[]{ 0xE9,0x52,0x03,0x00,0x00,0x90,0x90 };
			memcpy((void*)(PtrTypedefs::GameOffsets::BaseAddress + 0x187525), &TrialEnd_Patch, sizeof(TrialEnd_Patch));

			// Use the full version .bnk file instead of the trial one
			strcpy((char*)(PtrTypedefs::GameOffsets::BaseAddress + 0xD24848), "game_vo.bnk");
		}
	}
}