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
	namespace IW5
	{
#define min(a, b) (a < b) ? a : b

		void* malloc_n(size_t size)
		{
			void* ret = malloc(size);
			memset(ret, 0, size);
			return ret;
		}

		void IStructuredDataDef::addEntry(enumType_s type, int statIndexOffset, char* entryName)
		{
			newEnumEntry* newEntry = (newEnumEntry*)malloc_n(sizeof(newEnumEntry));
			newEntry->name = entryName;
			newEntry->statIndexOffset = statIndexOffset;
			newEntries[type][newEntries[type].size()] = newEntry;
		}

		char* strToLower(char* str)
		{
			int len = strlen(str);
			char* newStr = (char*)malloc_n(len + 1);

			for (int i = 0; i < len; i++)
			{
				newStr[i] = tolower(str[i]);
			}

			return newStr;
		}

		bool checkAlphabeticalOrder(char* entry1, char* entry2)
		{
			char* e1_l = strToLower(entry1);
			char* e2_l = strToLower(entry2);

			int len = min(strlen(e1_l), strlen(e2_l));

			for (int i = 0; i < len; i++)
			{
				if (*(e1_l + i) > *(e2_l + i))
					return false;
				if (*(e1_l + i) < *(e2_l + i))
					return true;
			}

			return (strlen(e1_l) <= strlen(e2_l));
		}

		void IStructuredDataDef::patchEnumWithMap(StructuredDataDefSet* data, enumType_s enumIndex,
		                                          std::map<int, newEnumEntry*> map)
		{
			StructuredDataEnum* currentEnum = &data->defs->enums[enumIndex];

			// Check if new enum has already been built
			if (newIndexCount[enumIndex])
			{
				currentEnum->entryCount = newIndexCount[enumIndex];
				currentEnum->entries = newIndices[enumIndex];
				return;
			}

			// Check if new weapons share the same index or if it is invalid
			for (std::size_t i = 0; i < map.size(); i++)
			{
				if (map[i]->statIndexOffset < 1)
				{
					ZONETOOL_ERROR("Invalid index (%d) for entry %s\n ", map[i]->statIndexOffset, map[i]->name);
				}

				for (std::size_t j = 0; j < map.size(); j++)
				{
					if (i == j) continue;

					if (map[i]->statIndexOffset == map[j]->statIndexOffset)
					{
						ZONETOOL_ERROR("Index duplication (%d) for entries %s and %s\n", map[i]->statIndexOffset, map[i]
->name, map[j]->name);
					}
				}
			}

			// Define new amount of indices
			newIndexCount[enumIndex] = currentEnum->entryCount + map.size();

			// Find last cac index
			int lastCacIndex = 0;
			for (int i = 0; i < currentEnum->entryCount; i++)
			{
				if (currentEnum->entries[i].index > lastCacIndex)
				{
					lastCacIndex = currentEnum->entries[i].index;
				}
			}

			// Create new enum
			newIndices[enumIndex] = (StructuredDataEnumEntry*)malloc_n(
				sizeof(StructuredDataEnumEntry) * (currentEnum->entryCount + map.size() + 1));
			memcpy(newIndices[enumIndex], currentEnum->entries,
			       sizeof(StructuredDataEnumEntry) * currentEnum->entryCount);

			// Apply new weapons to enum
			for (std::size_t i = 0; i < map.size(); i++)
			{
				int entryPos = 0;

				for (; entryPos < currentEnum->entryCount + i; entryPos++)
				{
					if (!strcmp(map[i]->name, newIndices[enumIndex][entryPos].name))
					{
						ZONETOOL_ERROR("Duplicate playerdatadef entry found: %s", map[i]->name);
					}

					// Search weapon position
					if (checkAlphabeticalOrder(map[i]->name, (char*)newIndices[enumIndex][entryPos].name))
						break;
				}

				for (int j = currentEnum->entryCount + i; j > entryPos; j--)
				{
					newIndices[enumIndex][j] = newIndices[enumIndex][j - 1];
				}

				newIndices[enumIndex][entryPos].index = map[i]->statIndexOffset + lastCacIndex;
				newIndices[enumIndex][entryPos].name = map[i]->name;
			}

			// Apply stuff to current player data
			currentEnum->entryCount = newIndexCount[enumIndex];
			currentEnum->entries = newIndices[enumIndex];
		}

		// Adds new entries to the structuredDataDef
		void IStructuredDataDef::manipulate(StructuredDataDefSet* data)
		{
			// clear existing data if present
			for (int i = 0; i < ENUM_COUNT; i++)
			{
				if (newEntries[i].size() > 0)
				{
					newEntries[i].clear();
				}
			}

			// Patch mp/playerdata.def if needed
			if (!strcmp(data->name, "mp/playerdata.def"))
			{
				// Weapons
				addEntry(ENUM_WEAPONS, 1, "iw5_ak74u");
				addEntry(ENUM_WEAPONS2, 1, "iw5_ak74u");
				addEntry(ENUM_WEAPONS, 2, "iw5_cheytac");
				addEntry(ENUM_WEAPONS2, 2, "iw5_cheytac");

				// Gametypes
				// addEntry(ENUM_GAMETYPES, 1, "oneflag");
				// addEntry(ENUM_GAMETYPES, 2, "vip");
				// addEntry(ENUM_GAMETYPES, 3, "gtnw");

				// Custom camos
				addEntry(ENUM_CAMOS, 1, "plutonium");

				// Reticles
				addEntry(ENUM_RETICLES, 1, "ret7");
				addEntry(ENUM_RETICLES, 2, "ret8");
				addEntry(ENUM_RETICLES, 3, "ret9");
				addEntry(ENUM_RETICLES, 4, "ret10");

				// Attachments
				// 

				ZONETOOL_INFO("Statfiles patched.");
			}
			if (!strcmp(data->name, "mp/recipes.def"))
			{
				// Weapons
				addEntry((enumType_s)0, 1, "iw5_ak74u");
				addEntry((enumType_s)6, 1, "iw5_ak74u");
				addEntry((enumType_s)0, 2, "iw5_cheytac");
				addEntry((enumType_s)6, 2, "iw5_cheytac");

				// Custom camos
				addEntry((enumType_s)2, 1, "plutonium");

				// Reticles
				addEntry((enumType_s)5, 1, "ret7");
				addEntry((enumType_s)5, 2, "ret8");
				addEntry((enumType_s)5, 3, "ret9");
				addEntry((enumType_s)5, 4, "ret10");

				// Gametypes
				// addEntry((enum_type)9, 1, "oneflag");
				// addEntry((enum_type)9, 2, "vip");
				// addEntry((enum_type)9, 3, "gtnw");

				ZONETOOL_INFO("Recipes patched.");
			}

			// Increment version
			data->defs->version += 1;

			// Patch enums
			for (int i = 0; i < ENUM_COUNT; i++)
			{
				if (newEntries[i].size() > 0)
				{
					patchEnumWithMap(data, static_cast<enumType_s>(i), newEntries[i]);
				}
			}
		}

		IStructuredDataDef::IStructuredDataDef()
		{
		}

		IStructuredDataDef::~IStructuredDataDef()
		{
		}

		void IStructuredDataDef::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data(), 1).structureddatadef;

			memset(newIndices, 0, sizeof(StructuredDataEnumEntry*) * ENUM_COUNT);
			memset(newIndexCount, 0, sizeof(int) * ENUM_COUNT);

			for (int i = 0; i < ENUM_COUNT; i++)
			{
				newEntries[i].clear();
			}

			// touch the asset
			manipulate(this->asset_);
		}

		void IStructuredDataDef::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IStructuredDataDef::load_depending(IZone* zone)
		{
		}

		std::string IStructuredDataDef::name()
		{
			return this->name_;
		}

		std::int32_t IStructuredDataDef::type()
		{
			return structureddatadef;
		}

		void IStructuredDataDef::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->asset_;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->defs)
			{
				buf->align(3);
				auto defs = buf->write(data->defs, data->defCount);

				for (unsigned int i = 0; i < data->defCount; i++)
				{
					auto curdef = &data->defs[i];

					if (curdef->enums)
					{
						buf->align(3);
						auto enums = buf->write(curdef->enums, curdef->enumCount);

						for (int j = 0; j < curdef->enumCount; j++)
						{
							if (enums[j].entries)
							{
								buf->align(3);
								auto entries = buf->write(enums[j].entries, enums[j].entryCount);

								for (int k = 0; k < enums[j].entryCount; k++)
								{
									if (entries[k].name)
									{
										buf->write_str(entries[k].name);
										ZoneBuffer::ClearPointer(&entries[k].name);
									}
								}

								ZoneBuffer::ClearPointer(&enums[j].entries);
							}
						}

						ZoneBuffer::ClearPointer(&defs[i].enums);
					}

					if (curdef->structs)
					{
						buf->align(3);
						auto structs = buf->write(curdef->structs, curdef->structCount);

						for (int j = 0; j < curdef->structCount; j++)
						{
							if (structs[j].properties)
							{
								buf->align(3);
								auto props = buf->write(structs[j].properties, structs[j].propertyCount);

								for (int k = 0; k < structs[j].propertyCount; k++)
								{
									if (props[k].name)
									{
										buf->write_str(props[k].name);
										ZoneBuffer::ClearPointer(&props[k].name);
									}
								}

								ZoneBuffer::ClearPointer(&structs[j].properties);
							}
						}

						ZoneBuffer::ClearPointer(&defs[i].structs);
					}

					if (curdef->indexedArrays)
					{
						buf->align(3);
						buf->write(curdef->indexedArrays, curdef->indexedArrayCount);
						ZoneBuffer::ClearPointer(&defs[i].indexedArrays);
					}

					if (curdef->enumedArrays)
					{
						buf->align(3);
						buf->write(curdef->enumedArrays, curdef->enumedArrayCount);
						ZoneBuffer::ClearPointer(&defs[i].enumedArrays);
					}
				}
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IStructuredDataDef::dump(StructuredDataDefSet* asset)
		{
		}
	}
}
