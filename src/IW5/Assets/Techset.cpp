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
	namespace IW5
	{
		MaterialTechnique* ITechset::parse_technique(const std::string& name, std::shared_ptr<ZoneMemory>& mem,
		                                                std::uint32_t index)
		{
			const auto path = "techsets\\" + name + ".technique";
			if (!FileSystem::FileExists(path))
			{
				return nullptr;
			}

			AssetReader reader(mem);
			if (!reader.Open(path))
			{
				return nullptr;
			}

			const auto header = reader.Single<MaterialTechniqueHeader>();
			const auto passes = reader.Array<MaterialPass>();

			const auto asset = mem->ManualAlloc<MaterialTechnique>(sizeof(MaterialTechniqueHeader) + (sizeof(MaterialPass) * header->numPasses));
			memcpy(&asset->hdr, header, sizeof MaterialTechniqueHeader);
			memcpy(asset->pass, passes, sizeof(MaterialPass) * header->numPasses);

			for (short i = 0; i < header->numPasses; i++)
			{
				if (passes[i].pixelShader)
				{
					passes[i].pixelShader = reader.Asset<PixelShader>();
				}

				if (passes[i].vertexShader)
				{
					passes[i].vertexShader = reader.Asset<VertexShader>();
				}

				if (passes[i].vertexDecl)
				{
					passes[i].vertexDecl = reader.Asset<VertexDecl>();
				}

				if (passes[i].argumentDef)
				{
					passes[i].argumentDef = reader.Array<ShaderArgumentDef>();

					for (auto arg = 0; arg < passes[i].perObjArgCount + passes[i].perPrimArgCount + passes[i].stableArgCount; arg++)
					{
						if (passes[i].argumentDef[arg].type == 1 || passes[i].argumentDef[arg].type == 8)
						{
							passes[i].argumentDef[arg].u.literalConst = reader.Array<float>();
						}
					}
				}
			}
			
			return asset;
		}

		MaterialTechniqueSet* ITechset::parse(const std::string& name,
		                                      std::shared_ptr<ZoneMemory>& mem)
		{
			const auto path = "techsets\\" + name + ".techset";
			if (!FileSystem::FileExists(path))
			{
				return nullptr;
			}

			AssetReader reader(mem);
			if (!reader.Open(path))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing techset \"%s\"...", name.data());

			const auto asset = reader.Single<MaterialTechniqueSet>();
			asset->name = reader.String();

			for (auto i = 0u; i < 54; i++)
			{
				if (asset->techniques[i])
				{
					asset->techniques[i] = ITechset::parse_technique(reader.String(), mem, i);
				}
			}
			
			return asset;
		}

		void ITechset::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			this->m_name = name;
			this->m_asset = this->parse(name, mem);

			if (!this->m_asset)
			{
				ZONETOOL_FATAL(
"Current zone is depending on missing techset \"%s\", zone may not work correctly!", name.data());
				// std::getchar();

				this->m_parsed = false;
				this->m_asset = DB_FindXAssetHeader(this->type(), this->name().data(), 1).
					techset;
			}
			this->m_parsed = true;
		}

		void ITechset::prepare(std::shared_ptr<ZoneBuffer>& buf,
		                       std::shared_ptr<ZoneMemory>& mem)
		{
		}

		void ITechset::load_depending(IZone* zone)
		{
			auto data = this->m_asset;

			for (std::int32_t technique = 0; technique < 54; technique++)
			{
				if (data->techniques[technique])
				{
					for (std::int32_t pass = 0; pass < data->techniques[technique]
					                                   ->hdr.numPasses; pass++)
					{
						auto& techniquePass = data->techniques[technique]->pass[pass];

						if (techniquePass.vertexDecl)
						{
							zone->AddAssetOfType(vertexdecl, techniquePass.vertexDecl->name);
						}

						if (techniquePass.vertexShader)
						{
							zone->AddAssetOfType(
								vertexshader, techniquePass.vertexShader->name);
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

			for (std::int32_t technique = 0; technique < 54; technique++)
			{
				if (!data->techniques[technique])
				{
					continue;
				}

				buf->align(3);

				auto techniqueHeader = buf->write(&data->techniques[technique]->hdr);
				auto techniquePasses = buf->write(
					data->techniques[technique]->pass, techniqueHeader->numPasses);

				if (techniqueHeader->numPasses > 1)
				{
					//					ZONETOOL_INFO("Technique %s has %u passes.", data->techniques[technique]->hdr.name, techniqueHeader->numPasses);
				}

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

							if (curArg->type == 1 || curArg->type == 8)
							{
								if (destArgs[arg].u.literalConst)
								{
									destArgs[arg].u.literalConst = buf->write_s(
										3, destArgs[arg].u.literalConst, 4);
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

			encrypt_data(dest, sizeof MaterialTechniqueSet);
		}

		void ITechset::dump_technique(MaterialTechnique* asset)
		{
			auto path = "techsets\\"s + asset->hdr.name + ".technique";
			
			AssetDumper dumper;
			if (!dumper.Open(path))
			{
				return;
			}

			dumper.Single(&asset->hdr);
			dumper.Array(asset->pass, asset->hdr.numPasses);

			for (short i = 0; i < asset->hdr.numPasses; i++)
			{
				if (asset->pass[i].pixelShader)
				{
					dumper.Asset(asset->pass[i].pixelShader);
				}

				if (asset->pass[i].vertexShader)
				{
					dumper.Asset(asset->pass[i].vertexShader);
				}

				if (asset->pass[i].vertexDecl)
				{
					dumper.Asset(asset->pass[i].vertexDecl);
				}

				if (asset->pass[i].argumentDef)
				{
					dumper.Array(asset->pass[i].argumentDef, asset->pass[i].perObjArgCount + asset->pass[i].perPrimArgCount + asset->pass[i].stableArgCount);
					for (auto arg = 0; arg < asset->pass[i].perObjArgCount + asset->pass[i].perPrimArgCount + asset->pass[i].stableArgCount; arg++)
					{
						if (asset->pass[i].argumentDef[arg].type == 1 || asset->pass[i].argumentDef[arg].type == 8)
						{
							dumper.Array(asset->pass[i].argumentDef[arg].u.literalConst, 4);
						}
					}
				}
			}
		}

		char* ITechset::parse_statebits(const std::string& techset, std::shared_ptr<ZoneMemory>& mem)
		{
			auto statebits = mem->Alloc<char>(54);
			
			auto path = "techsets\\" + techset + ".statebits";
			auto fp = FileSystem::FileOpen(path, "rb");

			if (fp)
			{
				fread(statebits, 54, 1, fp);
				FileSystem::FileClose(fp);

				return statebits;
			}
			
			return nullptr;
		}
		
		void ITechset::dump_statebits(const std::string& techset, char* statebits)
		{
			auto path = "techsets\\" + techset + ".statebits";
			auto fp = FileSystem::FileOpen(path, "wb");
			
			if (fp)
			{
				fwrite(statebits, 54, 1, fp);
				FileSystem::FileClose(fp);
			}
		}
		
		void ITechset::dump(MaterialTechniqueSet* asset)
		{
			auto path = "techsets\\"s + asset->name + ".techset";
			
			AssetDumper dumper;
			if (!dumper.Open(path))
			{
				return;
			}

			dumper.Single(asset);
			dumper.String(asset->name);

			for (auto i = 0u; i < 54; i++)
			{
				if (asset->techniques[i])
				{
					dumper.String(asset->techniques[i]->hdr.name);
					ITechset::dump_technique(asset->techniques[i]);
				}
			}
		}
	}
}
