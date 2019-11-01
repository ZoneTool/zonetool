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
	namespace IW4
	{
		struct newEnumEntry
		{
			char* name;
			int statIndexOffset;
		};

		enum enum_type
		{
			features,
			weapons,
			attachments,
			challenges,
			camos,
			perks,
			killstreaks,
			accolades,
			cardicons,
			cardtitles,
			cardnameplates,
			teams,
			gametypes,
			count,
		};

		class IStructuredDataDef : public IAsset
		{
		private:
			StructuredDataEnumEntry* newIndices[enum_type::count];
			int newIndexCount[enum_type::count];
			std::map<int, newEnumEntry*> newEntries[enum_type::count];

			std::string name_;
			StructuredDataDefSet* asset_;

			void add_entry(enum_type type, int stat_index_offset, const std::string& entry_name, ZoneMemory* mem);
			void patch_enum_with_map(StructuredDataDefSet* data, enum_type enumIndex, std::map<int, newEnumEntry*> map, ZoneMemory* mem);
			void manipulate(StructuredDataDefSet* data, ZoneMemory* mem);

		public:
			void init(const std::string& name, ZoneMemory* mem) override;
			void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
			void load_depending(IZone* zone) override;

			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, ZoneBuffer* buffer) override;

			static void dump(StructuredDataDefSet* asset);
		};
	}
}
