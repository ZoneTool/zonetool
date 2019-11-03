// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once
#include "IW4/Structs.hpp"

namespace ZoneTool
{
	namespace IW3
	{
		class ITechset
		{
		public:
			static void dumpTechniquePass(MaterialTechnique* asset);
			static void dump(MaterialTechniqueSet* asset, ZoneMemory* mem);
			static IW4::VertexDecl* dump_vertex_decl(const std::string& name, MaterialVertexDeclaration* vertex, ZoneMemory* mem);
			static IW4::VertexShader* dump_vertex_shader(MaterialVertexShader* shader, ZoneMemory* mem);
			static IW4::PixelShader* dump_pixel_shader(MaterialPixelShader* shader, ZoneMemory* mem);
			static void dump_statebits(const std::string& techset, char* statebits);
		};
	}
}
