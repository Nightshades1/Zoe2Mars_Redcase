#pragma once
#include "DataConfig.h"
struct TableOfContent;
struct Header;
struct TableOfContent
{
	char name[16];
	uint32_t sectorOffset;
};

struct Header
{
	__time32_t buildTime;
	uint16_t nVersion;
	uint16_t nTableOfContent_SectorCount;
	uint16_t nStageCount;
	uint16_t nGameId;
	uint16_t reserved;
};
bool Setup_HashList();
void StageDAT_Decrypt(std::string GamePath);