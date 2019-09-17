#pragma once

#include "../IW5/Assets/GlassWorld.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		class IGameWorldMp : public IW5::IGlassWorld
		{
		public:
			std::int32_t type() override;
			static void dump(GameWorldMp* asset);
		};
	}
}
