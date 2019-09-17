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

		XModel* IXModel::parse_new(const std::string& name, std::shared_ptr<ZoneMemory>& mem,
		                           const std::string& filename)
		{
			auto asset = mem->Alloc<XModel>();

			AssetReader read(mem);

			if (!read.Open(filename))
			{
				return nullptr;
			}

#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW5::IXModel::parse_new");
#endif

			ZONETOOL_INFO("Parsing xmodel \"%s\"...", name.c_str());

			asset->name = read.String();
			asset->scale = read.Float();
			asset->radius = read.Float();

			auto bounds = read.Array<Bounds>();
			memcpy(&asset->bounds, bounds, sizeof Bounds);

			asset->contents = read.Int();
			asset->flags = read.Int();
			asset->contents = read.Int();

			for (int i = 0; i < 6; i++)
			{
				asset->noScalePartBits[i] = read.Int();
			}

			// tags
			asset->numBones = read.Int();
			asset->numRootBones = read.Int();
			asset->boneNames = read.Array<short>();
			for (int i = 0; i < asset->numBones; i++)
			{
				asset->boneNames[i] = SL_AllocString(read.String());
			}
			asset->parentList = read.Array<unsigned char>();
			asset->tagAngles = read.Array<XModelAngle>();
			asset->tagPositions = read.Array<XModelTagPos>();
			asset->partClassification = read.Array<char>();
			asset->animMatrix = read.Array<DObjAnimMat>();
			asset->boneInfo = read.Array<XBoneInfo>();

			// surfaces
			asset->numSurfaces = read.Int();
			asset->materials = read.Array<Material*>();
			for (int i = 0; i < asset->numSurfaces; i++)
			{
				asset->materials[i] = read.Asset<Material>();
			}

			// lods
			asset->numLods = read.Int();
			asset->maxLoadedLod = read.Int();
			asset->lodRampType = read.Int();
			auto lods = read.Array<XSurfaceLod>();
			memcpy(asset->lods, lods, sizeof XSurfaceLod * 4);
			for (int i = 0; i < asset->numLods; i++)
			{
				asset->lods[i].surfaces = read.Asset<ModelSurface>();
			}

			// colSurfs
			asset->numColSurfs = read.Int();
			asset->colSurf = read.Array<XModelCollSurf_s>();
			for (int i = 0; i < asset->numColSurfs; i++)
			{
				asset->colSurf[i].tris = read.Array<XModelCollTri_s>();
			}

			// subassets
			asset->physPreset = read.Asset<PhysPreset>();
			asset->physCollmap = read.Asset<PhysCollmap>();

#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif

			return asset;
		}

		XModel* IXModel::parse(std::string name, std::shared_ptr<ZoneMemory>& mem)
		{
			return this->parse_new(name, mem, "XModel\\" + name + ".xme5");
		}

		void IXModel::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			this->isXME5OrNewer = true;

			this->m_name = name;
			this->m_asset = this->parse(name, mem);

			if (!this->m_asset)
			{
				this->m_asset = DB_FindXAssetHeader(this->type(), this->m_name.data(), 1).xmodel;
			}
		}

		void IXModel::prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
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
					zone->AddAssetOfType(material, data->materials[i]->name);
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
						zone->AddAssetOfType(xmodelsurfs, data->lods[i].surfaces->name);
					}
					else
					{
						zone->AddAssetOfTypePtr(xmodelsurfs, data->lods[i].surfaces);
					}
				}
			}

			// PhysCollmap
			if (data->physCollmap)
			{
				zone->AddAssetOfType(phys_collmap, data->physCollmap->name);
			}

			// PhysPreset
			if (data->physPreset)
			{
				zone->AddAssetOfType(physpreset, data->physPreset->name);
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

		void IXModel::write(IZone* zone, std::shared_ptr<ZoneBuffer>& buf)
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
					destMaterials[i] = reinterpret_cast<Material*>(zone->GetAssetPointer(
						material, data->materials[i]->name));
				}

				ZoneBuffer::ClearPointer(&dest->materials);
			}

			for (int i = 0; i < 4; i++)
			{
				if (!data->lods[i].surfaces) continue;
				dest->lods[i].surfaces = reinterpret_cast<ModelSurface*>(zone->GetAssetPointer(
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
				dest->physPreset = reinterpret_cast<PhysPreset*>(zone->GetAssetPointer(
					physpreset, data->physPreset->name));
			}

			if (data->physCollmap)
			{
				dest->physCollmap = reinterpret_cast<PhysCollmap*>(zone->GetAssetPointer(
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
			dump.Open("XModel\\"s + asset->name + ".xme5");

			// name
			dump.String(asset->name);

			// generic data
			dump.Float(asset->scale);
			dump.Float(asset->radius);
			dump.Array(&asset->bounds, 1);
			dump.Int(asset->contents);
			dump.Int(asset->flags);
			dump.Int(asset->contents);

			// partbits
			for (int i = 0; i < 6; i++)
			{
				dump.Int(asset->noScalePartBits[i]);
			}

			// tags
			dump.Int(asset->numBones);
			dump.Int(asset->numRootBones);
			dump.Array(asset->boneNames, asset->numBones);
			for (int i = 0; i < asset->numBones; i++)
			{
				dump.String(SL_ConvertToString(asset->boneNames[i]));
			}
			dump.Array(asset->parentList, asset->numBones - asset->numRootBones);
			dump.Array(asset->tagAngles, asset->numBones - asset->numRootBones);
			dump.Array(asset->tagPositions, asset->numBones - asset->numRootBones);
			dump.Array(asset->partClassification, asset->numBones);
			dump.Array(asset->animMatrix, asset->numBones);
			dump.Array(asset->boneInfo, asset->numBones);

			// surfaces
			dump.Int(asset->numSurfaces);
			dump.Array(asset->materials, asset->numSurfaces);
			for (int i = 0; i < asset->numSurfaces; i++)
			{
				dump.Asset(asset->materials[i]);
			}

			// lods
			dump.Int(asset->numLods);
			dump.Int(asset->maxLoadedLod);
			dump.Int(asset->lodRampType);
			dump.Array(asset->lods, 4);
			for (int i = 0; i < asset->numLods; i++)
			{
				dump.Asset(asset->lods[i].surfaces);
			}

			// colSurfs
			dump.Int(asset->numColSurfs);
			dump.Array(asset->colSurf, asset->numColSurfs);
			for (int i = 0; i < asset->numColSurfs; i++)
			{
				dump.Array(asset->colSurf[i].tris, asset->colSurf[i].numCollTris);
			}

			// subassets
			dump.Asset(asset->physPreset);
			dump.Asset(asset->physCollmap);

			dump.Close();

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
