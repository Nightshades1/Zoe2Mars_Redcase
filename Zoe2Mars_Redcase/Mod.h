#pragma once
namespace Mod
{
	inline void Nop_Patch(uintptr_t at, size_t len) 
	{
		memset((void*)at, 0x90, len);
	}
	void RemapMemory();
	void Init();
	void SetupPointers();
	void Disable_LCG_Decryption();
}