#pragma once

namespace ZoneTool
{
	namespace IW5
	{
		class IClipMap : public IAsset
		{
		private:
			std::string m_name;
			std::string m_filename;

			clipMap_t* m_asset;

		public:
			static clipMap_t* parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem);

			IClipMap();
			~IClipMap();

			void init(const std::string& name, std::shared_ptr<ZoneMemory>& mem) override;
			void prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem) override;
			void load_depending(IZone* zone) override;

			std::string name() override;
			std::int32_t type() override;
			void write_info(IZone* zone, std::shared_ptr<ZoneBuffer>& buf, ClipInfo* data, ClipInfo* dest);
			void write(IZone* zone, std::shared_ptr<ZoneBuffer>& buffer) override;

			static void dump(clipMap_t* asset);
		};
	}
}
