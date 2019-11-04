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
		class IPhysCollmap : public IAsset
		{
		private:
			std::string name_;
			PhysCollmap* asset_;

		public:
			IPhysCollmap();
			~IPhysCollmap();

			PhysCollmap* parse(const std::string& name, ZoneMemory* mem);

			void init(const std::string& name, ZoneMemory* mem) override;
			void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
			void load_depending(IZone* zone) override;

			std::string name() override;
			std::int32_t type() override;
			void write_CBrushWrapper(IZone* zone, ZoneBuffer* buf, cbrush_t* data);
			void write_BrushWrapper(IZone* zone, ZoneBuffer* buf, BrushWrapper* data);
			void write_PhysGeomInfo(IZone* zone, ZoneBuffer* buf, PhysGeomInfo* dest);
			void write(IZone* zone, ZoneBuffer* buffer) override;

			static void dump(PhysCollmap* asset);
		};
	}
}
