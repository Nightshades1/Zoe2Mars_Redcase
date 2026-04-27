#pragma once
namespace Utility
{
	uint32_t HashStr(const char* str, bool Is_ByteSwapped);
	uint32_t HashStr(const char* str);
	void LCG_InitializeKey(char* StageName);
	void LCG_Xor(int* pData, int iterations);
	void LogMessage(std::string Message);
	inline bool Is_VerboseEnabled = false;
}