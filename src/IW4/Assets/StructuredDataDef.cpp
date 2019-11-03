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
	namespace IW4
	{
		void IStructuredDataDef::add_entry(const enum_type type, const int stat_index_offset, const std::string& entry_name, ZoneMemory* mem)
		{
			const auto new_entry = mem->Alloc<newEnumEntry>();
			new_entry->name = _strdup(entry_name.data());
			new_entry->statIndexOffset = stat_index_offset;
			newEntries[type][newEntries[type].size()] = new_entry;
		}

		bool check_alphabetical_order(std::string entry1, std::string entry2)
		{
			std::transform(entry1.begin(), entry1.end(), entry1.begin(), tolower);
			std::transform(entry2.begin(), entry2.end(), entry2.begin(), tolower);

			const auto len = std::min(entry1.size(), entry2.size());
			const auto e1_l = entry1.data();
			const auto e2_l = entry2.data();

			for (int i = 0; i < len; i++)
			{
				if (*(e1_l + i) > *(e2_l + i))
					return false;
				if (*(e1_l + i) < *(e2_l + i))
					return true;
			}

			return entry1.size() <= entry2.size();
		}

		void IStructuredDataDef::patch_enum_with_map(StructuredDataDefSet* data, enum_type enumIndex,
		                                          std::map<int, newEnumEntry*> map, ZoneMemory* mem)
		{
			const auto current_enum = &data->defs->enums[enumIndex];

			// Check if new enum has already been built
			if (newIndexCount[enumIndex])
			{
				current_enum->entryCount = newIndexCount[enumIndex];
				current_enum->entries = newIndices[enumIndex];
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
			newIndexCount[enumIndex] = current_enum->entryCount + map.size();

			// Find last cac index
			auto last_cac_index = 0;
			for (auto i = 0; i < current_enum->entryCount; i++)
			{
				if (current_enum->entries[i].index > last_cac_index)
				{
					last_cac_index = current_enum->entries[i].index;
				}
			}

			// Create new enum
			newIndices[enumIndex] = mem->ManualAlloc<StructuredDataEnumEntry>(sizeof(StructuredDataEnumEntry) * (current_enum->entryCount + map.size() + 1));
			memcpy(newIndices[enumIndex], current_enum->entries, sizeof(StructuredDataEnumEntry) * current_enum->entryCount);

			// Apply new weapons to enum
			for (std::size_t i = 0; i < map.size(); i++)
			{
				auto entry_pos = 0;

				for (; entry_pos < current_enum->entryCount + i; entry_pos++)
				{
					if (!strcmp(map[i]->name, newIndices[enumIndex][entry_pos].name))
					{
						ZONETOOL_FATAL("Duplicate playerdatadef entry found: %s", map[i]->name);
					}

					// Search weapon position
					if (check_alphabetical_order(map[i]->name, (char*)newIndices[enumIndex][entry_pos].name))
					{
						break;
					}
				}

				for (int j = current_enum->entryCount + i; j > entry_pos; j--)
				{
					newIndices[enumIndex][j] = newIndices[enumIndex][j - 1];
				}

				newIndices[enumIndex][entry_pos].index = map[i]->statIndexOffset + last_cac_index;
				newIndices[enumIndex][entry_pos].name = map[i]->name;
			}

			// Apply stuff to current player data
			current_enum->entryCount = newIndexCount[enumIndex];
			current_enum->entries = newIndices[enumIndex];
		}

		// Adds new entries to the structuredDataDef
		void IStructuredDataDef::manipulate(StructuredDataDefSet* data, ZoneMemory* mem)
		{
			// clear existing data if present
			for (auto i = 0; i < enum_type::count; i++)
			{
				if (!newEntries[i].empty())
				{
					newEntries[i].clear();
				}
			}

			// Patch mp/playerdata.def if needed
			if (!strcmp(data->name, "mp/playerdata.def"))
			{
				// add new entries here
				add_entry(enum_type::weapons, 1, "cm901", mem);

				ZONETOOL_INFO("Statfiles patched.");
			}

			// Increment version
			data->defs->version += 1;

			// Patch enums
			for (auto i = 0; i < enum_type::count; i++)
			{
				if (!newEntries[i].empty())
				{
					patch_enum_with_map(data, static_cast<enum_type>(i), newEntries[i], mem);
				}
			}
		}

		void IStructuredDataDef::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data()).structureddatadef;

			memset(newIndices, 0, sizeof(StructuredDataEnumEntry*) * enum_type::count);
			memset(newIndexCount, 0, sizeof(int) * enum_type::count);

			for (int i = 0; i < enum_type::count; i++)
			{
				newEntries[i].clear();
			}

			// touch the asset
			manipulate(this->asset_, mem);
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
