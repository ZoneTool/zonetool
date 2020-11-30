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
		                           const std::string& filename, const std::function<std::uint16_t(const std::string&)>& allocString)
		{
			AssetReader read(mem);

			if (!read.open(filename))
			{
				return nullptr;
			}

#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW5::IXModel::parse_new");
#endif

			ZONETOOL_INFO("Parsing xmodel \"%s\"...", name.c_str());

			const auto asset = read.read_single<XModel>();
			asset->name = read.read_string();
			
			asset->boneNames = read.read_array<short>();
			for (int i = 0; i < asset->numBones; i++)
			{
				asset->boneNames[i] = allocString(read.read_string());
			}
			
			asset->parentList = read.read_array<unsigned char>();
			asset->tagAngles = read.read_array<XModelAngle>();
			asset->tagPositions = read.read_array<XModelTagPos>();
			asset->partClassification = read.read_array<char>();
			asset->animMatrix = read.read_array<DObjAnimMat>();
			asset->boneInfo = read.read_array<XBoneInfo>();

			// surfaces
			asset->materials = read.read_array<Material*>();
			for (int i = 0; i < asset->numSurfaces; i++)
			{
				asset->materials[i] = read.read_asset<Material>();
			}

			// lods
			for (int i = 0; i < asset->numLods; i++)
			{
				asset->lods[i].surfaces = read.read_asset<ModelSurface>();
			}

			// colSurfs
			asset->colSurf = read.read_array<XModelCollSurf_s>();
			for (int i = 0; i < asset->numColSurfs; i++)
			{
				asset->colSurf[i].tris = read.read_array<XModelCollTri_s>();
			}

			// subassets
			asset->physPreset = read.read_asset<PhysPreset>();
			asset->physCollmap = read.read_asset<PhysCollmap>();

			// fix tags
			for (auto i = 0u; i < asset->numBones; i++)
			{
				auto tag_name = std::string(SL_ConvertToString(asset->boneNames[i]));

				// fix xmodel tags
				if (tag_name.find("tag_") != std::string::npos &&
					tag_name.find("_scope") != std::string::npos &&
					tag_name.find("thermal") == std::string::npos)
				{
					ZONETOOL_INFO("fixing tag %s -> tag_scope", tag_name.data());
					asset->boneNames[i] = SL_AllocString("tag_scope");
				}
			}
			
#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif

			return asset;
		}

		XModel* IXModel::parse(std::string name, ZoneMemory* mem, const std::function<std::uint16_t(const std::string&)>& allocString)
		{			
			return IXModel::parse_new(name, mem, "XModel\\" + name + ".xme6", allocString);
		}

		void IXModel::init(const std::string& name, ZoneMemory* mem)
		{
			this->is_scope_model_ = false;
			this->name_ = name;
						
			if ((name.find("weapon_") != std::string::npos || name.find("viewmodel_") != std::string::npos) &&
				name.find("_scope") != std::string::npos &&
				!FileSystem::FileExists("XModel\\" + name + ".xme6"))
			{
				const auto model = IXModel::parse_new(name, mem, "XModel\\" + name.substr(0, name.size() - 6) + ".xme6", SL_AllocString);

				if (model != nullptr)
				{
					ZONETOOL_INFO("Raping sniper model to create a scope!");

					std::vector<XSurface> allocated_surfaces;
					std::vector<std::pair<Material*, std::int32_t>> scope_surfaces;

					for (auto i = 0u; i < model->lods[0].numSurfacesInLod; i++)
					{
						if (static_cast<std::string>(model->materials[i]->name).find("scope") != std::string::npos)
						{
							scope_surfaces.push_back({ model->materials[i], i });
						}
					}

					if (scope_surfaces.empty())
					{
						ZONETOOL_FATAL("Trying to rebuild scope model failed: No surfaces to build scope model!");
					}

					// alloc surfaces
					allocated_surfaces.resize(scope_surfaces.size());

					// force parse XSurfaces
					const auto surfaces = IXSurface::parse(model->lods[0].surfaces->name, mem);

					// copy XSurfaces
					for (auto i = 0u; i < scope_surfaces.size(); i++)
					{
						memcpy(&allocated_surfaces[i], &surfaces->xSurficies[scope_surfaces[i].second], sizeof XSurface);
					}

					// rebuild surfaces / model info
					model->numLods = 1;
					model->numSurfaces = scope_surfaces.size();

					// kill other lod data
					memset(&model->lods[1], 0, sizeof XSurfaceLod * 3);

					// fix materials
					for (auto i = 0u; i < scope_surfaces.size(); i++)
					{
						model->materials[i] = scope_surfaces[i].first;
					}

					// rebuild lod
					model->lods[0].numSurfacesInLod = allocated_surfaces.size();
					model->lods[0].surfaces->name = mem->StrDup(va("%s_scope", model->lods[0].surfaces->name));
					model->lods[0].surfaces->xSurficiesCount = allocated_surfaces.size();
					model->lods[0].surfaces->xSurficies = new XSurface[allocated_surfaces.size()];
					memcpy(model->lods[0].surfaces->xSurficies, allocated_surfaces.data(), sizeof XSurface * allocated_surfaces.size());

					// remove all bones besides for tag_scope
					XBoneInfo scope_info = {};
					auto found_bone = false;
					
					for (auto i = 0u; i < model->numBones; i++)
					{
						if (SL_ConvertToString(model->boneNames[i]) == "tag_scope"s)
						{
							memcpy(&scope_info, &model->boneInfo[i], sizeof XBoneInfo);
							found_bone = true;
							break;
						}
					}

					if (found_bone)
					{
						model->numBones = 1;
						model->numRootBones = 1;
						memcpy(&model->boneInfo[0], &scope_info, sizeof XBoneInfo);
						model->boneNames[0] = SL_AllocString("tag_scope");
					}
					else
					{
						ZONETOOL_FATAL("You dun goofed");
					}

					// test
					model->numColSurfs = 0;
					model->numSurfaces = 0;

					for (auto i = 0; i < 4; i++)
					{
						model->lods[i].numSurfacesInLod = 0;
						if (model->lods[i].surfaces)
						{
							model->lods[i].surfaces->xSurficiesCount = 0;
						}
					}
					
					//
					this->is_scope_model_ = true;
					this->asset_ = model;
				}
			}
			else
			{
				this->asset_ = this->parse(name, mem);
			}
			
			// don't reparse the surfaces
			// this->is_scope_model_ = true;
			
			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name_.data(), 1).xmodel;
			}
		}

		void IXModel::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
			// fixup scriptstrings
			auto xmodel = mem->Alloc<XModel>();
			memcpy(xmodel, this->asset_, sizeof XModel);

			// allocate bonenames
			if (this->asset_->boneNames)
			{
				xmodel->boneNames = mem->Alloc<short>(xmodel->numBones);
				memcpy(xmodel->boneNames, this->asset_->boneNames, sizeof(short) * xmodel->numBones);

				for (int i = 0; i < xmodel->numBones; i++)
				{
					if (xmodel->boneNames[i])
					{
						std::string bone = SL_ConvertToString(this->asset_->boneNames[i]);
						xmodel->boneNames[i] = buf->write_scriptstring(bone);
					}
				}
			}

			this->asset_ = xmodel;
		}

		void IXModel::load_depending(IZone* zone)
		{
#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW5::IXModel::load_depending");
#endif

			auto data = this->asset_;

			// Materials
			for (std::int32_t i = 0; i < data->numSurfaces; i++)
			{
				if (data->materials[i])
				{
					zone->add_asset_of_type(material, data->materials[i]->name);
				}
			}

			// XSurfaces
			for (auto i = 0u; i < data->numLods; i++)
			{
				if (data->lods[i].surfaces)
				{
					// write by pointer when fucking with scopes
					if (is_scope_model_)
					{
						zone->add_asset_of_type_by_pointer(xmodelsurfs, data->lods[i].surfaces);
					}
					else
					{
						zone->add_asset_of_type(xmodelsurfs, data->lods[i].surfaces->name);
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
			return this->name_;
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

			auto data = this->asset_;
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

		void IXModel::dump(XModel* asset, const std::function<const char* (std::uint16_t)>& convertToString)
		{
#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW5::IXModel::dump");
#endif

			AssetDumper dump;
			dump.open("XModel\\"s + asset->name + ".xme6");

			// name
			dump.dump_single(asset);
			dump.dump_string(asset->name);

			// tags
			dump.dump_array(asset->boneNames, asset->numBones);
			for (int i = 0; i < asset->numBones; i++)
			{
				dump.dump_string(convertToString(asset->boneNames[i]));
			}
			dump.dump_array(asset->parentList, asset->numBones - asset->numRootBones);
			dump.dump_array(asset->tagAngles, asset->numBones - asset->numRootBones);
			dump.dump_array(asset->tagPositions, asset->numBones - asset->numRootBones);
			dump.dump_array(asset->partClassification, asset->numBones);
			dump.dump_array(asset->animMatrix, asset->numBones);
			dump.dump_array(asset->boneInfo, asset->numBones);

			// surfaces
			dump.dump_array(asset->materials, asset->numSurfaces);
			for (int i = 0; i < asset->numSurfaces; i++)
			{
				dump.dump_asset(asset->materials[i]);
			}

			// lods
			for (int i = 0; i < asset->numLods; i++)
			{
				dump.dump_asset(asset->lods[i].surfaces);
			}

			// colSurfs
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
	}
}
