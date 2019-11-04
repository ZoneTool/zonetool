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
		const char* SL_ConvertToString(std::uint16_t index);

		class IXAnimParts : public IAsset
		{
		private:
			std::string name_;
			XAnimParts* asset_;

		public:
			void init(const std::string& name, ZoneMemory* mem) override;
			void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
			void load_depending(IZone* zone) override;

			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, ZoneBuffer* buffer) override;

			static void dump(XAnimParts* asset,
			                 const std::function<const char*(std::uint16_t)>& convertToString = SL_ConvertToString);
		};
	}
}
