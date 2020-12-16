// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "IW4/Structs.hpp"

namespace ZoneTool
{
	namespace IW5
	{
		std::unordered_map<std::string, std::string> mappedTechniqueSets =
		{
			//				IW4										IW5
			{"wc_l_sm_ua_b0c0q0n0s0p0_nocast", "wc_l_sm_ua_b0c0n0s0p0_nocast"},
			{"wc_ambient_t0c0", "mc_ambient_t0c0"},

			// mp_favela
			{"wc_l_ua_b0c0_nocast", "wc_l_ua_b0c0p0_nocast"},
			{"wc_distortion_scale_ua_zfeather", "wc_distortion_scale_ua"},
			{"wc_l_sm_t0c0d0n0s0p0", "wc_l_sm_t0c0n0s0p0"},
		};

		struct WaterWritable
		{
			float floatTime;
		};

		struct complex_s
		{
			float real;
			float imag;
		};

		struct water_t
		{
			WaterWritable writable;
			complex_s* H0;
			float* wTerm;
			int M;
			int N;
			float Lx;
			float Lz;
			float gravity;
			float windvel;
			float winddir[2];
			float amplitude;
			float codeConstant[4];
			GfxImage* image;
		};

#define MATERIAL_DUMP_STRING(entry) \
	matdata[#entry] = std::string(asset->entry);

#define MATERIAL_DUMP_INT(entry) \
	matdata[#entry] = asset->entry;

#define MATERIAL_INT(entry) \
	mat->entry = matdata[#entry].get<int>();

#define STATEBITENTRYNUM 48

#define MATERIAL_DUMP_BITS_ENTRY(entry,size) \
	nlohmann::json carr##entry; \
	for (int i = 0; i < size; i++) \
	{ \
		carr##entry[i] = asset->entry[i]; \
	} \
	matdata[#entry] = carr##entry;

#define MATERIAL_INT(entry) \
	mat->entry = matdata[#entry].get<int>();

#define MATERIAL_DUMP_CONST_ARRAY(entry,size) \
	nlohmann::json carr##entry; \
	for (int i = 0; i < size; i++) \
	{ \
		nlohmann::json cent##entry; \
		cent##entry["name"] = asset->entry[i].name; \
		cent##entry["nameHash"] = asset->entry[i].nameHash; \
		nlohmann::json centliteral##entry; \
		centliteral##entry[0] = asset->entry[i].literal[0]; \
		centliteral##entry[1] = asset->entry[i].literal[1]; \
		centliteral##entry[2] = asset->entry[i].literal[2]; \
		centliteral##entry[3] = asset->entry[i].literal[3]; \
		cent##entry["literal"] = centliteral##entry; \
		carr##entry[i] = cent##entry; \
	} \
	matdata[#entry] = carr##entry;


#define MATERIAL_DUMP_STATE_MAP(entry,size) \
	nlohmann::json carr##entry; \
	for (int i = 0; i < size; i++) \
	{ \
		nlohmann::json cent##entry; \
		cent##entry[0] = asset->entry[i].loadBits[0]; \
		cent##entry[1] = asset->entry[i].loadBits[1]; \
		carr##entry[i] = cent##entry; \
	} \
	matdata[#entry] = carr##entry;

		// Legacy zonetool code: REFACTOR ME!
		enum IWI_COMPRESSION_e
		{
			IWI_INVALID = 0x0,
			IWI_ARGB = 0x1,
			IWI_RGB8 = 0x2,
			IWI_DXT1 = 0xB,
			IWI_DXT3 = 0xC,
			IWI_DXT5 = 0xD,
		} IWI_COMPRESSION;

		enum MaterialMapHashes
		{
			HASH_COLORMAP = 0xa0ab1041,
			HASH_DETAILMAP = 0xeb529b4d,
			HASH_SPECULARMAP = 0x34ecccb3,
			HASH_NORMALMAP = 0x59d30d0f
		};

		enum MaterialSemantic
		{
			SEMANTIC_2D = 0x0,
			SEMANTIC_FUNCTION = 0x1,
			SEMANTIC_COLOR_MAP = 0x2,
			SEMANTIC_NORMAL_MAP = 0x5,
			SEMANTIC_SPECULAR_MAP = 0x8,
			SEMANTIC_WATER_MAP = 0xB
		};

		typedef struct
		{
			char magic[3]; //IWi
			char version; // 8
			int flags;
			short format; // see above
			short xsize;
			short ysize;
			short depth;
			int mipAddr4;
			int mipAddr3;
			int mipAddr2;
			int mipAddr1;
		} _IWI;

		// Image parsing
		_IWI* LoadIWIHeader(std::string name, ZoneMemory* mem)
		{
			auto path = "images\\" + name + ".iwi";

			if (FileSystem::FileExists(path))
			{
				auto buf = mem->Alloc<_IWI>();

				auto fp = FileSystem::FileOpen(path, "rb"s);
				if (fp)
				{
					fread(buf, sizeof(_IWI), 1, fp);
				}
				FileSystem::FileClose(fp);

				return buf;
			} // ZONETOOL_ERROR("Image \"%s\" does not exist!", name.c_str());

			return nullptr;
		}

		GfxImageLoadDef* GenerateLoadDef(GfxImage* image, short iwi_format, ZoneMemory* mem)
		{
			auto texture = mem->Alloc<GfxImageLoadDef>();

			switch (iwi_format)
			{
			case IWI_ARGB:
				texture->format = 21;
				break;
			case IWI_RGB8:
				texture->format = 20;
				break;
			case IWI_DXT1:
				texture->format = 0x31545844;
				break;
			case IWI_DXT3:
				texture->format = 0x33545844;
				break;
			case IWI_DXT5:
				texture->format = 0x35545844;
				break;
			}
			texture->dimensions[0] = image->width;
			texture->dimensions[1] = image->height;
			texture->dimensions[2] = image->depth;

			return texture;
		}

		GfxImage* Image_Parse(const char* name, char semantic, char category, char flags,
		                      ZoneMemory* mem)
		{
			_IWI* buf = LoadIWIHeader(name, mem);

			if (buf)
			{
				auto ret = mem->Alloc<GfxImage>();

				ret->height = buf->xsize;
				ret->width = buf->ysize;
				ret->depth = buf->depth;
				ret->dataLen1 = buf->mipAddr4 - 32;
				ret->dataLen2 = buf->mipAddr4 - 32;
				ret->name = strdup(name);
				ret->semantic = semantic;
				ret->category = category;
				ret->flags = flags;
				ret->mapType = 3; // hope that works lol

				ret->texture = GenerateLoadDef(ret, buf->format, mem);

				return ret;
			}

			return DB_FindXAssetHeader(image, name, 1).gfximage;
		}

		MaterialImage* Material_ParseMaps(nlohmann::json& matdata, ZoneMemory* mem)
		{
			auto mat = mem->Alloc<MaterialImage>(matdata.size());

			for (std::size_t i = 0; i < matdata.size(); i++)
			{
				mat[i].firstCharacter = matdata[i]["firstCharacter"].get<char>();
				mat[i].secondLastCharacter = matdata[i]["lastCharacter"].get<char>();
				mat[i].sampleState = matdata[i]["sampleState"].get<char>();
				mat[i].semantic = matdata[i]["semantic"].get<char>();
				mat[i].typeHash = matdata[i]["typeHash"].get<unsigned int>();

				std::string img = matdata[i]["image"].get<std::string>();
				mat[i].image = Image_Parse(img.data(), mat[i].semantic, 0, 0, mem);
			}

			return mat;
		}

		unsigned int R_HashString(const char* string)
		{
			unsigned int hash = 0;

			while (*string)
			{
				hash = (*string | 0x20) ^ (33 * hash);
				string++;
			}

			return hash;
		}

		extern std::unordered_map<std::int32_t, std::int32_t> iw3TechniqueMap;

		Material* IMaterial::parse(std::string name, ZoneMemory* mem)
		{
			auto path = "materials\\" + name;
			auto file = FileSystem::FileOpen(path, "rb"s);
			if (!file)
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing material \"%s\"...", name.c_str());

			auto size = FileSystem::FileSize(file);
			auto bytes = FileSystem::ReadBytes(file, size);
			FileSystem::FileClose(file);

			nlohmann::json matdata = nlohmann::json::parse(bytes);

			auto mat = mem->Alloc<Material>();
			mat->name = mem->StrDup(name);

			MATERIAL_INT(gameFlags);
			MATERIAL_INT(sortKey);
			MATERIAL_INT(animationX);
			MATERIAL_INT(animationY);

			mat->surfaceTypeBits = matdata["surfaceTypeBits"].get<unsigned int>();
			mat->stateFlags = matdata["stateFlags"].get<char>();
			mat->cameraRegion = matdata["cameraRegion"].get<unsigned short>();

			std::string techset = matdata["techniqueSet->name"];

			if (!techset.empty())
			{
				mat->techniqueSet = DB_FindXAssetHeader(XAssetType::techset, techset.data(), 1).techset;
			}

			nlohmann::json maps = matdata["maps"];
			if (maps.size())
			{
				mat->maps = Material_ParseMaps(maps, mem);
			}

			mat->numMaps = maps.size();

			nlohmann::json constantTable = matdata["constantTable"];
			if (constantTable.size() > 0)
			{
				auto constant_def = mem->Alloc<MaterialConstantDef>(constantTable.size());
				for (int i = 0; i < constantTable.size(); i++)
				{
					strncpy(constant_def[i].name, constantTable[i]["name"].get<std::string>().c_str(), 11);
					constant_def[i].name[11] = '\0';
					if (constantTable[i].find("nameHash") == constantTable[i].end())
					{
						ZONETOOL_WARNING(
"IT APPEARS THAT YOU ARE PARSING A MATERIAL FROM AN OLDER VERSION OF ZONETOOL. CONSIDER REDUMPING YOUR MATERIALS.");
						constant_def[i].nameHash = R_HashString(constant_def[i].name);
					}
					else
					{
						constant_def[i].nameHash = constantTable[i]["nameHash"].get<int>();
					}
					constant_def[i].literal[0] = constantTable[i]["literal"][0].get<float>();
					constant_def[i].literal[1] = constantTable[i]["literal"][1].get<float>();
					constant_def[i].literal[2] = constantTable[i]["literal"][2].get<float>();
					constant_def[i].literal[3] = constantTable[i]["literal"][3].get<float>();
				}
				mat->constantTable = constant_def;
			}
			else
			{
				mat->constantTable = nullptr;
			}
			mat->constantCount = constantTable.size();


			nlohmann::json stateMap = matdata["stateMap"];
			if (stateMap.size() > 0)
			{
				auto stateBits = mem->Alloc<GfxStateBits>(stateMap.size());
				for (int i = 0; i < stateMap.size(); i++)
				{
					stateBits[i].loadBits[0] = stateMap[i][0].get<unsigned int>();
					stateBits[i].loadBits[1] = stateMap[i][1].get<unsigned int>();
				}
				mat->stateMap = stateBits;
			}
			else
			{
				mat->stateMap = nullptr;
			}
			mat->stateBitsCount = stateMap.size();

			if (mat->techniqueSet)
			{
				auto* statebits = ITechset::parse_statebits(mat->techniqueSet->name, mem);
				memcpy(mat->stateBitsEntry, statebits, sizeof mat->stateBitsEntry);
			}

			auto max_state_index = 0;
			for (auto i = 0; i < 54; i++)
			{
				if (mat->stateBitsEntry[i] < 0)
				{
					continue;
				}
				
				if (mat->stateBitsEntry[i] > max_state_index)
				{
					max_state_index = mat->stateBitsEntry[i];
				}
			}

			if (max_state_index >= mat->stateBitsCount)
			{
				ZONETOOL_FATAL("Material %s is referencing more statebit entries than it has!", mat->name);
			}
			
			if (max_state_index != mat->stateBitsCount - 1)
			{
				ZONETOOL_INFO("Material %s has %u statebits but only %u are used, removing unused statebits.", mat->name, mat->stateBitsCount, max_state_index + 1);
				mat->stateBitsCount = max_state_index + 1;
			}
			
			// why???
			mat->NiceMemeIW = 0;

			return mat;
		}

		void IMaterial::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name_.data(), 1).material;
			}
		}

		void IMaterial::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IMaterial::load_depending(IZone* zone)
		{
			auto data = this->asset_;

			if (data->techniqueSet)
			{
				zone->add_asset_of_type(techset, data->techniqueSet->name);
			}

			for (char i = 0; i < data->numMaps; i++)
			{
				if (data->maps[i].image)
				{
					// use pointer rather than name here
					zone->add_asset_of_type_by_pointer(image, data->maps[i].image);
				}
			}
		}

		std::string IMaterial::name()
		{
			return this->name_;
		}

		std::int32_t IMaterial::type()
		{
			return material;
		}

		void IMaterial::write(IZone* zone, ZoneBuffer* buf)
		{
			auto dest = buf->at<Material>();
			auto data = this->asset_;

			buf->write(data);
			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->techniqueSet)
			{
				dest->techniqueSet = reinterpret_cast<MaterialTechniqueSet*>(zone->get_asset_pointer(
					techset, data->techniqueSet->name));
			}

			if (data->maps)
			{
				buf->align(3);
				auto destmaps = buf->write(data->maps, data->numMaps);

				for (int i = 0; i < data->numMaps; i++)
				{
					if (data->maps[i].semantic == 11)
					{
						ZONETOOL_FATAL("Watermaps are not supported. Material name: %s", data->name);
						destmaps[i].image = nullptr;
					}
					if (data->maps[i].image)
					{
						destmaps[i].image = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
							image, data->maps[i].image->name));
					}
				}

				ZoneBuffer::clear_pointer(&dest->maps);
			}

			if (data->constantTable)
			{
				/*buf->align(15);
				buf->write(
				ZoneBuffer::clear_pointer(&dest->constantTable);*/
				dest->constantTable = buf->write_s(15, data->constantTable, data->constantCount);
			}

			if (data->stateMap)
			{
				/*buf->align(3);
				buf->write(
				ZoneBuffer::clear_pointer(&dest->stateMap);*/
				dest->stateMap = buf->write_s(3, data->stateMap, data->stateBitsCount);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IMaterial::dump(Material* asset)
		{
			if (asset && asset->techniqueSet)
			{
				ITechset::dump_statebits(asset->techniqueSet->name, asset->stateBitsEntry);
			}

			nlohmann::json matdata;

			MATERIAL_DUMP_STRING(name);

			if (asset->techniqueSet)
			{
				MATERIAL_DUMP_STRING(techniqueSet->name);
			}

			MATERIAL_DUMP_INT(gameFlags);
			MATERIAL_DUMP_INT(sortKey);
			MATERIAL_DUMP_INT(animationX);
			MATERIAL_DUMP_INT(animationY);

			MATERIAL_DUMP_INT(unknown);
			MATERIAL_DUMP_INT(surfaceTypeBits);
			MATERIAL_DUMP_INT(stateFlags);
			MATERIAL_DUMP_INT(cameraRegion);

			MATERIAL_DUMP_CONST_ARRAY(constantTable, asset->constantCount);
			MATERIAL_DUMP_STATE_MAP(stateMap, asset->stateBitsCount);

			nlohmann::json material_images;
			for (int i = 0; i < asset->numMaps; i++)
			{
				nlohmann::json image;

				// watermap
				if (asset->maps[i].semantic == 11)
				{
					ZONETOOL_INFO("Dumping water data for image %s\n", asset->name);

					water_t* waterData = reinterpret_cast<water_t*>(asset->maps[i].image);

					image["image"] = waterData->image->name;

					nlohmann::json waterdata;
					waterdata["floatTime"] = waterData->writable.floatTime;
					waterdata["codeConstant"][0] = waterData->codeConstant[0];
					waterdata["codeConstant"][1] = waterData->codeConstant[1];
					waterdata["codeConstant"][2] = waterData->codeConstant[2];
					waterdata["codeConstant"][3] = waterData->codeConstant[3];
					waterdata["M"] = waterData->M;
					waterdata["N"] = waterData->N;
					waterdata["Lx"] = waterData->Lx;
					waterdata["Lz"] = waterData->Lz;
					waterdata["gravity"] = waterData->gravity;
					waterdata["windvel"] = waterData->windvel;
					waterdata["winddir"][0] = waterData->winddir[0];
					waterdata["winddir"][1] = waterData->winddir[1];
					waterdata["amplitude"] = waterData->amplitude;

					nlohmann::json waterComplexData;
					nlohmann::json wTerm;

					for (int i = 0; i < waterData->M * waterData->N; i++)
					{
						nlohmann::json complexdata;
						nlohmann::json curWTerm;

						complexdata["real"] = waterData->H0[i].real;
						complexdata["imag"] = waterData->H0[i].imag;

						curWTerm[i] = waterData->wTerm[i];

						waterComplexData[i] = complexdata;
					}

					waterdata["complex"] = waterComplexData;
					waterdata["wTerm"] = wTerm;

					image["waterinfo"] = waterdata;
				}
				else
				{
					if (asset->maps[i].image)
					{
						image["image"] = asset->maps[i].image->name;
					}
					else
					{
						image["image"] = "";
					}
				}

				image["semantic"] = asset->maps[i].semantic;
				image["sampleState"] = asset->maps[i].sampleState;
				image["lastCharacter"] = asset->maps[i].secondLastCharacter;
				image["firstCharacter"] = asset->maps[i].firstCharacter;
				image["typeHash"] = asset->maps[i].typeHash;

				// add image data to material
				material_images[i] = image;
			}
			matdata["maps"] = material_images;

			std::string assetstr = matdata.dump(4);

			auto assetPath = "materials\\"s + asset->name;

			auto fileAsset = FileSystem::FileOpen(assetPath, "wb");

			if (fileAsset)
			{
				fwrite(assetstr.c_str(), assetstr.size(), 1, fileAsset);
				FileSystem::FileClose(fileAsset);
			}
		}
	}
}
