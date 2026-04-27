#pragma once
#define DataConfig_Section_End 0x7F000000       // Mark the end of a section.
#define DataConfig_Section_NoCache 0x7F010000	// Typically used for Overlays.
#define DataConfig_Section_Cache 0x7F000002		// Files loaded to the cache are destroyed upon reloading the stage.
#define DataConfig_Section_Resident 0x7F000003	// Files loaded to the resident area will persist across stages loading.
#define DataConfig_Section_Sound 0x7F000010		// Sequenced Audio played by the Konami PS2 Sound Driver, Contains sound like Ardjet/Anubis Speech and some sound effect, BGM, ZOE1 Title Music.
inline std::map<UINT, std::string> ExtensionFound;
struct DataConfig_TAG;
struct DataConfig_GameFile_Item;
struct DataConfig_Section_Item;
struct Stage;
struct DataConfig_TAG
{
	uint32_t nExtensionHash;
	uint32_t nFileName_Hash;
	uint32_t Offset;
};
struct DataConfig_GameFile_Item
{
	uint32_t nExtensionHash;
	uint32_t nFileName_Hash;
	uint32_t Offset;
	std::string ExtensionName;
	std::string FileName;
};
struct DataConfig_Section_Item
{
	std::string SectionName;
	int nSectionSize;// size of the entire compressed section (folder)
	int* pData;// ptr to the compressed section (folder) - .sound is never compressed.
	std::vector<DataConfig_GameFile_Item> Files;
};
struct Stage
{
	int nSectorPos;
	std::string name;
	// The total size of the Stage in number of sector which include
	// the DataConfig + all files and the .sound section.
	int nSectorSize;
	std::vector<DataConfig_Section_Item> DataConfig_Sections;
};
namespace DataConfig
{
	std::string GetStringFromHash(uint32_t StringHashed);
	std::string GetSectionName(uint32_t& nExtensionHash);
	void ParseSections(DataConfig_TAG*& pDataConfig, int nStageIndex);
}