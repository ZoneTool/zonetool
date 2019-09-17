#pragma once

namespace ZoneTool
{
	namespace IW5
	{
		class IGfxImage : public IAsset
		{
		private:
			std::string m_name;
			GfxImage* m_asset;
			bool isMapImage;

			std::string clean_name(const std::string& name);
			GfxImage* parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem);

		public:
			IGfxImage();
			~IGfxImage();


			void init(const std::string& name, std::shared_ptr<ZoneMemory>& mem) override;
			void init(void* asset, std::shared_ptr<ZoneMemory>& mem) override;

			void prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem) override;
			void load_depending(IZone* zone) override;

			void* pointer() override { return m_asset; }
			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, std::shared_ptr<ZoneBuffer>& buffer) override;

			static void dump(GfxImage* asset);
		};
	}
}
