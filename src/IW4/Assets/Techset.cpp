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
		MaterialTechniqueSet* ITechset::parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			const auto iw5_techset = IW5::ITechset::parse(name, mem);
			auto asset = mem->Alloc<MaterialTechniqueSet>();
			memcpy(asset, iw5_techset, sizeof MaterialTechniqueSet);
			
			// port techniques to the correct index
			for (int i = 0; i < 54; i++)
			{
				auto dest_index = i;
				if (i >= 46)
				{
					dest_index -= 6;
				}
				else if (i >= 44)
				{
					dest_index -= 5;
				}
				else if (i >= 31)
				{
					dest_index -= 4;
				}
				else if (i >= 19)
				{
					dest_index -= 2;
				}

				asset->techniques[dest_index] = reinterpret_cast<MaterialTechnique*>(iw5_techset->techniques[i]);

				if (asset->techniques[dest_index])
				{
					auto& technique = asset->techniques[dest_index];
					for (short pass = 0; pass < technique->hdr.numPasses; pass++)
					{
						auto passDef = &technique->pass[pass];
						for (int arg = 0; arg < passDef->perObjArgCount + passDef->perPrimArgCount + passDef->stableArgCount; arg++)
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
				
				ZoneBuffer::ClearPointer(&techniqueHeader->name);
				ZoneBuffer::ClearPointer(&dest->techniques[technique]);
			}

			END_LOG_STREAM;
			buf->pop_stream();
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
				else if (i >= 44)
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

				iw5_techset->techniques[dest_index] = reinterpret_cast<IW5::MaterialTechnique*>(iw5_techset->techniques[i]);

				if (iw5_techset->techniques[dest_index])
				{
					auto& technique = iw5_techset->techniques[dest_index];
					for (short pass = 0; pass < technique->hdr.numPasses; pass++)
					{
						auto passDef = &technique->pass[pass];
						for (auto arg = 0; arg < passDef->perPrimArgCount + passDef->perObjArgCount + passDef->stableArgCount; arg++)
						{
							auto argDef = &passDef->argumentDef[arg];

							if (argDef->type > 1)
							{
								argDef->type++;
							}

							if (argDef->type == 4 || argDef->type == 6)
							{
								if (argDef->u.codeConst.index >= 58)
								{
									argDef->u.codeConst.index += 8;
								}
								else if (argDef->u.codeConst.index >= 31)
								{
									argDef->u.codeConst.index += 6;
								}
								else if (argDef->u.codeConst.index >= 21)
								{
									argDef->u.codeConst.index += 5;
								}
							}
						}
					}
				}
			}

			IW5::ITechset::dump(iw5_techset);
			delete iw5_techset;
		}
	}
}
