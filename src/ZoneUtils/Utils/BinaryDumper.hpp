// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

namespace ZoneTool
{
	struct dumpListEntry
	{
		void* start;
		int entryCount;
		int entrySize;
	};

	enum DUMP_TYPE
	{
		DUMP_TYPE_INT = 0,
		DUMP_TYPE_STRING = 1,
		DUMP_TYPE_ASSET = 2,
		DUMP_TYPE_ARRAY = 3,
		DUMP_TYPE_OFFSET = 4,
		DUMP_TYPE_FLOAT = 5,
	};

	const char DUMP_EXISTING = 1;
	const char DUMP_NONEXISTING = 0;

	class AssetDumper
	{
	protected:
		std::vector<dumpListEntry> list;
		FILE* fp = nullptr;

		template <typename T>
		void WriteInternal(const T& value)
		{
			if (fp)
			{
				fwrite(&value, sizeof T, 1, fp);
			}
		}

		template <typename T>
		void WriteInternal(const T& value, DUMP_TYPE type)
		{
			if (fp)
			{
				fwrite(&type, 1, 1, fp);
				fwrite(&value, sizeof T, 1, fp);
			}
		}

		void WriteString(const char* str)
		{
			if (fp)
			{
				fwrite(str, strlen(str) + 1, 1, fp);
			}
		}

	public:
		AssetDumper()
		{
			list.clear();
		}

		~AssetDumper()
		{
			// close file pointer
			Close();
		}

		bool Open(const std::string& filename)
		{
			Close();
			fp = FileSystem::FileOpen(filename, "wb");
			return fp != nullptr;
		}

		void Close()
		{
			list.clear();
			FileSystem::FileClose(fp);
		}

		void Float(float f)
		{
			WriteInternal(f, DUMP_TYPE_FLOAT);
		}

		void Int(std::int32_t i)
		{
			WriteInternal(i, DUMP_TYPE_INT);
		}

		void Uint(std::uint32_t i)
		{
			WriteInternal(i, DUMP_TYPE_INT);
		}

		void String(char* s)
		{
			if (fp)
			{
				if (s)
				{
					int listIndex = 0;
					for (auto oldEntry : list)
					{
						if (oldEntry.start == s && oldEntry.entrySize == 1 && oldEntry.entryCount == 1)
						{
							this->WriteInternal<std::int32_t>(listIndex, DUMP_TYPE_OFFSET);
							this->WriteInternal<std::int32_t>(0);
							return;
						}
						listIndex++;
					}

					dumpListEntry entry;
					entry.entryCount = 1;
					entry.entrySize = 1;
					entry.start = reinterpret_cast<void*>(s);
					list.push_back(entry);

					this->WriteInternal<char>(DUMP_EXISTING, DUMP_TYPE_STRING);
					this->WriteString(s);
				}
				else
				{
					this->WriteInternal<char>(DUMP_NONEXISTING, DUMP_TYPE_STRING);
				}
			}
		}

		void String(const char* s)
		{
			this->String(const_cast<char*>(s));
		}

		template <typename T>
		void Asset(T* asset)
		{
			if (fp)
			{
				if (asset && asset->name)
				{
					int listIndex = 0;
					for (auto oldEntry : list)
					{
						if ((int)oldEntry.start <= (int)asset &&
							(int)oldEntry.start + (oldEntry.entryCount ? oldEntry.entryCount - 1 : 0) * oldEntry.
							entrySize >= (int)asset &&
							((int)asset - (int)oldEntry.start) % oldEntry.entrySize == 0)
						{
							this->WriteInternal<std::int32_t>(listIndex, DUMP_TYPE_OFFSET);
							this->WriteInternal<std::int32_t>(((int)asset - (int)oldEntry.start) / oldEntry.entrySize);
							return;
						}
						listIndex++;
					}

					dumpListEntry entry;
					entry.entryCount = 1;
					entry.entrySize = sizeof(T);
					entry.start = (void*)asset;
					list.push_back(entry);

					this->WriteInternal<char>(DUMP_EXISTING, DUMP_TYPE_ASSET);
					this->WriteString(asset->name);
				}
				else
				{
					this->WriteInternal<char>(DUMP_NONEXISTING, DUMP_TYPE_ASSET);
				}
			}
		}

		template <typename T>
		void Array(T* asset, int arraySize)
		{
			if (fp)
			{
				if (asset && arraySize > 0)
				{
					int listIndex = 0;
					for (auto oldEntry : list)
					{
						if (sizeof(T) == oldEntry.entrySize &&
							// Filter out the structs it definitly can't be without having to actually save what struct it is
							(int)oldEntry.start <= (int)asset && // Check if it is in the range of the current array
							(int)oldEntry.start + (oldEntry.entryCount ? oldEntry.entryCount - 1 : 0) * oldEntry.
							entrySize >= (int)asset && // ^
							((int)asset - (int)oldEntry.start) % oldEntry.entrySize == 0)
							// Check if the asset is actually at the start of an array entry
						{
							this->WriteInternal<std::int32_t>(listIndex, DUMP_TYPE_OFFSET);
							this->WriteInternal<std::int32_t>(((int)asset - (int)oldEntry.start) / oldEntry.entrySize);
							return;
						}
						listIndex++;
					}

					dumpListEntry entry;
					entry.entryCount = arraySize;
					entry.entrySize = sizeof(T);
					entry.start = (void*)asset;
					list.push_back(entry);

					this->WriteInternal<std::uint32_t>(arraySize, DUMP_TYPE_ARRAY);
					fwrite(asset, sizeof(T), arraySize, fp);
				}
				else
				{
					this->WriteInternal<std::uint32_t>(0, DUMP_TYPE_ARRAY);
				}
			}
		}
	};

	class AssetReader
	{
	protected:
		std::vector<dumpListEntry> list;
		FILE* fp = nullptr;
		std::shared_ptr<ZoneMemory> memory;

		template <typename T>
		T ReadInternal()
		{
			T value;

			if (fp)
			{
				fread(&value, sizeof T, 1, fp);
			}

			return value;
		}

		char* ReadStr()
		{
			char tempBuf[1024];
			char ch = 0;
			int i = 0;

			if (fp)
			{
				do
				{
					fread(&ch, 1, 1, fp);

					tempBuf[i++] = ch;

					if (i >= sizeof(tempBuf))
					{
						throw std::exception("this is wrong");
					}
				}
				while (ch);
			}

			char* retval = memory->Alloc<char>(i); // new char[i];
			strcpy(retval, tempBuf);

			return retval;
		}

	public:
		AssetReader(std::shared_ptr<ZoneMemory>& mem)
		{
			memory = mem;

			list.clear();
		}

		~AssetReader()
		{
			// close file
			Close();
		}

		bool Open(const std::string& filename, bool preferLocal = false)
		{
			Close();

			FileSystem::PreferLocalOverExternal(preferLocal);
			fp = FileSystem::FileOpen(filename, "rb");
			FileSystem::PreferLocalOverExternal(false);

			return fp != nullptr;
		}

		void Close()
		{
			list.clear();
			FileSystem::FileClose(fp);
		}

		float Float()
		{
			if (fp)
			{
				char type = this->ReadInternal<char>();
				if (type != DUMP_TYPE_FLOAT)
				{
					printf("Reader error: Type not DUMP_TYPE_FLOAT but %i", type);
					throw;
					return 0;
				}
				return this->ReadInternal<float>();
			}
			return 0;
		}

		int Int()
		{
			if (fp)
			{
				char type = this->ReadInternal<char>();
				if (type != DUMP_TYPE_INT)
				{
					printf("Reader error: Type not DUMP_TYPE_INT but %i", type);
					throw;
					return 0;
				}
				return this->ReadInternal<std::int32_t>();
			}
			return 0;
		}

		unsigned int Uint()
		{
			if (fp)
			{
				char type = this->ReadInternal<char>();
				if (type != DUMP_TYPE_INT)
				{
					printf("Reader error: Type not DUMP_TYPE_INT but %i", type);
					throw;
					return 0;
				}
				return this->ReadInternal<std::uint32_t>();
			}
			return 0;
		}

		char* String()
		{
			if (fp)
			{
				char type = this->ReadInternal<char>();

				if (type == DUMP_TYPE_STRING)
				{
					char existing = this->ReadInternal<char>();
					if (existing == DUMP_NONEXISTING)
					{
						return nullptr;
					}
					char* output = this->ReadStr(); // freadstr(fp);

					dumpListEntry entry;
					entry.entryCount = 1;
					entry.entrySize = 1;
					entry.start = (void*)output;
					list.push_back(entry);

					return output;
				}
				if (type == DUMP_TYPE_OFFSET)
				{
					int listIndex = this->ReadInternal<int>(); // freadint(fp);
					int arrayIndex = this->ReadInternal<int>();

					return reinterpret_cast<char*>((int)list[listIndex].start + list[listIndex].entrySize * arrayIndex);
				}
				printf("Reader error: Type not DUMP_TYPE_STRING or DUMP_TYPE_OFFSET but %i", type);
				throw;
				return nullptr;
			}
			return nullptr;
		}

		template <typename T>
		T* Asset()
		{
			if (fp)
			{
				char type = this->ReadInternal<char>();

				if (type == DUMP_TYPE_ASSET)
				{
					char existing = this->ReadInternal<char>();
					if (existing == DUMP_NONEXISTING)
					{
						return nullptr;
					}
					const char* name = this->ReadStr();
					// T* asset = new T;

					auto asset = memory->Alloc<T>();

					memset(asset, 0, sizeof(T));
					asset->name = (char*)name;

					dumpListEntry entry;
					entry.entryCount = 1;
					entry.entrySize = sizeof(T);
					entry.start = (void*)asset;
					list.push_back(entry);

					return asset;
				}
				if (type == DUMP_TYPE_OFFSET)
				{
					int listIndex = this->ReadInternal<int>();
					int arrayIndex = this->ReadInternal<int>();

					return reinterpret_cast<T*>((int)list[listIndex].start + list[listIndex].entrySize * arrayIndex);
				}
				printf("Reader error: Type not DUMP_TYPE_ASSET or DUMP_TYPE_OFFSET but %i", type);
				throw;
				return nullptr;
			}
			return nullptr;
		}

		template <typename T>
		T* Array()
		{
			if (fp)
			{
				char type = this->ReadInternal<char>();

				if (type == DUMP_TYPE_ARRAY)
				{
					unsigned int arraySize = this->ReadInternal<std::uint32_t>();

					if (arraySize <= 0)
						return nullptr;

					auto nArray = memory->Alloc<T>(arraySize);
					fread((void*)nArray, sizeof(T), arraySize, fp);

					dumpListEntry entry;
					entry.entryCount = arraySize;
					entry.entrySize = sizeof(T);
					entry.start = (void*)nArray;
					list.push_back(entry);

					return nArray;
				}
				if (type == DUMP_TYPE_OFFSET)
				{
					int listIndex = this->ReadInternal<int>();
					int arrayIndex = this->ReadInternal<int>();

					return reinterpret_cast<T*>((int)list[listIndex].start + list[listIndex].entrySize * arrayIndex);
				}
				printf("Reader error: Type not DUMP_TYPE_ARRAY or DUMP_TYPE_OFFSET but %i\n", type);
				throw;
				return nullptr;
			}
			return nullptr;
		}
	};
}
