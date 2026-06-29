#pragma once
#include <nlohmann/json.hpp>
using json = nlohmann::json;
namespace SoundTest
{
	inline const wchar_t* JSON_Sounds_Filename = L".\\ModAssets\\Sounds.json";
	struct SoundBank
	{
		std::string Bank_Name;
		std::optional<std::string> Stage;
		std::vector<std::string> Play_Events;
	};
	struct JSON_SoundsFile
	{
		std::vector<SoundBank> SoundBanks;
	};
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SoundBank, Bank_Name, Stage, Play_Events)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JSON_SoundsFile, SoundBanks)
	inline JSON_SoundsFile SoundsFile;
}