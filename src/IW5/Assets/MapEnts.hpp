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
	namespace IW5
	{
		class IMapEnts : public IAsset
		{
		private:
			std::string m_name;
			MapEnts* m_asset;

		public:
			MapEnts* parse(std::string name, std::shared_ptr<ZoneMemory>& mem);
			IMapEnts();
			~IMapEnts();

			void init(const std::string& name, std::shared_ptr<ZoneMemory>& mem) override;
			void prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem) override;
			void load_depending(IZone* zone) override;
			static void load_depending_internal(IZone* zone, const char* entityString);

			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, std::shared_ptr<ZoneBuffer>& buffer) override;

			static void dump(MapEnts* asset);

			// sadly, this cannot be moved to a CPP file.
			static void write_triggers(IZone* zone, std::shared_ptr<ZoneBuffer>& buf, MapTriggers* dest);
		};
	}
}
