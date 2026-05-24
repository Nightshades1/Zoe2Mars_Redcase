#include "pch.h"
#include "StageDec.h"
#include "StageDAT.h"
#include "Utility.h"
#include <CLI/CLI.hpp>

int main(int argc, char** argv)
{
	CLI::App app("STAGE.DAT Tool for \"Zone Of The Enders The 2nd Runner\"","StageDec");
	bool Is_Extracting = false;
	bool Is_Building = false;
	std::string Path;

	app.add_option("-p,--path",Path,"path to the STAGE.DAT otherwise \".\\ZOE2\\STAGE.DAT\" by default");
	CLI::Option* Verbose_Option = app.add_flag("-v,--verbose",Utility::Is_VerboseEnabled, "Verbose mode - Additional logging.");
	CLI::Option* Extract_Option = app.add_flag("-e,--extract", Is_Extracting, "Extract all files from \"STAGE.DAT\" and reproduce the development \"stage\" folder with all sections.");
	CLI::Option* Build_Option   = app.add_flag("-b,--build", Is_Building, "Build \"STAGE.DAT\" based on the development \"stage\" folder.");
	
	Extract_Option->excludes(Build_Option);
	Build_Option->excludes(Extract_Option);
	CLI11_PARSE(app,argc,argv);
	if (Is_Extracting && !Is_Building)
	{
		if (Path.empty())
		{
			std::cerr << "-e require -p --path pointing to the encrypted STAGE.DAT" << std::endl;
			return 1;
		}
		if (!Utility::Setup_HashList()) { return 1; }
		StageDAT::DAT_ExtractEncrypted(Path);
		return 0;
	}
	else if (Is_Building && !Is_Extracting) 
	{
		auto startTime = std::chrono::high_resolution_clock::now();
		if (StageBuilder::Build())
		{
			auto endTime = std::chrono::high_resolution_clock::now();
			auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
			double durationSeconds = durationMs / 1000.0;
			std::cout << "Task to build STAGE.DAT completed in: " << std::fixed << std::setprecision(3) << durationSeconds << " seconds" << std::endl;
			return 0;
		}
	}

	return 0;
}