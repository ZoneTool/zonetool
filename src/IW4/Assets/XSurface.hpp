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
		class IXSurface : public IAsset
		{
		private:
			std::string name_;
			XModelSurfs* asset_ = nullptr;

		public:
			XModelSurfs* parse(const std::string& name, ZoneMemory* mem);

			void init(const std::string& name, ZoneMemory* mem) override;
			void init(void* asset, ZoneMemory* mem) override;

			void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
			void load_depending(IZone* zone) override;

			void* pointer() override { return asset_; }
			std::string name() override;
			std::int32_t type() override;
			void write_xsurfaces(IZone* zone, ZoneBuffer* buf, XSurface* data, std::uint16_t count);
			void write(IZone* zone, ZoneBuffer* buffer) override;

			static void dump(XModelSurfs* asset);
		};
	}
}
