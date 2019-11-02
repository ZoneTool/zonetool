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
			std::string m_name;
			XModel* m_asset;
			bool isXME5OrNewer;

			XModel* parse_new(const std::string& name, ZoneMemory* mem, const std::string& filename);
			XModel* parse(std::string name, ZoneMemory* mem);

			IXSurface* dependingSurfaces[4];

		public:
			void init(const std::string& name, ZoneMemory* mem) override;
			void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
			void load_depending(IZone* zone) override;

			void* pointer() override { return m_asset; }
			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, ZoneBuffer* buffer) override;

			static XModel* remove_attachments(XModel* asset);
			static void dump(XModel* asset,
			                 const std::function<const char*(uint16_t)>& convertToString = SL_ConvertToString);
		};
	}
}
