// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"

#include "../IW4/Assets/VertexDecl.hpp"
#include "../IW4/Assets/VertexShader.hpp"
#include "../IW4/Assets/PixelShader.hpp"

#pragma comment(lib, "ntdll.lib")

namespace ZoneTool
{
	namespace IW3
	{
		std::string GenerateNameForVertexDecl(MaterialVertexDeclaration* vertexDecl)
		{
			// base name + crc32
			auto name = "iw3_vertexdecl_"s;
			auto crc32 = RtlCrc32(vertexDecl->routing.data, sizeof(MaterialStreamRouting) * vertexDecl->streamCount, 0);

			// append crc32 to vertexdecl name
			name += std::to_string(crc32);

			// return generated name
			return name;
		}

		void ITechset::dumpVertexDecl(const char* name, MaterialVertexDeclaration* vertex)
		{
			// convert to IW4
			auto asset = new IW4::VertexDecl;
			memset(asset, 0, sizeof IW4::VertexDecl);

			asset->name = name;

			asset->hasOptionalSource = vertex->hasOptionalSource;
			asset->streamCount = vertex->streamCount;

			memcpy(asset->streams, vertex->routing.data, sizeof asset->streams);
			memcpy(asset->declarations, vertex->routing.decl, sizeof(void*) * 16);

			for (int i = 0; i < asset->streamCount; i++)
			{
				if (asset->streams[i].dest >= 4)
				{
					asset->streams[i].dest += 1;
				}
			}

			// shit out a warning
			if (asset->streamCount > 13)
			{
				ZONETOOL_ERROR("Vertexdecl %s has more than 13 streams.", name);
			}

			// lets pray it works
			IW4::IVertexDecl::dump(asset);

			delete[] asset;
		}

		void ITechset::dumpVertexShader(MaterialVertexShader* shader)
		{
			// convert to IW4
			auto asset = new IW4::VertexShader;
			memset(asset, 0, sizeof IW4::VertexShader);

			asset->name = shader->name;
			asset->shader = shader->prog.vs;
			asset->codeLen = shader->prog.loadDef.programSize;
			asset->bytecode = PDWORD(shader->prog.loadDef.program);

			// dump shader
			IW4::IVertexShader::dump(asset);

			delete[] asset;
		}

		void ITechset::dumpPixelShader(MaterialPixelShader* shader)
		{
			// convert to IW4
			auto asset = new IW4::PixelShader;
			memset(asset, 0, sizeof IW4::PixelShader);

			asset->name = shader->name;
			asset->shader = shader->prog.ps;
			asset->codeLen = shader->prog.loadDef.programSize;
			asset->bytecode = PDWORD(shader->prog.loadDef.program);

			// dump shader
			IW4::IPixelShader::dump(asset);

			delete[] asset;
		}

		void ITechset::dumpTechniquePass(MaterialTechnique* asset)
		{
			auto fp = FileSystem::FileOpen("techsets\\"s + asset->hdr.name + ".technique"s, "wb");
			if (!fp)
			{
				return;
			}

			Json jsonfile;

			jsonfile["type"] = 2;

			jsonfile["hdr"]["name"] = asset->hdr.name;
			jsonfile["hdr"]["numPasses"] = asset->hdr.numPasses;
			jsonfile["hdr"]["flags"] = asset->hdr.flags;

			for (auto pass = 0u; pass < asset->hdr.numPasses; pass++)
			{
				if (asset->pass[pass].vertexDecl)
				{
					auto vertexDeclName = GenerateNameForVertexDecl(asset->pass[pass].vertexDecl);
					jsonfile["pass"][pass]["VertexDecl"] = vertexDeclName;
					dumpVertexDecl(&vertexDeclName[0], asset->pass[pass].vertexDecl);
				}
				else
				{
					jsonfile["pass"][pass]["VertexDecl"] = nullptr;
				}

				if (asset->pass[pass].pixelShader)
				{
					jsonfile["pass"][pass]["PixelShader"] = asset->pass[pass].pixelShader->name;
					dumpPixelShader(asset->pass[pass].pixelShader);
				}
				else
				{
					jsonfile["pass"][pass]["PixelShader"] = nullptr;
				}

				if (asset->pass[pass].vertexShader)
				{
					jsonfile["pass"][pass]["VertexShader"] = asset->pass[pass].vertexShader->name;
					dumpVertexShader(asset->pass[pass].vertexShader);
				}
				else
				{
					jsonfile["pass"][pass]["VertexShader"] = nullptr;
				}

				jsonfile["pass"][pass]["perPrimArgCount"] = asset->pass[pass].perPrimArgCount;
				jsonfile["pass"][pass]["perObjArgCount"] = asset->pass[pass].perObjArgCount;
				jsonfile["pass"][pass]["stableArgCount"] = asset->pass[pass].stableArgCount;
				jsonfile["pass"][pass]["customSamplerFlags"] = asset->pass[pass].customSamplerFlags;

				for (auto a = 0u; a < asset->pass[pass].perPrimArgCount +
				     asset->pass[pass].perObjArgCount +
				     asset->pass[pass].stableArgCount; a++)
				{
					auto arg = &asset->pass[pass].args[a];

					jsonfile["pass"][pass]["argumentDef"][a]["dest"] = arg->dest;
					jsonfile["pass"][pass]["argumentDef"][a]["type"] = arg->type;

					if (arg->type == 1 || arg->type == 7)
					{
						if (arg->u.literalConst != nullptr)
						{
							for (size_t j = 0; j < 4; j++)
							{
								jsonfile["pass"][pass]["argumentDef"][a]["value"][j] = arg->u.literalConst[j];
							}
						}
						else
						{
							jsonfile["pass"][pass]["argumentDef"][a]["value"] = nullptr;
						}
					}
					else if (arg->type == 3 || arg->type == 5)
					{
						jsonfile["pass"][pass]["argumentDef"][a]["value"]["firstRow"] = arg->u.codeConst.firstRow;
						jsonfile["pass"][pass]["argumentDef"][a]["value"]["rowCount"] = arg->u.codeConst.rowCount;
						jsonfile["pass"][pass]["argumentDef"][a]["value"]["index"] = arg->u.codeConst.index;
					}
					else
					{
						jsonfile["pass"][pass]["argumentDef"][a]["value"] = arg->u.codeSampler;
					}
				}
			}

			// Write JSON Object to file
			auto ob = Json(jsonfile).dump();
			fwrite(&ob[0], ob.size(), 1, fp);
			fflush(fp);
			fclose(fp);
		}

		void ITechset::dump(MaterialTechniqueSet* asset)
		{
			auto fp = FileSystem::FileOpen("techsets\\"s + asset->name + ".techset"s, "wb");
			if (!fp)
			{
				return;
			}

			constexpr auto numTechniques = sizeof(asset->techniques) / sizeof(MaterialTechnique*);

			std::map<std::string, Json> jsonfile;

			jsonfile["name"] = asset->name;
			jsonfile["pad"] = asset->pad;
			jsonfile["type"] = 2;

			for (auto i = 0u; i < numTechniques; i++)
			{
				if (asset->techniques[i])
				{
					jsonfile["techniques"][i]["name"] = asset->techniques[i]->hdr.name;
					jsonfile["techniques"][i]["index"] = i;

					dumpTechniquePass(asset->techniques[i]);
				}
				else
				{
					jsonfile["techniques"][i] = nullptr;
				}
			}

			// Write JSON Object to file
			auto ob = Json(jsonfile).dump();
			fwrite(&ob[0], ob.size(), 1, fp);
			fflush(fp);
			fclose(fp);
		}
	}
}
