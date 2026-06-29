#pragma once
#include <AK/SoundEngine/Common/AkSoundEngine.h>
namespace ModPanel
{
	struct LevelEntry
	{
		std::string StageName = "";
		int g_nStoryFlag = 0;
		std::string LevelDescription = "";
		LevelEntry(std::string StageName, int g_nStoryFlag, std::string LevelDescription)
		{
			this->StageName = StageName;
			this->g_nStoryFlag = g_nStoryFlag;
			this->LevelDescription = LevelDescription;
		}
		LevelEntry(std::string StageName, std::string LevelDescription)
		{
			this->StageName = StageName;
			this->LevelDescription = LevelDescription;
		}
	};
	// GLOBALS
	inline const wchar_t* Settings_FilePath = L".\\ModAssets\\Settings.ini";
	inline bool Is_FontResized = false;
	inline bool Is_Initalized = false;
	inline bool Is_Hidden = false;
	inline ImGuiContext* pCtx = nullptr;
	inline ImGuiIO* pIO = nullptr;
	inline ImGuiStyle* MainStyle = nullptr;
	inline std::vector<ImFont*> pFont;
	inline int Settings_Theme = 0;
	inline int Settings_MainMenu_Soundtrack = 0;
	inline int Settings_FontSize = 0;
	inline float Settings_GlobalAlpha = 1.0f;
	inline bool Is_CygamesFiles_Intercepted = false;
	inline std::stringstream Log_String;
	inline bool Is_LogEnabled = false;
	inline int DifficultyIndex = 2;
	inline bool Settings_Renderdoc = false;

	void DrawLevelSelection_SpecialStages();
	void DrawLevelSelection();
	void DrawMainTab();
	void DrawSettingsTab();
	void DrawMainWindow();
	void DrawLogTab();
	void Send_LoadRequest(int num);
	void ApplyTheme(int nThemeIndex);
	void LoadSettings();
	namespace Utility
	{
		inline void CreateTooltip_OnSameLine(const char* TooltipText)
		{
			ImGui::SameLine();
			ImGui::TextDisabled("(?)");
			if (ImGui::IsItemHovered())
			{
				if (ImGui::BeginTooltip())
				{
					ImGui::TextUnformatted(TooltipText);
					ImGui::EndTooltip();
				}
			}
		}
		inline void CreateTooltip(const char* TooltipText)
		{
			ImGui::TextDisabled("(?)");
			if (ImGui::IsItemHovered())
			{
				if (ImGui::BeginTooltip())
				{
					ImGui::TextUnformatted(TooltipText);
					ImGui::EndTooltip();
				}
			}
		}
	}
}