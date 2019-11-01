// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW5
	{
		char* freadstr(FILE* file)
		{
			char tempBuf[1024];
			char ch = 0;
			int i = 0;

			do
			{
				fread(&ch, 1, 1, file);

				tempBuf[i++] = ch;

				if (i >= sizeof(tempBuf))
				{
					throw std::exception("this is wrong");
				}
			}
			while (ch);

			char* retval = new char[i];
			strcpy(retval, tempBuf);

			return retval;
		}

		int freadint(FILE* file)
		{
			int value;
			fread(&value, sizeof(int), 1, file);
			return value;
		}

		XModel* IXModel::parse_new(const std::string& name, ZoneMemory* mem,
		                           const std::string& filename)
		{
			auto asset = mem->Alloc<XModel>();

			AssetReader read(mem);

			if (!read.open(filename))
			{
				return nullptr;
			}

#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW5::IXModel::parse_new");
#endif

			ZONETOOL_INFO("Parsing xmodel \"%s\"...", name.c_str());

			asset->name = read.read_string();
			asset->scale = read.read_float();
			asset->radius = read.read_float();

			auto bounds = read.read_array<Bounds>();
			memcpy(&asset->bounds, bounds, sizeof Bounds);

			asset->contents = read.read_int();
			asset->flags = read.read_int();
			asset->contents = read.read_int();

			for (int i = 0; i < 6; i++)
			{
				asset->noScalePartBits[i] = read.read_int();
			}

			// tags
			asset->numBones = read.read_int();
			asset->numRootBones = read.read_int();
			asset->boneNames = read.read_array<short>();
			for (int i = 0; i < asset->numBones; i++)
			{
				asset->boneNames[i] = SL_AllocString(read.read_string());
			}
			asset->parentList = read.read_array<unsigned char>();
			asset->tagAngles = read.read_array<XModelAngle>();
			asset->tagPositions = read.read_array<XModelTagPos>();
			asset->partClassification = read.read_array<char>();
			asset->animMatrix = read.read_array<DObjAnimMat>();
			asset->boneInfo = read.read_array<XBoneInfo>();

			// surfaces
			asset->numSurfaces = read.read_int();
			asset->materials = read.read_array<Material*>();
			for (int i = 0; i < asset->numSurfaces; i++)
			{
				asset->materials[i] = read.read_asset<Material>();
			}

			// lods
			asset->numLods = read.read_int();
			asset->maxLoadedLod = read.read_int();
			asset->lodRampType = read.read_int();
			auto lods = read.read_array<XSurfaceLod>();
			memcpy(asset->lods, lods, sizeof XSurfaceLod * 4);
			for (int i = 0; i < asset->numLods; i++)
			{
				asset->lods[i].surfaces = read.read_asset<ModelSurface>();
			}

			// colSurfs
			asset->numColSurfs = read.read_int();
			asset->colSurf = read.read_array<XModelCollSurf_s>();
			for (int i = 0; i < asset->numColSurfs; i++)
			{
				asset->colSurf[i].tris = read.read_array<XModelCollTri_s>();
			}

			// subassets
			asset->physPreset = read.read_asset<PhysPreset>();
			asset->physCollmap = read.read_asset<PhysCollmap>();

#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif

			return asset;
		}

		XModel* IXModel::parse(std::string name, ZoneMemory* mem)
		{
			return this->parse_new(name, mem, "XModel\\" + name + ".xme5");
		}

		void IXModel::init(const std::string& name, ZoneMemory* mem)
		{
			this->isXME5OrNewer = true;

			this->m_name = name;
			this->m_asset = this->parse(name, mem);

			if (!this->m_asset)
			{
				this->m_asset = DB_FindXAssetHeader(this->type(), this->m_name.data(), 1).xmodel;
			}
		}

		void IXModel::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
			// fixup scriptstrings
			auto xmodel = mem->Alloc<XModel>();
			memcpy(xmodel, this->m_asset, sizeof XModel);

			// allocate bonenames
			if (this->m_asset->boneNames)
			{
				xmodel->boneNames = mem->Alloc<short>(xmodel->numBones);
				memcpy(xmodel->boneNames, this->m_asset->boneNames, sizeof(short) * xmodel->numBones);

				for (int i = 0; i < xmodel->numBones; i++)
				{
					if (xmodel->boneNames[i])
					{
						std::string bone = SL_ConvertToString(this->m_asset->boneNames[i]);
						xmodel->boneNames[i] = buf->write_scriptstring(bone);
					}
				}
			}

			this->m_asset = xmodel;
		}

		void IXModel::load_depending(IZone* zone)
		{
#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW5::IXModel::load_depending");
#endif

			auto data = this->m_asset;

			// Materials
			for (std::int32_t i = 0; i < data->numSurfaces; i++)
			{
				if (data->materials[i])
				{
					zone->add_asset_of_type(material, data->materials[i]->name);
				}
			}

			// XSurfaces
			for (std::int32_t i = 0; i < 4; i++)
			{
				if (data->lods[i].surfaces)
				{
					// Add the pointer rather than re-finding it by name, because it might have been parsed.
					if (isXME5OrNewer)
					{
						zone->add_asset_of_type(xmodelsurfs, data->lods[i].surfaces->name);
					}
					else
					{
						zone->add_asset_of_type_by_pointer(xmodelsurfs, data->lods[i].surfaces);
					}
				}
			}

			// PhysCollmap
			if (data->physCollmap)
			{
				zone->add_asset_of_type(phys_collmap, data->physCollmap->name);
			}

			// PhysPreset
			if (data->physPreset)
			{
				zone->add_asset_of_type(physpreset, data->physPreset->name);
			}

#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif
		}

		std::string IXModel::name()
		{
			return this->m_name;
		}

		std::int32_t IXModel::type()
		{
			return xmodel;
		}

		void IXModel::write(IZone* zone, ZoneBuffer* buf)
		{
#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW5::IXModel::write");
#endif

			assert(sizeof XModel, 308);

			auto data = this->m_asset;
			auto dest = buf->write<XModel>(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->boneNames)
			{
				buf->align(1);
				buf->write(data->boneNames, data->numBones);
				ZoneBuffer::ClearPointer(&dest->boneNames);
			}

			if (data->parentList)
			{
				buf->align(0);
				buf->write(data->parentList, data->numBones - data->numRootBones);
				ZoneBuffer::ClearPointer(&dest->parentList);
			}

			if (data->tagAngles)
			{
				buf->align(1);
				buf->write(data->tagAngles, data->numBones - data->numRootBones);
				ZoneBuffer::ClearPointer(&dest->tagAngles);
			}

			if (data->tagPositions)
			{
				buf->align(3);
				buf->write(data->tagPositions, data->numBones - data->numRootBones);
				ZoneBuffer::ClearPointer(&dest->tagPositions);
			}

			if (data->partClassification)
			{
				buf->align(0);
				buf->write(data->partClassification, data->numBones);
				ZoneBuffer::ClearPointer(&dest->partClassification);
			}

			if (data->animMatrix)
			{
				buf->align(3);
				buf->write(data->animMatrix, data->numBones);
				ZoneBuffer::ClearPointer(&dest->animMatrix);
			}

			if (data->materials)
			{
				buf->align(3);

				auto destMaterials = buf->write(data->materials, data->numSurfaces);
				for (int i = 0; i < data->numSurfaces; i++)
				{
					destMaterials[i] = reinterpret_cast<Material*>(zone->get_asset_pointer(
						material, data->materials[i]->name));
				}

				ZoneBuffer::ClearPointer(&dest->materials);
			}

			for (int i = 0; i < 4; i++)
			{
				if (!data->lods[i].surfaces) continue;
				dest->lods[i].surfaces = reinterpret_cast<ModelSurface*>(zone->get_asset_pointer(
					xmodelsurfs, data->lods[i].surfaces->name));

				sizeof XSurfaceLod;
			}

			if (data->colSurf)
			{
				buf->align(3);
				auto destSurf = buf->write(data->colSurf, data->numColSurfs);

				for (int i = 0; i < data->numColSurfs; i++)
				{
					destSurf[i].tris = buf->write_s(3, data->colSurf[i].tris, data->colSurf[i].numCollTris);
				}

				ZoneBuffer::ClearPointer(&dest->colSurf);
			}

			if (data->boneInfo)
			{
				buf->align(3);
				buf->write(data->boneInfo, data->numBones);
				ZoneBuffer::ClearPointer(&dest->boneInfo);
			}

			if (data->physPreset)
			{
				dest->physPreset = reinterpret_cast<PhysPreset*>(zone->get_asset_pointer(
					physpreset, data->physPreset->name));
			}

			if (data->physCollmap)
			{
				dest->physCollmap = reinterpret_cast<PhysCollmap*>(zone->get_asset_pointer(
					phys_collmap, data->physCollmap->name));
			}

			END_LOG_STREAM;
			buf->pop_stream();

#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif
		}

		void IXModel::dump(XModel* asset)
		{
#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW5::IXModel::dump");
#endif

			AssetDumper dump;
			dump.open("XModel\\"s + asset->name + ".xme5");

			// name
			dump.dump_string(asset->name);

			// generic data
			dump.dump_float(asset->scale);
			dump.dump_float(asset->radius);
			dump.dump_array(&asset->bounds, 1);
			dump.dump_int(asset->contents);
			dump.dump_int(asset->flags);
			dump.dump_int(asset->contents);

			// partbits
			for (int i = 0; i < 6; i++)
			{
				dump.dump_int(asset->noScalePartBits[i]);
			}

			// tags
			dump.dump_int(asset->numBones);
			dump.dump_int(asset->numRootBones);
			dump.dump_array(asset->boneNames, asset->numBones);
			for (int i = 0; i < asset->numBones; i++)
			{
				dump.dump_string(SL_ConvertToString(asset->boneNames[i]));
			}
			dump.dump_array(asset->parentList, asset->numBones - asset->numRootBones);
			dump.dump_array(asset->tagAngles, asset->numBones - asset->numRootBones);
			dump.dump_array(asset->tagPositions, asset->numBones - asset->numRootBones);
			dump.dump_array(asset->partClassification, asset->numBones);
			dump.dump_array(asset->animMatrix, asset->numBones);
			dump.dump_array(asset->boneInfo, asset->numBones);

			// surfaces
			dump.dump_int(asset->numSurfaces);
			dump.dump_array(asset->materials, asset->numSurfaces);
			for (int i = 0; i < asset->numSurfaces; i++)
			{
				dump.dump_asset(asset->materials[i]);
			}

			// lods
			dump.dump_int(asset->numLods);
			dump.dump_int(asset->maxLoadedLod);
			dump.dump_int(asset->lodRampType);
			dump.dump_array(asset->lods, 4);
			for (int i = 0; i < asset->numLods; i++)
			{
				dump.dump_asset(asset->lods[i].surfaces);
			}

			// colSurfs
			dump.dump_int(asset->numColSurfs);
			dump.dump_array(asset->colSurf, asset->numColSurfs);
			for (int i = 0; i < asset->numColSurfs; i++)
			{
				dump.dump_array(asset->colSurf[i].tris, asset->colSurf[i].numCollTris);
			}

			// subassets
			dump.dump_asset(asset->physPreset);
			dump.dump_asset(asset->physCollmap);

			dump.close();

#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif
		}

		IXModel::IXModel()
		{
		}

		IXModel::~IXModel()
		{
		}
	}
}
