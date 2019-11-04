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
		class IGfxImage : public IAsset
		{
		private:
			std::string name_;
			GfxImage* asset_;
			bool isMapImage;

			std::string clean_name(const std::string& name);
			GfxImage* parse(const std::string& name, ZoneMemory* mem);

		public:
			IGfxImage();
			~IGfxImage();

			static void dump_iwi(const std::string& name);

			void init(const std::string& name, ZoneMemory* mem) override;
			void init(void* asset, ZoneMemory* mem) override;

			void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
			void load_depending(IZone* zone) override;

			void* pointer() override { return asset_; }
			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, ZoneBuffer* buffer) override;

			static void dump(GfxImage* asset);
		};
	}
}
