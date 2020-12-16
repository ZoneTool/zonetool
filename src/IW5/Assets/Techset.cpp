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

			const auto asset = mem->ManualAlloc<MaterialTechnique>(sizeof(MaterialTechniqueHeader) + (sizeof(MaterialPass) * header->passCount));
			memcpy(&asset->hdr, header, sizeof MaterialTechniqueHeader);
			memcpy(asset->pass, passes, sizeof(MaterialPass) * header->passCount);

			for (short i = 0; i < header->passCount; i++)
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
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data(), 1).
					techset;

				if (DB_IsXAssetDefault(this->type(), this->name().data()))
				{
					ZONETOOL_FATAL("Techset %s not found.", &name[0]);
				}
			}
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
					                                   ->hdr.passCount; pass++)
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
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());
			dest->remappedTechniques = nullptr;

//#ifdef DEBUG
//			if (IsDebuggerPresent() && data->techniques[5] == nullptr && data->techniques[9] == nullptr)
//			{
//				__debugbreak();
//			}
//#endif
//			
//			assert(data->techniques[5] != nullptr || data->techniques[9] != nullptr);
			
			for (auto technique = 0; technique < 54; technique++)
			{
				if (!data->techniques[technique])
				{
					continue;
				}

				buf->align(3);

				auto* technique_header = buf->write(&data->techniques[technique]->hdr);
				auto* technique_passes = buf->write(
					data->techniques[technique]->pass, technique_header->passCount);

				for (auto pass = 0; pass < technique_header->passCount; pass++)
				{
					if (technique_passes[pass].vertexDecl)
					{
						technique_passes[pass].vertexDecl =
							reinterpret_cast<VertexDecl*>(zone->get_asset_pointer(
								vertexdecl, technique_passes[pass].vertexDecl->name));
					}

					if (technique_passes[pass].vertexShader)
					{
						technique_passes[pass].vertexShader =
							reinterpret_cast<VertexShader*>(zone->get_asset_pointer(
								vertexshader, technique_passes[pass].vertexShader->name));
					}

					if (technique_passes[pass].pixelShader)
					{
						technique_passes[pass].pixelShader =
							reinterpret_cast<PixelShader*>(zone->get_asset_pointer(
								pixelshader, technique_passes[pass].pixelShader->name));
					}

					if (technique_passes[pass].argumentDef)
					{
						buf->align(3);
						auto destArgs = buf->write(technique_passes[pass].argumentDef,
						                           technique_passes[pass].perPrimArgCount +
						                           technique_passes[pass].perObjArgCount +
						                           technique_passes[pass].stableArgCount);

						for (auto arg = 0; arg <
						     technique_passes[pass].perPrimArgCount +
						     technique_passes[pass].perObjArgCount +
						     technique_passes[pass].stableArgCount; arg++)
						{
							auto curArg = &technique_passes[pass].argumentDef[arg];

							if (curArg->type == 1 || curArg->type == 8)
							{
								if (destArgs[arg].u.literalConst)
								{
									destArgs[arg].u.literalConst = buf->write_s(
										3, destArgs[arg].u.literalConst, 4);
								}
							}
						}

						ZoneBuffer::clear_pointer(&technique_passes[pass].argumentDef);
					}
				}

				buf->write_str(technique_header->name);
				ZoneBuffer::clear_pointer(&technique_header->name);

				ZoneBuffer::clear_pointer(&dest->techniques[technique]);
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
			dumper.dump_array(asset->pass, asset->hdr.passCount);

			dumper.dump_string(asset->hdr.name);
			
			for (short i = 0; i < asset->hdr.passCount; i++)
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
				for (int p = 0; p < technique->hdr.passCount; p++)
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
				json["flags"] = technique->hdr.flags;
				json["passCount"] = technique->hdr.passCount;
				json["pass"] = pass_array;
				
				auto meme = json.dump();
				
				auto fp = fopen(va("techsets/techniques/%s.%s.json", technique->hdr.name, is_iw5 ? "iw5" : "iw4").data(), "wb");
				fwrite(meme.data(), meme.size(), 1, fp);
				fclose(fp);
			}
		}
		
		void yeet(MaterialTechniqueSet* asset)
		{
			auto path = "techsets\\"s + asset->name + ".techset.txt";

			auto fp = FileSystem::FileOpen(path, "wb");

			for (auto i = 0u; i < 54; i++)
			{
				if (asset->techniques[i])
				{
					fprintf(fp, "%i: %s\n", i, asset->techniques[i]->hdr.name);
				}
				else
				{
					fprintf(fp, "%i: nullptr\n", i);
				}
			}

			FileSystem::FileClose(fp);
		}

		void ITechset::dump(MaterialTechniqueSet* asset)
		{
			yeet(asset);
			
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
