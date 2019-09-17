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
		class IMenuDef : public IAsset
		{
		private:
			std::string m_name;
			menuDef_t* m_asset;

		public:
			IMenuDef();
			~IMenuDef();

			void init(const std::string& name, std::shared_ptr<ZoneMemory>& mem) override;
			void prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem) override;
			void load_depending(IZone* zone) override;

			std::string name() override;
			std::int32_t type() override;
			void write_MenuData(IZone* zone, std::shared_ptr<ZoneBuffer>& buf, menuData_t* data);
			void write(IZone* zone, std::shared_ptr<ZoneBuffer>& buffer) override;

			static void dump(menuDef_t* asset);
		};
	}
}
