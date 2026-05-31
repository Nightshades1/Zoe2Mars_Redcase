#pragma once
#include "GameTypes/Jehuty/Jehuty.h"
namespace GameOffsets
{
	inline std::uintptr_t BaseAddress = 0;
	inline std::uintptr_t* g_pSceneManager = nullptr;
	inline char* g_CurrentStage = nullptr;
	inline char* g_MainResident_ResourceName = nullptr;
	inline int* g_nLoadRequest = nullptr;
	inline int* g_nStoryFlag = nullptr;
	namespace Jehuty
	{
		inline int* g_pCharacterFlags = nullptr;
		inline int* g_CurrentExp = nullptr;
		inline GameTypes::Jehuty::SubWeapons* pSubWeapons = nullptr;
	}
}