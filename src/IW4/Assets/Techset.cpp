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
