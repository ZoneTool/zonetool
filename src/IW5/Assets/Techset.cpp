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
		MaterialTechnique* ITechset::parse_technique(const std::string& name, ZoneMemory* mem,
		                                                std::uint32_t index)
		{
			const auto path = "techsets\\" + name + ".technique";
			if (!FileSystem::FileExists(path))
			{
				ZONETOOL_FATAL("technique \"%s\" is missing.", name.data());
				return nullptr;
			}

			AssetReader reader(mem);
			if (!reader.open(path))
			{
				ZONETOOL_FATAL("technique \"%s\" is missing.", name.data());
				return nullptr;
			}

			ZONETOOL_INFO("Parsing technique \"%s\"...", name.data());

			const auto header = reader.read_single<MaterialTechniqueHeader>();
			const auto passes = reader.read_array<MaterialPass>();

			header->name = reader.read_string();

			const auto asset = mem->ManualAlloc<MaterialTechnique>(sizeof(MaterialTechniqueHeader) + (sizeof(MaterialPass) * header->numPasses));
			memcpy(&asset->hdr, header, sizeof MaterialTechniqueHeader);
			memcpy(asset->pass, passes, sizeof(MaterialPass) * header->numPasses);

			for (short i = 0; i < header->numPasses; i++)
			{
				if (asset->pass[i].pixelShader)
				{
					asset->pass[i].pixelShader = reader.read_asset<PixelShader>();
				}

				if (asset->pass[i].vertexShader)
				{
					asset->pass[i].vertexShader = reader.read_asset<VertexShader>();
				}

				if (asset->pass[i].vertexDecl)
				{
					asset->pass[i].vertexDecl = reader.read_asset<VertexDecl>();
				}

				if (asset->pass[i].argumentDef)
				{
					asset->pass[i].argumentDef = reader.read_array<ShaderArgumentDef>();

					for (auto arg = 0; arg < asset->pass[i].perObjArgCount + asset->pass[i].perPrimArgCount + asset->pass[i].stableArgCount; arg++)
					{
						if (asset->pass[i].argumentDef[arg].type == 1 || asset->pass[i].argumentDef[arg].type == 8)
						{
							asset->pass[i].argumentDef[arg].u.literalConst = reader.read_array<float>();
						}
					}
				}
			}

			reader.close();
			
			return asset;
		}

		MaterialTechniqueSet* ITechset::parse(const std::string& name,
		                                      ZoneMemory* mem)
		{
			const auto path = "techsets\\" + name + ".techset";
			if (!FileSystem::FileExists(path))
			{
				return nullptr;
			}

			AssetReader reader(mem);
			if (!reader.open(path))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing techset \"%s\"...", name.data());

			const auto asset = reader.read_single<MaterialTechniqueSet>();
			asset->name = reader.read_string();

			for (auto i = 0u; i < 54; i++)
			{
				if (asset->techniques[i])
				{
					asset->techniques[i] = ITechset::parse_technique(reader.read_string(), mem, i);
				}
			}

			asset->remappedTechniques = nullptr;
			
			reader.close();
			
			return asset;
		}

		void ITechset::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				ZONETOOL_FATAL(
"Current zone is depending on missing techset \"%s\", zone may not work correctly!", name.data());
				// std::getchar();

				this->m_parsed = false;
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data(), 1).
					techset;
			}
			this->m_parsed = true;
		}

		void ITechset::prepare(ZoneBuffer* buf,
		                       ZoneMemory* mem)
		{
		}

		void ITechset::load_depending(IZone* zone)
		{
			auto data = this->asset_;

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
							zone->add_asset_of_type(vertexdecl, techniquePass.vertexDecl->name);
						}

						if (techniquePass.vertexShader)
						{
							zone->add_asset_of_type(
								vertexshader, techniquePass.vertexShader->name);
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
			if (!dumper.open(path))
			{
				return;
			}

			dumper.dump_single(&asset->hdr);
			dumper.dump_array(asset->pass, asset->hdr.numPasses);

			dumper.dump_string(asset->hdr.name);
			
			for (short i = 0; i < asset->hdr.numPasses; i++)
			{
				if (asset->pass[i].pixelShader)
				{
					dumper.dump_asset(asset->pass[i].pixelShader);
					IPixelShader::dump(asset->pass[i].pixelShader);
				}

				if (asset->pass[i].vertexShader)
				{
					dumper.dump_asset(asset->pass[i].vertexShader);
					IVertexShader::dump(asset->pass[i].vertexShader);
				}

				if (asset->pass[i].vertexDecl)
				{
					dumper.dump_asset(asset->pass[i].vertexDecl);
					IVertexDecl::dump(asset->pass[i].vertexDecl);
				}

				if (asset->pass[i].argumentDef)
				{
					dumper.dump_array(asset->pass[i].argumentDef, asset->pass[i].perObjArgCount + asset->pass[i].perPrimArgCount + asset->pass[i].stableArgCount);
					for (auto arg = 0; arg < asset->pass[i].perObjArgCount + asset->pass[i].perPrimArgCount + asset->pass[i].stableArgCount; arg++)
					{
						if (asset->pass[i].argumentDef[arg].type == 1 || asset->pass[i].argumentDef[arg].type == 8)
						{
							dumper.dump_array(asset->pass[i].argumentDef[arg].u.literalConst, 4);
						}
					}
				}
			}

			dumper.close();
		}

		char* ITechset::parse_statebits(const std::string& techset, ZoneMemory* mem)
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

			ZONETOOL_FATAL("statebits for techset \"%s\" are missing!", techset.data());
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

		void ITechset::dump_technique_data(MaterialTechniqueSet* asset, bool is_iw5)
		{
			std::filesystem::create_directories("techsets/techniques");
			
			for (int i = 0; i < 54; i++)
			{
				if (!asset->techniques[i])
				{
					continue;
				}

				auto technique = asset->techniques[i];

				std::vector<nlohmann::json> pass_array;
				for (int p = 0; p < technique->hdr.numPasses; p++)
				{
					auto current_pass = &technique->pass[p];
					nlohmann::json pass;

					std::vector<nlohmann::json> arg_array;

					for (int a = 0; a < current_pass->perObjArgCount + current_pass->perPrimArgCount + current_pass->stableArgCount; a++)
					{
						auto current_arg = &current_pass->argumentDef[a];
						nlohmann::json arg;

						arg["type"] = current_arg->type;
						
						if (current_arg->type == 1 || current_arg->type == 8)
						{
							arg["value"][0] = current_arg->u.literalConst[0];
							arg["value"][1] = current_arg->u.literalConst[0];
							arg["value"][2] = current_arg->u.literalConst[0];
							arg["value"][3] = current_arg->u.literalConst[0];
						}
						else if (current_arg->type == 4 || current_arg->type == 6)
						{
							arg["firstRow"] = current_arg->u.codeConst.firstRow;
							arg["rowCount"] = current_arg->u.codeConst.rowCount;
							arg["index"] = current_arg->u.codeConst.index;
						}
						else
						{
							arg["value"] = current_arg->u.codeSampler;
						}

						arg_array.push_back(arg);
					}
					
					pass["perObjArgCount"] = current_pass->perObjArgCount;
					pass["perPrimArgCount"] = current_pass->perPrimArgCount;
					pass["stableArgCount"] = current_pass->stableArgCount;
					pass["pixelShader"] = current_pass->pixelShader ? current_pass->pixelShader->name : "";
					pass["vertexShader"] = current_pass->vertexShader ? current_pass->vertexShader->name : "";
					pass["vertexDecl"] = current_pass->vertexDecl ? current_pass->vertexDecl->name : "";
					pass["args"] = arg_array;
					
					pass_array.push_back(pass);
				}
				
				nlohmann::json json;
				json["name"] = technique->hdr.name;
				json["index"] = i;
				json["flags"] = technique->hdr.unk;
				json["numPasses"] = technique->hdr.numPasses;
				json["pass"] = pass_array;
				
				auto meme = json.dump();
				
				auto fp = fopen(va("techsets/techniques/%s.%s.json", technique->hdr.name, is_iw5 ? "iw5" : "iw4").data(), "wb");
				fwrite(meme.data(), meme.size(), 1, fp);
				fclose(fp);
			}
		}
		
		void ITechset::dump(MaterialTechniqueSet* asset)
		{
			// dump_technique_data(asset);
			
			auto path = "techsets\\"s + asset->name + ".techset";
			
			AssetDumper dumper;
			if (!dumper.open(path))
			{
				return;
			}

			dumper.dump_single(asset);
			dumper.dump_string(asset->name);

			for (auto i = 0u; i < 54; i++)
			{
				if (asset->techniques[i])
				{
					dumper.dump_string(asset->techniques[i]->hdr.name);
					ITechset::dump_technique(asset->techniques[i]);
				}
			}

			dumper.close();
		}
	}
}
