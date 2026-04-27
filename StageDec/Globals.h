#pragma once
#define PS2_DISC_SECTOR_SIZE 2048
#define LCG_MULTIPLIER 0x2E90EDD
namespace Globals
{
	inline std::vector<Stage> AllStages;
	inline std::map<UINT, std::string> Sorted_HashMap_GameString;
	inline std::map<UINT, std::string> Sorted_HashMap_File_Extension
	{
		// Scene  Init / Mission Script
		{0x62877D7F,"bin"}, // Scene Binary initialization
		{0x000863BE,"scx"}, // Scene Compiled Bytecode (scripting mission objective, events, global game variables, setting up the Stage, compiled to bytecode)

		//Motion System (MT_SYSTEM)
		{0x00ED22C7,"mtl"}, // Motion Table, same function as mtz, Should be a giant list of Quaternions for each frame of the animation (for bone transformations)
		{0x0000856A,"mts"}, // Motion Stream (Pointer to the actual motion data) 
		{0x003B530D,"mtz"},

		//Hazardous Core (HZD_CORE) Stage + Character Collision + Used for the AI Path finding system and destroyable objects
		{0x0037B1E7,"hzt"},// HZD_TREE, "HazardTree"
		{0x001373D8,"trz"},
		{0x0010ABBD,"hz2"},

		//Drawing Graphics System (DG_System)
		{0x003B12FB,"mdz"},// 3D Model
		{0x001E5452,"tex"},// Texture (Image Data+CLUT)
		{0x001CED17,"lit"},// Stage lighting
		{0x066E60C1,"flw"},// Flow Mask used on every Orbital frame depending of the health (red,yellow,blue)
		{0x038AB1D7,"mfl"},// Mask flow ?
		{0x00104639,"ric"},// ps2 icon

		//SPR_System (SPRITES)
		{0x000054D5,"o2d"},// 2D Widget (UI)

		//GM_InitSystem (Game Manager ?)
		{0x03696528,"fnt"}, // Font ?
		{0x00037E6B,"eft"}, // Sound effect ?
		{0x00000E56,"var"},
		{0x0000321E,"rpd"},
		{0x00CB3B7A,"cvz"}, // Perhaps an encrypted .csv ?
		{0x0688D9EB,"ene"},
		{0x398168E9,"row"},
	};
	inline int g_LCG_Seed = 0;
	inline int g_LCG_Key = 0;
	inline Header* pStageDat_Header = nullptr;
	inline std::vector<BYTE> g_STAGE_DAT;
}