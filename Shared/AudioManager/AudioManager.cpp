#include "AudioManager.h"

// NOTE: This is The original audio banks of each .sdx sound pack
// Primarily to fix Wwise issues on the MARS version by redirecting
// Each sdx's to their proper wwise .bnk with the format "us%08x.bnk"
// --Nightshades 15/06/2026
namespace AudioManager
{
	JSON_Stages_Container JSON_OriginalSoundPacks{};
	JSON_Stages_Container JSON_NewSoundPacks{};
	JSON_Stages_OverridesSounds JSON_Overrides_Sounds{};
	void SetupTables(std::string BaseDirPath)
	{
		std::ifstream File(std::format("{}\\{}", BaseDirPath.c_str(), SoundPacks_FileName), std::ios::binary);
		if (!File) { return; }
		json JSON_OldOffsets = json::parse(File);
		JSON_OriginalSoundPacks = JSON_OldOffsets.get<JSON_Stages_Container>();
		File.close();

		File = std::ifstream(std::format("{}\\{}", BaseDirPath.c_str(), New_SoundPacks_FileName), std::ios::binary);
		if (!File) { return; }
		json JSON_NewOffsets = json::parse(File);
		JSON_NewSoundPacks = JSON_NewOffsets.get<JSON_Stages_Container>();
		File.close();

		File = std::ifstream(std::format("{}\\{}", BaseDirPath.c_str(), OverrideSounds_FileName), std::ios::binary);
		if (!File) { return; }
		json JSON_OverrideSounds = json::parse(File);
		JSON_Overrides_Sounds = JSON_OverrideSounds.get<JSON_Stages_OverridesSounds>();
		File.close();
	}
}