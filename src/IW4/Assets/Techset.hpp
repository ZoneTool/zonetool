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
		class ITechset : public IAsset
		{
		private:
			std::string name_;
			MaterialTechniqueSet* asset_;
			bool m_parsed;
			
		public:
			static MaterialTechniqueSet* parse(const std::string& name, ZoneMemory* mem);
			static char* parse_statebits(const std::string& techset, ZoneMemory* mem);
			
			void init(const std::string& name, ZoneMemory* mem) override;
			void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
			void load_depending(IZone* zone) override;
			void* pointer() override { return asset_; }

			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, ZoneBuffer* buffer) override;

			static void dump_statebits(const std::string& techset, char* statebits);
			static void dump(MaterialTechniqueSet* asset);
		};
	}
}
#pragma once
