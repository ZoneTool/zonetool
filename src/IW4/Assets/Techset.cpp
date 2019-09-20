// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include <d3d9.h>

namespace ZoneTool
{
	namespace IW4
	{
		bool IsMaterialMapNamePart(const std::string& namePart)
		{
			static std::vector<std::string> nameParts =
			{
				"r0",
				"c0",
				"n0",
				"s0",
				"t0",
				"b0",
				"q0",
				"o0",
				"tc",
				"px",
			};

			return std::find(nameParts.begin(), nameParts.end(), namePart) != nameParts.end();
		}

		std::string ConcatStrings(const std::vector<std::string>& strings, std::size_t startAt, std::size_t stopAt)
		{
			auto retVal = ""s;

			if (startAt < 0)
			{
				startAt = 0;
			}

			for (int i = startAt; i < std::max(stopAt, strings.size()); i++)
			{
				retVal += strings[i] + ((i == stopAt - 1) ? "" : "_");
			}

			return retVal;
		}

		// parts that can be removed from the shader
		std::vector<std::string> removableNameParts =
		{
			"fog",
			"dfog",
			"tex",
			"dtex",
			"mdet",
		};

		// parts of the shader name that can be replaced
		std::unordered_map<std::string, std::string> replaceableNameParts =
		{
			{"dfog", "fog"},
			{"dtex", "tex"},
		};

		std::vector<std::string> TryFindIW4Shader(const std::string& iw3Shader)
		{
			std::vector<std::string> alternativeShaders;

			auto nameParts = split(iw3Shader, {'_'});
			for (int i = 0; i < nameParts.size(); i++)
			{
				if (nameParts[i].size() > 2)
				{
					auto substr = nameParts[i].substr(0, 2);

					if (IsMaterialMapNamePart(substr))
					{
						auto front = ConcatStrings(nameParts, 0, i - 1);
						auto trail = ConcatStrings(nameParts, i, -1);

						// attempt to add _fog_ and _dfog_ to the shader name
						alternativeShaders.push_back(front + "_fog_" + trail);
						alternativeShaders.push_back(front + "_dfog_" + trail);

						// attempt to add _mdet_ to the shader name
						alternativeShaders.push_back(front + "_mdet_" + trail);
					}
				}

				// attempt to remove _fog_ / _dfog_ from the material
				auto itr = std::find(removableNameParts.begin(), removableNameParts.end(), nameParts[i]);
				//removableNameParts.find(nameParts[i]);
				if (itr != removableNameParts.end())
				{
					auto front = ConcatStrings(nameParts, 0, i - 1);
					auto trail = ConcatStrings(nameParts, i, -1);

					alternativeShaders.push_back(front + "_" + trail);
				}

				// check if we can replace name parts
				auto itr2 = replaceableNameParts.find(nameParts[i]);
				if (itr2 != replaceableNameParts.end())
				{
					auto front = ConcatStrings(nameParts, 0, i - 1);
					auto trail = ConcatStrings(nameParts, i, -1);

					alternativeShaders.push_back(front + "_" + itr->data() + "_" + trail);
				}
			}

			return alternativeShaders;
		}

		std::unordered_map<std::string, std::string> mappedTechniques =
		{
			// { "mc_l_sm_heat_r0c0n0s0_sat", "mc_l_sm_r0c0n0s0" },
			// { "tools_b0c0", "tools" },
			// { "mc_l_flag_t0c0", "mc_l_hsm_flag_t0c0" },
		};

		bool ITechset::IsMappedTechset(const std::string& name)
		{
			auto itr = mappedTechniques.find(name);
			return itr != mappedTechniques.end();
		}

		std::string ITechset::GetMappedTechset(const std::string& name)
		{
			if (IsMappedTechset(name))
			{
				return mappedTechniques[name];
			}

			return name;
		}

		ITechset::ITechset()
		{
		}

		ITechset::~ITechset()
		{
		}

		enum class techniqueSetType : std::uint8_t
		{
			iw4,
			iw5,
			iw3,
		};

		enum class code_constant : std::uint16_t
		{
			killstreak = 140,
			// custom
			sun_position,
			sun_diffuse,
			sun_specular,
			shadow_params,
			never_dirty_ps_begin,
			// what the fuck is this
			never_dirty_ps_end,
			// ^
			random,
			// custom
		};

		std::unordered_map<std::int32_t, std::int32_t> iw3CodeConstMap =
		{
			{IW3::CONST_SRC_CODE_MAYBE_DIRTY_PS_BEGIN, CONST_SRC_CODE_MAYBE_DIRTY_PS_BEGIN},
			{IW3::CONST_SRC_CODE_LIGHT_POSITION, CONST_SRC_CODE_LIGHT_POSITION},
			{IW3::CONST_SRC_CODE_LIGHT_DIFFUSE, CONST_SRC_CODE_LIGHT_DIFFUSE},
			{IW3::CONST_SRC_CODE_LIGHT_SPECULAR, CONST_SRC_CODE_LIGHT_SPECULAR},
			{IW3::CONST_SRC_CODE_LIGHT_SPOTDIR, CONST_SRC_CODE_LIGHT_SPOTDIR},
			{IW3::CONST_SRC_CODE_LIGHT_SPOTFACTORS, CONST_SRC_CODE_LIGHT_SPOTFACTORS},
			{IW3::CONST_SRC_CODE_NEARPLANE_ORG, CONST_SRC_CODE_NEARPLANE_ORG},
			{IW3::CONST_SRC_CODE_NEARPLANE_DX, CONST_SRC_CODE_NEARPLANE_DX},
			{IW3::CONST_SRC_CODE_NEARPLANE_DY, CONST_SRC_CODE_NEARPLANE_DY},
			{IW3::CONST_SRC_CODE_SHADOW_PARMS, 0},
			{IW3::CONST_SRC_CODE_SHADOWMAP_POLYGON_OFFSET, CONST_SRC_CODE_SHADOWMAP_POLYGON_OFFSET},
			{IW3::CONST_SRC_CODE_RENDER_TARGET_SIZE, CONST_SRC_CODE_RENDER_TARGET_SIZE},
			{IW3::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT, CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT},
			{
				IW3::CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR,
				CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR
			},
			{IW3::CONST_SRC_CODE_DOF_EQUATION_SCENE, CONST_SRC_CODE_DOF_EQUATION_SCENE},
			{IW3::CONST_SRC_CODE_DOF_LERP_SCALE, CONST_SRC_CODE_DOF_LERP_SCALE},
			{IW3::CONST_SRC_CODE_DOF_LERP_BIAS, CONST_SRC_CODE_DOF_LERP_BIAS},
			{IW3::CONST_SRC_CODE_DOF_ROW_DELTA, CONST_SRC_CODE_DOF_ROW_DELTA},
			{IW3::CONST_SRC_CODE_PARTICLE_CLOUD_COLOR, CONST_SRC_CODE_PARTICLE_CLOUD_COLOR},
			{IW3::CONST_SRC_CODE_GAMETIME, CONST_SRC_CODE_GAMETIME},
			{IW3::CONST_SRC_CODE_MAYBE_DIRTY_PS_END, CONST_SRC_CODE_MAYBE_DIRTY_PS_END},
			{IW3::CONST_SRC_CODE_ALWAYS_DIRTY_PS_BEGIN, CONST_SRC_CODE_ALWAYS_DIRTY_PS_BEGIN},
			{IW3::CONST_SRC_CODE_PIXEL_COST_FRACS, CONST_SRC_CODE_PIXEL_COST_FRACS},
			{IW3::CONST_SRC_CODE_PIXEL_COST_DECODE, CONST_SRC_CODE_PIXEL_COST_DECODE},
			{IW3::CONST_SRC_CODE_FILTER_TAP_0, CONST_SRC_CODE_FILTER_TAP_0},
			{IW3::CONST_SRC_CODE_FILTER_TAP_1, CONST_SRC_CODE_FILTER_TAP_1},
			{IW3::CONST_SRC_CODE_FILTER_TAP_2, CONST_SRC_CODE_FILTER_TAP_2},
			{IW3::CONST_SRC_CODE_FILTER_TAP_3, CONST_SRC_CODE_FILTER_TAP_3},
			{IW3::CONST_SRC_CODE_FILTER_TAP_4, CONST_SRC_CODE_FILTER_TAP_4},
			{IW3::CONST_SRC_CODE_FILTER_TAP_5, CONST_SRC_CODE_FILTER_TAP_5},
			{IW3::CONST_SRC_CODE_FILTER_TAP_6, CONST_SRC_CODE_FILTER_TAP_6},
			{IW3::CONST_SRC_CODE_FILTER_TAP_7, CONST_SRC_CODE_FILTER_TAP_7},
			{IW3::CONST_SRC_CODE_COLOR_MATRIX_R, CONST_SRC_CODE_COLOR_MATRIX_R},
			{IW3::CONST_SRC_CODE_COLOR_MATRIX_G, CONST_SRC_CODE_COLOR_MATRIX_G},
			{IW3::CONST_SRC_CODE_COLOR_MATRIX_B, CONST_SRC_CODE_COLOR_MATRIX_B},
			{IW3::CONST_SRC_CODE_ALWAYS_DIRTY_PS_END, CONST_SRC_CODE_ALWAYS_DIRTY_PS_END},
			{IW3::CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION, CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION},
			{IW3::CONST_SRC_CODE_SHADOWMAP_SCALE, CONST_SRC_CODE_SHADOWMAP_SCALE},
			{IW3::CONST_SRC_CODE_ZNEAR, CONST_SRC_CODE_ZNEAR},
			{IW3::CONST_SRC_CODE_SUN_POSITION, CONST_SRC_CODE_LIGHT_POSITION},
			{IW3::CONST_SRC_CODE_SUN_DIFFUSE, CONST_SRC_CODE_LIGHT_DIFFUSE},
			{IW3::CONST_SRC_CODE_SUN_SPECULAR, CONST_SRC_CODE_LIGHT_SPECULAR},
			{IW3::CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE, CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE},
			{IW3::CONST_SRC_CODE_DEBUG_BUMPMAP, CONST_SRC_CODE_DEBUG_BUMPMAP},
			{IW3::CONST_SRC_CODE_MATERIAL_COLOR, CONST_SRC_CODE_MATERIAL_COLOR},
			{IW3::CONST_SRC_CODE_FOG, CONST_SRC_CODE_FOG},
			{IW3::CONST_SRC_CODE_FOG_COLOR, CONST_SRC_CODE_FOG_COLOR_LINEAR},
			{IW3::CONST_SRC_CODE_GLOW_SETUP, CONST_SRC_CODE_GLOW_SETUP},
			{IW3::CONST_SRC_CODE_GLOW_APPLY, CONST_SRC_CODE_GLOW_APPLY},
			{IW3::CONST_SRC_CODE_COLOR_BIAS, CONST_SRC_CODE_COLOR_BIAS},
			{IW3::CONST_SRC_CODE_COLOR_TINT_BASE, CONST_SRC_CODE_COLOR_TINT_BASE},
			{IW3::CONST_SRC_CODE_COLOR_TINT_DELTA, CONST_SRC_CODE_COLOR_TINT_DELTA},
			{IW3::CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS, CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS},
			{IW3::CONST_SRC_CODE_ENVMAP_PARMS, CONST_SRC_CODE_ENVMAP_PARMS},
			{IW3::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST, CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST},
			{IW3::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE, CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE},
			{IW3::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET, CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET},
			{IW3::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX, CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX0},
			{IW3::CONST_SRC_CODE_DEPTH_FROM_CLIP, CONST_SRC_CODE_DEPTH_FROM_CLIP},
			{IW3::CONST_SRC_CODE_CODE_MESH_ARG_0, CONST_SRC_CODE_CODE_MESH_ARG_0},
			{IW3::CONST_SRC_CODE_CODE_MESH_ARG_1, CONST_SRC_CODE_CODE_MESH_ARG_1},
			{IW3::CONST_SRC_CODE_CODE_MESH_ARG_LAST, CONST_SRC_CODE_CODE_MESH_ARG_LAST},
			{IW3::CONST_SRC_CODE_BASE_LIGHTING_COORDS, CONST_SRC_CODE_BASE_LIGHTING_COORDS},
			{IW3::CONST_SRC_CODE_COUNT_FLOAT4, CONST_SRC_CODE_COUNT_FLOAT4},
			{IW3::CONST_SRC_FIRST_CODE_MATRIX, CONST_SRC_FIRST_CODE_MATRIX},
			{IW3::CONST_SRC_CODE_WORLD_MATRIX, CONST_SRC_CODE_WORLD_MATRIX0},
			{IW3::CONST_SRC_CODE_INVERSE_WORLD_MATRIX, CONST_SRC_CODE_INVERSE_WORLD_MATRIX0},
			{IW3::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX, CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0},
			{IW3::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX, CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX0},
			{IW3::CONST_SRC_CODE_VIEW_MATRIX, CONST_SRC_CODE_VIEW_MATRIX},
			{IW3::CONST_SRC_CODE_INVERSE_VIEW_MATRIX, CONST_SRC_CODE_INVERSE_VIEW_MATRIX},
			{IW3::CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX, CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX},
			{IW3::CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX, CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX},
			{IW3::CONST_SRC_CODE_PROJECTION_MATRIX, CONST_SRC_CODE_PROJECTION_MATRIX},
			{IW3::CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX, CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX},
			{IW3::CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX, CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX},
			{
				IW3::CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX,
				CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX
			},
			{IW3::CONST_SRC_CODE_WORLD_VIEW_MATRIX, CONST_SRC_CODE_WORLD_VIEW_MATRIX0},
			{IW3::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX, CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX0},
			{IW3::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX, CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX0},
			{
				IW3::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX,
				CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX0
			},
			{IW3::CONST_SRC_CODE_VIEW_PROJECTION_MATRIX, CONST_SRC_CODE_VIEW_PROJECTION_MATRIX},
			{IW3::CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX, CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX},
			{IW3::CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX, CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX},
			{
				IW3::CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX,
				CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX
			},
			{IW3::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX, CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX0},
			{
				IW3::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX,
				CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX0
			},
			{
				IW3::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX,
				CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0
			},
			{
				IW3::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX,
				CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0
			},
			{IW3::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX, CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX},
			{IW3::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX, CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX},
			{IW3::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX, CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX},
			{
				IW3::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX,
				CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX
			},
			{IW3::CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX, CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX},
			{
				IW3::CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX,
				CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX
			},
			{
				IW3::CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX,
				CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX
			},
			{
				IW3::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX,
				CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX
			},
			{IW3::CONST_SRC_TOTAL_COUNT, CONST_SRC_TOTAL_COUNT},
			{IW3::CONST_SRC_NONE, CONST_SRC_NONE},
		};

		MaterialTechnique* ITechset::parseTechniquePass(const std::string& name, std::shared_ptr<ZoneMemory>& mem,
		                                                std::uint32_t index)
		{
			isExternalTechnique[index] = true;

			FileSystem::PreferLocalOverExternal(true);

			auto path = "techsets\\" + name + ".technique";

			if (!FileSystem::FileExists(path) /*|| !FileSystem::FileExists(statePath)*/)
			{
				FileSystem::PreferLocalOverExternal(false);
				return nullptr;
			}

			ZONETOOL_INFO("Parsing technique %s ...", &name[0]);

			auto fp = FileSystem::FileOpen(path, "rb");
			auto size = FileSystem::FileSize(fp);
			auto bytes = FileSystem::ReadBytes(fp, size);
			FileSystem::PreferLocalOverExternal(false);

			if (FileSystem::IsExternalFile(fp))
			{
				auto log = fopen("techsets.txt", "a");
				if (log)
				{
					fprintf(log, "External technique %s\n", &name[0]);
					fclose(log);
				}
			}
			else
			{
				isExternalTechnique[index] = false;
			}
			FileSystem::FileClose(fp);

			if (bytes.empty())
			{
				ZONETOOL_FATAL("Technique %s appears to be empty you fucking nutsack", &name[0]);
				return nullptr;
			}

			auto json = nlohmann::json::parse(bytes);
			auto techtype = static_cast<techniqueSetType>(json["type"].get<std::uint8_t>());

			auto numPasses = json["hdr"]["numPasses"].get<short>();
			auto asset = mem->ManualAlloc<MaterialTechnique>(
				sizeof MaterialTechnique + (sizeof MaterialPass * numPasses));
			asset->hdr.name = mem->StrDup(json["hdr"]["name"].get<std::string>());
			asset->hdr.unk = json["hdr"]["flags"].get<short>();
			asset->hdr.numPasses = numPasses;

			if (asset->hdr.numPasses > 1)
			{
				// MessageBoxA(NULL, asset->hdr.name, NULL, NULL);
			}

			for (auto pass = 0u; pass < numPasses; pass++)
			{
				auto& curpass = json["pass"][pass];

				if (!curpass["PixelShader"].is_null())
				{
					auto shaderName = curpass["PixelShader"].get<std::string>();

					asset->pass[pass].pixelShader = DB_FindXAssetHeader(pixelshader, &shaderName[0]).pixelshader;
				}
				if (!curpass["VertexDecl"].is_null())
				{
					auto shaderName = curpass["VertexDecl"].get<std::string>();

					asset->pass[pass].vertexDecl = DB_FindXAssetHeader(vertexdecl, &shaderName[0]).vertexdecl;
				}
				if (!curpass["VertexShader"].is_null())
				{
					auto shaderName = curpass["VertexShader"].get<std::string>();

					asset->pass[pass].vertexShader = DB_FindXAssetHeader(vertexshader, &shaderName[0]).vertexshader;
				}

				asset->pass[pass].perPrimArgCount = curpass["perPrimArgCount"].get<char>();
				asset->pass[pass].perObjArgCount = curpass["perObjArgCount"].get<char>();
				asset->pass[pass].stableArgCount = curpass["stableArgCount"].get<char>();
				asset->pass[pass].customSamplerFlags = curpass["customSamplerFlags"].get<char>();

				auto totalCount = asset->pass[pass].perPrimArgCount +
					asset->pass[pass].perObjArgCount +
					asset->pass[pass].stableArgCount;

				// alloc args
				asset->pass[pass].argumentDef = mem->Alloc<ShaderArgumentDef>(totalCount);

				for (auto arg = 0u; arg < totalCount; arg++)
				{
					auto& jsondef = curpass["argumentDef"][arg];
					auto argdef = &asset->pass[pass].argumentDef[arg];

					argdef->dest = jsondef["dest"].get<short>();
					argdef->type = jsondef["type"].get<short>();

					if (techtype == techniqueSetType::iw5)
					{
						// types seem to be +1 when converting
						// from iw4 to iw5. type 1 is the same however.
						if (argdef->type > 2)
						{
							argdef->type--;
						}
					}

					// _D3DSHADER_PARAM_REGISTER_TYPE
					if (argdef->type == 1 || argdef->type == 7)
					{
						if (!jsondef["value"].is_null())
						{
							argdef->u.literalConst = mem->Alloc<float>(4);

							for (auto lit = 0u; lit < 4; lit++)
							{
								argdef->u.literalConst[lit] = jsondef["value"][lit].get<float>();
							}
						}
					}
					else if (argdef->type == 3 || argdef->type == 5)
					{
						argdef->u.codeConst.firstRow = jsondef["value"]["firstRow"].get<char>();
						argdef->u.codeConst.rowCount = jsondef["value"]["rowCount"].get<char>();
						argdef->u.codeConst.index = jsondef["value"]["index"].get<unsigned short>();

						if (techtype == techniqueSetType::iw5)
						{
							if (argdef->u.codeConst.index >= 66)
							{
								argdef->u.codeConst.index -= 8;
							}
							else if (argdef->u.codeConst.index >= 37)
							{
								argdef->u.codeConst.index -= 6;
							}
							else if (argdef->u.codeConst.index >= 26)
							{
								argdef->u.codeConst.index -= 5;
							}
						}
						else if (techtype == techniqueSetType::iw3)
						{
							if (iw3CodeConstMap.find(argdef->u.codeConst.index) != iw3CodeConstMap.end())
							{
								argdef->u.codeConst.index = iw3CodeConstMap[argdef->u.codeConst.index];
							}
							else
							{
								ZONETOOL_FATAL("codeConst %u is not mapped in technique %s!", argdef->u.codeConst.index, asset->hdr.name);
							}
						}
					}
					else
					{
						argdef->u.codeSampler = jsondef["value"].get<std::uint32_t>();
					}
				}
			}

			return asset;
		}

		std::unordered_map<std::int32_t, std::int32_t> iw3TechniqueMap =
		{
			{IW3::TECHNIQUE_DEPTH_PREPASS, TECHNIQUE_DEPTH_PREPASS},
			{IW3::TECHNIQUE_BUILD_FLOAT_Z, TECHNIQUE_BUILD_FLOAT_Z},
			{IW3::TECHNIQUE_BUILD_SHADOWMAP_DEPTH, TECHNIQUE_BUILD_SHADOWMAP_DEPTH},
			{IW3::TECHNIQUE_BUILD_SHADOWMAP_COLOR, TECHNIQUE_BUILD_SHADOWMAP_COLOR},
			{IW3::TECHNIQUE_UNLIT, TECHNIQUE_UNLIT},
			{IW3::TECHNIQUE_EMISSIVE, TECHNIQUE_EMISSIVE},
			{IW3::TECHNIQUE_EMISSIVE_SHADOW, TECHNIQUE_EMISSIVE_SHADOW},
			{IW3::TECHNIQUE_LIT_BEGIN, TECHNIQUE_LIT_BEGIN},
			{IW3::TECHNIQUE_LIT, TECHNIQUE_LIT},
			{IW3::TECHNIQUE_LIT_SUN, TECHNIQUE_LIT_SUN},
			{IW3::TECHNIQUE_LIT_SUN_SHADOW, TECHNIQUE_LIT_SUN_SHADOW},
			{IW3::TECHNIQUE_LIT_SPOT, TECHNIQUE_LIT_SPOT},
			{IW3::TECHNIQUE_LIT_SPOT_SHADOW, TECHNIQUE_LIT_SPOT_SHADOW},
			{IW3::TECHNIQUE_LIT_OMNI, TECHNIQUE_LIT_OMNI},
			{IW3::TECHNIQUE_LIT_OMNI_SHADOW, TECHNIQUE_LIT_OMNI_SHADOW},
			{IW3::TECHNIQUE_LIT_INSTANCED, TECHNIQUE_LIT_INSTANCED},
			{IW3::TECHNIQUE_LIT_INSTANCED_SUN, TECHNIQUE_LIT_INSTANCED_SUN},
			{IW3::TECHNIQUE_LIT_INSTANCED_SUN_SHADOW, TECHNIQUE_LIT_INSTANCED_SUN_SHADOW},
			{IW3::TECHNIQUE_LIT_INSTANCED_SPOT, TECHNIQUE_LIT_INSTANCED_SPOT},
			{IW3::TECHNIQUE_LIT_INSTANCED_OMNI, TECHNIQUE_LIT_INSTANCED_OMNI},
			{IW3::TECHNIQUE_LIT_END, TECHNIQUE_LIT_END},
			{IW3::TECHNIQUE_LIGHT_SPOT, TECHNIQUE_LIGHT_SPOT},
			{IW3::TECHNIQUE_LIGHT_OMNI, TECHNIQUE_LIGHT_OMNI},
			{IW3::TECHNIQUE_LIGHT_SPOT_SHADOW, TECHNIQUE_LIGHT_SPOT_SHADOW},
			{IW3::TECHNIQUE_FAKELIGHT_NORMAL, TECHNIQUE_FAKELIGHT_NORMAL},
			{IW3::TECHNIQUE_FAKELIGHT_VIEW, TECHNIQUE_FAKELIGHT_VIEW},
			{IW3::TECHNIQUE_SUNLIGHT_PREVIEW, TECHNIQUE_SUNLIGHT_PREVIEW},
			{IW3::TECHNIQUE_CASE_TEXTURE, TECHNIQUE_CASE_TEXTURE},
			{IW3::TECHNIQUE_WIREFRAME_SOLID, TECHNIQUE_WIREFRAME_SOLID},
			{IW3::TECHNIQUE_WIREFRAME_SHADED, TECHNIQUE_WIREFRAME_SHADED},
			{IW3::TECHNIQUE_SHADOWCOOKIE_CASTER, 0xFFFFFFFF},
			{IW3::TECHNIQUE_SHADOWCOOKIE_RECEIVER, 0xFFFFFFFF},
			{IW3::TECHNIQUE_DEBUG_BUMPMAP, TECHNIQUE_DEBUG_BUMPMAP},
			{IW3::TECHNIQUE_DEBUG_BUMPMAP_INSTANCED, TECHNIQUE_DEBUG_BUMPMAP_INSTANCED},
			{IW3::TECHNIQUE_COUNT, TECHNIQUE_COUNT},
			{IW3::TECHNIQUE_TOTAL_COUNT, TECHNIQUE_TOTAL_COUNT},
			{IW3::TECHNIQUE_NONE, TECHNIQUE_NONE},
		};

		MaterialTechniqueSet* ITechset::parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			// clear external states
			memset(isExternalTechnique, false, sizeof isExternalTechnique);

			FileSystem::PreferLocalOverExternal(true);
			auto path = "techsets\\" + name + ".techset";
			if (!FileSystem::FileExists(path))
			{
				FileSystem::PreferLocalOverExternal(false);
				return nullptr;
			}

			auto fp = FileSystem::FileOpen(path, "rb");
			auto size = FileSystem::FileSize(fp);
			auto bytes = FileSystem::ReadBytes(fp, size);
			FileSystem::PreferLocalOverExternal(false);

			if (FileSystem::IsExternalFile(fp))
			{
				auto log = fopen("techsets.txt", "a");
				if (log)
				{
					fprintf(log, "External techset %s\n", &name[0]);
					fclose(log);
				}
			}
			FileSystem::FileClose(fp);

			if (bytes.empty())
			{
				ZONETOOL_ERROR("Techset file %s is fucking empty you dumbfuck", &name[0]);
				return nullptr;
			}

			ZONETOOL_INFO("Parsing techset \"%s\"...", &name[0]);

			auto json = nlohmann::json::parse(bytes);

			auto asset = mem->Alloc<MaterialTechniqueSet>();

			asset->name = mem->StrDup(json["name"].get<std::string>());

			auto techtype = static_cast<techniqueSetType>(json["type"].get<std::uint8_t>());

			auto& techniques = json["techniques"];

			// loop through IW5 techniques
			for (auto i = 0u; i < techniques.size(); i++)
			{
				// this variable will hold the destination index of the technique
				auto destIndex = i;

				// remap the techset indexes when porting from IW5
				if (techtype == techniqueSetType::iw5)
				{
					if (i >= 46)
					{
						destIndex -= 6;
					}
					else if (i >= 44)
					{
						destIndex -= 5;
					}
					else if (i >= 31)
					{
						destIndex -= 4;
					}
					else if (i >= 19)
					{
						destIndex -= 2;
					}
				}
				else if (techtype == techniqueSetType::iw3)
				{
					// obtain the IW4 index for the current technique
					destIndex = iw3TechniqueMap[i];

					// skip the technique if its not being used in IW4
					if (destIndex == 0xFFFFFFFF)
					{
						continue;
					}
				}

				// 
				if (techniques[i].is_null())
				{
					asset->techniques[destIndex] = nullptr;
					// skip empty techniques
					continue;
				}

				auto techniqueName = techniques[i]["name"].get<std::string>();

				// first attempt to load the non _sat_ variant
				auto technique = this->parseTechniquePass(RemoveSatFromName(techniqueName), mem, destIndex);

				// load the _sat_ variant if the normal one is not found
				if (!technique)
				{
					technique = this->parseTechniquePass(techniqueName, mem, destIndex);
				}

				// set the technique
				asset->techniques[destIndex] = technique;

				// these techniques do not exist in iw4, use the technique from the index before this one!
				if (techtype == techniqueSetType::iw3)
				{
					/*if (destIndex >= 9 && destIndex <= 29)
					{
						asset->techniques[destIndex + 1] = technique;
					}*/
				}
			}

			return asset;
		}

		void ITechset::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			this->m_name = name;
			this->m_asset = this->parse(this->m_name, mem);

			if (!this->m_asset)
			{
				ZONETOOL_ERROR("Current zone is depending on missing techset \"%s\", zone may not work correctly!", name.data());

				this->m_parsed = false;
				this->m_asset = DB_FindXAssetHeader(this->type(), &this->name()[0]).techset;
			}
			else
			{
				this->m_parsed = true;
			}
		}

		void ITechset::prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
		{
		}

		void ITechset::load_depending(IZone* zone)
		{
			auto data = this->m_asset;

			for (std::int32_t technique = 0; technique < 48; technique++)
			{
				if (data->techniques[technique])
				{
					for (std::int32_t pass = 0; pass < data->techniques[technique]->hdr.numPasses; pass++)
					{
						auto& techniquePass = data->techniques[technique]->pass[pass];

						if (techniquePass.vertexDecl)
						{
							zone->AddAssetOfType(vertexdecl, techniquePass.vertexDecl->name);
						}
						if (techniquePass.vertexShader)
						{
							zone->AddAssetOfType(vertexshader, techniquePass.vertexShader->name);
						}
						if (techniquePass.pixelShader)
						{
							zone->AddAssetOfType(pixelshader, techniquePass.pixelShader->name);
						}
					}
				}
			}
		}

		std::string ITechset::name()
		{
			return this->m_name;
		}

		std::int32_t ITechset::type()
		{
			return techset;
		}

		void ITechset::write(IZone* zone, std::shared_ptr<ZoneBuffer>& buf)
		{
			auto data = this->m_asset;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			dest->remappedTechniques = nullptr;

			for (std::int32_t technique = 0; technique < 48; technique++)
			{
				if (!data->techniques[technique])
				{
					continue;
				}

				buf->align(3);

				auto techniqueHeader = buf->write(&data->techniques[technique]->hdr);
				auto techniquePasses = buf->write(data->techniques[technique]->pass, techniqueHeader->numPasses);

				for (std::int32_t pass = 0; pass < techniqueHeader->numPasses; pass++)
				{
					if (techniquePasses[pass].vertexDecl)
					{
						techniquePasses[pass].vertexDecl =
							reinterpret_cast<VertexDecl*>(zone->GetAssetPointer(
								vertexdecl, techniquePasses[pass].vertexDecl->name));
					}

					if (techniquePasses[pass].vertexShader)
					{
						techniquePasses[pass].vertexShader =
							reinterpret_cast<VertexShader*>(zone->GetAssetPointer(
								vertexshader, techniquePasses[pass].vertexShader->name));
					}

					if (techniquePasses[pass].pixelShader)
					{
						techniquePasses[pass].pixelShader =
							reinterpret_cast<PixelShader*>(zone->GetAssetPointer(
								pixelshader, techniquePasses[pass].pixelShader->name));
					}

					if (techniquePasses[pass].argumentDef)
					{
						buf->align(3);
						auto destArgs = buf->write(techniquePasses[pass].argumentDef,
						                           techniquePasses[pass].perPrimArgCount +
						                           techniquePasses[pass].perObjArgCount +
						                           techniquePasses[pass].stableArgCount);

						for (auto arg = 0; arg <
						     techniquePasses[pass].perPrimArgCount +
						     techniquePasses[pass].perObjArgCount +
						     techniquePasses[pass].stableArgCount; arg++)
						{
							auto curArg = &techniquePasses[pass].argumentDef[arg];

							if (curArg->type == 1 || curArg->type == 7)
							{
								if (destArgs[arg].u.literalConst)
								{
									destArgs[arg].u.literalConst = buf->write_s(3, destArgs[arg].u.literalConst, 4);
								}
							}
						}

						ZoneBuffer::ClearPointer(&techniquePasses[pass].argumentDef);
					}
				}

				buf->write_str(techniqueHeader->name);
				// buf->write_str(va("%s_technique%i", &this->name()[0], technique));
				ZoneBuffer::ClearPointer(&techniqueHeader->name);
				ZoneBuffer::ClearPointer(&dest->techniques[technique]);
			}

			END_LOG_STREAM;
			buf->pop_stream();

			// encrypt_data(dest, sizeof MaterialTechniqueSet);
		}

		void ITechset::dump(MaterialTechniqueSet* asset)
		{
			// if (!FileSystem::FileExists("techsets\\"s + asset->name + ".techset"s))
			{
				dumpToJson(asset);
			}
		}

		void ITechset::dumpTechniquePass(MaterialTechnique* asset)
		{
			auto fp = FileSystem::FileOpen("techsets\\"s + asset->hdr.name + ".technique"s, "wb");
			if (!fp)
			{
				return;
			}

			Json jsonfile;

			jsonfile["type"] = (int)techniqueSetType::iw4;

			jsonfile["hdr"]["name"] = asset->hdr.name;
			jsonfile["hdr"]["numPasses"] = asset->hdr.numPasses;
			jsonfile["hdr"]["flags"] = asset->hdr.unk;

			for (auto pass = 0u; pass < asset->hdr.numPasses; pass++)
			{
				if (asset->pass[pass].vertexDecl)
				{
					jsonfile["pass"][pass]["VertexDecl"] = asset->pass[pass].vertexDecl->name;
				}
				else
				{
					jsonfile["pass"][pass]["VertexDecl"] = nullptr;
				}

				if (asset->pass[pass].pixelShader)
				{
					jsonfile["pass"][pass]["PixelShader"] = asset->pass[pass].pixelShader->name;
				}
				else
				{
					jsonfile["pass"][pass]["PixelShader"] = nullptr;
				}

				if (asset->pass[pass].vertexShader)
				{
					jsonfile["pass"][pass]["VertexShader"] = asset->pass[pass].vertexShader->name;
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
					auto arg = &asset->pass[pass].argumentDef[a];

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

		std::string ITechset::RemoveSatFromName(const std::string& name)
		{
			auto newName = ""s;

			if (name.find("_") != std::string::npos)
			{
				auto nameparts = split(name, {'_'});
				for (auto& part : nameparts)
				{
					if (part == "sat"s)
					{
						continue;
					}

					newName += part + "_";
				}

				return newName.substr(0, newName.length() - 1);
			}

			return name;
		}

		std::unordered_map<std::string, std::vector<std::string>> techniqueUsage;

		void ITechset::dumpToJson(MaterialTechniqueSet* asset)
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
			jsonfile["type"] = (int)techniqueSetType::iw4;

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
