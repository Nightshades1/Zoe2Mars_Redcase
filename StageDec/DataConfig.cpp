#include "pch.h"
#include "StageDec.h"
#include "DataConfig.h"
#include "Globals.h"
#include "Utility.h"

namespace DataConfig
{
	std::string GetStringFromHash(uint32_t StringHashed)
	{
		std::ostringstream ss;
		if (Globals::Sorted_HashMap_File_Extension.count(StringHashed) > 0)
		{
			ss << Globals::Sorted_HashMap_File_Extension[StringHashed];
			return std::string(ss.str());
		}
		if (Globals::Sorted_HashMap_GameString.count(StringHashed) > 0)
		{
			ss << Globals::Sorted_HashMap_GameString[StringHashed];
			return std::string(ss.str());
		}
		ss << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << StringHashed;
		return std::string(ss.str());
	}

	std::string GetSectionName(uint32_t& nExtensionHash)
	{
		switch (nExtensionHash)
		{
		case DataConfig_Section_Cache: return ".cache";
		case DataConfig_Section_NoCache: return ".nocache";
		case DataConfig_Section_Resident: return ".resident";
		case DataConfig_Section_Sound: return ".sound";
		case DataConfig_Section_End: return ".end";
		default: return "";
		}
	}

	// Decrypt every sections(Compressed Folder) of this stage and
	// Append everything about this stage in the Globals::AllStages
	void ParseSections(DataConfig_TAG*& pDataConfig, int nStageIndex)
	{
		int nStageSectorPos = Globals::AllStages[nStageIndex].nSectorPos;
		int TagCount = pDataConfig->nExtensionHash;
		int nTagSize = TagCount * sizeof(DataConfig_TAG) + 4;
		pDataConfig = (DataConfig_TAG*)((char*)pDataConfig + 4);// Skip the TAG count.


		int nCurrentSection_DataOffset = ((Globals::AllStages[nStageIndex].nSectorPos << 11) + nTagSize + 2047) >> 11 << 11;// The offset of the 1st section
		Utility::LogMessage(std::format("nTagSize[{:d}] nTagCount[{:d}]", nTagSize, TagCount));
		
		DataConfig_Section_Item SectionItem{};
		DataConfig_GameFile_Item GameFile{};
		int nSectionIndex = 0;
		for (int TagIndex = 0; TagIndex < TagCount; TagIndex++)
		{
			DataConfig_TAG* pData = (DataConfig_TAG*)(pDataConfig + TagIndex);
			if (pData->nExtensionHash == DataConfig_Section_NoCache || pData->nExtensionHash == DataConfig_Section_Cache || pData->nExtensionHash == DataConfig_Section_Resident)
			{
				SectionItem.SectionName = DataConfig::GetSectionName(pData->nExtensionHash);
				Utility::LogMessage(std::format("{:<30} {:08X} {:08X} {:08X} Alloc[{:d}]", DataConfig::GetSectionName(pData->nExtensionHash), pData->nExtensionHash, pData->nFileName_Hash, pData->Offset, pData->Offset));
				if ((pData[1].nExtensionHash & 0xFF000000) == 0x7E000000)// If the next TAG hold the Section Size metadata
				{
					int* SectionData = (int*)((char*)Globals::g_STAGE_DAT.data() + nCurrentSection_DataOffset);
					int nSectionSize = ((pData[1].nExtensionHash & 0xFFFFFF) + 2047) >> 11 << 11;

					const int v1 = *(unsigned __int16*)SectionData ^ 0x9385;
					Globals::g_LCG_Seed = v1 | ((*(unsigned __int16*)SectionData ^ 0xF6F3) << 16);
					Globals::g_LCG_Key = 278 * v1;
					Utility::LCG_Xor((int*)SectionData, nSectionSize / 4);
					*(WORD*)SectionData += 0x8F3;
					SectionItem.pData = SectionData;
					SectionItem.nSectionSize = nSectionSize;

					Utility::LogMessage(std::format("{:<30} {:08X} {:08X} {:08X} nSectionSize(sector):[{:08d}] nCurrentSection_DataOffset(sector):[{:08d}]", "[COMPRESSED FOLDER]", pData[1].nExtensionHash, pData[1].nFileName_Hash, pData[1].Offset, nSectionSize >> 11, nCurrentSection_DataOffset >> 11));

					// Adding the section size and rounding it to 1 sector
					// It goes straight to the next section :)
					nCurrentSection_DataOffset += nSectionSize & 0xFFFFF800;
				}
			}
			else if (pData->nExtensionHash == DataConfig_Section_Sound)// The only section that isn't compressed since it hold one or more Audio track
			{
				SectionItem.SectionName = ".sound";
				Utility::LogMessage(std::format("{:<30} {:08X} {:08X} {:08X} Alloc[{:d}]", ".sound", pData->nExtensionHash, pData->nFileName_Hash, pData->Offset, pData->Offset));
			}
			else if (pData->nExtensionHash == DataConfig_Section_End)
			{
				Utility::LogMessage(std::format("{:<30} {:08X} {:08X} {:08X} Free[{:d}]", ".end", pData->nExtensionHash, pData->nFileName_Hash, pData->Offset, pData->Offset));
				Globals::AllStages[nStageIndex].DataConfig_Sections.push_back(SectionItem);
				SectionItem = {};
				nSectionIndex++;
			}
			else if ((pData->nExtensionHash & 0xFF000000) != 0x7F000000 && (pData->nExtensionHash & 0xFF000000) != 0x7E000000 || pData->nExtensionHash == 0x7F000000)
			{
				if (pData->nExtensionHash != 0 && pData->nFileName_Hash != 0) 
				{
					std::string ExtName = DataConfig::GetStringFromHash(pData->nExtensionHash);
					std::string FileName = DataConfig::GetStringFromHash(pData->nFileName_Hash);
					ExtensionFound[pData->nExtensionHash] = ExtName;
					GameFile.nExtensionHash = pData->nExtensionHash;
					GameFile.nFileName_Hash = pData->nFileName_Hash;
					GameFile.Offset = pData->Offset;
					GameFile.FileName = FileName;
					GameFile.ExtensionName = ExtName;
					SectionItem.Files.push_back(GameFile);
					Utility::LogMessage(std::format("{:<30} {:08X} {:08X} {:08X} {:<39}{}|{}", "[FILE]", pData->nExtensionHash, pData->nFileName_Hash, pData->Offset, "", ExtName, FileName));
				}
				else // Not a file metadata, not a section, just a TAG.
				{
					Utility::LogMessage(std::format("{:<30} {:08X} {:08X} {:08X}", "", pData->nExtensionHash, pData->nFileName_Hash, pData->Offset));
				}
			}
		}
		Utility::LogMessage("======END OF DATA.CNF======\n");
	}
}