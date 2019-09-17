#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW4
	{
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
			VMProtectBeginUltra("IW4::IXModel::parse_new");
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

				//if (asset->lods[i].surfaces && strlen(asset->lods[i].surfaces->name))
				//{
				//	auto xsurf = new IXSurface;
				//	xsurf->init(static_cast<std::string>(asset->lods[i].surfaces->name), mem);

				//	dependingSurfaces[i] = xsurf;
				//}
				//else
				//{
				//	dependingSurfaces[i] = nullptr;
				//	asset->lods[i].surfaces = nullptr;
				//}
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

			read.Close();

			return asset;
		}

		XModel* IXModel::parse(std::string name, std::shared_ptr<ZoneMemory>& mem)
		{
			auto path = ""s;

			path = "XModel\\" + name + ".xme5";
			isXME5OrNewer = false;
			/*auto xme5asset = */ // return this->parse_new(name, mem, path);

			/*if (xme5asset)
			{
				this->isXME5OrNewer = true;
				return xme5asset;
			}*/

			// old code
			int version = 2;

			path = "XModel\\" + name + ".xme2";
			auto file = FileSystem::FileOpen(path, "rb"s);

			if (!file)
			{
				path = "XModel\\" + name + ".xme3";
				file = FileSystem::FileOpen(path, "rb"s);

				if (!file)
				{
					path = "XModel\\" + name + ".xme4";
					file = FileSystem::FileOpen(path, "rb"s);

					if (!file)
					{
						path = "XModel\\" + name + ".xme5";
						file = FileSystem::FileOpen(path, "rb"s);

						if (!file)
						{
							return nullptr;
						}
						isXME5OrNewer = true;

						FileSystem::FileClose(file);
						return this->parse_new(name, mem, path);
					}
					version = 4;
				}
				else
				{
					version = 3;
				}
			}

			FileSystem::FileClose(file);
			return nullptr;
		}

		void IXModel::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			memset(this->dependingSurfaces, 0, sizeof this->dependingSurfaces);

			this->isXME5OrNewer = false;
			this->m_name = name;
			this->m_asset = this->parse(name, mem);

			if (!this->m_asset)
			{
				this->m_asset = DB_FindXAssetHeader(this->type(), this->m_name.data()).xmodel;
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
			VMProtectBeginUltra("IW4::IXModel::load_depending");
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

				//if (data->lods[i].surfaces && dependingSurfaces[i] != nullptr)
				//{
				//	dependingSurfaces[i]->load_depending(zone);
				//}
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
			VMProtectBeginUltra("IW4::IXModel::write");
#endif

			auto data = this->m_asset;
			auto dest = buf->write<XModel>(data);

			assert(sizeof XModel, 304);

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

				//if (data->lods[i].surfaces && dependingSurfaces[i])
				//{
				//	buf->push_stream(0);
				//	buf->align(3);
				//	dependingSurfaces[i]->write(zone, buf);
				//	buf->pop_stream();

				//	//ZoneBuffer::ClearPointer(&dest->lods[i].surfaces);
				//	//delete[] dependingSurfaces[i];

				//	dest->lods[i].numSurfacesInLod = data->lods[i].surfaces->xSurficiesCount;
				//	dest->lods[i].surfaces = (ModelSurface*)-1;
				//}
				//else
				//{
				//	dest->lods[i].surfaces = nullptr;
				//}
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

			if (data->physCollmap)
			{
				dest->physCollmap = reinterpret_cast<PhysCollmap*>(zone->GetAssetPointer(
					phys_collmap, data->physCollmap->name));
			}

			if (data->physPreset)
			{
				dest->physPreset = reinterpret_cast<PhysPreset*>(zone->GetAssetPointer(
					physpreset, data->physPreset->name));
			}

			END_LOG_STREAM;
			buf->pop_stream();

#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif
		}

		void IXModel::dump_internal(XModel* asset, const std::function<const char*(uint16_t)>& convertToString)
		{
#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW4::IXModel::dump");
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
				dump.String(convertToString(asset->boneNames[i]));
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

		bool starts_with(const std::string& input, const std::string& str)
		{
			return (input.size() >= str.size() && input.substr(0, str.size()) == str);
		}

		std::vector<std::string> attachment_strings
		{
			"red_dot",
			"thermal",
			"red_cross",
			"acog",
			"eotech",
			"suppressor",
			"reflex",
			"foregrip",
			"motion",
		};

		bool is_attachment_material(const std::string& material)
		{
			for (auto& att : attachment_strings)
			{
				if (material.find(att) != std::string::npos)
				{
					return true;
				}
			}

			return false;
		}

		XModel* IXModel::remove_attachments(XModel* asset)
		{
			// allocate new model
			auto model = new XModel;
			memcpy(model, asset, sizeof XModel);

			// 
			std::vector<Material*> new_materials{};
			std::vector<std::pair<std::int32_t, XSurface>> new_surfaces{};

			// remove all materials that we don't need
			for (auto i = 0u; i < model->numSurfaces; i++)
			{
				if (model->materials && model->materials[i])
				{
					auto material_name = static_cast<std::string>(model->materials[i]->name);

					// if the material does not belong to an attachment, add it back to the xmodel
					if (!is_attachment_material(material_name))
					{
						// add the material
						new_materials.push_back(model->materials[i]);

						// add the surface too
						for (auto lod = 0; lod < 4; lod++)
						{
							// check if the surface belongs to the current lod
							if (i >= model->lods[lod].surfIndex && i < model->lods[lod].surfIndex + model->lods[lod].
								numSurfacesInLod)
							{
								// add surface to vector
								new_surfaces.emplace_back(
									lod, model->lods[lod].surfaces->xSurficies[i - model->lods[lod].surfIndex]);
							}
						}
					}
				}
			}

			// make sure everything went according to plan
			assert(new_materials.size() == new_surfaces.size());

			// rebuild material array
			model->materials = new Material*[new_materials.size()];
			memcpy(model->materials, new_materials.data(), sizeof(Material*) * new_materials.size());

			// rebuild lods
			auto surface_start_index = 0u;
			for (auto lod = 0; lod < 4; lod++)
			{
				if (!model->lods[lod].surfaces)
				{
					continue;
				}

				std::vector<XSurface> model_surfaces_for_lod{};

				// loop through every surface that should be re-added to our custom model
				for (auto& surface : new_surfaces)
				{
					// if the lod matches, add it to the queue
					if (lod == surface.first)
					{
						model_surfaces_for_lod.push_back(surface.second);
					}
				}

				// fix current lod
				model->lods[lod].numSurfacesInLod = model_surfaces_for_lod.size();
				model->lods[lod].surfIndex = surface_start_index;

				// alloc new XModelSurfs asset
				const auto new_model_surface = new ModelSurface;
				memcpy(new_model_surface, model->lods[lod].surfaces, sizeof ModelSurface);

				new_model_surface->name = _strdup(va("%s_no_attach", new_model_surface->name).data());
				new_model_surface->xSurficiesCount = model_surfaces_for_lod.size();

				new_model_surface->xSurficies = new XSurface[model_surfaces_for_lod.size()];
				memcpy(new_model_surface->xSurficies, model_surfaces_for_lod.data(),
				       sizeof(XSurface) * model_surfaces_for_lod.size());

				// patch surfaces ptr
				model->lods[lod].surfaces = new_model_surface;

				// increment surface_start_index for next lod
				surface_start_index += model_surfaces_for_lod.size();
			}

			// some logging
			ZONETOOL_INFO("%u out of %u surfaces have been removed from model %s.", asset->numSurfaces - new_materials.
size(), asset->numSurfaces, asset->name);

			// fix xmodel settings
			model->numSurfaces = new_materials.size();
			if (model->numSurfaces == 0)
			{
				return nullptr;
			}

			// swap xmodel name
			model->name = _strdup(va("%s_no_attach", asset->name).data());

			// return asset
			return model;
		}

		void IXModel::dump(XModel* asset, const std::function<const char*(uint16_t)>& convertToString)
		{
			const auto name = static_cast<std::string>(asset->name);

			// generate attachment-less xmodels
			if (starts_with(name, "viewmodel_") || starts_with(name, "weapon_"))
			{
				const auto new_model = remove_attachments(asset);

				if (new_model)
				{
					// dump attachment-less xmodel
					dump_internal(new_model, convertToString);

					// dump the XSurfaces
					for (auto lod = 0; lod < 4; lod++)
					{
						if (new_model->lods[lod].surfaces)
						{
							IXSurface::dump(new_model->lods[lod].surfaces);
						}
					}
				}
			}

			// dump regular xmodel
			dump_internal(asset, convertToString);
		}

		IXModel::IXModel()
		{
		}

		IXModel::~IXModel()
		{
		}
	}
}
