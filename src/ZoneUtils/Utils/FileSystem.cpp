// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"

namespace ZoneTool
{
	std::string FileSystem::m_fastfile = "";
	bool FileSystem::preferLocal = false;
	bool FileSystem::forceExternalAssets = false;

	void FileSystem::ForceExternalAssets(bool force)
	{
		forceExternalAssets = force;
	}

	std::string FileSystem::GetDestFolderForAsset(const std::string& name)
	{
		std::string path = "";

		if (forceExternalAssets || !preferLocal)
		{
			std::string path = "zonetool\\" + m_fastfile + "\\" + name;
			if (std::filesystem::exists(path))
			{
				return "zonetool\\" + m_fastfile + "\\";
			}

			path = "zonetool\\" + name;
			if (std::filesystem::exists(path))
			{
				return "zonetool\\";
			}
		}
		else
		{
			path = "zonetool\\" + name;
			if (std::filesystem::exists(path))
			{
				return "zonetool\\";
			}

			std::string path = "zonetool\\" + m_fastfile + "\\" + name;
			if (std::filesystem::exists(path))
			{
				return "zonetool\\" + m_fastfile + "\\";
			}
		}


		path = "zonetool\\images\\" + name;
		if (std::filesystem::exists(path))
		{
			return "zonetool\\images\\";
		}

		/*path = "dump\\" + m_fastfile + "\\" + name;
		if (std::filesystem::exists(path))
		{
			return "dump\\" + m_fastfile + "\\";
		}*/

		return "";
	}

	std::string FileSystem::GetFilePath(const std::string& name)
	{
		auto destfolder = GetDestFolderForAsset(name);
		if (!destfolder.empty())
		{
			return destfolder + name;
		}

		return "";
	}

	bool FileSystem::FileExists(const std::string& name)
	{
		return GetFilePath(name).size() > 1;
	}

	void FileSystem::SetFastFile(const std::string& ff)
	{
		m_fastfile = ff;
	}

	std::string FileSystem::GetFastFile()
	{
		return m_fastfile;
	}

	void FileSystem::CreateDirectory(const std::string& name)
	{
		std::filesystem::create_directories(name);
	}

	std::unordered_map<FILE*, std::string> filePaths;
	std::unordered_map<FILE*, std::string> relativeFilePaths;

	FILE* FileSystem::FileOpen(const std::string& name, const std::string& mode)
	{
		if (mode[0] == 'r')
		{
			auto path = GetFilePath(name);
			if (!path.empty())
			{
				auto fp = fopen(path.data(), mode.data());

				filePaths[fp] = path;
				relativeFilePaths[fp] = GetDestFolderForAsset(name);

				return fp;
			}
		}

		if (mode[0] == 'w' || mode[0] == 'a')
		{
			auto path = "dump\\" + m_fastfile + "\\" + name;

			std::size_t pos1 = std::string::npos, pos2 = std::string::npos, pos = std::string::npos;
			pos1 = path.find_last_of("/");
			pos2 = path.find_last_of("\\");

			if (pos1 != std::string::npos && pos2 != std::string::npos)
			{
				if (pos1 > pos2) pos = pos1;
				else pos = pos2;
			}
			else if (pos1 != std::string::npos && pos2 == std::string::npos)
			{
				pos = pos1;
			}
			else if (pos2 != std::string::npos && pos1 == std::string::npos)
			{
				pos = pos2;
			}
			else
			{
				pos = std::string::npos;
			}

			if (pos != std::string::npos)
			{
				std::string dir = path.substr(0, pos);
				CreateDirectory(dir);
			}

			auto fp = fopen(path.data(), mode.data());
			filePaths[fp] = path;
			relativeFilePaths[fp] = "dump\\" + m_fastfile + "\\";

			return fp;
		}

		return nullptr;
	}

	std::size_t FileSystem::FileSize(FILE* fp)
	{
		if (fp)
		{
			auto i = ftell(fp);
			fseek(fp, 0, SEEK_END);

			auto ret = ftell(fp);
			fseek(fp, i, SEEK_SET);

			return ret;
		}

		return 0;
	}

	std::string FileSystem::GetRelativeFolderForFile(FILE* fp)
	{
		auto itr = relativeFilePaths.find(fp);
		if (itr != relativeFilePaths.end())
		{
			return itr->second;
		}

		return "";
	}

	std::string FileSystem::GetFullPathForFile(FILE* fp)
	{
		auto itr = filePaths.find(fp);
		if (itr != filePaths.end())
		{
			return itr->second;
		}

		return "";
	}

	bool FileSystem::IsExternalFile(FILE* fp)
	{
		auto itr = relativeFilePaths.find(fp);
		if (itr != relativeFilePaths.end())
		{
			return itr->second != "zonetool\\";
		}

		return false;
	}

	void FileSystem::FileClose(FILE* fp)
	{
		if (fp)
		{
			auto itr = filePaths.find(fp);
			if (itr != filePaths.end())
			{
				itr->second.clear();
			}

			itr = relativeFilePaths.find(fp);
			if (itr != relativeFilePaths.end())
			{
				itr->second.clear();
			}

			fclose(fp);
		}
	}

	std::vector<std::uint8_t> FileSystem::ReadBytes(FILE* fp, std::size_t size)
	{
		if (size)
		{
			// alloc vector
			std::vector<std::uint8_t> buffer;
			buffer.resize(size);

			// read data
			fread(&buffer[0], size, 1, fp);

			// return data
			return buffer;
		}

		return {};
	}

	std::shared_ptr<FileReader> FileSystem::ToReader(FILE* fp)
	{
		auto size = FileSize(fp);
		auto buffer = ReadBytes(fp, size);

		auto reader = std::make_shared<FileReader>(buffer);
		return reader;
	}

	void FileSystem::PreferLocalOverExternal(bool state)
	{
		preferLocal = state;
	}

	char* FileSystem::ReadString(FILE* fp, ZoneMemory* mem)
	{
		char tempBuf[1024];
		char ch = 0;
		int i = 0;

		do
		{
			fread(&ch, 1, 1, fp);

			tempBuf[i++] = ch;

			if (i >= sizeof(tempBuf))
			{
				throw std::exception("this is wrong");
			}
		} while (ch);

		if (!mem)
		{
			char* retval = new char[i];
			strcpy(retval, tempBuf);

			return retval;
		}

		auto retval = mem->Alloc<char>(i);
		strcpy(retval, tempBuf);

		return retval;
	}

	int FileSystem::ReadInt(FILE* fp)
	{
		int data = 0;

		if (fp)
		{
			fread(&data, sizeof(int), 1, fp);
		}

		return data;
	}


}
