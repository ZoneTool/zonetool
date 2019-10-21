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
		class ITechset : public IAsset
		{
		private:
			std::string m_name;
			MaterialTechniqueSet* m_asset;
			bool m_parsed;
			
		public:
			static MaterialTechnique* parse_technique_pass(const std::string& name, std::shared_ptr<ZoneMemory>& mem,
				std::uint32_t index);
			static MaterialTechniqueSet* parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem);
			
			void init(const std::string& name, std::shared_ptr<ZoneMemory>& mem) override;
			void prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem) override;
			void load_depending(IZone* zone) override;

			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, std::shared_ptr<ZoneBuffer>& buffer) override;

			static void dump(MaterialTechniqueSet* asset);
			static void dump_technique_pass(MaterialTechnique* asset);
		};
	}
}
