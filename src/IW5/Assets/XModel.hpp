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
		class IXModel : public IAsset
		{
		private:
			std::string m_name;
			XModel* m_asset;
			bool isXME5OrNewer;

			XModel* parse_new(const std::string& name, ZoneMemory* mem, const std::string& filename);
			XModel* parse(std::string name, ZoneMemory* mem);

		public:
			IXModel();
			~IXModel();

			void init(const std::string& name, ZoneMemory* mem) override;
			void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
			void load_depending(IZone* zone) override;

			void* pointer() override { return m_asset; }
			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, ZoneBuffer* buffer) override;

			static void dump(XModel* asset);
		};
	}
}
