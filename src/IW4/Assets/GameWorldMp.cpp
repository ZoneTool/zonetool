// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
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
