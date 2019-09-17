#pragma once

namespace ZoneTool
{
	namespace IW3
	{
		class IMaterial
		{
		public:
			static void dump(Material* asset);
			static void dumpStateBits(Material* mat);
		};
	}
}
