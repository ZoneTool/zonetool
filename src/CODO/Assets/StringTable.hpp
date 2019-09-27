// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

namespace ZoneTool::CODO
{
	class IStringTable : public IAsset
	{
	private:
		std::string m_name;
		StringTable* m_asset;

	public:
		IStringTable();
		~IStringTable();

		void init(const std::string& name, std::shared_ptr<ZoneMemory>& mem) override;
		void prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem) override;
		void load_depending(IZone* zone) override;

		std::string name() override;
		std::int32_t type() override;
		void write(IZone* zone, std::shared_ptr<ZoneBuffer>& buffer) override;

		static void dump(StringTable* asset);
	};
}
