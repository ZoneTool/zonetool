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

		void endian_convert_xmodel(alpha::XModel* data)
		{
			endian_convert(&data->name);
			endian_convert(&data->numBones);
			endian_convert(&data->numRootBones);
			endian_convert(&data->numSurfaces);
			endian_convert(&data->scale);

			for (auto a = 0; a < 5; a ++)
			{
				endian_convert(&data->noScalePartBits[a]);
			}

			endian_convert(&data->boneNames);
			endian_convert(&data->parentList);
			endian_convert(&data->tagAngles);
			endian_convert(&data->tagPositions);
			endian_convert(&data->partClassification);
			endian_convert(&data->animMatrix);
			endian_convert(&data->materials);

			for (auto a = 0; a < 4; a++)
			{
				endian_convert(&data->lods[a].dist);
				endian_convert(&data->lods[a].numSurfacesInLod);
				endian_convert(&data->lods[a].surfIndex);
				endian_convert(&data->lods[a].surfaces);

				for (auto b = 0; b < 5; b++)
				{
					endian_convert(&data->lods[a].partBits[b]);
				}

				endian_convert(&data->lods[a].surfs);
			}
			
			endian_convert(&data->maxLoadedLod);
			endian_convert(&data->numLods);
			endian_convert(&data->collLod);
			endian_convert(&data->flags);
			endian_convert(&data->colSurf);
			endian_convert(&data->numColSurfs);
			endian_convert(&data->contents);
			endian_convert(&data->boneInfo);
			endian_convert(&data->radius);
			endian_convert(&data->bounds.halfSize[0]);
			endian_convert(&data->bounds.halfSize[1]);
			endian_convert(&data->bounds.halfSize[2]);
			endian_convert(&data->bounds.midPoint[0]);
			endian_convert(&data->bounds.midPoint[1]);
			endian_convert(&data->bounds.midPoint[2]);
			endian_convert(&data->invHighMipRadius);
			endian_convert(&data->memUsage);
			endian_convert(&data->physPreset);
			endian_convert(&data->physCollmap);
		}
		
		void write_colsurf(IZone* zone, ZoneBuffer* buf, XModel* data, XModelCollSurf_s* dest)
		{
			for (int i = 0; i < data->numColSurfs; i++)
			{
				dest[i].tris = buf->write_s(3, data->colSurf[i].tris, data->colSurf[i].numCollTris);
			}
		}

		void write_lod_pc(IZone* zone, ZoneBuffer* buf, XModel* data, XModel* dest, std::size_t index)
		{
			dest->lods[index].surfaces = reinterpret_cast<XModelSurfs*>(zone->get_asset_pointer(
				xmodelsurfs, data->lods[index].surfaces->name));
		}

		void write_lod_console(IZone* zone, ZoneBuffer* buf, alpha::XModel* data, alpha::XModel* dest, std::size_t index)
		{
			
		}
		
		template <typename T> void write_model(IZone* zone, ZoneBuffer* buf, const std::string& name, T* data, bool is_console)
		{
			auto dest = buf->write(data);

			assert(sizeof XModel, 304);
			assert(sizeof alpha::XModel, 268);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(name);

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
					((Material**)destMaterials)[i] = reinterpret_cast<Material*>(zone->get_asset_pointer(
						material, data->materials[i]->name));
				}

				ZoneBuffer::ClearPointer(&dest->materials);
			}

			for (int i = 0; i < 4; i++)
			{
				if (!data->lods[i].surfaces) continue;

				if (zone->get_target() == zone_target::pc)
				{
					write_lod_pc(zone, buf, (XModel*)data, (XModel*)dest, i);
				}
				else
				{
					write_lod_console(zone, buf, (alpha::XModel*)data, (alpha::XModel*)dest, i);
				}
			}

			if (data->colSurf)
			{
				buf->align(3);
				auto destSurf = buf->write(data->colSurf, data->numColSurfs);

				if (!is_console)
				{
					write_colsurf(zone, buf, (XModel*)data, (XModelCollSurf_s*)destSurf);
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

			if (zone->get_target() == zone_target::pc)
			{
				endian_convert_xmodel(reinterpret_cast<alpha::XModel*>(dest));
			}
		}
		
		void IXModel::write(IZone* zone, ZoneBuffer* buf)
		{
#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW4::IXModel::write");
#endif

			if (zone->get_target() == zone_target::pc)
			{
				auto data = this->asset_;
				write_model<XModel>(zone, buf, this->name(), data, false);
			}
			else
			{
				write_model<alpha::XModel>(zone, buf, this->name(), nullptr, true);
			}

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
				const auto new_model_surface = new XModelSurfs;
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
					const auto iw5_model = new IW5::XModel;
					memcpy(iw5_model, new_model, sizeof XModel);
					iw5_model->unk = 0;
					
					IW5::IXModel::dump(iw5_model, convertToString);

					delete iw5_model;
					
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

			const auto iw5_model = new IW5::XModel;
			memcpy(iw5_model, asset, sizeof XModel);
			iw5_model->unk = 0;
			
			// dump regular xmodel
			IW5::IXModel::dump(iw5_model, convertToString);

			delete iw5_model;
		}
	}
}
