#pragma once

#include "../IW5/Assets/GlassWorld.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		class IGameWorldSp : public IAsset
		{
		private:
			std::string m_name;
			GameWorldSp* m_asset;

		public:
			void init(const std::string& name, std::shared_ptr<ZoneMemory>& mem) override;
			void prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem) override;
			void load_depending(IZone* zone) override;

			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, std::shared_ptr<ZoneBuffer>& buffer) override;

			static void dump(GameWorldSp* asset);
		};
	}
}
