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
	namespace IW3
	{
		class ITechset
		{
		public:
			static void dumpTechniquePass(MaterialTechnique* asset);
			static void dump(MaterialTechniqueSet* asset);
			static void dumpVertexDecl(const char* name, MaterialVertexDeclaration* vertex);
			static void dumpVertexShader(MaterialVertexShader* shader);
			static void dumpPixelShader(MaterialPixelShader* shader);
		};
	}
}
