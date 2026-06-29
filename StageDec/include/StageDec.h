#pragma once
struct StageDAT_FileInfo
{
	std::filesystem::path FilePath;
	uint32_t FileName_Hash;
	uint32_t FileExtension_Hash;
	size_t FileSize;
};

struct StageDAT_Sections
{
	std::string SectionName;
	std::vector<StageDAT_FileInfo> Files;
};

struct StageDAT_StageMetadata
{
	std::string StageName;
	std::vector<StageDAT_Sections> Sections;

	StageDAT_Sections GetSectionByName(std::string SectionName)
	{
		for (auto& section : Sections)
		{
			if (section.SectionName == SectionName)
			{
				return section;
			}
		}
	}
};