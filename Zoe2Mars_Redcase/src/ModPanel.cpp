#include "pch.h"
#include "GameTypes/Jehuty/Jehuty.h"
#include "ModPanel.h"
#include "PtrTypedefs.h"
#include "SoundTest.h"

namespace ModPanel
{
	static int TextSeparator_Y_Spacing = 10;
	std::vector<LevelEntry*> SpecialLevels
	{
		new LevelEntry("ntitle",0,"Main Menu"),
		new LevelEntry("vstitle",0,"Versus Mode"),
		new LevelEntry("result",0,"Result (story completed)"),
		new LevelEntry("extitle",99999,"Extra Missions"),
	};
	std::vector<LevelEntry*> AllStoryLevels
	{
		new LevelEntry("cad1",0,"Callisto Demo - Game Start (FMV)"),
		new LevelEntry("ca01",4,"Callisto - Lev mini-game"),
		new LevelEntry("cad1",5,"Callisto Demo - Jehuty (Demo)"),
		new LevelEntry("cad1",13,"Callisto Demo - Start"),
		new LevelEntry("ca00",16,"Callisto - Ardjet intro (FMV)"),
		new LevelEntry("ca00",19,"Callisto - Mosquito's tutorial"),
		new LevelEntry("ca00",20,"Callisto - Rick distress signal (DEMO)"),
		new LevelEntry("ca01",21,"Callisto - Bahram ship approaching (DEMO)"),
		new LevelEntry("ca01",22,"Callisto - Start (Save Rick)"),
		new LevelEntry("ca01",23,"Callisto - Raptors released from Bahram cargo (DEMO)"),
		new LevelEntry("ca01",24,"Callisto - kill to progress"),
		new LevelEntry("ca01",25,"Callisto - Raptors released from Bahram cargo (DEMO)"),
		new LevelEntry("ca01",26,"Callisto - kill to progress"),
		new LevelEntry("ca02",30,"Callisto - VS Ardjet-1 (FMV)"),
		new LevelEntry("ca02",34,"Callisto - VS Ardjet-1"),
		new LevelEntry("ca02",35,"Callisto - Ardjet escaping to Bahram cargo (FMV)"),
		new LevelEntry("ba01",38,"Bahram Airship - Jehuty entering (DEMO)"),
		new LevelEntry("ba01",39,"Bahram Airship - Start"),
		new LevelEntry("ba01",40,"Bahram Airship - Tutorial grabbing object (DEMO)"),
		new LevelEntry("ba01",43,"Bahram Airship - Kill to progress"),
		new LevelEntry("ba01",45,"Bahram Airship - Mummyhead intro (DEMO)"),
		new LevelEntry("ba01",46,"Bahram Airship - Kill to progress"),
		new LevelEntry("ba01",48,"Bahram Airship - Jehuty entering the corridor with the view on Mars (DEMO)"),
		new LevelEntry("ba01",49,"Bahram Airship - Start (Corridor)"),
		new LevelEntry("ba02",49,"Bahram Airship - Start (Past the door)"),
		new LevelEntry("ba02",50,"Bahram Airship - VS Nephtis-1 (DEMO)"),
		new LevelEntry("ba02",51,"Bahram Airship - VS Nephtis-1"),
		new LevelEntry("ba02",54,"Bahram Airship - Nephtis defeated (DEMO)"),
		new LevelEntry("ba02",56,"Bahram Airship - Start (After Nephtis defeat)"),
		new LevelEntry("ba03",57,"Bahram Airship - VS Anubis-1 (DEMO)"),
		new LevelEntry("ba03",58,"Bahram Airship - VS Anubis-1"),
		new LevelEntry("ba03",59,"Bahram Airship - Jehuty is defeated (DEMO)"),
		new LevelEntry("ba03",61,"Bahram Airship - Nohman proposing Dingo to join back Bahram (FMV)"),
		new LevelEntry("ba04",62,"Bahram Airship - (FMV)"),
		new LevelEntry("ba04",64,"Bahram Airship - (DEMO)"),
		new LevelEntry("ba04",67,"Bahram Airship - Start"),
		new LevelEntry("ba04",68,"Bahram Airship - Entering Deimos station (DEMO)"),
		new LevelEntry("ds01",69,"Deimos Station - Start"),
		new LevelEntry("ds00",70,"Deimos Station - Cyclops intro (DEMO)"),
		new LevelEntry("ds00",71,"Deimos Station - Start"),
		new LevelEntry("ds00g",72,"Deimos Station - Manta + scanner intro (DEMO)"),
		new LevelEntry("ds00g",73,"Deimos Station - Start"),
		new LevelEntry("ds01",75,"Deimos Station - Taper SOS (DEMO)"),
		new LevelEntry("ds01g",76,"Deimos Station - Start (Search Taper)"),
		new LevelEntry("ds01",78,"Deimos Station - Found Taper (DEMO)"),
		new LevelEntry("ds01",83,"Deimos Station - Obtain \"Geyser\" (DEMO)"),
		new LevelEntry("ds02",85,"Deimos Station - Start (Escort Taper)"),
		new LevelEntry("ds03",86,"Deimos Station - Taper leaving deimos part 1 (DEMO)"),
		new LevelEntry("ds03",87,"Deimos Station - Taper leaving deimos part 2 (DEMO)"),
		new LevelEntry("ds03",88,"Deimos Station - Taper leaving deimos part 3, enemies approaching (DEMO)"),
		new LevelEntry("ds03",89,"Deimos Station - Start"),
		new LevelEntry("ds03",90,"Deimos Station - Leaving to Mars (DEMO)"),
		new LevelEntry("wn00",93,"Wilderness - Intro (FMV)"),
		new LevelEntry("wn00",94,"Wilderness - Ken talking to Dingo (DEMO+FMV)"),
		new LevelEntry("wn00",96,"Wilderness - Start"),
		new LevelEntry("wn01",97,"Wilderness - VicViper (DEMO)"),
		new LevelEntry("wn01",98,"Wilderness - VS VicViper"),
		new LevelEntry("wn01",99,"Wilderness - VicViper defeated (FMV)"),
		new LevelEntry("wn01",103,"Wilderness - obtain \"Gauntlet\" (DEMO)"),
		new LevelEntry("wn01",105,"Wilderness - VicViper leaving, Bahram fleet passing by (DEMO)"),
		new LevelEntry("wn02",107,"Wilderness - Start"),
		new LevelEntry("cy01",109,"Canyon - Start"),
		new LevelEntry("cy02",113,"Canyon - VS Nephtis-2 (DEMO)"),
		new LevelEntry("cy02",114,"Canyon - VS Nephtis-2"),
		new LevelEntry("cy02",118,"Canyon - Nephtis grabbing Jehuty and thrusting through wall (DEMO)"),
		new LevelEntry("vc01",120,"Vascilia City - VS Nephtis-3 (DEMO)"),
		new LevelEntry("vc01",121,"Vascilia City - VS Nephtis-3"),
		new LevelEntry("vc01",124,"Vascilia City - Nephtis-3 defeated, obtain \"Comet\" (DEMO)"),
		new LevelEntry("vc02",125,"Vascilia City - Taper & LEV's deploying to guard the city (DEMO)"),
		new LevelEntry("vc02",128,"Vascilia City - Defend the city"),
		new LevelEntry("vc02",129,"Vascilia City - Finished to defend"),
		new LevelEntry("vc02",130,"Vascilia City - Generic talk after the defense (DEMO)"),
		new LevelEntry("vc02",134,"Vascilia City - Air's Cliff Tunnel, train powering on (DEMO)"),
		new LevelEntry("vc03",136,"Vascilia City - Start"),
		new LevelEntry("vc03",137,"Vascilia City - Jehuty ready to chase (DEMO)"),
		new LevelEntry("tr01",138,"Train - Jehuty catching up with the train (DEMO)"),
		new LevelEntry("tr01",139,"Train - Start"),
		new LevelEntry("tr01",140,"Train - Derailing (DEMO)"),
		new LevelEntry("tr01",141,"Train - Obtain \"Decoy\" (DEMO)"),
		new LevelEntry("tr02",142,"Train - To Energy Facility (DEMO)"),
		new LevelEntry("tr02",144,"Train - To Energy Facility Start"),
		new LevelEntry("ef01",146,"Energy Facility - Nephtis (DEMO)"),
		new LevelEntry("ef01",147,"Energy Facility - VS Nephtis"),
		new LevelEntry("ef02",149,"Energy Facility - Nephthis defeated, obtain \"Vector Canon\" (DEMO)"),
		new LevelEntry("ef02",150,"Energy Facility - Viola inject her A.I and hack Ardjet (DEMO)"),
		new LevelEntry("ef02g",153,"Energy Facility - VS Viola A.I"),
		new LevelEntry("ef02",155,"Energy Facility - Viola A.I deleted (DEMO)"),
		new LevelEntry("ef03",156,"Energy Facility - Obtain \"Wisp\" (DEMO)"),
		new LevelEntry("ef03",157,"Energy Facility - Carry Ardjet to the top"),
		new LevelEntry("ef03",158,"Energy Facility - Reached the top (DEMO)"),
		new LevelEntry("ef03",159,"Energy Facility - Ken talking (DEMO)"),
		new LevelEntry("hed1",162,"Heights Demo (DEMO)"),
		new LevelEntry("hed1",163,"Heights Demo (DEMO)"),
		new LevelEntry("he01",164,"Heights - (DEMO)"),
		new LevelEntry("he01",165,"Heights - (DEMO)"),
		new LevelEntry("he01",166,"Heights - Going to Inhert fortress (DEMO)"),
		new LevelEntry("he01",167,"Heights - Inhert intro (DEMO)"),
		new LevelEntry("fc01",168,"Fortress City - Inhert sending enemies (DEMO)"),
		new LevelEntry("fc01",169,"Fortress City - Kill to progress"),
		new LevelEntry("fc01",170,"Fortress City - Descending within the fortress (DEMO)"),
		new LevelEntry("fr01",171,"Fortress - Start"),
		new LevelEntry("fr01c",172,"Fortress - Start"),
		new LevelEntry("fr02",173,"Fortress - Inhert talking about compressed space (DEMO+FMV)"),
		new LevelEntry("fr02",176,"Fortress - VS Inhert-1 (DEMO)"),
		new LevelEntry("fr02",177,"Fortress - VS Inhert-1"),
		new LevelEntry("fr02",178,"Fortress - VS Inhert-2 (DEMO)"),
		new LevelEntry("fr02",179,"Fortress - ADA will assist you in detecting the attacks (DEMO)"),
		new LevelEntry("fr02",180,"Fortress - VS Inhert-2"),
		new LevelEntry("fr03",181,"Fortress - Inhert defeated (DEMO)"),
		new LevelEntry("fr03",182,"Fortress - Obtain \"Homing Missile\" (DEMO+FMV)"),
		new LevelEntry("fr04",185,"Fortress - Obtain \"Zero Shift\" and meet Anubis (DEMO)"),
		new LevelEntry("fr04g",186,"Fortress - VS Anubis-2"),
		new LevelEntry("fr05",187,"Fortress - Jehuty escaping Anubis (DEMO)"),
		new LevelEntry("fr05",188,"Fortress - Jehuty leaving fortress city (DEMO)"),
		new LevelEntry("af01",189,"AirFight - Intro (DEMO)"),
		new LevelEntry("af01",190,"AirFight - Fleet intro (DEMO)"),
		new LevelEntry("af01",191,"AirFight - ADA instructing on how to use the \"Vector Cannon\" (DEMO)"),
		new LevelEntry("af01g",192,"AirFight - Kill to progress"),
		new LevelEntry("af02",193,"AirFight - VS Zakat (DEMO+FMV)"),
		new LevelEntry("af02",195,"AirFight - VS Zakat"),
		new LevelEntry("af02",196,"AirFight - Zakat defeated, obtain \"F-Mine\" (DEMO+FMV)"),
		new LevelEntry("cf01",198,"Confused Fight - (FMV)"),
		new LevelEntry("cf01",199,"Confused Fight - (DEMO)"),
		new LevelEntry("cf01",201,"Confused Fight - Elena/Leo speech (DEMO+FMV)"),
		new LevelEntry("cf01",211,"Confused Fight - Aumaan Entrance - Start"),
		new LevelEntry("cf01",212,"Confused Fight - Aumaan Entrance - (DEMO)"),
		new LevelEntry("cf01",213,"Confused Fight - Aumaan Entrance - (FMV)"),
		new LevelEntry("cf01",215,"Confused Fight - Aumaan Entrance - (DEMO)"),
		new LevelEntry("mf01",216,"Materials F - intro (DEMO)"),
		new LevelEntry("mf01",219,"Materials F - Start"),
		new LevelEntry("mf01c",220,"Materials F - Ken talking (DEMO)"),
		new LevelEntry("mf01c",221,"Materials F - Kill to progress"),
		new LevelEntry("mf01c",222,"Materials F - Leopardo intro (DEMO)"),
		new LevelEntry("mf01c",223,"Materials F - Kill to progress"),
		new LevelEntry("mf02",224,"Materials F - Destroy the catapult"),
		new LevelEntry("mf02",225,"Materials F - Entering the room in front of Aumaan Wall (DEMO)"),
		new LevelEntry("mf02",226,"Materials F - Entering the room in front of Aumaan Wall (DEMO)"),
		new LevelEntry("mf02",227,"Materials F - ADA speech (DEMO)"),
		new LevelEntry("mf02",230,"Materials F - Destroy the exterior shield"),
		new LevelEntry("fa01",231,"Fortress Aumaan - VS Anubis-3 (DEMO)"),
		new LevelEntry("fa01",232,"Fortress Aumaan - VS Anubis-3"),
		new LevelEntry("fad1",233,"Fortress Aumaan Demo - VicViper (DEMO+FMV)"),
		new LevelEntry("csd1",236,"Compressed Space Demo - VS Anubis-4 (DEMO+FMV)"),           // res2-3 only due to missing dependencies
		new LevelEntry("cs01",240,"Compressed Space - VS Anubis-4"),                           // res2-3 only due to missing dependencies
		new LevelEntry("am01",241,"Aumaan - Anubis carried there (DEMO)"),                     // res3 only up to the end of the game due to missing dependencies
		new LevelEntry("am01",244,"Aumaan - Start"),
		new LevelEntry("am01",245,"Aumaan - Anubis escaping to Aumaan Core (DEMO)"),
		new LevelEntry("am01f",247,"Aumaan - Kill to progress"),
		new LevelEntry("am01g",248,"Aumaan - Nephthis clones intro (DEMO)"),
		new LevelEntry("am01g",249,"Aumaan - Kill to progress"),
		new LevelEntry("am01g",250,"Aumaan - To Aumaan Core (DEMO)"),
		new LevelEntry("am02",251,"Aumaan - VS Aumaan Anubis (DEMO)"),
		new LevelEntry("am02",253,"Aumaan - VS Aumaan Anubis"),
		new LevelEntry("am02",254,"Aumaan - Aumaan Anubis defeated (Demo)"),
		new LevelEntry("am02",255,"Aumaan - Kill Nohman"),
		new LevelEntry("amd2",256,"Aumaan Demo - (DEMO+FMV)"),
		new LevelEntry("amd2",267,"Aumaan Demo - (DEMO+FMV)"),
		new LevelEntry("ex01",353,"EXTRA 1 - Hidden passge leading to unlock Aumaan Anubis"),
		new LevelEntry("ex02",353,"EXTRA 2 - VS Aumaan Anubis"),
		new LevelEntry("ex03",353,"EXTRA 3 - VS Naked Jehuty"),
	};
	void DrawLevelSelection_SpecialStages()
	{
		if (ImGui::BeginTable("##04", 3, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
		{
			ImGui::TableSetupColumn("Stage", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("StoryFlag", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Description");
			ImGui::TableHeadersRow();
			for (int LevelIndex = 0; LevelIndex < ModPanel::SpecialLevels.size(); LevelIndex++)
			{
				ImGui::PushID(LevelIndex);
				for (int row = 0; row < 1; row++)
				{
					ImGui::TableNextRow(row);
					for (int column = 0; column < 3; column++)
					{
						ImGui::TableSetColumnIndex(column);
						switch (column)
						{
						case 0:ImGui::TextUnformatted(ModPanel::SpecialLevels[LevelIndex]->StageName.data());
							break;
						case 1:ImGui::TextUnformatted(std::to_string(ModPanel::SpecialLevels[LevelIndex]->g_nStoryFlag).c_str());
							break;
						case 2:
						{
							if (ImGui::Selectable(ModPanel::SpecialLevels[LevelIndex]->LevelDescription.c_str()))
							{
								strcpy_s(PtrTypedefs::GameOffsets::g_CurrentStage, 16, ModPanel::SpecialLevels[LevelIndex]->StageName.data());
								*PtrTypedefs::GameOffsets::g_nStoryFlag = ModPanel::SpecialLevels[LevelIndex]->g_nStoryFlag;
								strcpy_s(PtrTypedefs::GameOffsets::g_MainResident_ResourceName, 16, "res01");
								Send_LoadRequest(4113);
							}
						}
						}
					}
				}
				ImGui::PopID();
			}
			ImGui::EndTable();
		}
	}
	static void DrawLevelSelection_AllStoryLevels()
	{
		if (ImGui::BeginTable("##03", 3, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
		{
			ImGui::TableSetupColumn("Stage", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("StoryFlag", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Description");
			ImGui::TableHeadersRow();
			for (int LevelIndex = 0; LevelIndex < ModPanel::AllStoryLevels.size(); LevelIndex++)
			{
				ImGui::PushID(LevelIndex);
				for (int row = 0; row < 1; row++)
				{
					ImGui::TableNextRow(row);
					for (int column = 0; column < 3; column++)
					{
						ImGui::TableSetColumnIndex(column);
						switch (column)
						{
						case 0:ImGui::TextUnformatted(ModPanel::AllStoryLevels[LevelIndex]->StageName.data());
							break;
						case 1:ImGui::TextUnformatted(std::to_string(ModPanel::AllStoryLevels[LevelIndex]->g_nStoryFlag).data());
							break;
						case 2:
						{
							if (ImGui::Selectable(ModPanel::AllStoryLevels[LevelIndex]->LevelDescription.data()))
							{
								if (ModPanel::AllStoryLevels[LevelIndex]->g_nStoryFlag >= 236 && ModPanel::AllStoryLevels[LevelIndex]->g_nStoryFlag <= 240)
								{
									if (*PtrTypedefs::GameOffsets::Jehuty::g_pCharacterFlags == GameTypes::Jehuty::JEHUTY_DAMAGED)
									{
										strcpy_s(PtrTypedefs::GameOffsets::g_MainResident_ResourceName, 16, "res02");
									}
									else
									{
										strcpy_s(PtrTypedefs::GameOffsets::g_MainResident_ResourceName, 16, "res03");
									}
								}
								else if (ModPanel::AllStoryLevels[LevelIndex]->g_nStoryFlag >= 241 && ModPanel::AllStoryLevels[LevelIndex]->g_nStoryFlag <= 267)
								{
									strcpy_s(PtrTypedefs::GameOffsets::g_MainResident_ResourceName, 16, "res03");
								}
								strcpy_s(PtrTypedefs::GameOffsets::g_CurrentStage, 16, ModPanel::AllStoryLevels[LevelIndex]->StageName.c_str());
								*PtrTypedefs::GameOffsets::g_nStoryFlag = ModPanel::AllStoryLevels[LevelIndex]->g_nStoryFlag;
								Send_LoadRequest(4113);
							}
						}
						}
					}
				}
				ImGui::PopID();
			}
			ImGui::EndTable();
		}
	}
	void DrawLevelSelection()
	{
		if (ImGui::BeginChild("LevelWindow", ImVec2(), ImGuiChildFlags_Borders))
		{
			DrawLevelSelection_SpecialStages();
			DrawLevelSelection_AllStoryLevels();
		}
		ImGui::EndChild();
	}
	void DrawMainTab()
	{
		static const char* Difficulty_Elements[] = { "Very Easy","Easy","Normal","Hard","Very Hard","Super Hard","Extreme" };
		if (ImGui::BeginTabItem("Main"))
		{
			ImGui::SeparatorText("Story Mode");
			ImGui::Text("Current EXP : %d/4950000", *PtrTypedefs::GameOffsets::Jehuty::g_CurrentExp);
			ImGui::SameLine();
			if (ImGui::SmallButton("Max"))
			{
				*PtrTypedefs::GameOffsets::Jehuty::g_CurrentExp = 4950000;
			}
			ImGui::SameLine();
			if (ImGui::SmallButton("Give all subweapons"))
			{
				PtrTypedefs::GameOffsets::Jehuty::pSubWeapons->Is_Geyser_Owned = true;
				PtrTypedefs::GameOffsets::Jehuty::pSubWeapons->Is_Comet_Owned = true;
				PtrTypedefs::GameOffsets::Jehuty::pSubWeapons->Is_Gauntlet_Owned = true;
				PtrTypedefs::GameOffsets::Jehuty::pSubWeapons->Is_Decoy_Owned = true;
				PtrTypedefs::GameOffsets::Jehuty::pSubWeapons->Is_VectorCannon_Owned = true;
				PtrTypedefs::GameOffsets::Jehuty::pSubWeapons->Is_HomingMissile_Owned = true;
				PtrTypedefs::GameOffsets::Jehuty::pSubWeapons->Is_Wisp_Owned = true;
				PtrTypedefs::GameOffsets::Jehuty::pSubWeapons->Is_FloatingMine_Owned = true;
				PtrTypedefs::GameOffsets::Jehuty::pSubWeapons->Is_ZeroShift_Owned = true;
				PtrTypedefs::GameOffsets::Jehuty::pSubWeapons->Is_Phalanx_Owned = true;
				PtrTypedefs::GameOffsets::Jehuty::pSubWeapons->Is_Halberd_Owned = true;
				PtrTypedefs::GameOffsets::Jehuty::pSubWeapons->Is_Mummy_Owned = true;
			}
			ImGui::Text("Difficulty : %s", Difficulty_Elements[*PtrTypedefs::GameOffsets::g_Difficulty]);
			ImGui::Text("Jehuty Flags : %d", *PtrTypedefs::GameOffsets::Jehuty::g_pCharacterFlags);
			Utility::CreateTooltip_OnSameLine("Affect which Jehuty version you control\nRequire specific \"resident\" stage to load Jehuty Naked / Damaged.");
			ImGui::Text("Change Story Character");
			Utility::CreateTooltip_OnSameLine(R"(Starting from the "Compressed Space" (VS Anubis) You can only play as Naked/Damaged Jehuty.
After the "Compressed Space" level, you can only play as Naked Jehuty.

Do not use outside of the story.
Do not restart/change the stage during DEMO/FMV.)");
			// Change Story Character
			{
				static int SelectedIndex = 0;
				static const char* CharacterFlags_ItemLabel[] = { "Jehuty","Jehuty V2","Damaged Jehuty","Naked Jehuty" };
				if (ImGui::BeginCombo("##01", CharacterFlags_ItemLabel[SelectedIndex]))
				{
					for (int i = 0; i < ARRAYSIZE(CharacterFlags_ItemLabel); i++)
					{
						const bool Is_Selected = (SelectedIndex == i);
						if (ImGui::Selectable(CharacterFlags_ItemLabel[i], Is_Selected))
						{
							SelectedIndex = i;
						}
						if (Is_Selected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				ImGui::SameLine();
				if (ImGui::Button("Set"))
				{
					switch (SelectedIndex)
					{
					case 0:
					{
						if (*PtrTypedefs::GameOffsets::g_nStoryFlag >= 236 && *PtrTypedefs::GameOffsets::g_nStoryFlag <= 267)
						{
							break;
						}
						*PtrTypedefs::GameOffsets::Jehuty::g_pCharacterFlags = GameTypes::Jehuty::E_kCharacterFlags::JEHUTY;
						strcpy_s(PtrTypedefs::GameOffsets::g_MainResident_ResourceName, 16, "res01");
						break;
					}
					case 1:
					{
						if (*PtrTypedefs::GameOffsets::g_nStoryFlag >= 236 && *PtrTypedefs::GameOffsets::g_nStoryFlag <= 267)
						{
							break;
						}
						*PtrTypedefs::GameOffsets::Jehuty::g_pCharacterFlags = GameTypes::Jehuty::E_kCharacterFlags::JEHUTY_V2;
						strcpy_s(PtrTypedefs::GameOffsets::g_MainResident_ResourceName, 16, "res01");
						break;
					}
					case 2:
					{
						*PtrTypedefs::GameOffsets::Jehuty::g_pCharacterFlags = GameTypes::Jehuty::E_kCharacterFlags::JEHUTY_DAMAGED;
						strcpy_s(PtrTypedefs::GameOffsets::g_MainResident_ResourceName, 16, "res02");
						break;
					}
					case 3:
					{
						*PtrTypedefs::GameOffsets::Jehuty::g_pCharacterFlags = GameTypes::Jehuty::E_kCharacterFlags::JEHUTY_NAKED;
						strcpy_s(PtrTypedefs::GameOffsets::g_MainResident_ResourceName, 16, "res03");
						break;
					}
					}
					Send_LoadRequest(4113);
				}
			}
			ImGui::SliderInt("Difficulty", &ModPanel::DifficultyIndex, 0, 6, Difficulty_Elements[ModPanel::DifficultyIndex], ImGuiSliderFlags_NoInput);
			Utility::CreateTooltip_OnSameLine("Affect certain skills in versus mode,\nIt won't work in the Extra Mission due to the SCX (scripting system) rolling back to Normal difficulty.\nUse the restart scene button to apply.");
			ImGui::Dummy(ImVec2(0, ModPanel::TextSeparator_Y_Spacing));
			ImGui::SeparatorText("Stage Info");
			{
				ImGui::Text("Stage : %s", PtrTypedefs::GameOffsets::g_CurrentStage);
				ImGui::Text("StoryFlag : %d", *PtrTypedefs::GameOffsets::g_nStoryFlag);
				ImGui::Text("Primary Resident : %s", PtrTypedefs::GameOffsets::g_MainResident_ResourceName);
				ImGui::Text("Load Request : %d", *PtrTypedefs::GameOffsets::g_nLoadRequest);
				if (ImGui::SmallButton("Restart scene"))
				{
					Send_LoadRequest(18);
				}
				Utility::CreateTooltip_OnSameLine("Restart the scene without reloading the stage data\n\nDo not restart/change the stage during DEMO/FMV.");
				ImGui::SameLine();
				if (ImGui::SmallButton("Restart Stage"))
				{
					Send_LoadRequest(4113);
				}
				Utility::CreateTooltip_OnSameLine("Restart the entire stage and save SCX variables\n\nDo not restart/change the stage during DEMO/FMV.");
			}
			ImGui::Dummy(ImVec2(0, TextSeparator_Y_Spacing));
			ImGui::SeparatorText("All Story Levels / Specials");
			ImGui::TextUnformatted("Click on the stage description to load it.");
			DrawLevelSelection();
			ImGui::EndTabItem();
		}
	}
	void DrawSettingsTab()
	{
		if (ImGui::BeginTabItem("Settings"))
		{
			static const char* FontSize_UI_Elements[] = { "12px","18px","24px", "36px", "48px", "60px", "72px" };
			static const char* Theme_UI_Elements[] = { "ZOE","Dark","Light","Classic" };
			static const char* MainMenu_Soundtrack_Elements[] = { "Default","ZOE1 Intro","ZOE1 Main Menu" };
			if (ImGui::SliderInt("Font Size", &ModPanel::Settings_FontSize, 0, pFont.size() - 1, FontSize_UI_Elements[ModPanel::Settings_FontSize], ImGuiSliderFlags_NoInput))
			{
				std::wstring FontSize_Str = std::to_wstring(ModPanel::Settings_FontSize);
				WritePrivateProfileStringW(L"Settings", L"FontSize", FontSize_Str.c_str(), ModPanel::Settings_FilePath);
				Is_FontResized = true;
			}
			if (ImGui::SliderInt("Theme", &ModPanel::Settings_Theme, 0, ARRAYSIZE(Theme_UI_Elements) - 1, Theme_UI_Elements[ModPanel::Settings_Theme], ImGuiSliderFlags_NoInput))
			{
				std::wstring Theme_Str = std::to_wstring(Settings_Theme);
				WritePrivateProfileStringW(L"Settings", L"Theme", Theme_Str.c_str(), ModPanel::Settings_FilePath);
				ApplyTheme(ModPanel::Settings_Theme);
			}
			if (ImGui::SliderInt("Main Menu Soundtrack", &ModPanel::Settings_MainMenu_Soundtrack, 0, ARRAYSIZE(MainMenu_Soundtrack_Elements) - 1, MainMenu_Soundtrack_Elements[ModPanel::Settings_MainMenu_Soundtrack], ImGuiSliderFlags_NoInput))
			{
				std::wstring Theme_Str = std::to_wstring(Settings_MainMenu_Soundtrack);
				WritePrivateProfileStringW(L"Settings", L"MainMenu_Soundtrack", Theme_Str.c_str(), ModPanel::Settings_FilePath);
			}
			if (ImGui::SliderFloat("Global Alpha", &ModPanel::Settings_GlobalAlpha, 0.2f, 1.0f, "%.1f", ImGuiSliderFlags_NoInput))
			{
				std::wstring GlobalAlpha_Str = std::to_wstring(ModPanel::Settings_GlobalAlpha);
				WritePrivateProfileStringW(L"Settings", L"GlobalAlpha", GlobalAlpha_Str.c_str(), Settings_FilePath);
				ModPanel::MainStyle->Alpha = ModPanel::Settings_GlobalAlpha;
			}
			ImGui::SeparatorText("Experimental Features");
			if (ImGui::Checkbox("Intercept files from Cygames .pack", &ModPanel::Is_CygamesFiles_Intercepted))
			{
				std::wstring Intercept_Cygames_files = std::to_wstring(ModPanel::Is_CygamesFiles_Intercepted);
				WritePrivateProfileStringW(L"Settings", L"Intercept_Cygames_files", Intercept_Cygames_files.c_str(), Settings_FilePath);
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::SetItemTooltip(
					R"(Automatically extract the files as the game load/decompress them only when needed,
If the file doesn't exist it will extract it at the appropriate path.
				
If the file exist it will instead override it with your own file and override the decompressed file at the memory address with the updated file size.
				
If you wish to simply make the game directly load your files, extract all cygames .pack and move them in a backup folder,
the game will instead look for the files in the game directory.)");
			}
			if (ImGui::Checkbox("Renderdoc Injection", &ModPanel::Settings_Renderdoc))
			{
				WritePrivateProfileStringW(L"Settings", L"Renderdoc", std::to_wstring(ModPanel::Settings_Renderdoc).c_str(), ModPanel::Settings_FilePath);
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::SetItemTooltip("Required to have \"renderdoc.dll\" in the game directory,\nOnce injected at game startup, you can open Renderdoc then select \"File\" -> \"Attach to a running instance\".");
			}
			ImGui::EndTabItem();
		}
	}
	static void DrawSoundTestTab()
	{
		if (ImGui::BeginTabItem("Sound Test"))
		{
			ImGui::TextUnformatted("Using it will kill all sounds channel.");
			if (ImGui::BeginChild("SoundTest",ImVec2(),ImGuiChildFlags_Borders))
			{
				for (int StageIndex = 0; StageIndex < SoundTest::SoundsFile.SoundBanks.size(); StageIndex++)
				{
					auto& SoundBank = SoundTest::SoundsFile.SoundBanks;
					ImGui::PushID(StageIndex);
					if (ImGui::CollapsingHeader(SoundBank[StageIndex].Stage.has_value() ? SoundBank[StageIndex].Stage->data() : SoundBank[StageIndex].Bank_Name.data()))
					{
						if (ImGui::BeginTable("Event Table", 1, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
						{
							ImGui::TableSetupColumn("Event Name");
							ImGui::TableHeadersRow();
							for (int EventIdx = 0; EventIdx < SoundBank[StageIndex].Play_Events.size(); EventIdx++)
							{
								ImGui::TableNextRow();
								ImGui::TableSetColumnIndex(0);
								if (ImGui::Selectable(SoundBank[StageIndex].Play_Events[EventIdx].data()))
								{
									PtrTypedefs::WWISE::SoundEngine::PostEvent("Stop_All", 0u, 0u, nullptr, nullptr, 0u, nullptr, 0u);
									PtrTypedefs::WWISE::SoundEngine::PostEvent(SoundBank[StageIndex].Play_Events[EventIdx].c_str(), Mod::Custom_AkGameObj, 0u, nullptr, nullptr, 0u, nullptr, 0u);
								}
							}
							ImGui::EndTable();
						}
					}
					ImGui::PopID();
				}
			}
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
	}
	void DrawLogTab()
	{
		static bool Log_Checkbox = false;
		if (ImGui::BeginTabItem("Log"))
		{
			if (ImGui::Checkbox("Enable Logging", &Log_Checkbox))
			{
				Is_LogEnabled = !Is_LogEnabled;
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear"))
			{
				Log_String.str(std::string());
			}
			if (ImGui::BeginChild("LogWindow", ImVec2(), ImGuiChildFlags_Borders))
			{
				ImGui::TextUnformatted(Log_String.str().data());
			}
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
	}
	void DrawMainWindow()
	{
		if (ImGui::Begin("ZONE OF THE ENDERS THE 2nd RUNNER MARS", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse))
		{
			if (ImGui::BeginTabBar("TabBar1", ImGuiTabBarFlags_None))
			{
				DrawMainTab();
				DrawSettingsTab();
				DrawSoundTestTab();
				DrawLogTab();
				ImGui::EndTabBar();
			}
		}
		ImGui::End();
	}

	void Send_LoadRequest(int num)
	{
		if (!*PtrTypedefs::GameOffsets::g_nLoadRequest)
		{
			if (*PtrTypedefs::GameOffsets::g_Difficulty != ModPanel::DifficultyIndex)
			{
				*PtrTypedefs::GameOffsets::g_Difficulty = ModPanel::DifficultyIndex;
			}
			if (PtrTypedefs::GameOffsets::g_CurrentStage != "ntitle")
			{
				PtrTypedefs::WWISE::SoundEngine::PostEvent("Stop_MainTheme", 0, 0u, nullptr, nullptr, 0u, nullptr, 0);
			}
			*PtrTypedefs::GameOffsets::g_nLoadRequest = num;
		}
	}

	void ApplyTheme(int nThemeIndex)
	{
		switch (Settings_Theme)
		{
		case 0:
		{
			ImVec4* colors = ImGui::GetStyle().Colors;
			colors[ImGuiCol_Text] = ImVec4(0.64f, 1.00f, 0.98f, 1.00f);
			colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
			colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			colors[ImGuiCol_PopupBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_Border] = ImVec4(0.00f, 1.00f, 0.93f, 0.52f);
			colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 1.00f, 0.93f, 0.52f);
			colors[ImGuiCol_FrameBgHovered] = ImVec4(0.00f, 1.00f, 0.94f, 0.71f);
			colors[ImGuiCol_FrameBgActive] = ImVec4(0.00f, 1.00f, 0.94f, 0.78f);
			colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
			colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
			colors[ImGuiCol_ScrollbarBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 1.00f, 0.93f, 0.52f);
			colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 1.00f, 0.94f, 0.71f);
			colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 1.00f, 0.94f, 0.78f);
			colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 0.94f, 0.71f);
			colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 1.00f, 0.93f, 0.52f);
			colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 1.00f, 0.94f, 0.71f);
			colors[ImGuiCol_Button] = ImVec4(0.00f, 1.00f, 0.93f, 0.52f);
			colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 1.00f, 0.94f, 0.71f);
			colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 0.94f, 0.78f);
			colors[ImGuiCol_Header] = ImVec4(0.00f, 1.00f, 0.93f, 0.52f);
			colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 1.00f, 0.94f, 0.71f);
			colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 0.94f, 0.78f);
			colors[ImGuiCol_Separator] = ImVec4(0.00f, 1.00f, 0.93f, 0.52f);
			colors[ImGuiCol_SeparatorHovered] = ImVec4(0.00f, 1.00f, 0.94f, 0.71f);
			colors[ImGuiCol_SeparatorActive] = ImVec4(0.00f, 1.00f, 0.94f, 0.78f);
			colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 0.93f, 0.52f);
			colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 1.00f, 0.94f, 0.71f);
			colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 1.00f, 0.94f, 0.78f);
			colors[ImGuiCol_TabHovered] = ImVec4(0.00f, 1.00f, 0.94f, 0.71f);
			colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_TabSelected] = ImVec4(0.00f, 1.00f, 0.94f, 0.71f);
			colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_TabDimmed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_DockingPreview] = ImVec4(0.00f, 1.00f, 0.94f, 0.71f);
			colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
			colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
			colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 1.00f, 0.93f, 0.52f);
			colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 1.00f, 0.93f, 0.52f);
			colors[ImGuiCol_TableBorderLight] = ImVec4(0.00f, 1.00f, 0.93f, 0.52f);
			colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
			colors[ImGuiCol_TextLink] = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
			colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
			colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
			colors[ImGuiCol_NavCursor] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
			colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
			colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
			colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
		}break;
		case 1: ImGui::StyleColorsDark();    break;
		case 2: ImGui::StyleColorsLight();   break;
		case 3: ImGui::StyleColorsClassic(); break;
		}
	}

	void LoadSettings()
	{
		ModPanel::MainStyle->FrameRounding = 6.0f;
		ModPanel::MainStyle->GrabRounding = 6.0f;
		ModPanel::MainStyle->WindowRounding = 4.0f;
		ModPanel::MainStyle->ChildRounding = 6.0f;
		ModPanel::MainStyle->PopupRounding = 6.0f;
		ModPanel::MainStyle->WindowTitleAlign = ImVec2(0.5f, 0.5f);
		ModPanel::Settings_Theme = GetPrivateProfileIntW(L"Settings", L"Theme", 0, ModPanel::Settings_FilePath);
		ModPanel::Settings_MainMenu_Soundtrack = GetPrivateProfileIntW(L"Settings", L"MainMenu_Soundtrack", 0, ModPanel::Settings_FilePath);
		ModPanel::Settings_FontSize = GetPrivateProfileIntW(L"Settings", L"FontSize", 0, ModPanel::Settings_FilePath);
		ModPanel::Is_CygamesFiles_Intercepted = GetPrivateProfileIntW(L"Settings", L"Intercept_Cygames_files", false, ModPanel::Settings_FilePath);
		std::wstring GlobalAlpha_str{};
		GetPrivateProfileStringW(L"Settings", L"GlobalAlpha", L"1.0", GlobalAlpha_str.data(), 4, ModPanel::Settings_FilePath);
		float GlobalAlpha = std::stof(GlobalAlpha_str);
		if (GlobalAlpha > 1.0f)
		{
			Settings_GlobalAlpha = 1.0f;
			WritePrivateProfileStringW(L"Settings", L"GlobalAlpha", GlobalAlpha_str.c_str(), ModPanel::Settings_FilePath);
		}
		else if (GlobalAlpha < 0.2f)
		{
			Settings_GlobalAlpha = 0.2f;
			WritePrivateProfileStringW(L"Settings", L"GlobalAlpha", GlobalAlpha_str.c_str(), ModPanel::Settings_FilePath);
		}
		ModPanel::pIO->FontDefault = pFont.at(Settings_FontSize);
		ModPanel::Settings_GlobalAlpha = GlobalAlpha;
		ModPanel::MainStyle->Alpha = GlobalAlpha;
		ModPanel::ApplyTheme(Settings_Theme);

		std::ifstream JSON_SoundsFile(SoundTest::JSON_Sounds_Filename);
		json j;
		JSON_SoundsFile >> j;
		SoundTest::SoundsFile = j.get<SoundTest::JSON_SoundsFile>();
		JSON_SoundsFile.close();
	}
}