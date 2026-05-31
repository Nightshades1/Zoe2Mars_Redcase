#include "pch.h"
#include "StageDec.h"
#include "StageDAT.h"
#include "Utility.h"
#include <CLI/CLI.hpp>

int main(int argc, char** argv)
{
std::string UsageStr =
R"(USAGE EXAMPLE:
stage:
  extract -> StageDec.exe stage -e -p "C:/games/zoe2/ZOE2/backup/STAGE.DAT"
  build   -> StageDec.exe stage -b

hash:
  hash a string          -> StageDec.exe hash -s jeh_def.mdz
  hash a list of strings -> StageDec.exe hash -l <path>/MyList.txt
)";

	CLI::App app("StageDec");
	CLI::Option* Verbose_Option = app.add_flag("-v", Utility::Is_VerboseEnabled, "Additional logging during the extraction/building process of the STAGE.DAT");
	auto DefaultOption = app.option_defaults();
	bool Is_Extracting = false;
	bool Is_Building = false;
	std::string Path{ "" };

	auto Stage_SubCommand   = app.add_subcommand("stage","Subcommands related to the STAGE.DAT");
	auto Stage_Path_Option  = Stage_SubCommand->add_option("-p", Path, "full path to the encrypted STAGE.DAT example: \"C:/MyGame/ZOE2/STAGE.DAT\"")->check(CLI::ExistingFile)->option_text("[IN:PATH]");
	auto Stage_Extract_Flag = Stage_SubCommand->add_flag("-e", Is_Extracting, "Extract all stages in a \"stage\" folder with all sections.");
	auto Stage_Build_Flag   = Stage_SubCommand->add_flag("-b", Is_Building, "Build \"STAGE.DAT\" based on the \"stage\" folder.");
	Stage_SubCommand->callback([&]() 
	{
		if (Is_Extracting)
		{
			if (!Utility::Setup_HashList()) { return 1; }
			StageDAT::DAT_ExtractEncrypted(Path);
		}
		else if (Is_Building)
		{
			auto startTime = std::chrono::high_resolution_clock::now();
			if (StageBuilder::Build())
			{
				auto endTime = std::chrono::high_resolution_clock::now();
				auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
				double durationSeconds = durationMs / 1000.0;
				std::cout << "STAGE.DAT built in: " << std::fixed << std::setprecision(3) << durationSeconds << " seconds" << std::endl;
			}
		}
	});

	std::string StringToHash{ "" };
	std::string TextFile_StrToBeHashed_Path{ "" };
	auto Hash_SubCommand = app.add_subcommand("hash", "Subcommand related to strcode32 hash");
	auto Hash_Str32_Option = Hash_SubCommand->add_option("-s", StringToHash, "Return strcode32 hash")->option_text("[IN:TEXT]");
	auto Hash_GenerateHashes_Option = Hash_SubCommand->add_option("-l", TextFile_StrToBeHashed_Path, "Load a file at [PATH] containing one string/line and generate a HashedStrings.txt")->check(CLI::ExistingFile)->option_text("[IN:PATH]");
	Hash_SubCommand->callback([&]() 
	{
		if (!StringToHash.empty()) 
		{
			std::cout << "0x" << std::uppercase << std::hex << Utility::HashStr(StringToHash.c_str(), false) << std::endl;
		}
		else if (!TextFile_StrToBeHashed_Path.empty())
		{
			auto TextFile = std::ifstream(TextFile_StrToBeHashed_Path);
			std::ofstream OutFile("HashedStrings.txt", std::ios::binary);
			std::string Line{ "" };
			while (std::getline(TextFile, Line))
			{
				if (Line.empty()) { continue; }
				OutFile << std::format("0x{:<8X} : {}", Utility::HashStr(Line.c_str(), false), Line) << std::endl;
			}
			TextFile.close();
		}
	});

	Stage_Extract_Flag->excludes(Stage_Build_Flag);
	Stage_Build_Flag->excludes(Stage_Extract_Flag);
	Stage_Extract_Flag->needs(Stage_Path_Option);

	Hash_Str32_Option->excludes(Hash_GenerateHashes_Option);
	Hash_GenerateHashes_Option->excludes(Hash_Str32_Option);
	if (argc == 1) 
	{
		std::cout << UsageStr << std::endl;
		std::cout << app.help("", CLI::AppFormatMode::Normal) << std::endl;
		return 0;
	}
	CLI11_PARSE(app, argc, argv);
}