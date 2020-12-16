// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "IW5/Assets/XModel.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		XModel* IXModel::parse(std::string name, ZoneMemory* mem)
		{
			return reinterpret_cast<XModel*>(IW5::IXModel::parse(name, mem, SL_AllocString));
		}

		void IXModel::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name_.data()).xmodel;
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
			VMProtectBeginUltra("IW4::IXModel::load_depending");
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
			for (std::int32_t i = 0; i < 4; i++)
			{
				if (data->lods[i].surfaces)
				{
					zone->add_asset_of_type(xmodelsurfs, data->lods[i].surfaces->name);
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
			VMProtectBeginUltra("IW4::IXModel::write");
#endif

			auto data = this->asset_;
			auto dest = buf->write<XModel>(data);

			assert(sizeof XModel, 304);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->boneNames)
			{
				buf->align(1);
				buf->write(data->boneNames, data->numBones);
				ZoneBuffer::clear_pointer(&dest->boneNames);
			}

			if (data->parentList)
			{
				buf->align(0);
				buf->write(data->parentList, data->numBones - data->numRootBones);
				ZoneBuffer::clear_pointer(&dest->parentList);
			}

			if (data->tagAngles)
			{
				buf->align(1);
				buf->write(data->tagAngles, data->numBones - data->numRootBones);
				ZoneBuffer::clear_pointer(&dest->tagAngles);
			}

			if (data->tagPositions)
			{
				buf->align(3);
				buf->write(data->tagPositions, data->numBones - data->numRootBones);
				ZoneBuffer::clear_pointer(&dest->tagPositions);
			}

			if (data->partClassification)
			{
				buf->align(0);
				buf->write(data->partClassification, data->numBones);
				ZoneBuffer::clear_pointer(&dest->partClassification);
			}

			if (data->animMatrix)
			{
				buf->align(3);
				buf->write(data->animMatrix, data->numBones);
				ZoneBuffer::clear_pointer(&dest->animMatrix);
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

				ZoneBuffer::clear_pointer(&dest->materials);
			}

			for (int i = 0; i < 4; i++)
			{
				if (!data->lods[i].surfaces) continue;
				dest->lods[i].surfaces = reinterpret_cast<XModelSurfs*>(zone->get_asset_pointer(
					xmodelsurfs, data->lods[i].surfaces->name));

				//if (data->lods[i].surfaces && dependingSurfaces[i])
				//{
				//	buf->push_stream(0);
				//	buf->align(3);
				//	dependingSurfaces[i]->write(zone, buf);
				//	buf->pop_stream();

				//	//ZoneBuffer::clear_pointer(&dest->lods[i].surfaces);
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

				ZoneBuffer::clear_pointer(&dest->colSurf);
			}

			if (data->boneInfo)
			{
				buf->align(3);
				buf->write(data->boneInfo, data->numBones);
				ZoneBuffer::clear_pointer(&dest->boneInfo);
			}

			if (data->physCollmap)
			{
				dest->physCollmap = reinterpret_cast<PhysCollmap*>(zone->get_asset_pointer(
					phys_collmap, data->physCollmap->name));
			}

			if (data->physPreset)
			{
				dest->physPreset = reinterpret_cast<PhysPreset*>(zone->get_asset_pointer(
					physpreset, data->physPreset->name));
			}

			END_LOG_STREAM;
			buf->pop_stream();

#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif
		}

#ifdef GENERATE_IW5_MODELS
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
		
		std::vector<std::string> iw5_camos =
		{
			"autumn",
			"blue",
			"choco",
			"classic",
			"d_urban",
			"gold",
			"hex",
			"marine",
			"multi",
			"red",
			"snake",
			"snow",
			"winter"
		};
		
		XModel* IXModel::generate_iw5_camo_model(const std::string& camo, XModel* asset)
		{
			// allocate new model
			auto* model = remove_attachments(asset);
			
			// patch xmodel name
			const auto model_name = std::string(asset->name).substr(0, strlen(asset->name) - 10).append(camo).append("_no_attach");
			model->name = _strdup(model_name.data());

			// patch materials
			Material* patched_material = nullptr;
			for (auto i = 0u; i < model->numSurfaces; i++)
			{
				if (!model->materials[i])
				{
					continue;	
				}
				
				if (strstr(model->materials[i]->name, "blue_tiger"))
				{
					if (patched_material == nullptr)
					{
						patched_material = new Material;
						memcpy(patched_material, model->materials[i], sizeof Material);

						const auto material_name = std::string(patched_material->name).substr(0, strlen(patched_material->name) - 10).append(camo);
						patched_material->name = _strdup(material_name.data());

						assert(patched_material->maps[patched_material->numMaps - 1].firstCharacter == 'd');
						
						if (camo == "gold"s)
						{
							patched_material->numMaps -= 1;
							patched_material->techniqueSet = new MaterialTechniqueSet;
							memcpy(patched_material->techniqueSet, model->materials[i]->techniqueSet, sizeof MaterialTechniqueSet);

							auto technique_name = std::string(patched_material->techniqueSet->name);
							const auto detail_pos = technique_name.find_first_of("d0");
							technique_name.replace(detail_pos + 2, 2, "");

							patched_material->techniqueSet->name = _strdup(technique_name.data());
						}
						
						patched_material->maps = new MaterialImage[patched_material->numMaps];
						memcpy(patched_material->maps, model->materials[i]->maps, sizeof MaterialImage * patched_material->numMaps);

						for (auto map = 0; map < patched_material->numMaps; map++)
						{
							if (camo == "gold")
							{
								if (patched_material->maps[map].firstCharacter == 'c')
								{
									patched_material->maps[map].image = new GfxImage;
									memcpy(patched_material->maps[map].image, model->materials[i]->maps[map].image, sizeof GfxImage);
									patched_material->maps[map].image->name = "detail_gold_col";
								}
								else if (patched_material->maps[map].firstCharacter == 's')
								{
									patched_material->maps[map].image = new GfxImage;
									memcpy(patched_material->maps[map].image, model->materials[i]->maps[map].image, sizeof GfxImage);
									patched_material->maps[map].image->name = "~detail_gold_spc-r-42g-42b-42~cf35b1a3";
								}
							}
							else if (patched_material->maps[map].firstCharacter == 'd' && camo != "gold")
							{
								patched_material->maps[map].image = new GfxImage;
								memcpy(patched_material->maps[map].image, model->materials[i]->maps[map].image, sizeof GfxImage);
								patched_material->maps[map].image->name = _strdup(&va("weapon_camo_%s", camo.data())[0]);
							}
						}
						
						IMaterial::dump(patched_material);
					}

					model->materials[i] = patched_material;
				}
			}
			
			return model;
		}
		
		XModel* IXModel::remove_attachments(XModel* asset)
		{
			// allocate new model
			auto* model = new XModel;
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
									lod, model->lods[lod].surfaces->surfs[i - model->lods[lod].surfIndex]);
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
				auto* new_model_surface = new XModelSurfs;
				memcpy(new_model_surface, model->lods[lod].surfaces, sizeof XModelSurfs);

				new_model_surface->name = _strdup(va("%s_no_attach", new_model_surface->name).data());
				new_model_surface->numsurfs = model_surfaces_for_lod.size();

				new_model_surface->surfs = new XSurface[model_surfaces_for_lod.size()];
				memcpy(new_model_surface->surfs, model_surfaces_for_lod.data(),
				       sizeof(XSurface) * model_surfaces_for_lod.size());

				// patch surfaces ptr
				model->lods[lod].surfaces = new_model_surface;

				// increment surface_start_index for next lod
				surface_start_index += model_surfaces_for_lod.size();

				// dump lod
				IXSurface::dump(model->lods[lod].surfaces);
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
#endif
		
		void IXModel::dump(XModel* asset, const std::function<const char*(uint16_t)>& convertToString)
		{
			const auto name = static_cast<std::string>(asset->name);

#ifdef GENERATE_IW5_MODELS
			// generate attachment-less xmodels
			if (starts_with(name, "viewmodel_") || starts_with(name, "weapon_"))
			{
				if (name.find("blue_tiger") != std::string::npos)
				{
					for (auto& iw5_camo : iw5_camos)
					{
						auto* new_model = generate_iw5_camo_model(iw5_camo, asset);

						auto* iw5_model = new IW5::XModel;
						memcpy(iw5_model, new_model, sizeof XModel);
						iw5_model->flags = 0;

						IW5::IXModel::dump(iw5_model, convertToString);

						delete iw5_model;
					}
				}
				
				auto* new_model = remove_attachments(asset);
				
				if (new_model)
				{
					auto* iw5_model = new IW5::XModel;
					memcpy(iw5_model, new_model, sizeof XModel);
					iw5_model->flags = 0;
					
					IW5::IXModel::dump(iw5_model, convertToString);

					delete iw5_model;
				}
			}
#endif

			auto* iw5_model = new IW5::XModel;
			memcpy(iw5_model, asset, sizeof XModel);
			iw5_model->unk = 0;
			
			// dump regular xmodel
			IW5::IXModel::dump(iw5_model, convertToString);

			delete iw5_model;
		}
	}
}
