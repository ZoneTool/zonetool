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
		static const char* SL_ConvertToString(std::uint16_t index);

		class IXSurface;

		class IXModel : public IAsset
		{
		private:
			std::string name_;
			XModel* asset_ = nullptr;

			XModel* parse_new(const std::string& name, ZoneMemory* mem, const std::string& filename);
			XModel* parse(std::string name, ZoneMemory* mem);

		public:
			void init(const std::string& name, ZoneMemory* mem) override;
			void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
			void load_depending(IZone* zone) override;

			void* pointer() override { return asset_; }
			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, ZoneBuffer* buffer) override;

			static XModel* generate_iw5_camo_model(const std::string& camo, XModel* asset);
			static XModel* remove_attachments(XModel* asset);
			static void dump(XModel* asset,
			                 const std::function<const char*(uint16_t)>& convertToString = SL_ConvertToString);
		};
	}
}
