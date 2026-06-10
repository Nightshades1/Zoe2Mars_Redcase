#include "pch.h"
#include "StageDAT.h"
#include "Utility.h"
#include <assert.h>
namespace StageDAT
{
	// Fully decrypted STAGE.DAT and the development \"stage\" folder with all files.
	void DAT_ExtractEncrypted(std::string FilePath)
	{
		std::ifstream Dat(FilePath, std::ios::binary | std::ios::ate);
		if (!Dat.is_open()) {
			std::cerr << "Cannot find " << FilePath << std::endl;
			return;
		}
		size_t FileSize = Dat.tellg();
		Dat.seekg(0, std::ios::beg);
		m_StageDAT = std::vector<std::uint8_t>(FileSize);
		Dat.read(reinterpret_cast<char*>(m_StageDAT.data()), FileSize);
		m_pStageDAT_Header = reinterpret_cast<Header*>(m_StageDAT.data());

		std::cout << "Decrypting/Extracting STAGE.DAT please wait ..." << std::endl;
		DAT_DecryptHeader_TOC_DataConfig();
		DAT_ProcessSections();
	}

	// Decrypt the STAGE.DAT header, the table of content and Data.cnf of all stages.
	void DAT_DecryptHeader_TOC_DataConfig()
	{
		// Pass 1 - Decrypt the header metadata
		m_CryptoSeed = m_pStageDAT_Header->Seed;
		m_cryptoKey  = m_CryptoSeed ^ 0xF0F0;
		DAT_DecryptBuffer(reinterpret_cast<int*>(m_pStageDAT_Header) + 1, 3);

		// Pass 2 - Decrypt the table of content (list of stage name + sector offset)
		DAT_DecryptBuffer(reinterpret_cast<int*>((char*)m_pStageDAT_Header + sizeof(Header)), (sizeof(TableOfContent) * m_pStageDAT_Header->StageCount) / 4);

		m_pStageDAT_Toc = reinterpret_cast<TableOfContent*>((char*)m_pStageDAT_Header + sizeof(Header));

		// These data after the TOC does strictly nothing.
		std::uint8_t* EndOfTOC = reinterpret_cast<std::uint8_t*>(m_pStageDAT_Toc) + sizeof(TableOfContent) * m_pStageDAT_Header->StageCount;
		memset(EndOfTOC, NULL, Utility::nBytesRemaining(EndOfTOC, m_pStageDAT_Header));

		for (int StageIndex = 0; StageIndex < m_pStageDAT_Header->StageCount; StageIndex++)
		{
			m_pStageDataConfig = reinterpret_cast<int*>((char*)m_StageDAT.data() + m_pStageDAT_Toc[StageIndex].sectorOffset * SECTOR_SIZE);
			DAT_Init_StageSeed(m_pStageDAT_Toc[StageIndex].name);
			DAT_DecryptBuffer(reinterpret_cast<int*>(m_pStageDataConfig), SECTOR_SIZE / 4);
			Utility::Generate_Hash_GameString(m_pStageDAT_Toc[StageIndex].name);
		}
	}

	void DAT_ProcessSections()
	{
		for (int StageIndex = 0; StageIndex < m_pStageDAT_Header->StageCount; StageIndex++)
		{
			std::string Stage_DirectoryPath(std::format(".\\stage\\{}", m_pStageDAT_Toc[StageIndex].name));
			if (!std::filesystem::exists(Stage_DirectoryPath))
			{
				std::filesystem::create_directories(Stage_DirectoryPath);
			}
			std::ofstream DataCnf_Output(std::format(".\\stage\\{}\\data.cnf", m_pStageDAT_Toc[StageIndex].name));
			m_pStageDataConfig  = reinterpret_cast<int*>((char*)m_StageDAT.data() + m_pStageDAT_Toc[StageIndex].sectorOffset * SECTOR_SIZE);
			int  TagNum         = *(int*)m_pStageDataConfig;
			int  TagSize        = TagNum * sizeof(SectionTag) + 4; // Include the END TAG
			int* pSectionData   = (int*)((char*)m_pStageDataConfig + SECTOR_ALIGN(TagSize));
			std::vector<BYTE> DecompressedSection_Data{ 0 };
			std::string SectionName;
			m_pStageDataConfig++;
			SectionTag* pTagIterator = reinterpret_cast<SectionTag*>(m_pStageDataConfig);
			SectionTag* SectionTag = static_cast<struct SectionTag*>(pTagIterator); // This pointer is never incremented

			/*Nullify all !*/
			int* SomeMetadata = (int*)((char*)m_pStageDataConfig + TagSize);
			memset(SomeMetadata, NULL, Utility::nBytesRemaining(SomeMetadata, m_StageDAT.data()));

			Utility::LogMessage(std::format("======{}======", m_pStageDAT_Toc[StageIndex].name));
			std::string StageName = m_pStageDAT_Toc[StageIndex].name;
			for (int TagIndex = 0; TagIndex < TagNum; TagIndex++, pTagIterator++)
			{
				if (IS_SECTION_TAG(pTagIterator->Identifier))
				{
					SectionName = Map_Section_ID_ToName[pTagIterator->Identifier];
					DataCnf_Output << SectionName << std::endl;
					Utility::LogMessage(std::format("{:<30} {:08X} {:08X} {:08X} Alloc[{:d}]", SectionName, pTagIterator->Identifier, pTagIterator->paddding, pTagIterator->AllocSize, pTagIterator->AllocSize));
					if (IS_COMPRESSION_TAG(pTagIterator[1].Identifier))
					{
						CompressionInfo* CompInfo = (CompressionInfo*)&pTagIterator[1];
						size_t CompressedSize = SECTOR_ALIGN(COMPRESSION_SIZE(CompInfo->CompressedSize));

						/*Decrypt / Decompress*/
						const int v1 = *(unsigned __int16*)pSectionData ^ 0x9385;
						m_CryptoSeed = v1 | ((*(unsigned __int16*)pSectionData ^ 0xF6F3) << 16);
						m_cryptoKey  = 278 * v1;
						DAT_DecryptBuffer((int*)pSectionData, CompressedSize / 4);
						*(WORD*)pSectionData += 0x8F3;

						DecompressedSection_Data.resize(pTagIterator->AllocSize);
						z_stream strm{ 0 };
						strm.avail_in  = CompressedSize;
						strm.next_in   = reinterpret_cast<Bytef*>(pSectionData);
						strm.next_out  = reinterpret_cast<Bytef*>(DecompressedSection_Data.data());
						strm.avail_out = DecompressedSection_Data.size();
						inflateInit(&strm);
						int err = inflate(&strm, Z_SYNC_FLUSH);
						if (err != Z_STREAM_END)
						{
							std::cout << "[ZLIB] Error: End of stream not reached ! Exiting !" << err << std::endl;
							std::exit(0);
						}
						DecompressedSection_Data.resize(strm.total_out);
						Utility::LogMessage(std::format("{:<30} {:08X} {:08X} {:08X} CompressedSize:[{:d}]", "[COMPRESSED FOLDER]", CompInfo->CompressedSize, CompInfo->padding, CompInfo->DecompressedSize, CompressedSize));
						//Next Section
						pSectionData = (int*)((char*)pSectionData + SECTOR_ALIGN(CompressedSize));
					}
					if (pTagIterator->Identifier == DATACONFIG_SECTION_SOUND)
					{
						int StreamIndex          = 0;
						size_t Stream_Size       = 0;
						size_t RelativeOffset    = (std::uintptr_t)pSectionData - (std::uintptr_t)m_StageDAT.data();
						while (TagIndex++, pTagIterator++, pTagIterator->Identifier != DATACONFIG_SECTION_END)
						{
							AudioStream_TAG* AudioStream  = (AudioStream_TAG*)pTagIterator;
							DataCnf_Output << std::format("SoundPack{:d}.sdx", AudioStream->SoundPackID) << std::endl;
							Utility::LogMessage(std::format("SoundPackID({:d}) {:>16}{:08X} {:08X} {:08X} RelativeOffset:{:08X} (Mars -> us{:08X}.bnk) Offset[{:d}]",AudioStream->SoundPackID, "", AudioStream->SoundPackID, AudioStream->padding, AudioStream->Offset,RelativeOffset,RelativeOffset >> 11,AudioStream->Offset));

							bool HasNext_Stream           = (AudioStream[1].Offset != 0);
							HasNext_Stream ? Stream_Size  = AudioStream[1].Offset - AudioStream->Offset : Stream_Size = SectionTag->AllocSize - AudioStream->Offset;
							std::filesystem::path OutPath = std::format(".\\stage\\{}\\.sound\\SoundPack{:d}.sdx", m_pStageDAT_Toc[StageIndex].name, AudioStream->SoundPackID);
							Utility::SaveFile(OutPath, pSectionData, Stream_Size);
							if (HasNext_Stream) { 
								pSectionData += Stream_Size / 4;
								RelativeOffset = (std::uintptr_t)pSectionData - (std::uintptr_t)m_StageDAT.data();
								StreamIndex++;
							}
						}
					}
					if (pTagIterator->Identifier == DATACONFIG_SECTION_END)
					{
						DecompressedSection_Data.clear();
						bool Has_NextTag = (TagIndex < TagNum - 2);
						Has_NextTag ? Utility::LogMessage(std::format("{:<30} {:08X} {:08X} {:08X} Free[{:d}]\n", ".end", pTagIterator->Identifier, pTagIterator->paddding, pTagIterator->AllocSize, pTagIterator->AllocSize)) : Utility::LogMessage(std::format("{:<30} {:08X} {:08X} {:08X} Free[{:d}]", ".end", pTagIterator->Identifier, pTagIterator->paddding, pTagIterator->AllocSize, pTagIterator->AllocSize));
					}
				}
				else if (!IS_SECTION_TAG(pTagIterator->Identifier) && !IS_COMPRESSION_TAG(pTagIterator->Identifier) && !IS_END_TAG(pTagIterator->Identifier))
				{
					FileTag*    FileInfo       = reinterpret_cast<FileTag*>(pTagIterator);
					BYTE*       pFileData      = DecompressedSection_Data.data() + FileInfo->Offset;
					size_t      FileSize       = 0;
					std::string ExtName        = Utility::GetStringFromHash(FileInfo->ExtensionHash);
					std::string FileName       = Utility::GetStringFromHash(FileInfo->FileNameHash);

					//Enforce the proper extension in case the filename hash have a collision
					FileName.starts_with("0x") ? DataCnf_Output << FileName + "." + ExtName << std::endl : DataCnf_Output << FileName + "." + ExtName << std::endl;
					Utility::LogMessage(std::format("{:<30} {:08X} {:08X} {:08X} {:>60}|{}.{}", "[FILE]", FileInfo->ExtensionHash, FileInfo->FileNameHash, FileInfo->Offset, ExtName, FileName,ExtName));

					// If the next TAG is another FILE (otherwise the last), Calculate the file size based on the next file offset.
					bool Has_NextFile       = (FileInfo[1].ExtensionHash != 0 && FileInfo[1].FileNameHash != 0 && FileInfo[1].Offset != 0);
					Has_NextFile ? FileSize = FileInfo[1].Offset - FileInfo->Offset : FileSize = DecompressedSection_Data.size() - FileInfo->Offset;

					std::filesystem::path OutPath;
					FileName.starts_with("0x") ? OutPath = std::format(".\\stage\\{}\\{}\\{}.{}", m_pStageDAT_Toc[StageIndex].name, SectionName, FileName, ExtName) : OutPath = std::format(".\\stage\\{}\\{}\\{}.{}", m_pStageDAT_Toc[StageIndex].name, SectionName, FileName,ExtName);
					Utility::SaveFile(OutPath, pFileData, FileSize);
				}
			}
			Utility::LogMessage("======END OF DATA.CNF======\n");
		}
		std::ofstream OutDat("STAGE.DAT", std::ios::binary);
		OutDat.write((char*)m_StageDAT.data(), m_StageDAT.size());
	}

	void DAT_DecryptBuffer(int* pData, int iterations)
	{
		for (int i = 0; i < iterations; ++i)
		{
			*pData++    ^= m_CryptoSeed;
			m_CryptoSeed = m_cryptoKey + XOR_MULTIPLIER * m_CryptoSeed;
		}
	}

	// Initializes the key and seed based on the stage name.
	void DAT_Init_StageSeed(char* StageName)
	{
		int NameHash = 129 * Utility::HashStr(StageName, false);
		m_CryptoSeed = NameHash + m_pStageDAT_Header->Seed + 0xA78925D9;
		m_cryptoKey  = NameHash + 0x7A88FB59;
	}
}

namespace StageBuilder
{
	namespace fs = std::filesystem;
	namespace {
		std::vector<StageInfo> SetupPaths_LocalDataConfig()
		{
			if (!fs::exists(".\\stage"))
			{
				std::cerr << "Error: \"stage\" folder doesn't exist" << std::endl;
				ExitProcess(1);
			}
			std::vector<StageInfo> StageInfo_List;
			for (auto& DirEntry : fs::recursive_directory_iterator(".\\stage"))
			{
				bool Is_InSection = false;
				fs::path Path = DirEntry.path();
				if (!DirEntry.is_regular_file()) continue;
				if (Path.extension() != ".cnf") continue;
				if (Path.filename() == "data.cnf")
				{
					StageInfo StageInfo{};
					Section SectionInfo{};
					std::ifstream DataCnf(Path);
					fs::path StagePath = Path.parent_path();
					fs::path SectionPath;
					std::string StageName = StagePath.stem().string();
					StageInfo.StageName = StageName;
					std::string SectionName;
					std::string Line;
					int LineNum    = 0;
					int SectionIdx = -1;
					while (std::getline(DataCnf, Line))
					{
						if (Line.empty()) continue;
						if (!Is_InSection)
						{
							if (Map_Section_NameToID.count(Line) > 0)
							{
								SetSection:
								Is_InSection                  = true;
								SectionName                   = Line;
								SectionPath                   = fs::path(StagePath).append(SectionName);
								SectionInfo.SectionName       = SectionName;
								SectionInfo.SectionIdentifier = Map_Section_NameToID[SectionName];
								StageInfo.SectionList.push_back(SectionInfo);
								SectionIdx++;
								if (!fs::exists(SectionPath)) {
									std::cerr << std::format("Folder \"{}\" doesn't exist, line:{:d}", SectionPath.string(), LineNum) << std::endl;
									ExitProcess(1);
								}
							}
							else {
								std::cerr << std::format("Error: Unknown section \"{}\" at line:{:d}", Line, LineNum) << std::endl;
								ExitProcess(1);
							}
							continue;
						}
						if (Map_Section_NameToID.count(Line) > 0)
						{
							goto SetSection;
						}
						fs::path    FilePath = fs::path(SectionPath).append(Line);
						std::string FileName = FilePath.filename().string();
						std::string ExtName  = FilePath.extension().stem().string().substr(1);
						if (!fs::exists(FilePath))
						{
							std::cerr << std::format("Error: File \"{}\" doesn't exist, line:{:d}", Line, LineNum) << std::endl;
							ExitProcess(1);
						}
						StageBuilder::File FileInfo;
						FileName.starts_with("0x") ? FileInfo.FileName_Hash = std::stoul(FileName, nullptr, 16) : FileInfo.FileName_Hash = Utility::HashStr(FileName.c_str(), false);
						if(ExtName == "sdx")
						{
							uint32_t SoundPackID;
							if (sscanf(FileName.c_str(), "SoundPack%d.sdx",&SoundPackID))
							{
								FileInfo.SoundPackID = SoundPackID;
							}
						}
						FileInfo.Size                                       = fs::directory_entry(FilePath).file_size();
						FileInfo.Path                                       = FilePath;
						FileInfo.FileName                                   = FileName;
						FileInfo.ExtName                                    = ExtName;
						FileInfo.Extension_Hash                             = Utility::HashMap_ExtensionToHash[ExtName];
						StageInfo.SectionList.at(SectionIdx).SectionSize   += FileInfo.Size;
						StageInfo.SectionList.at(SectionIdx).FilesCount    += 1;
						StageInfo.FilesCount                               += 1;
						StageInfo.TotalSize                                += FileInfo.Size;
						StageInfo.SectionList.at(SectionIdx).FileList.push_back(FileInfo);
						LineNum++;
					}
					StageInfo_List.push_back(StageInfo);
				}
			}
			std::sort(StageInfo_List.begin(), StageInfo_List.end(), [](const StageInfo& A, const StageInfo& B)
			{
				return A.StageName == "init" && B.StageName != "init";
			});
			return StageInfo_List;
		}
		std::vector<StageInfo> SetupPathsFromDir()
		{
			if (!fs::exists(".\\stage"))
			{
				std::cerr << "Error: \"stage\" folder doesn't exist" << std::endl;
				ExitProcess(1);
			}
			std::vector<StageInfo> StageInfo_List;
			for (auto& StageDir : fs::directory_iterator(".\\stage"))
			{
				StageInfo StageInfo{};
				if (StageDir.is_directory())
				{
					std::string StageName = StageDir.path().stem().string();
					StageInfo.StageName   = StageName;
					for (auto& SectionDir : fs::directory_iterator(StageDir))
					{
						std::string SectionName = SectionDir.path().stem().string();
						if (SectionDir.is_directory() && Map_Section_NameToID.count(SectionName) > 0)
						{
							size_t FileOffset = 0; // The offset of the file inside the current section.
							StageBuilder::Section SectionInfo;
							SectionInfo.SectionName       = SectionName;
							SectionInfo.SectionIdentifier = Map_Section_NameToID[SectionName];
							for (auto& GameFile : fs::directory_iterator(SectionDir))
							{
								StageBuilder::File FileInfo;
								std::string FileName = GameFile.path().filename().string();
								std::string ExtName  = GameFile.path().extension().stem().string().substr(1);
								if (GameFile.is_regular_file() && Utility::HashMap_ExtensionToHash.count(ExtName) > 0)
								{
									if (GameFile.file_size() == 0) 
									{
										std::cerr << std::format("Warning: File \"{}\" have no content and will be ignored.",GameFile.path().string()) << std::endl;
										continue;
									}
									FileName.starts_with("0x") ? FileInfo.FileName_Hash = std::stoul(FileName,nullptr,16) : FileInfo.FileName_Hash = Utility::HashStr(FileName.c_str(), false);
									if (ExtName == "sdx")
									{
										uint32_t SoundPackID;
										if (sscanf(FileName.c_str(), "SoundPack%d.sdx", &SoundPackID))
										{
											FileInfo.SoundPackID = SoundPackID;
										}
									}
									FileInfo.Size              = GameFile.file_size();
									FileInfo.Path              = GameFile;
									FileInfo.FileName          = FileName;
									FileInfo.ExtName           = ExtName;
									FileInfo.Extension_Hash    = Utility::HashMap_ExtensionToHash[ExtName];
									SectionInfo.SectionSize   += FileInfo.Size;
									SectionInfo.FilesCount    += 1;
									StageInfo.FilesCount      += 1;
									StageInfo.TotalSize       += FileInfo.Size;
									FileOffset                += FileInfo.Size;
									SectionInfo.FileList.push_back(FileInfo);
								}
							}
							// Sort all files extension
							// NOTE: Required by the game as it initialize the files as soon as they are readt --Nightshades 24/05/2026
							std::sort(SectionInfo.FileList.begin(), SectionInfo.FileList.end(),[](StageBuilder::File& A,StageBuilder::File& B)
							{
								int WeightA = GetFilesWeight(A.ExtName);
								int WeightB = GetFilesWeight(B.ExtName);
								if (WeightA != WeightB)
								{
									return WeightA < WeightB;
								}
								return A.Extension_Hash > B.Extension_Hash;
							});
							StageInfo.SectionList.push_back(SectionInfo);
						}
					}
					StageInfo_List.push_back(StageInfo);
					// Sort all sections by weights
					std::sort(StageInfo_List.at(StageInfo_List.size()-1).SectionList.begin(), StageInfo_List.at(StageInfo_List.size()-1).SectionList.end(),[](StageBuilder::Section& A, StageBuilder::Section& B)
						{
							int WeightA = GetSectionWeight(A.SectionName);
						    int WeightB = GetSectionWeight(B.SectionName);
						    if (WeightA < WeightB) return true;
						    return false;
					    });
				}
			}
			// NOTE: Critical to the game engine if the INIT stage isn't the 1st one, sounds wont play correctly, sort stages --Nightshades 24/05/2026
			std::sort(StageInfo_List.begin(), StageInfo_List.end(), [](const StageInfo& A,const StageInfo& B) 
			{
					return A.StageName == "init" && B.StageName != "init";
			});
			return StageInfo_List;
		}
	}
	bool Build()
	{
		std::vector<StageBuilder::StageInfo> StageInfo = SetupPaths_LocalDataConfig();
		//std::vector<StageBuilder::StageInfo> StageInfo = SetupPathsFromDir();
		if (StageInfo.size() > 0)
		{
			constexpr uint64_t TwoGB           = 2ULL * 1024ULL * 1024ULL * 1024ULL;
			LPVOID Cache                       = VirtualAlloc(nullptr, TwoGB, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			uint8_t* WorkingPtr                = static_cast<uint8_t*>(Cache);
			Header*  pHeader                   = reinterpret_cast<Header*>(WorkingPtr);
			pHeader->StageCount                = StageInfo.size();
			pHeader->TableOfContent_SectorSize = 2;
			WorkingPtr += sizeof(Header);
			
			TableOfContent* Toc = reinterpret_cast<TableOfContent*>(WorkingPtr);
			size_t  StageOffset = pHeader->TableOfContent_SectorSize * SECTOR_SIZE;
			for (int StageIndex = 0; StageIndex < StageInfo.size(); StageIndex++)
			{
				int TagSize = (sizeof(SectionTag) * StageInfo[StageIndex].FilesCount) + (sizeof(SectionTag) * StageInfo[StageIndex].SectionList.size() * 2) + sizeof(SectionTag);// Include the END TAG
				std::string StageName(StageInfo[StageIndex].StageName);
				memcpy(Toc[StageIndex].name, StageName.data(), 16);
				
				Toc[StageIndex].sectorOffset = StageOffset >> 11;
				size_t SectionOffset         = SECTOR_ALIGN(TagSize);
				WorkingPtr                   = static_cast<uint8_t*>(Cache) + StageOffset;
				*(int*)WorkingPtr            = TagSize / sizeof(SectionTag);
				WorkingPtr                  += 4;

				Utility::LogMessage(std::format("Files:{:d}\nSection:{:d}\nTagSize:{:d} ({:d} sector)\n======{}======",
					StageInfo[StageIndex].FilesCount,
					StageInfo[StageIndex].SectionList.size(),
					TagSize,
					SectionOffset >> 11,
					StageName
				));

				/*Write all tags and each section/files*/
				for (int SectionIndex = 0; SectionIndex < StageInfo[StageIndex].SectionList.size(); SectionIndex++)
				{
					size_t   FileOffset     = 0;
					int      StreamIdx      = 0;
					uint8_t* SectionData    = (static_cast<uint8_t*>(Cache) + StageOffset + SectionOffset);
					auto     CurrentSection = StageInfo[StageIndex].SectionList[SectionIndex];

					/*The total size of all files uncompressed*/
					SectionTag* pSectionTag = (SectionTag*)WorkingPtr;
					pSectionTag->Identifier = CurrentSection.SectionIdentifier;
					pSectionTag->paddding   = 0;
					pSectionTag->AllocSize  = CurrentSection.SectionSize;
					WorkingPtr += sizeof(SectionTag);

					Utility::LogMessage(std::format("[{}]", CurrentSection.SectionName));
					for (auto& GameFile : CurrentSection.FileList)
					{
						if (CurrentSection.SectionIdentifier == DATACONFIG_SECTION_SOUND)
						{
							AudioStream_TAG Tag(GameFile.SoundPackID, 0, FileOffset);
							memcpy(WorkingPtr, &Tag, sizeof(AudioStream_TAG));
							WorkingPtr += sizeof(AudioStream_TAG);
							StreamIdx++;
						}
						else
						{
							FileTag Tag(GameFile.Extension_Hash, GameFile.FileName_Hash, FileOffset);
							memcpy(WorkingPtr, &Tag, sizeof(FileTag));
							WorkingPtr += sizeof(FileTag);
						}
						Utility::LogMessage(std::format("{:>25} {:<10} Offset:{:08X} {:<5} FileSize(dec):{:d}",GameFile.FileName.c_str(),"", reinterpret_cast<DWORD>(SectionData + FileOffset) - (DWORD)Cache,"", GameFile.Size));
						HANDLE hFile = CreateFileW(
							GameFile.Path.c_str(),
							GENERIC_READ,
							FILE_SHARE_READ,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
							NULL
						);
						ReadFile(hFile, SectionData+FileOffset, static_cast<DWORD>(GameFile.Size), NULL, NULL);
						CloseHandle(hFile);
						FileOffset += GameFile.Size;
					}
					SectionOffset += SECTOR_ALIGN(pSectionTag->AllocSize);
					SectionTag SectionTag_End{};
					SectionTag_End.paddding   = 0;
					SectionTag_End.Identifier = DATACONFIG_SECTION_END;
					SectionTag_End.AllocSize  = pSectionTag->AllocSize;
					memcpy(WorkingPtr, &SectionTag_End, sizeof(SectionTag));
					WorkingPtr += sizeof(SectionTag);
				}
				Utility::LogMessage("======END OF STAGE======\n");
				StageOffset += SectionOffset;
			}
			size_t TotalSize = StageOffset;
			HANDLE OutFile = CreateFileW(
				L".\\STAGE.DAT",
				GENERIC_WRITE,
				0,
				NULL,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
				NULL
			);
			WriteFile(OutFile, Cache, TotalSize, NULL, NULL);
			CloseHandle(OutFile);
			VirtualFree(Cache, NULL, MEM_RELEASE);
			return true;
		}
		std::cerr << "Error: Nothing to build" << std::endl;
		return false;
	}
}

int GetSectionWeight(std::string& SectionName)
{
	if (SectionName == ".binary")   return 0;
	if (SectionName == ".nocache")  return 1;
	if (SectionName == ".cache")    return 2;
	if (SectionName == ".resident") return 3;
	if (SectionName == ".sound")    return 4;
}

int GetFilesWeight(std::string& ExtName)
{
	if (ExtName == "fnt") return 0;
	if (ExtName == "var") return 1;
	if (ExtName == "cnf") return 2; // data.cnf requires investigation.
	if (ExtName == "trz") return 3;
	if (ExtName == "tex") return 4;
	if (ExtName == "rpd") return 5;
	if (ExtName == "row") return 6;
	if (ExtName == "ric") return 7;
	if (ExtName == "o2d") return 8;
	if (ExtName == "mtz") return 9;
	if (ExtName == "mfl") return 10;
	if (ExtName == "mdz") return 11;
	if (ExtName == "lit") return 12;
	if (ExtName == "hzt") return 13;
	if (ExtName == "hz2") return 14;
	if (ExtName == "flw") return 15;
	if (ExtName == "ene") return 16;
	if (ExtName == "eft") return 17;
	if (ExtName == "cvz") return 18;
	if (ExtName == "mtl") return 19;
	if (ExtName == "mts") return 20;
	if (ExtName == "scx") return 21;
	return 22;
}