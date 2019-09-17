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
