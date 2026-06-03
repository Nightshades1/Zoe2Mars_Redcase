#pragma once
namespace Utility
{
	inline std::map<UINT, std::string> HashMap_GameString{};
	inline std::map<std::string, UINT> HashMap_ExtensionToHash
	{
		{"bin",0x62877D7F},{"scx",0x000863BE},{"mtl",0x00ED22C7},{"mts",0x0000856A},{"mtz",0x003B530D},{"hzt",0x0037B1E7},
		{"trz",0x001373D8},{"hz2",0x0010ABBD},{"mdz",0x003B12FB},{"tex",0x001E5452},{"lit",0x001CED17},{"flw",0x066E60C1},
		{"mfl",0x038AB1D7},{"ric",0x00104639},{"o2d",0x000054D5},{"fnt",0x03696528},{"eft",0x00037E6B},{"var",0x00000E56},
		{"rpd",0x0000321E},{"cvz",0x00CB3B7A},{"ene",0x0688D9EB},{"row",0x398168E9},{"sdx",0},{"cnf",0x0C983C6B}
	};
	inline std::map<UINT, std::string> HashMap_HashToExtension
	{
		//Section Stuff
		{0x0C983C6B,"cnf"},
		// Scene  Init / Mission Script
		{0x62877D7F,"bin"}, // Scene Binary initialization
		{0x000863BE,"scx"}, // Scene Compiled Bytecode (scripting mission objective,Starting pos, events, global game variables, setting up the Stage, compiled to bytecode)

		//Motion System (MT_SYSTEM)
		{0x00ED22C7,"mtl"}, // Motion Table
		{0x0000856A,"mts"}, // Motion Sequence Table
		{0x003B530D,"mtz"}, // Seems to be the alias of .mtl, both use the same function

		//Hazardous Core (HZD_CORE) Stage + Character Collision
		{0x0037B1E7,"hzt"},// HZD_TREE, "HazardTree" / Hazardous Traps (destroyable object - player collision bounding box)
		{0x001373D8,"trz"},// Trigger Zones (player collision - bounding boxes) / Destroyable object (bounding boxes that receive the damage)
		{0x0010ABBD,"hz2"},// List of planes that shape the stage mesh(es) collision.

		//Drawing Graphics System (DG_System)
		{0x003B12FB,"mdz"},// 3D Model
		{0x001E5452,"tex"},// Texture (Image Data+CLUT)
		{0x001CED17,"lit"},// Stage light
		{0x066E60C1,"flw"},// Flow line used on every Orbital frame depending of the health (red,yellow,blue)
		{0x038AB1D7,"mfl"},// Motion flow
		{0x00104639,"ric"},// ps2 icon

		//SPR_System (SPRITES)
		{0x000054D5,"o2d"},// 2D Layout

		//GM_InitSystem (Game)
		{0x03696528,"fnt"}, // Font ?
		{0x00037E6B,"eft"}, // Sound effect ?
		{0x00000E56,"var"}, //SCX Related ?
		{0x0000321E,"rpd"}, //VR ?
		{0x00CB3B7A,"cvz"}, //List of vertices for basic models (mostly debris spawned from an object being destroyed eg: Calisto ice etc)
		{0x0688D9EB,"ene"}, //Enemies starting position
		{0x398168E9,"row"}, //Image
	};
	std::string GetStringFromHash(uint32_t nStringHashed);
	uint32_t HashStr(const char* str, bool Is_ByteSwapped);
	uint32_t HashStr(const char* str);
	void LogMessage(std::string Message);
	size_t nBytesRemaining(void* CurrentAddress, void* StartingAddress);
	size_t PadMemory(std::ofstream& File);
	inline bool Is_VerboseEnabled = false;
	void SaveFile(std::filesystem::path& OutPath, void* pFileData, size_t FileSize);
	void Generate_Hash_GameString(std::string StageName);
	bool Setup_HashList();
	bool Is_ValidFile_Extension(std::string ExtensionName);
}