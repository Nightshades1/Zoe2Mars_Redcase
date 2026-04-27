#include "pch.h"
#include "Utility.h"
#include "StageDec.h"
#include "Globals.h"
namespace Utility
{
	uint32_t HashStr(const char* str, bool Is_ByteSwapped) {
		uint32_t step = 0;
		uint32_t hash = 0;
		int seed = *str;

		while (*str) {
			int shiftedSeed = seed << (step++ & 0xF);
			hash += (hash << (seed & 0xF)) | (seed + shiftedSeed + (hash >> 3));
			seed = *++str;
		}
		if (Is_ByteSwapped) {
			return _byteswap_ulong(hash);
		}
		return hash;
	}

	uint32_t HashStr(const char* str)
	{
		return HashStr(str, false);
	}

	// Initializes the LCG key and seed based on the stage name (must be hashed)
	// and The build time of the STAGE.DAT header.
	void LCG_InitializeKey(char* StageName)
	{
		int NameHash = 129 * HashStr(StageName, false);
		Globals::g_LCG_Seed = NameHash + Globals::pStageDat_Header->buildTime + 0xA78925D9;
		Globals::g_LCG_Key = NameHash + 0x7A88FB59;
	}

	// Encrypts/Decrypt the data using the regular LCG algorithm.
	void LCG_Xor(int* pData, int iterations)
	{
		for (int i = 0; i < iterations; ++i)
		{
			*pData++ ^= Globals::g_LCG_Seed;
			Globals::g_LCG_Seed = Globals::g_LCG_Key + LCG_MULTIPLIER * Globals::g_LCG_Seed;
		}
	}
	void LogMessage(std::string Message)
	{
		if (Utility::Is_VerboseEnabled)
		{
			std::cout << Message << std::endl;
		}
	}
}
