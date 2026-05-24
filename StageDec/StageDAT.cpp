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
			std::ofstream DataCnf_Output(std::format("stage\\{}\\data.cnf", m_pStageDAT_Toc[StageIndex].name));
			m_pStageDataConfig  = reinterpret_cast<int*>((char*)m_StageDAT.data() + m_pStageDAT_Toc[StageIndex].sectorOffset * SECTOR_SIZE);
			int  TagNum         = *(int*)m_pStageDataConfig;
			int  TagSize        = TagNum * sizeof(SectionTag) + 4; // Include the END TAG
			int* pSectionData   = (int*)((char*)m_pStageDataConfig + SECTOR_ALIGN(TagSize));
			std::vector<BYTE> DecompressedSection_Data{ 0 };
			std::string SectionName;
			m_pStageDataConfig++;
			SectionTag* pTagIterator = reinterpret_cast<SectionTag*>(m_pStageDataConfig);

			/*Nullify all !*/
			int* EditorMetadata = (int*)((char*)m_pStageDataConfig + TagSize);
			memset(EditorMetadata, NULL, Utility::nBytesRemaining(EditorMetadata, m_StageDAT.data()));

			Utility::LogMessage(std::format("======{}======", m_pStageDAT_Toc[StageIndex].name));
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
						int    AudioStream_Index = 0;
						size_t Stream_Size       = 0;

						while (TagIndex++, pTagIterator++, pTagIterator->Identifier != DATACONFIG_SECTION_END)
						{
							DataCnf_Output << std::format("SoundPack{:d}.sdx", AudioStream_Index) << std::endl;
							AudioStream_TAG* AudioStream  = (AudioStream_TAG*)pTagIterator;
							Utility::LogMessage(std::format("TrackIndex({:d}) {:<16} {:08X} {:08X} {:08X} Offset[{:d}]", AudioStream->StreamIndex, "", AudioStream->StreamIndex, AudioStream->padding, AudioStream->Offset, AudioStream->Offset));

							bool HasNext_Stream           = (AudioStream[1].Offset != 0);
							HasNext_Stream ? Stream_Size  = AudioStream[1].Offset - AudioStream->Offset : Stream_Size = pTagIterator->AllocSize;
							std::filesystem::path OutPath = std::format(".\\stage\\{}\\.sound\\SoundPack{:d}.sdx", m_pStageDAT_Toc[StageIndex].name, AudioStream_Index);
							Utility::SaveFile(OutPath, pSectionData, Stream_Size);
							if (HasNext_Stream) { pSectionData += Stream_Size, AudioStream_Index++; }
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
					FileName.starts_with("0x") ? DataCnf_Output << FileName + "." + ExtName << std::endl : DataCnf_Output << FileName << std::endl;
					Utility::LogMessage(std::format("{:<30} {:08X} {:08X} {:08X} {:>60}|{}", "[FILE]", FileInfo->ExtensionHash, FileInfo->FileNameHash, FileInfo->Offset, ExtName, FileName));

					// If the next TAG is another FILE (otherwise the last), Calculate the file size based on the next file offset.
					bool Has_NextFile       = (FileInfo[1].ExtensionHash != 0 && FileInfo[1].FileNameHash != 0 && FileInfo[1].Offset != 0);
					Has_NextFile ? FileSize = FileInfo[1].Offset - FileInfo->Offset : FileSize = DecompressedSection_Data.size() - FileInfo->Offset;

					std::filesystem::path OutPath;
					FileName.starts_with("0x") ? OutPath = std::format(".\\stage\\{}\\{}\\{}.{}", m_pStageDAT_Toc[StageIndex].name, SectionName, FileName, ExtName) : OutPath = std::format(".\\stage\\{}\\{}\\{}", m_pStageDAT_Toc[StageIndex].name, SectionName, FileName);
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
					StageInfo.StageName = StageName;
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
								std::string ExtName = GameFile.path().extension().stem().string().substr(1);
								if (GameFile.is_regular_file() && Utility::HashMap_ExtensionToHash.count(ExtName) > 0)
								{
									if (GameFile.file_size() == 0) 
									{
										std::cerr << std::format("Warning: File \"{}\" have no content and will be ignored.",GameFile.path().string()) << std::endl;
										continue;
									}
									FileName.starts_with("0x") ? FileInfo.FileName_Hash = std::stoul(FileName,nullptr,16) : FileInfo.FileName_Hash = Utility::HashStr(FileName.c_str(), false);
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
							/*Sort all files extension*/
							std::sort(SectionInfo.FileList.begin(), SectionInfo.FileList.end(),[](StageBuilder::File A,StageBuilder::File B)
							{
								int WeightA = GetFilesWeight(A.ExtName);
								int WeightB = GetFilesWeight(B.ExtName);
								if (WeightA < WeightB) return true;
								return false;
							});
							StageInfo.SectionList.push_back(SectionInfo);
						}
					}
					StageInfo_List.push_back(StageInfo);
					/*Sort all sections by weights*/
					std::sort(StageInfo_List.at(StageInfo_List.size()-1).SectionList.begin(), StageInfo_List.at(StageInfo_List.size()-1).SectionList.end(),[](StageBuilder::Section A, StageBuilder::Section B)
						{
							int WeightA = GetSectionWeight(A.SectionName);
						    int WeightB = GetSectionWeight(B.SectionName);
						    if (WeightA < WeightB) return true;
						    return false;
					    });
				}
			}
			return StageInfo_List;
		}
	}
	bool Build()
	{
		std::vector<StageBuilder::StageInfo> StageInfo = SetupPathsFromDir();
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

					SectionTag Tag(CurrentSection.SectionIdentifier, 0, CurrentSection.SectionSize);
					memcpy(WorkingPtr, &Tag, sizeof(SectionTag));
					WorkingPtr += sizeof(SectionTag);

					Utility::LogMessage(std::format("[{}]", CurrentSection.SectionName));
					for (auto& GameFile : CurrentSection.FileList)
					{
						if (CurrentSection.SectionIdentifier == DATACONFIG_SECTION_SOUND)
						{
							AudioStream_TAG Tag(StreamIdx, 0, FileOffset);
							memcpy(WorkingPtr, &Tag, sizeof(FileTag));
							WorkingPtr += sizeof(FileTag);
							StreamIdx++;
						}
						else
						{
							FileTag Tag(GameFile.Extension_Hash, GameFile.FileName_Hash, FileOffset);
							memcpy(WorkingPtr, &Tag, sizeof(FileTag));
							WorkingPtr += sizeof(FileTag);
						}
						Utility::LogMessage(std::format("{:>25}", GameFile.FileName));
						std::ifstream TempFile(GameFile.Path, std::ios::binary);
						TempFile.read(reinterpret_cast<char*>(SectionData + FileOffset), GameFile.Size);
						FileOffset += GameFile.Size;
					}
					// Directly the next Stage if it was the last section
					SectionOffset += SECTOR_ALIGN(CurrentSection.SectionSize);
					Tag.Identifier = DATACONFIG_SECTION_END;
					memcpy(WorkingPtr, &Tag, sizeof(SectionTag));
					WorkingPtr += sizeof(SectionTag);
				}
				Utility::LogMessage("======END OF STAGE======\n");
				StageOffset += SectionOffset;
			}
			size_t TotalSize = StageOffset;
			std::ofstream OutFile(".\\STAGE.DAT", std::ios::binary);
			OutFile.write(static_cast<char*>(Cache), TotalSize);
			OutFile.close();
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
	if (ExtName == "mfl") return 9;
	if (ExtName == "mdz") return 10;
	if (ExtName == "lit") return 11;
	if (ExtName == "hzt") return 12;
	if (ExtName == "hz2") return 13;
	if (ExtName == "flw") return 14;
	if (ExtName == "ene") return 15;
	if (ExtName == "eft") return 16;
	if (ExtName == "cvz") return 17;
	if (ExtName == "mtl") return 18;
	if (ExtName == "mts") return 19;
	if (ExtName == "scx") return 20;
	return 21;
}