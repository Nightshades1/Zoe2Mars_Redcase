#pragma once
#define SECTOR_SIZE                       2048
#define XOR_MULTIPLIER                    0x2E90EDD
#define SECTOR_ALIGN(a)	                  ((((a)+SECTOR_SIZE-1)/SECTOR_SIZE)*SECTOR_SIZE)
#define DATACONFIG_SECTION_END	          0x7F000000
#define DATACONFIG_SECTION_NOCACHE		  0x7F000001 // Temporary files that are unloaded after loading another stage.
#define DATACONFIG_SECTION_CACHE		  0x7F000002 // Files loaded to the cache remain in memory for fast I/O.
#define DATACONFIG_SECTION_RESIDENT		  0x7F000003 // Files loaded to the resident area will persist across stages loading.
#define DATACONFIG_SECTION_SOUND		  0x7F000010 // Sequenced Audio played by the Konami PS2 Sound Driver, Contains sound like Ardjet/Anubis Speech and some sound effect, BGM, ZOE1 Title Music.
#define DATACONFIG_SECTION_BINARY		  0x7F010000 // There can be only a single .bin per stage, Debug symbol for the scene scenario (?)
#define DATACONFIG_BLOCK(no)             (0x7F000100 | (no))
#define DATACONFIG_COMPRESSION_FLAG		  0x7E000000
#define DATACONFIG_NOP			          0x70000000
#define IS_SECTION_TAG(data)	          (((data)&0xFF000000 ) == 0x7F000000 )
#define IS_SECTION_END(data)	          ((data) == DATACONFIG_SECTION_END )
#define IS_PRELOAD_TAG(data)	          (((data) & ~0xFF) != DATACONFIG_BLOCK(0))
#define IS_END_TAG(data)		          ((data) == 0)
#define IS_COMPRESSION_TAG(data)          (((data)&0xFF000000 ) == 0x7E000000 )
#define COMPRESSION_SIZE(data)	          ((data)&0x00FFFFFF)
#define IS_COMMAND_TAG(data)	          (((data)&0xF0000000) == 0x70000000)

inline std::unordered_map<std::string, uint32_t> Map_Section_NameToID
{
	{".binary",  DATACONFIG_SECTION_BINARY},
	{".nocache", DATACONFIG_SECTION_NOCACHE},
	{".cache",   DATACONFIG_SECTION_CACHE},
	{".resident",DATACONFIG_SECTION_RESIDENT},
	{".sound",   DATACONFIG_SECTION_SOUND},
};
inline std::unordered_map<uint32_t, std::string> Map_Section_ID_ToName
{
	{DATACONFIG_SECTION_BINARY,".binary",},
	{DATACONFIG_SECTION_NOCACHE,".nocache"},
	{DATACONFIG_SECTION_CACHE,".cache"},
	{DATACONFIG_SECTION_RESIDENT,".resident"},
	{DATACONFIG_SECTION_SOUND,".sound"},
};

struct FileTag { uint32_t ExtensionHash; uint32_t FileNameHash; uint32_t Offset; };
struct SectionTag { uint32_t Identifier; uint32_t paddding; uint32_t AllocSize; };
struct CompressionInfo { uint32_t CompressedSize; uint32_t padding; uint32_t DecompressedSize; };
struct AudioStream_TAG { uint32_t StreamIndex; uint32_t padding; uint32_t Offset; };
struct TableOfContent
{
	char name[16];
	uint32_t sectorOffset;
};

struct Header
{
	__time32_t Seed;
	uint16_t Version;
	// Header + Table of content in sector size
	uint16_t TableOfContent_SectorSize;
	uint16_t StageCount;
	uint16_t GameId;
	uint32_t InstallSize;
};

namespace StageDAT
{
	void DAT_ExtractEncrypted(std::string FilePath);
	namespace {
		Header* m_pStageDAT_Header = nullptr;
		std::vector<std::uint8_t> m_StageDAT;
		TableOfContent* m_pStageDAT_Toc = nullptr;
		uint32_t m_cryptoKey = 0, m_CryptoSeed = 0;
		int* m_pStageDataConfig = nullptr;
	}
	void DAT_DecryptHeader_TOC_DataConfig();
	void DAT_ProcessSections();
	void DAT_DecryptBuffer(int* pData, int iterations);
	void DAT_Init_StageSeed(char* StageName);
}

namespace StageBuilder
{
	namespace fs = std::filesystem;
	struct File {
		std::string FileName;
		std::string ExtName;
		size_t Size = 0;
		fs::path Path;
		uint32_t Extension_Hash = 0;
		uint32_t FileName_Hash = 0;
	};
	struct Section {
		std::string SectionName;
		uint32_t SectionIdentifier;
		size_t SectionSize = 0;
		int FilesCount = 0;
		std::vector<File> FileList;
	};
	struct StageInfo {
		std::string StageName;
		// Total Size of all sections (does not include TAGS)
		size_t TotalSize;
		int FilesCount = 0;
		std::vector<Section> SectionList;
	};
	namespace {
		inline std::vector<fs::path> PathList;
		std::vector<StageInfo> SetupPathsFromDir();
	}
	inline std::vector<std::filesystem::path> Paths;
	bool Build();
}

int GetSectionWeight(std::string& SectionName);
int GetFilesWeight(std::string& ExtName);