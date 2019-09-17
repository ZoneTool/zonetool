#pragma once

namespace ZoneTool
{
	namespace IW5
	{
		class IFxWorld : public IAsset
		{
		private:
			std::string m_name;
			FxWorld* m_asset;
			GlassWorld* glassmap;

		public:
			IFxWorld();
			~IFxWorld();

			static FxWorld* parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem);

			void init(const std::string& name, std::shared_ptr<ZoneMemory>& mem) override;
			void prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem) override;
			void load_depending(IZone* zone) override;

			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, std::shared_ptr<ZoneBuffer>& buffer) override;

			static void dump(FxWorld* asset);
		};
	}
}
