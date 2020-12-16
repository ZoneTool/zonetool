// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"

// Material parsing
#define MATERIAL_INT(entry) \
	mat->entry = matdata[#entry].get<int>();

#define MATERIAL_DUMP_INT(entry) \
	matdata[#entry] = mat->entry;

#define MATERIAL_DUMP_STRING(entry) \
	matdata[#entry] = std::string(mat->entry);

#define MATERIAL_DUMP_MATIMG_ARRAY(entry,size) \
	nlohmann::json matimg##entry; \
	for (int i = 0; i < size; i++) \
	{ \
		nlohmann::json img##entry; \
		img##entry["image"] = mat->entry[i].image->name; \
		img##entry["semantic"] = (int)mat->entry[i].semantic; \
		img##entry["sampleState"] = (int)mat->entry[i].sampleState; \
		img##entry["lastCharacter"] = (char)mat->entry[i].secondLastCharacter; \
		img##entry["firstCharacter"] = (char)mat->entry[i].firstCharacter; \
		img##entry["typeHash"] = (unsigned int)mat->entry[i].typeHash; \
		matimg##entry[i] = img##entry; \
	} \
	matdata[#entry] = matimg##entry;

#define MATERIAL_DUMP_CONST_ARRAY(entry,size) \
	nlohmann::json carr##entry; \
	for (int i = 0; i < size; i++) \
	{ \
		nlohmann::json cent##entry; \
		cent##entry["name"] = mat->entry[i].name; \
		nlohmann::json centliteral##entry; \
		centliteral##entry[0] = mat->entry[i].literal[0]; \
		centliteral##entry[1] = mat->entry[i].literal[1]; \
		centliteral##entry[2] = mat->entry[i].literal[2]; \
		centliteral##entry[3] = mat->entry[i].literal[3]; \
		cent##entry["nameHash"] = mat->entry[i].nameHash; \
		cent##entry["literal"] = centliteral##entry; \
		carr##entry[i] = cent##entry; \
	} \
	matdata[#entry] = carr##entry;

#define MATERIAL_DUMP_STATE_MAP(entry,size) \
	nlohmann::json carr##entry; \
	for (int i = 0; i < size; i++) \
	{ \
		nlohmann::json cent##entry; \
		cent##entry[0] = mat->entry[i].loadBits[0]; \
		cent##entry[1] = mat->entry[i].loadBits[1]; \
		carr##entry[i] = cent##entry; \
	} \
	matdata[#entry] = carr##entry;

#define MATERIAL_DUMP_BITS_ENTRY(entry,size) \
	nlohmann::json carr##entry; \
	for (int i = 0; i < size; i++) \
	{ \
		carr##entry[i] = mat->entry[i]; \
	} \
	matdata[#entry] = carr##entry;


namespace ZoneTool
{
	namespace IW3
	{
		void IMaterial::dump_statebits(Material* mat)
		{
			if (mat && mat->techniqueSet)
			{
				ITechset::dump_statebits(va("iw3/%s", mat->techniqueSet->name), mat->stateBitsEntry);
			}
		}
		
		//std::map<std::uint32_t, std::uint32_t> mapped_keys
		//{
		//	{ 0, 43 },
		//	{ 3, 0 },
		//	{ 4, 1 },
		//	{ 5, 2 },
		//	{ 6, 3 },
		//	{ 7, 4 },
		//	{ 8, 5 },
		//	{ 9, 6 },
		//	{ 10, 7 },
		//	{ 11, 8 },
		//	{ 12, 9 },
		//	{ 24, 13 },
		//	{ 38, 24 },
		//	{ 39, 25 },
		//	{ 40, 26 },
		//	{ 41, 27 },
		//	{ 42, 28 },
		//	{ 43, 29 },
		//	{ 48, 48 },
		//	{ 58, 51 },
		//	{ 59, 33 },
		//};

		std::map<std::uint32_t, std::uint32_t> mapped_keys
		{
			{ 0, 43 },
			{ 3, 0 },
			{ 4, 1 },
			{ 5, 2 },
			{ 9, 6 },				// not sure!
			{ 10, 7 },
			{ 11, 8 },				// not sure!
			{ 12, 0 },
			{ 24, 9 },
			{ 38, 28 },				// not sure!
			{ 39, 29 },
			{ 43, 29 },				// was 47 but that crashes, not sure!
			{ 48, 48 },
			{ 59, 53 },
		};

		void IMaterial::dump(Material* mat, ZoneMemory* mem)
		{
			if (mat)
			{
				dump_statebits(mat);

				auto path = "materials\\"s + mat->name;

				auto file = FileSystem::FileOpen(path, "wb");
				if (!file)
				{
					return;
				}
				
				nlohmann::json matdata;

				MATERIAL_DUMP_STRING(name);

				if (mat->techniqueSet)
				{
					matdata["techniqueSet->name"] = va("iw3/%s", mat->techniqueSet->name);
				}

				MATERIAL_DUMP_INT(gameFlags);
				// MATERIAL_DUMP_INT(sortKey);
				MATERIAL_DUMP_INT(animationX);
				MATERIAL_DUMP_INT(animationY);

				auto key_itr = mapped_keys.find(mat->sortKey);
				if (key_itr != mapped_keys.end())
				{
					matdata["sortKey"] = key_itr->second;
				}
				else
				{
					matdata["sortKey"] = mat->sortKey;
					ZONETOOL_WARNING("[%s]: sortKey %u is not mapped!", mat->name, mat->sortKey);
				}

				matdata["unknown"] = 0;

				MATERIAL_DUMP_INT(surfaceTypeBits);
				MATERIAL_DUMP_INT(stateFlags);
				MATERIAL_DUMP_INT(cameraRegion);

				MATERIAL_DUMP_CONST_ARRAY(constantTable, mat->constantCount);
				MATERIAL_DUMP_STATE_MAP(stateMap, mat->stateBitsCount);

				nlohmann::json material_images;
				for (int i = 0; i < mat->numMaps; i++)
				{
					nlohmann::json image;

					// watermap
					if (mat->maps[i].semantic == 11)
					{
						water_t* waterData = reinterpret_cast<water_t*>(mat->maps[i].image);

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
						image["image"] = mat->maps[i].image->name;
					}

					image["semantic"] = mat->maps[i].semantic;
					image["sampleState"] = mat->maps[i].sampleState;
					image["lastCharacter"] = mat->maps[i].secondLastCharacter;
					image["firstCharacter"] = mat->maps[i].firstCharacter;
					image["typeHash"] = mat->maps[i].typeHash;

					// add image data to material
					material_images[i] = image;
				}
				matdata["maps"] = material_images;

				auto assetData = matdata.dump(4);

				// write data to disk
				fwrite(&assetData[0], assetData.size(), 1, file);
				FileSystem::FileClose(file);
			}
		}
	}
}
