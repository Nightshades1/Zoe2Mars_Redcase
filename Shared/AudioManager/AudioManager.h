#pragma once
#include <nlohmann/json.hpp>
namespace AudioManager
{
	using json = nlohmann::json;
	inline constexpr const char* New_SoundPacks_FileName = "NewSdxOffsets.json";
	inline constexpr const char* SoundPacks_FileName = "DefaultOffsets.json";
	inline constexpr const char* OverrideSounds_FileName = "OverrideSounds.json";
	struct JSON_OverridesSounds_Metadata
	{
		std::optional<std::string> Comment;
		uint32_t SoundPack_Index = 0;
		std::string Override_ID;
	};
	struct JSON_SoundPack_Metadata
	{
		std::string ID;
	};
	struct JSON_Stages_Container
	{
		std::unordered_map<std::string, std::vector<JSON_SoundPack_Metadata>> SoundPacks;
	};
	struct JSON_Stages_OverridesSounds
	{
		std::unordered_map<std::string, std::vector<JSON_OverridesSounds_Metadata>> SoundPacks;
	};
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JSON_OverridesSounds_Metadata, Comment, SoundPack_Index, Override_ID)
		NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JSON_SoundPack_Metadata, ID)
		NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JSON_Stages_Container, SoundPacks)
		NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JSON_Stages_OverridesSounds, SoundPacks)
		extern JSON_Stages_Container JSON_OriginalSoundPacks;
	extern JSON_Stages_Container JSON_NewSoundPacks;
	extern JSON_Stages_OverridesSounds JSON_Overrides_Sounds;
	void SetupTables(std::string BaseDirPath);
}