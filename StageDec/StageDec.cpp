#include "pch.h"
#include "StageDec.h"
#include "DataConfig.h"
#include "Utility.h"
#include "Globals.h"
#include <CLI/CLI.hpp>
bool Setup_HashList()
{
	uint32_t Hash = 0;
	std::ifstream strlist("strlist.txt");
	if (!strlist.is_open())
	{
		std::cerr << "Couldn't open strlist.txt, Aborting..." << std::endl;
		return false;
	}
	std::string Line;
	while (std::getline(strlist, Line))
	{
		if (Line.empty()) continue;
		if (size_t pos = Line.find(".mts") != Line.npos)
		{
			size_t dot = Line.find_last_of(".");
			std::string File_Name = Line.substr(0, dot);
			for (auto& ext : Globals::Sorted_HashMap_File_Extension)
			{
				std::string Full_FileName = File_Name + "." + ext.second;
				Hash = Utility::HashStr(Full_FileName.c_str());
				Globals::Sorted_HashMap_GameString[Hash] = std::string(Full_FileName);
			}
		}
		for (auto& ext : Globals::Sorted_HashMap_File_Extension)
		{
			std::string TempStr = std::string(Line) + ext.second;
			Hash = Utility::HashStr(TempStr.c_str());
			Globals::Sorted_HashMap_GameString[Hash] = TempStr;
		}
		Hash = Utility::HashStr(Line.c_str());
		Globals::Sorted_HashMap_GameString[Hash] = std::string(Line);
	}
	return true;
}

void StageDAT_Decrypt(std::string GamePath)
{
	std::ifstream File(GamePath, std::ios::binary | std::ios::ate);
	if (!File.is_open())
	{
		std::cerr << "STAGE.DAT not found, aborting ..." << std::endl;
		return;
	}
	Utility::LogMessage("Handling STAGE.DAT... Please wait\n");
	size_t size = File.tellg();
	File.seekg(0, std::ios_base::beg);
	Globals::g_STAGE_DAT.resize(size);
	File.read(reinterpret_cast<char*>(Globals::g_STAGE_DAT.data()), size);
	File.close();
	Globals::pStageDat_Header = (Header*)Globals::g_STAGE_DAT.data();

	// Pass 1 - Decrypt the header metadata
	Globals::g_LCG_Seed = Globals::pStageDat_Header->buildTime;
	Globals::g_LCG_Key = Globals::g_LCG_Seed ^ 0xF0F0;
	Utility::LCG_Xor((int*)Globals::g_STAGE_DAT.data() + 1, 3);

	// Pass 2 - Decrypt the table of content (list of stage name + sector offset)
	Utility::LCG_Xor((int*)((char*)Globals::g_STAGE_DAT.data() + 16), (sizeof(TableOfContent) * Globals::pStageDat_Header->nStageCount) / 4);

	// Pass 3 - Decrypt one whole PS2 sector (2048 Bytes) for each stage DataConfig Metadata
	TableOfContent* pTableOfContent = (TableOfContent*)((char*)Globals::g_STAGE_DAT.data() + 16);
	for (int i = 0; i < Globals::pStageDat_Header->nStageCount; i++)//pStageDat_Header->nStageCount
	{
		TableOfContent TocEntry = pTableOfContent[i];
		DataConfig_TAG* pDataConfig = (DataConfig_TAG*)((char*)Globals::g_STAGE_DAT.data() + TocEntry.sectorOffset * PS2_DISC_SECTOR_SIZE);
		Utility::LCG_InitializeKey(TocEntry.name);
		Utility::LCG_Xor((int*)pDataConfig, PS2_DISC_SECTOR_SIZE / 4);

		Stage StageData{0};
		StageData.name = (char*)TocEntry.name;
		StageData.nSectorPos = TocEntry.sectorOffset;
		if (i < Globals::pStageDat_Header->nStageCount - 1)
		{
			StageData.nSectorSize = pTableOfContent[i + 1].sectorOffset - pTableOfContent[i].sectorOffset;
		}
		else
		{
			StageData.nSectorSize = Globals::g_STAGE_DAT.size() >> 11 - pTableOfContent[i].sectorOffset;
		}
		Globals::AllStages.push_back(StageData);
		for (auto& ext : Globals::Sorted_HashMap_File_Extension) //populate the hash map "stage.<extension>" slight bruteforce approach
		{
			std::string Stage_Plus_Extension = std::string(TocEntry.name) + "." + ext.second;
			Globals::Sorted_HashMap_GameString[Utility::HashStr(Stage_Plus_Extension.c_str(), false)] = Stage_Plus_Extension;
		}
		DataConfig::ParseSections(pDataConfig, i);
	}

	// // Decompress all sections(compressed folders) and extract all files
	//for (auto& Stage : Globals::AllStages)
	//{
	//	for (auto& Section : Stage.DataConfig_Sections)
	//	{
	//		for (auto& File : Section.Files)
	//		{
	//			std::string OutPath = std::format(".\\stage\\{}\\{}\\{}", Stage.name, Section.SectionName, File.FileName);
	//			if (!std::filesystem::exists(OutPath))
	//			{
	//				std::filesystem::create_directories(OutPath);
	//			}
	//		}
	//	}
	//}
	//
	std::ofstream pOut("STAGE.DAT", std::ios::binary);
	pOut.write((char*)Globals::g_STAGE_DAT.data(), Globals::g_STAGE_DAT.size());
}

int main(int argc, char** argv)
{
	Setup_HashList();
	CLI::App app("Multi-Purpose Tool for \"Zone Of The Enders The 2nd Runner\"","StageDec");
	bool Is_Extracting = false;
	bool Is_Building = false;
	CLI::Option* Path = app.add_flag("-p,--path", "path to the STAGE.DAT");
	CLI::Option* Verbose_Option = app.add_flag("-v,--verbose",Utility::Is_VerboseEnabled, "Verbose - Additional logging.");
	CLI::Option *Extract_Option = app.add_flag("-e,--extract", Is_Extracting, "Only decrypt (for now) - Extract all files from STAGE.DAT and reproduce the development \"stage\" folder with all sections.");
	CLI::Option* Build_Option = app.add_flag("-b,--build", Is_Building, "Build a fully decrypted STAGE.DAT based on the development \"stage\" folder.");
	Extract_Option->excludes(Build_Option);
	Build_Option->excludes(Extract_Option);
	CLI11_PARSE(app,argc,argv);

	return 0;
}