#pragma once

namespace ZoneTool
{
	namespace IW5
	{
		struct newEnumEntry
		{
			char* name;
			int statIndexOffset;
		};

		enum enumType_s
		{
			ENUM_FEATURES,
			ENUM_WEAPONS,
			ENUM_WEAPONS2,
			// Both weapon enums seem different, though they are similar
			ENUM_ATTACHEMENTS,
			ENUM_CHALLENGES,
			ENUM_CAMOS,
			ENUM_RETICLES,
			ENUM_PERKS,
			ENUM_KILLSTREAKS,
			ENUM_ACCOLADES,
			ENUM_CARDNAMEPLATES,
			ENUM_TEAMS,
			ENUM_GAMETYPES,
			ENUM_XP_MULTIPLIERS,
			ENUM_MAX
		};

#define ENUM_COUNT ENUM_MAX

		class IStructuredDataDef : public IAsset
		{
		private:
			StructuredDataEnumEntry* newIndices[ENUM_COUNT];
			int newIndexCount[ENUM_COUNT];
			std::map<int, newEnumEntry*> newEntries[ENUM_COUNT];

			std::string m_name;
			StructuredDataDefSet* m_asset;

			void addEntry(enumType_s type, int statIndexOffset, char* entryName);

			void patchEnumWithMap(StructuredDataDefSet* data, enumType_s enumIndex, std::map<int, newEnumEntry*> map);

			void IStructuredDataDef::manipulate(StructuredDataDefSet* data);

		public:
			IStructuredDataDef();
			~IStructuredDataDef();

			void init(const std::string& name, std::shared_ptr<ZoneMemory>& mem) override;
			void prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem) override;
			void load_depending(IZone* zone) override;

			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, std::shared_ptr<ZoneBuffer>& buffer) override;

			static void dump(StructuredDataDefSet* asset);
		};
	}
}
