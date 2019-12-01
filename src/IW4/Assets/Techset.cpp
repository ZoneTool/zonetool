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
#include "IW5/Assets/Techset.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		std::unordered_map<std::int32_t, std::int32_t> constant_map =
		{
			{ IW5::CONST_SRC_CODE_LIGHT_POSITION, CONST_SRC_CODE_LIGHT_POSITION },
			{ IW5::CONST_SRC_CODE_LIGHT_DIFFUSE, CONST_SRC_CODE_LIGHT_DIFFUSE },
			{ IW5::CONST_SRC_CODE_LIGHT_SPECULAR, CONST_SRC_CODE_LIGHT_SPECULAR },
			{ IW5::CONST_SRC_CODE_LIGHT_SPOTDIR, CONST_SRC_CODE_LIGHT_SPOTDIR },
			{ IW5::CONST_SRC_CODE_LIGHT_SPOTFACTORS, CONST_SRC_CODE_LIGHT_SPOTFACTORS },
			{ IW5::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT, CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT },
			{ IW5::CONST_SRC_CODE_PARTICLE_CLOUD_COLOR, CONST_SRC_CODE_PARTICLE_CLOUD_COLOR },
			{ IW5::CONST_SRC_CODE_GAMETIME, CONST_SRC_CODE_GAMETIME },
			//{ IW5::CONST_SRC_CODE_EYEOFFSET, CONST_SRC_CODE_EYEOFFSET },
			//{ IW5::CONST_SRC_CODE_COLOR_SATURATION_R, CONST_SRC_CODE_COLOR_SATURATION_R },
			//{ IW5::CONST_SRC_CODE_COLOR_SATURATION_G, CONST_SRC_CODE_COLOR_SATURATION_G },
			//{ IW5::CONST_SRC_CODE_COLOR_SATURATION_B, CONST_SRC_CODE_COLOR_SATURATION_B },
			{ IW5::CONST_SRC_CODE_PIXEL_COST_FRACS, CONST_SRC_CODE_PIXEL_COST_FRACS },
			{ IW5::CONST_SRC_CODE_PIXEL_COST_DECODE, CONST_SRC_CODE_PIXEL_COST_DECODE },
			{ IW5::CONST_SRC_CODE_FILTER_TAP_0, CONST_SRC_CODE_FILTER_TAP_0 },
			{ IW5::CONST_SRC_CODE_FILTER_TAP_1, CONST_SRC_CODE_FILTER_TAP_1 },
			{ IW5::CONST_SRC_CODE_FILTER_TAP_2, CONST_SRC_CODE_FILTER_TAP_2 },
			{ IW5::CONST_SRC_CODE_FILTER_TAP_3, CONST_SRC_CODE_FILTER_TAP_3 },
			{ IW5::CONST_SRC_CODE_FILTER_TAP_4, CONST_SRC_CODE_FILTER_TAP_4 },
			{ IW5::CONST_SRC_CODE_FILTER_TAP_5, CONST_SRC_CODE_FILTER_TAP_5 },
			{ IW5::CONST_SRC_CODE_FILTER_TAP_6, CONST_SRC_CODE_FILTER_TAP_6 },
			{ IW5::CONST_SRC_CODE_FILTER_TAP_7, CONST_SRC_CODE_FILTER_TAP_7 },
			{ IW5::CONST_SRC_CODE_COLOR_MATRIX_R, CONST_SRC_CODE_COLOR_MATRIX_R },
			{ IW5::CONST_SRC_CODE_COLOR_MATRIX_G, CONST_SRC_CODE_COLOR_MATRIX_G },
			{ IW5::CONST_SRC_CODE_COLOR_MATRIX_B, CONST_SRC_CODE_COLOR_MATRIX_B },
			{ IW5::CONST_SRC_CODE_RENDER_TARGET_SIZE, CONST_SRC_CODE_RENDER_TARGET_SIZE },
			{ IW5::CONST_SRC_CODE_SHADOWMAP_POLYGON_OFFSET, CONST_SRC_CODE_SHADOWMAP_POLYGON_OFFSET},
			//{ IW5::CONST_SRC_CODE_RENDER_SOURCE_SIZE, CONST_SRC_CODE_RENDER_SOURCE_SIZE },
			{ IW5::CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR, CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR },
			{ IW5::CONST_SRC_CODE_DOF_EQUATION_SCENE, CONST_SRC_CODE_DOF_EQUATION_SCENE },
			{ IW5::CONST_SRC_CODE_DOF_LERP_SCALE, CONST_SRC_CODE_DOF_LERP_SCALE },
			{ IW5::CONST_SRC_CODE_DOF_LERP_BIAS, CONST_SRC_CODE_DOF_LERP_BIAS },
			{ IW5::CONST_SRC_CODE_DOF_ROW_DELTA, CONST_SRC_CODE_DOF_ROW_DELTA },
			{ IW5::CONST_SRC_CODE_MOTION_MATRIX_X, CONST_SRC_CODE_MOTION_MATRIX_X },
			{ IW5::CONST_SRC_CODE_MOTION_MATRIX_Y, CONST_SRC_CODE_MOTION_MATRIX_Y },
			{ IW5::CONST_SRC_CODE_MOTION_MATRIX_W, CONST_SRC_CODE_MOTION_MATRIX_W },
			{ IW5::CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION, CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION },
			{ IW5::CONST_SRC_CODE_SHADOWMAP_SCALE, CONST_SRC_CODE_SHADOWMAP_SCALE },
			{ IW5::CONST_SRC_CODE_ZNEAR, CONST_SRC_CODE_ZNEAR },
			{ IW5::CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE, CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE },
			{ IW5::CONST_SRC_CODE_DEBUG_BUMPMAP, CONST_SRC_CODE_DEBUG_BUMPMAP },
			{ IW5::CONST_SRC_CODE_MATERIAL_COLOR, CONST_SRC_CODE_MATERIAL_COLOR },
			{ IW5::CONST_SRC_CODE_FOG, CONST_SRC_CODE_FOG },
			{ IW5::CONST_SRC_CODE_FOG_COLOR_LINEAR, CONST_SRC_CODE_FOG_COLOR_LINEAR },
			{ IW5::CONST_SRC_CODE_FOG_COLOR_GAMMA, CONST_SRC_CODE_FOG_COLOR_GAMMA },
			{ IW5::CONST_SRC_CODE_FOG_SUN_CONSTS, CONST_SRC_CODE_FOG_SUN_CONSTS },
			{ IW5::CONST_SRC_CODE_FOG_SUN_COLOR_LINEAR, CONST_SRC_CODE_FOG_SUN_COLOR_LINEAR },
			{ IW5::CONST_SRC_CODE_FOG_SUN_COLOR_GAMMA, CONST_SRC_CODE_FOG_SUN_COLOR_GAMMA },
			{ IW5::CONST_SRC_CODE_FOG_SUN_DIR, CONST_SRC_CODE_FOG_SUN_DIR },
			{ IW5::CONST_SRC_CODE_GLOW_SETUP, CONST_SRC_CODE_GLOW_SETUP },
			{ IW5::CONST_SRC_CODE_GLOW_APPLY, CONST_SRC_CODE_GLOW_APPLY },
			{ IW5::CONST_SRC_CODE_COLOR_BIAS, CONST_SRC_CODE_COLOR_BIAS },
			{ IW5::CONST_SRC_CODE_COLOR_TINT_BASE, CONST_SRC_CODE_COLOR_TINT_BASE },
			{ IW5::CONST_SRC_CODE_COLOR_TINT_DELTA, CONST_SRC_CODE_COLOR_TINT_DELTA },
			{ IW5::CONST_SRC_CODE_COLOR_TINT_QUADRATIC_DELTA, CONST_SRC_CODE_COLOR_TINT_QUADRATIC_DELTA },
			{ IW5::CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS, CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS },
			{ IW5::CONST_SRC_CODE_ENVMAP_PARMS, CONST_SRC_CODE_ENVMAP_PARMS },
			{ IW5::CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST, CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST },
			{ IW5::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST, CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST },
			{ IW5::CONST_SRC_CODE_COMPOSITE_FX_DISTORTION, CONST_SRC_CODE_COMPOSITE_FX_DISTORTION },
			{ IW5::CONST_SRC_CODE_POSTFX_FADE_EFFECT, CONST_SRC_CODE_POSTFX_FADE_EFFECT },
			{ IW5::CONST_SRC_CODE_VIEWPORT_DIMENSIONS, CONST_SRC_CODE_VIEWPORT_DIMENSIONS },
			{ IW5::CONST_SRC_CODE_FRAMEBUFFER_READ, CONST_SRC_CODE_FRAMEBUFFER_READ },
			//{ IW5::CONST_SRC_CODE_THERMAL_COLOR_OFFSET, CONST_SRC_CODE_THERMAL_COLOR_OFFSET },
			//{ IW5::CONST_SRC_CODE_PLAYLIST_POPULATION_PARAMS, CONST_SRC_CODE_PLAYLIST_POPULATION_PARAMS },
			{ IW5::CONST_SRC_CODE_BASE_LIGHTING_COORDS, CONST_SRC_CODE_BASE_LIGHTING_COORDS },
			{ IW5::CONST_SRC_CODE_LIGHT_PROBE_AMBIENT, CONST_SRC_CODE_LIGHT_PROBE_AMBIENT },
			{ IW5::CONST_SRC_CODE_NEARPLANE_ORG, CONST_SRC_CODE_NEARPLANE_ORG },
			{ IW5::CONST_SRC_CODE_NEARPLANE_DX, CONST_SRC_CODE_NEARPLANE_DX },
			{ IW5::CONST_SRC_CODE_NEARPLANE_DY, CONST_SRC_CODE_NEARPLANE_DY },
			{ IW5::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE, CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE },
			{ IW5::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET, CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET },
			{ IW5::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX0, CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX0 },
			{ IW5::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX1, CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX1 },
			{ IW5::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX2, CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX2 },
			{ IW5::CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR0, CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR0 },
			{ IW5::CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR1, CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR1 },
			{ IW5::CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR2, CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR2 },
			{ IW5::CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM0, CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM0 },
			{ IW5::CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM1, CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM1 },
			{ IW5::CONST_SRC_CODE_DEPTH_FROM_CLIP, CONST_SRC_CODE_DEPTH_FROM_CLIP },
			{ IW5::CONST_SRC_CODE_CODE_MESH_ARG_0, CONST_SRC_CODE_CODE_MESH_ARG_0 },
			{ IW5::CONST_SRC_CODE_CODE_MESH_ARG_1, CONST_SRC_CODE_CODE_MESH_ARG_1 },
			{ IW5::CONST_SRC_CODE_VIEW_MATRIX, CONST_SRC_CODE_VIEW_MATRIX },
			{ IW5::CONST_SRC_CODE_INVERSE_VIEW_MATRIX, CONST_SRC_CODE_INVERSE_VIEW_MATRIX },
			{ IW5::CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX, CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX },
			{ IW5::CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX, CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX },
			{ IW5::CONST_SRC_CODE_PROJECTION_MATRIX, CONST_SRC_CODE_PROJECTION_MATRIX },
			{ IW5::CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX, CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX },
			{ IW5::CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX, CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX },
			{ IW5::CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX, CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX },
			{ IW5::CONST_SRC_CODE_VIEW_PROJECTION_MATRIX, CONST_SRC_CODE_VIEW_PROJECTION_MATRIX },
			{ IW5::CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX, CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX },
			{ IW5::CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX, CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX },
			{ IW5::CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX, CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX },
			{ IW5::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX, CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX },
			{ IW5::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX, CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX },
			{ IW5::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX, CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX },
			{ IW5::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX, CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX },
			{ IW5::CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX, CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX },
			{ IW5::CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX, CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX },
			{ IW5::CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX, CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX },
			{ IW5::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX, CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX },
			{ IW5::CONST_SRC_CODE_WORLD_MATRIX0, CONST_SRC_CODE_WORLD_MATRIX0 },
			{ IW5::CONST_SRC_CODE_INVERSE_WORLD_MATRIX0, CONST_SRC_CODE_INVERSE_WORLD_MATRIX0 },
			{ IW5::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0, CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0 },
			{ IW5::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX0, CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX0 },
			{ IW5::CONST_SRC_CODE_WORLD_VIEW_MATRIX0, CONST_SRC_CODE_WORLD_VIEW_MATRIX0 },
			{ IW5::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX0, CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX0 },
			{ IW5::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX0, CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX0 },
			{ IW5::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX0, CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX0 },
			{ IW5::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX0, CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX0 },
			{ IW5::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX0, CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX0 },
			{ IW5::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0, CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0 },
			{ IW5::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0, CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0 },
			{ IW5::CONST_SRC_CODE_WORLD_MATRIX1, CONST_SRC_CODE_WORLD_MATRIX1 },
			{ IW5::CONST_SRC_CODE_INVERSE_WORLD_MATRIX1, CONST_SRC_CODE_INVERSE_WORLD_MATRIX1 },
			{ IW5::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX1, CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX1 },
			{ IW5::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX1, CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX1 },
			{ IW5::CONST_SRC_CODE_WORLD_VIEW_MATRIX1, CONST_SRC_CODE_WORLD_VIEW_MATRIX1 },
			{ IW5::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX1, CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX1 },
			{ IW5::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX1, CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX1 },
			{ IW5::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX1, CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX1 },
			{ IW5::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX1, CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX1 },
			{ IW5::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX1, CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX1 },
			{ IW5::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1, CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1 },
			{ IW5::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1, CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1 },
			{ IW5::CONST_SRC_CODE_WORLD_MATRIX2, CONST_SRC_CODE_WORLD_MATRIX2 },
			{ IW5::CONST_SRC_CODE_INVERSE_WORLD_MATRIX2, CONST_SRC_CODE_INVERSE_WORLD_MATRIX2 },
			{ IW5::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX2, CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX2 },
			{ IW5::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX2, CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX2 },
			{ IW5::CONST_SRC_CODE_WORLD_VIEW_MATRIX2, CONST_SRC_CODE_WORLD_VIEW_MATRIX2 },
			{ IW5::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX2, CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX2 },
			{ IW5::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX2, CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX2 },
			{ IW5::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX2, CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX2 },
			{ IW5::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX2, CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX2 },
			{ IW5::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX2, CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX2 },
			{ IW5::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2, CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2 },
			{ IW5::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2, CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2 },
			{ IW5::CONST_SRC_TOTAL_COUNT, CONST_SRC_TOTAL_COUNT },
			{ IW5::CONST_SRC_NONE, CONST_SRC_NONE },
		};
		std::unordered_map<std::int32_t, std::int32_t> constant_map_reversed;

		MaterialTechniqueSet* ITechset::parse(const std::string& name, ZoneMemory* mem)
		{
			const auto iw5_techset = IW5::ITechset::parse(name, mem);

			if (!iw5_techset)
			{
				return nullptr;
			}
			
			auto asset = mem->Alloc<MaterialTechniqueSet>();
			memcpy(asset, iw5_techset, 12);
			memset(asset->techniques, 0, sizeof asset->techniques);
			
			// port techniques to the correct index
			for (int i = 0; i < 54; i++)
			{
				auto dest_index = i;
				if (i >= (46 + 6))
				{
					dest_index -= 6;
				}
				else if (i >= (40 + 5))
				{
					dest_index -= 5;
				}
				else if (i >= (31 + 4))
				{
					dest_index -= 4;
				}
				else if (i >= (19 + 2))
				{
					dest_index -= 2;
				}

				if (iw5_techset->techniques[i])
				{
					const auto technique_size = sizeof(MaterialTechnique) + (sizeof(MaterialPass) * iw5_techset->techniques[i]->hdr.numPasses);
					asset->techniques[dest_index] = mem->ManualAlloc<MaterialTechnique>(technique_size);
					memcpy(asset->techniques[dest_index], iw5_techset->techniques[i], technique_size);
					
					auto& technique = asset->techniques[dest_index];
					for (short pass = 0; pass < technique->hdr.numPasses; pass++)
					{
						auto passDef = &technique->pass[pass];

						const auto arg_count = passDef->perObjArgCount + passDef->perPrimArgCount + passDef->stableArgCount;
						if (arg_count)
						{
							passDef->argumentDef = mem->Alloc<ShaderArgumentDef>(arg_count);
							memcpy(passDef->argumentDef, iw5_techset->techniques[i]->pass[pass].argumentDef, sizeof(ShaderArgumentDef) * arg_count);
						}
						
						for (int arg = 0; arg < arg_count; arg++)
						{
							auto argDef = &passDef->argumentDef[arg];
							
							if (argDef->type > 2)
							{
								argDef->type--;
							}

							if (argDef->type == 3 || argDef->type == 5)
							{
								const auto itr = constant_map.find(argDef->u.codeConst.index);
								if (itr != constant_map.end())
								{
									argDef->u.codeConst.index = itr->second;
								}
								else
								{
									ZONETOOL_WARNING("code constant %u is not mapped for technique %s!", argDef->u.codeConst.index, technique->hdr.name);

									if (argDef->u.codeConst.index >= 66)
									{
										argDef->u.codeConst.index -= 8;
									}
									else if (argDef->u.codeConst.index >= 37)
									{
										argDef->u.codeConst.index -= 6;
									}
									else if (argDef->u.codeConst.index >= 26)
									{
										argDef->u.codeConst.index -= 5;
									}
								}
							}
						}
					}
				}
			}
			
			return asset;
		}

		void ITechset::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(this->name_, mem);

			if (!this->asset_)
			{
				ZONETOOL_FATAL("Current zone is depending on missing techset \"%s\", zone may not work correctly!", name.data());

				this->m_parsed = false;
				this->asset_ = DB_FindXAssetHeader(this->type(), &this->name()[0]).techset;
			}
			else
			{
				this->m_parsed = true;
			}
		}

		void ITechset::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void ITechset::load_depending(IZone* zone)
		{
			auto data = this->asset_;

			for (std::int32_t technique = 0; technique < 48; technique++)
			{
				if (data->techniques[technique])
				{
					for (std::int32_t pass = 0; pass < data->techniques[technique]->hdr.numPasses; pass++)
					{
						auto& techniquePass = data->techniques[technique]->pass[pass];

						if (techniquePass.vertexDecl)
						{
							zone->add_asset_of_type(vertexdecl, techniquePass.vertexDecl->name);
						}
						if (techniquePass.vertexShader)
						{
							zone->add_asset_of_type(vertexshader, techniquePass.vertexShader->name);
						}
						if (techniquePass.pixelShader)
						{
							zone->add_asset_of_type(pixelshader, techniquePass.pixelShader->name);
						}
					}
				}
			}
		}

		std::string ITechset::name()
		{
			return this->name_;
		}

		std::int32_t ITechset::type()
		{
			return techset;
		}

		void ITechset::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->asset_;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());
			dest->remappedTechniques = static_cast<MaterialTechniqueSet*>(zone->get_asset_pointer(techset, this->name()));

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
							reinterpret_cast<VertexDecl*>(zone->get_asset_pointer(
								vertexdecl, techniquePasses[pass].vertexDecl->name));
					}

					if (techniquePasses[pass].vertexShader)
					{
						techniquePasses[pass].vertexShader =
							reinterpret_cast<VertexShader*>(zone->get_asset_pointer(
								vertexshader, techniquePasses[pass].vertexShader->name));
					}

					if (techniquePasses[pass].pixelShader)
					{
						techniquePasses[pass].pixelShader =
							reinterpret_cast<PixelShader*>(zone->get_asset_pointer(
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
				
				ZoneBuffer::ClearPointer(&techniqueHeader->name);
				ZoneBuffer::ClearPointer(&dest->techniques[technique]);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		char* ITechset::parse_statebits(const std::string& techset, ZoneMemory* mem)
		{
			auto iw5_statebits = IW5::ITechset::parse_statebits(techset, mem);
			auto statebits = mem->Alloc<char>(48);
			memset(statebits, 0xFF, 48);

			for (int i = 0; i < 54; i++)
			{
				auto dest_index = i;
				if (i >= (46 + 6))
				{
					dest_index -= 6;
				}
				else if (i >= (40 + 5))
				{
					dest_index -= 5;
				}
				else if (i >= (31 + 4))
				{
					dest_index -= 4;
				}
				else if (i >= (19 + 2))
				{
					dest_index -= 2;
				}

				statebits[dest_index] = iw5_statebits[i];
			}
			
			return statebits;
		}
		
		void ITechset::dump_statebits(const std::string& techset, char* statebits)
		{
			char iw5_statebits[54];
			memset(iw5_statebits, 0xFF, sizeof iw5_statebits);

			for (int i = 0; i < 48; i++)
			{
				auto dest_index = i;

				if (i >= 46)
				{
					dest_index += 6;
				}
				else if (i >= 40)
				{
					dest_index += 5;
				}
				else if (i >= 31)
				{
					dest_index += 4;
				}
				else if (i >= 19)
				{
					dest_index += 2;
				}

				iw5_statebits[dest_index] = statebits[i];
			}
			
			IW5::ITechset::dump_statebits(techset, iw5_statebits);
		}
		
		void ITechset::dump(MaterialTechniqueSet* asset)
		{
			if (constant_map_reversed.empty())
			{
				for (auto& const_map : constant_map)
				{
					constant_map_reversed[const_map.second] = const_map.first;
				}
			}
			
			auto iw5_techset = new IW5::MaterialTechniqueSet;
			memset(iw5_techset, 0, sizeof IW5::MaterialTechniqueSet);

			iw5_techset->name = asset->name;
			iw5_techset->pad = asset->pad;
			
			for (int i = 0; i < 48; i++)
			{
				auto dest_index = i;
				
				if (i >= 46)
				{
					dest_index += 6;
				}
				else if (i >= 40)
				{
					dest_index += 5;
				}
				else if (i >= 31)
				{
					dest_index += 4;
				}
				else if (i >= 19)
				{
					dest_index += 2;
				}

				if (asset->techniques[i])
				{
					const auto size = sizeof IW5::MaterialTechniqueHeader + (sizeof(MaterialPass) * asset->techniques[i]->hdr.numPasses);
					iw5_techset->techniques[dest_index] = reinterpret_cast<IW5::MaterialTechnique*>(
						new char[size]);

					memcpy(iw5_techset->techniques[dest_index], asset->techniques[i], size);
					
					auto& technique = iw5_techset->techniques[dest_index];
					for (short pass = 0; pass < technique->hdr.numPasses; pass++)
					{
						const auto pass_def = &technique->pass[pass];

						const auto arg_count = pass_def->perPrimArgCount + pass_def->perObjArgCount + pass_def->stableArgCount;
						if (arg_count > 0)
						{
							pass_def->argumentDef = new IW5::ShaderArgumentDef[arg_count];
							memcpy(pass_def->argumentDef, asset->techniques[i]->pass[pass].argumentDef, sizeof(IW5::ShaderArgumentDef) * arg_count);
						}
						
						for (auto arg = 0; arg < arg_count; arg++)
						{
							auto arg_def = &pass_def->argumentDef[arg];

							if (arg_def->type > 1)
							{
								arg_def->type++;
							}
							
							if (arg_def->type == 4 || arg_def->type == 6)
							{
								const auto itr = constant_map_reversed.find(arg_def->u.codeConst.index);
								if (itr != constant_map_reversed.end())
								{
									arg_def->u.codeConst.index = itr->second;
								}
								else
								{
									ZONETOOL_WARNING("code constant %u is not mapped for technique %s!", arg_def->u.codeConst.index, technique->hdr.name);

									if (arg_def->u.codeConst.index >= 58)
									{
										arg_def->u.codeConst.index += 8;
									}
									else if (arg_def->u.codeConst.index >= 31)
									{
										arg_def->u.codeConst.index += 6;
									}
									else if (arg_def->u.codeConst.index >= 21)
									{
										arg_def->u.codeConst.index += 5;
									}
								}
							}
						}
					}
				}
			}

			// IW5::ITechset::dump_technique_data(iw5_techset, false);
			IW5::ITechset::dump(iw5_techset);

			for (int i = 0; i < 54; i++)
			{
				if (iw5_techset->techniques[i])
				{
					for (short pass = 0; pass < iw5_techset->techniques[i]->hdr.numPasses; pass++)
					{
						delete iw5_techset->techniques[i]->pass[pass].argumentDef;
					}
					delete iw5_techset->techniques[i];
				}
			}
			
			delete iw5_techset;
		}
	}
}
