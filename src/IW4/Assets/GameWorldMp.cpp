#include "stdafx.hpp"
#include "../IW5/Assets/GlassWorld.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		std::int32_t IGameWorldMp::type()
		{
			return game_map_mp;
		}

		void IGameWorldMp::dump(GameWorldMp* asset)
		{
			IGlassWorld::dump(reinterpret_cast<IW5::GlassWorld*>(asset));
		}
	}
}
