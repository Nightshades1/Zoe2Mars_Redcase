#include "pch.h"
#include "StageDAT.h"
#include "Utility.h"
namespace Utility
{
	std::string GetStringFromHash(uint32_t nStringHashed)
	{
		std::ostringstream ss;
		if (HashMap_HashToExtension.count(nStringHashed) > 0)
		{
			ss << HashMap_HashToExtension[nStringHashed];
			return std::string(ss.str());
		}
		if (HashMap_GameString.count(nStringHashed) > 0)
		{
			ss << HashMap_GameString[nStringHashed];
			return std::string(ss.str());
		}
		ss << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << nStringHashed;
		return std::string(ss.str());
	}

	uint32_t HashStr(const char* str, bool Is_ByteSwapped) {
		uint32_t step = 0;
		uint32_t hash = 0;
		int seed = *str;

		while (*str) {
			int shiftedSeed = seed << (step++ & 0xF);
			hash += (hash << (seed & 0xF)) | (seed + shiftedSeed + (hash >> 3));
			seed = *++str;
		}
		if (Is_ByteSwapped) {
			return _byteswap_ulong(hash);
		}
		return hash;
	}

	uint32_t HashStr(const char* str)
	{
		return HashStr(str, false);
	}
	void LogMessage(std::string Message)
	{
		if (Utility::Is_VerboseEnabled)
		{
			std::cout << Message << std::endl;
		}
	}
	size_t nBytesRemaining(void* CurrentAddress, void* StartingAddress)
	{
		size_t remaining = 2048 - (reinterpret_cast<std::uint8_t*>(CurrentAddress) - reinterpret_cast<std::uint8_t*>(StartingAddress)) % 2048;
		return remaining;
	}
	//@return Current position in the file after padding.
	size_t PadMemory(std::ofstream &File)
	{
		size_t CurrentOffset = File.tellp();
		size_t Padding_Till_NextSector = 2048 - CurrentOffset % 2048;
		char* pBuffer = new char[Padding_Till_NextSector] {0};
		File.write(pBuffer, Padding_Till_NextSector);
		delete[] pBuffer;
		return Padding_Till_NextSector + CurrentOffset;
	}
	void SaveFile(std::filesystem::path& OutPath,void* pFileData,size_t FileSize)
	{
		if (!std::filesystem::exists(OutPath.parent_path()))
		{
			std::filesystem::create_directories(OutPath.parent_path());
		}
		if (!std::filesystem::exists(OutPath))
		{
			std::ofstream ExtractFile(OutPath, std::ios::binary);
			ExtractFile.write(reinterpret_cast<char*>(pFileData), FileSize);
		}
	}
	// populate the hash map "stage.<extension>" slight bruteforce approach
	void Generate_Hash_GameString(std::string StageName)
	{
		for (auto& ext : HashMap_HashToExtension)
		{
			std::string Stage_Plus_Extension = StageName + "." + ext.second;
			HashMap_GameString[Utility::HashStr(Stage_Plus_Extension.c_str(), false)] = Stage_Plus_Extension;
		}
	}

	bool Setup_HashList()
	{
		uint32_t Hash = 0;
		std::ifstream strlist("strlist.txt");
		if (!strlist.is_open())
		{
			std::cerr << "Couldn't open strlist.txt, Aborting..." << std::endl;
			return false;
		}
		std::string Line;
		while (std::getline(strlist, Line))
		{
			if (Line.empty()) continue;
			if (size_t pos = Line.find(".mts") != Line.npos)
			{
				size_t dot = Line.find_last_of(".");
				std::string File_Name = Line.substr(0, dot);
				for (auto& ext : HashMap_HashToExtension)
				{
					std::string Full_FileName = File_Name + "." + ext.second;
					Hash = Utility::HashStr(Full_FileName.c_str());
					HashMap_GameString[Hash] = std::string(Full_FileName);
				}
			}
			for (auto& ext : HashMap_HashToExtension)
			{
				std::string TempStr = std::string(Line) + ext.second;
				Hash = Utility::HashStr(TempStr.c_str());
				HashMap_GameString[Hash] = TempStr;
			}
			Hash = Utility::HashStr(Line.c_str());
			HashMap_GameString[Hash] = std::string(Line);
		}
		return true;
	}

	bool Is_ValidFile_Extension(std::string ExtensionName)
	{
		if (HashMap_ExtensionToHash.count(ExtensionName) > 0)
		{
			return true;
		}
		return false;
	}
}
