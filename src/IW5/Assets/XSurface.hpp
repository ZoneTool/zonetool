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
		class IXSurface : public IAsset
		{
		private:
			std::string m_name;
			ModelSurface* m_asset;

			void write_xsurfices(IZone* zone, std::shared_ptr<ZoneBuffer>& buf, XSurface* data, std::int16_t count);

		public:
			IXSurface();
			~IXSurface();

			ModelSurface* parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem);

			void init(const std::string& name, std::shared_ptr<ZoneMemory>& mem) override;
			void init(void* asset, std::shared_ptr<ZoneMemory>& mem) override;

			void prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem) override;
			void load_depending(IZone* zone) override;

			void* pointer() override { return m_asset; }
			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, std::shared_ptr<ZoneBuffer>& buffer) override;

			static void dump(ModelSurface* asset);
		};
	}
}
