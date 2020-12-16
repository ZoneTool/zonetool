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
		short SL_AllocString(const std::string& str);
		const char* SL_ConvertToString(std::uint16_t index);
		
		class IXModel : public IAsset
		{
		private:
			std::string name_;
			XModel* asset_ = nullptr;
			bool is_scope_model_;

		public:
			static XModel* parse_new(const std::string& name, ZoneMemory* mem, const std::string& filename, const std::function<std::uint16_t(const std::string&)>& allocString = SL_AllocString);
			static XModel* parse(std::string name, ZoneMemory* mem, const std::function<std::uint16_t(const std::string&)>& allocString = SL_AllocString);

			void init(const std::string& name, ZoneMemory* mem) override;
			void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
			void load_depending(IZone* zone) override;

			void* pointer() override { return asset_; }
			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, ZoneBuffer* buffer) override;

			static void dump(XModel* asset,
				const std::function<const char* (std::uint16_t)>& convertToString = SL_ConvertToString);
		};
	}
}
