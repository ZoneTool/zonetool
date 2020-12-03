// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "../IW5/Assets/GfxWorld.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		/*legacy zonetool code, refactor me!*/
		GfxWorld* IGfxWorld::parse(const std::string& name, ZoneMemory* mem)
		{
			auto iw5_gfxmap = IW5::IGfxWorld::parse(name, mem);

			if (!iw5_gfxmap)
			{
				return nullptr;
			}

			// convert to IW4 format
			auto gfxmap = mem->Alloc<GfxWorld>();

			// copy struct data
			memcpy(&gfxmap->name, &iw5_gfxmap->name, Difference(&gfxmap->cells, &gfxmap->name));

			// allocate cells
			gfxmap->cells = new GfxCell[gfxmap->dpvsPlanes.cellCount];
			memset(gfxmap->cells, 0, sizeof(GfxCell) * gfxmap->dpvsPlanes.cellCount);

			// copy cell data
			for (int i = 0; i < gfxmap->dpvsPlanes.cellCount; i++)
			{
				memcpy(&gfxmap->cells[i], &iw5_gfxmap->cells[i], sizeof GfxCell);
			}

			// copy worldDraw data
			memcpy(gfxmap->worldDraw._portpad0, iw5_gfxmap->worldDraw._portpad0, 16);
			memcpy(gfxmap->worldDraw._portpad1, iw5_gfxmap->worldDraw._portpad1, 56);

			// copy remaining GfxWorld data
			memcpy(&gfxmap->lightGrid, &iw5_gfxmap->lightGrid,
			       Difference(&gfxmap->fogTypesAllowed + 1, &gfxmap->lightGrid));

			// return converted gfxmap
			return gfxmap;
		}

		IGfxWorld::IGfxWorld()
		{
		}

		IGfxWorld::~IGfxWorld()
		{
		}

		void IGfxWorld::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = "maps/"s + (currentzone.substr(0, 3) == "mp_" ? "mp/" : "") + currentzone + ".d3dbsp"; // name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), name.data()).gfxworld;
			}
		}

		void IGfxWorld::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IGfxWorld::load_depending(IZone* zone)
		{
#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW4::IGfxWorld::load_depending");
#endif

			auto data = this->asset_;
			
			// Skies
			if (data->skyCount)
			{
				for (unsigned int i = 0; i < data->skyCount; i++)
				{
					if (data->skies[i].skyImage)
					{
						zone->add_asset_of_type(image, data->skies[i].skyImage->name);
					}
				}
			}

			// ReflectionImages
			if (data->worldDraw.reflectionImages)
			{
				for (unsigned int i = 0; i < data->worldDraw.reflectionProbeCount; i++)
				{
					if (data->worldDraw.reflectionImages[i])
					{
						zone->add_asset_of_type(image, data->worldDraw.reflectionImages[i]->name);
					}
				}
			}

			// Lightmaps
			if (data->worldDraw.lightmaps)
			{
				for (int i = 0; i < data->worldDraw.lightmapCount; i++)
				{
					if (data->worldDraw.lightmaps[i].primary)
					{
						zone->add_asset_of_type(image, data->worldDraw.lightmaps[i].primary->name);
					}

					if (data->worldDraw.lightmaps[i].secondary)
					{
						zone->add_asset_of_type(image, data->worldDraw.lightmaps[i].secondary->name);
					}
				}
			}

			// SkyImage (Unused?)
			if (data->worldDraw.skyImage)
			{
				zone->add_asset_of_type(image, data->worldDraw.skyImage->name);
			}

			// OutdoorImage (Unused?)
			if (data->worldDraw.outdoorImage)
			{
				zone->add_asset_of_type(image, data->worldDraw.outdoorImage->name);
			}

			if (zone->get_target() != zone_target::pc)
			{
				return;
			}
			
			// MaterialMemory
			if (data->materialMemory)
			{
				for (int i = 0; i < data->materialMemoryCount; i++)
				{
					if (data->materialMemory[i].material)
					{
						zone->add_asset_of_type(material, data->materialMemory[i].material->name);
					}
				}
			}
			
			// Sunflare_t
			if (data->sun.spriteMaterial)
			{
				zone->add_asset_of_type(material, data->sun.spriteMaterial->name);
			}

			if (data->sun.flareMaterial)
			{
				zone->add_asset_of_type(material, data->sun.flareMaterial->name);
			}

			// OutdoorImage
			if (data->outdoorImage)
			{
				zone->add_asset_of_type(image, data->outdoorImage->name);
			}

			// Dpvs.Surfaces
			if (data->dpvs.surfaces)
			{
				for (int i = 0; i < data->indexCount; i++)
				{
					if (data->dpvs.surfaces[i].material)
					{
						zone->add_asset_of_type(material, data->dpvs.surfaces[i].material->name);
					}
				}
			}

			if (data->dpvs.smodelDrawInsts)
			{
				for (unsigned int i = 0; i < data->dpvs.smodelCount; i++)
				{
					if (data->dpvs.smodelDrawInsts[i].model)
					{
						zone->add_asset_of_type(xmodel, data->dpvs.smodelDrawInsts[i].model->name);
					}
				}
			}

#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif
		}

		std::string IGfxWorld::name()
		{
			return this->name_;
		}

		std::int32_t IGfxWorld::type()
		{
			return gfx_map;
		}

		void IGfxWorld::write(IZone* zone, ZoneBuffer* buf)
		{
#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW4::IGfxWorld::write");
#endif

			if (zone->get_target() == zone_target::pc)
			{
				auto data = this->asset_;
				auto dest = buf->write(data);

				buf->push_stream(3);
				START_LOG_STREAM;

				if (data->name)
				{
					dest->name = buf->write_str(this->name());
				}
				if (data->baseName)
				{
					dest->baseName = buf->write_str(this->name());
				}

				if (data->skies)
				{
					buf->align(3);
					auto skiesArray = buf->write(data->skies, data->skyCount);

					for (std::uint32_t i = 0; i < data->skyCount; i++)
					{
						if (data->skies[i].skyStartSurfs)
						{
							buf->align(3);
							buf->write_p(data->skies[i].skyStartSurfs, data->skies[i].skySurfCount);
							ZoneBuffer::clear_pointer(&skiesArray[i].skyStartSurfs);
						}

						if (data->skies[i].skyImage)
						{
							skiesArray[i].skyImage = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
								image, data->skies[i].skyImage->name));
						}
					}

					ZoneBuffer::clear_pointer(&dest->skies);
				}

				if (dest->dpvsPlanes.planes)
				{
					dest->dpvsPlanes.planes = buf->write_s(3, data->dpvsPlanes.planes, data->planeCount);
				}

				if (dest->dpvsPlanes.nodes)
				{
					buf->align(1);
					buf->write_p(data->dpvsPlanes.nodes, data->nodeCount);
					ZoneBuffer::clear_pointer(&dest->dpvsPlanes.nodes);
				}

				buf->push_stream(2);
				if (dest->dpvsPlanes.sceneEntCellBits)
				{
					buf->align(3);
					buf->write(data->dpvsPlanes.sceneEntCellBits, data->dpvsPlanes.cellCount << 11);
					ZoneBuffer::clear_pointer(&dest->dpvsPlanes.sceneEntCellBits);
				}
				buf->pop_stream();

				if (data->aabbTreeCounts)
				{
					buf->align(3);
					buf->write_p(data->aabbTreeCounts, data->dpvsPlanes.cellCount);
					ZoneBuffer::clear_pointer(&dest->aabbTreeCounts);
				}

				if (data->aabbTree)
				{
					buf->align(127);
					auto cell_tree = buf->write_p(data->aabbTree, data->dpvsPlanes.cellCount);

					for (std::int32_t i = 0; i < data->dpvsPlanes.cellCount; i++)
					{
						if (data->aabbTree[i].aabbtree)
						{
							buf->align(3);
							auto gfx_aabb_tree = buf->write_p(data->aabbTree[i].aabbtree,
								data->aabbTreeCounts[i].aabbTreeCount);

							for (std::int32_t i2 = 0; i2 < data->aabbTreeCounts[i].aabbTreeCount; i2++)
							{
								if (data->aabbTree[i].aabbtree[i2].smodelIndexes)
								{
									gfx_aabb_tree[i2].smodelIndexes = buf->write_s(
										1, data->aabbTree[i].aabbtree[i2].smodelIndexes,
										data->aabbTree[i].aabbtree[i2].smodelIndexCount);
								}
							}

							ZoneBuffer::clear_pointer(&cell_tree[i].aabbtree);
						}
					}

					ZoneBuffer::clear_pointer(&dest->aabbTree);
				}

				if (data->cells)
				{
					buf->align(3);
					auto gfx_cell = buf->write(data->cells, data->dpvsPlanes.cellCount);

					for (std::int32_t i = 0; i < data->dpvsPlanes.cellCount; i++)
					{
						if (data->cells[i].portals)
						{
							buf->align(3);
							auto gfx_portal = buf->write(data->cells[i].portals, data->cells[i].portalCount);

							for (std::int32_t i2 = 0; i2 < data->cells[i].portalCount; i2++)
							{
								if (data->cells[i].portals[i2].vertices)
								{
									buf->align(3);
									buf->write(data->cells[i].portals[i2].vertices, data->cells[i].portals[i2].vertexCount);
									ZoneBuffer::clear_pointer(&gfx_portal[i2].vertices);
								}
							}

							ZoneBuffer::clear_pointer(&gfx_cell[i].portals);
						}

						if (data->cells[i].reflectionProbes)
						{
							buf->align(0);
							buf->write(data->cells[i].reflectionProbes, data->cells[i].reflectionProbeCount);
							ZoneBuffer::clear_pointer(&gfx_cell[i].reflectionProbes);
						}

						ZoneBuffer::clear_pointer(&dest->cells);
					}
				}

				if (data->worldDraw.reflectionImages)
				{
					buf->align(3);
					auto reflectionProbes = buf->write(data->worldDraw.reflectionImages,
						data->worldDraw.reflectionProbeCount);

					for (std::uint64_t i = 0; i < data->worldDraw.reflectionProbeCount; i++)
					{
						if (reflectionProbes[i])
						{
							reflectionProbes[i] = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
								image, data->worldDraw.reflectionImages[i]->name));
						}
					}

					ZoneBuffer::clear_pointer(&dest->worldDraw.reflectionImages);
				}

				if (data->worldDraw.reflectionProbes)
				{
					buf->align(3);
					buf->write(data->worldDraw.reflectionProbes, data->worldDraw.reflectionProbeCount);
					ZoneBuffer::clear_pointer(&dest->worldDraw.reflectionProbes);
				}

				buf->push_stream(2);
				if (data->worldDraw.reflectionProbeTextures)
				{
					buf->align(3);
					buf->write(data->worldDraw.reflectionProbeTextures, data->worldDraw.reflectionProbeCount);
					ZoneBuffer::clear_pointer(&dest->worldDraw.reflectionProbeTextures);
				}
				buf->pop_stream();

				if (data->worldDraw.lightmaps)
				{
					buf->align(3);
					auto gfx_lightmap_array = buf->write(data->worldDraw.lightmaps, data->worldDraw.lightmapCount);

					for (std::int32_t i = 0; i < data->worldDraw.lightmapCount; i++)
					{
						if (data->worldDraw.lightmaps[i].primary)
						{
							gfx_lightmap_array[i].primary = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
								image, data->worldDraw.lightmaps[i].primary->name));
						}

						if (data->worldDraw.lightmaps[i].secondary)
						{
							gfx_lightmap_array[i].secondary = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
								image, data->worldDraw.lightmaps[i].secondary->name));
						}
					}

					ZoneBuffer::clear_pointer(&dest->worldDraw.lightmaps);
				}

				buf->push_stream(2);
				if (data->worldDraw.lightmapPrimaryTextures)
				{
					buf->align(3);
					buf->write_p(data->worldDraw.lightmapPrimaryTextures, data->worldDraw.lightmapCount);
					ZoneBuffer::clear_pointer(&dest->worldDraw.lightmapPrimaryTextures);
				}

				if (data->worldDraw.lightmapSecondaryTextures)
				{
					buf->align(3);
					buf->write_p(data->worldDraw.lightmapSecondaryTextures, data->worldDraw.lightmapCount);
					ZoneBuffer::clear_pointer(&dest->worldDraw.lightmapSecondaryTextures);
				}
				buf->pop_stream();

				if (data->worldDraw.skyImage)
				{
					dest->worldDraw.skyImage = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
						image, data->worldDraw.skyImage->name));
				}

				if (data->worldDraw.outdoorImage)
				{
					dest->worldDraw.outdoorImage = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
						image, data->worldDraw.outdoorImage->name));
				}

				if (data->worldDraw.vd.vertices)
				{
					buf->align(3);
					buf->write_p(data->worldDraw.vd.vertices, data->worldDraw.vertexCount);
					ZoneBuffer::clear_pointer(&dest->worldDraw.vd.vertices);
				}

				if (data->worldDraw.vld.data)
				{
					buf->align(0);
					buf->write_p(data->worldDraw.vld.data, data->worldDraw.vertexLayerDataSize);
					ZoneBuffer::clear_pointer(&dest->worldDraw.vld.data);
				}

				if (data->worldDraw.indices)
				{
					buf->align(1);
					buf->write_p(data->worldDraw.indices, data->worldDraw.indexCount);
					ZoneBuffer::clear_pointer(&dest->worldDraw.indices);
				}

				if (data->lightGrid.rowDataStart)
				{
					buf->align(1);
					buf->write_p(data->lightGrid.rowDataStart,
						data->lightGrid.maxs[data->lightGrid.rowAxis] - data->lightGrid.mins[data
						->lightGrid.rowAxis] +
						1);
					ZoneBuffer::clear_pointer(&dest->lightGrid.rowDataStart);
				}

				if (data->lightGrid.rawRowData)
				{
					buf->align(0);
					buf->write_p(data->lightGrid.rawRowData, data->lightGrid.rawRowDataSize);
					ZoneBuffer::clear_pointer(&dest->lightGrid.rawRowData);
				}

				if (data->lightGrid.entries)
				{
					buf->align(3);
					buf->write(data->lightGrid.entries, data->lightGrid.entryCount);
					ZoneBuffer::clear_pointer(&dest->lightGrid.entries);
				}

				if (data->lightGrid.colors)
				{
					buf->align(3);
					buf->write(data->lightGrid.colors, data->lightGrid.colorCount);
					ZoneBuffer::clear_pointer(&dest->lightGrid.colors);
				}

				if (data->models)
				{
					buf->align(3);
					buf->write(data->models, data->modelCount);
					ZoneBuffer::clear_pointer(&dest->models);
				}

				if (data->materialMemory)
				{
					buf->align(3);
					auto memory = buf->write(data->materialMemory, data->materialMemoryCount);

					for (std::int32_t i = 0; i < data->materialMemoryCount; i++)
					{
						memory[i].material = reinterpret_cast<Material*>(zone->get_asset_pointer(
							material, data->materialMemory[i].material->name));
					}

					ZoneBuffer::clear_pointer(&dest->materialMemory);
				}

				if (data->sun.spriteMaterial)
				{
					dest->sun.spriteMaterial = reinterpret_cast<Material*>(zone->get_asset_pointer(
						material, data->sun.spriteMaterial->name));
				}
				if (data->sun.flareMaterial)
				{
					dest->sun.flareMaterial = reinterpret_cast<Material*>(zone->get_asset_pointer(
						material, data->sun.flareMaterial->name));
				}

				if (data->outdoorImage)
				{
					dest->outdoorImage = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(image, data->outdoorImage->name)
						);
				}

				buf->push_stream(2);
				if (data->cellCasterBits[0])
				{
					buf->align(3);
					buf->write(data->cellCasterBits[0],
						data->dpvsPlanes.cellCount * ((data->dpvsPlanes.cellCount + 31) >> 5));
					ZoneBuffer::clear_pointer(&dest->cellCasterBits[0]);
				}

				if (data->cellCasterBits[1])
				{
					buf->align(3);
					buf->write(data->cellCasterBits[1], (data->dpvsPlanes.cellCount + 31) >> 5);
					ZoneBuffer::clear_pointer(&dest->cellCasterBits[1]);
				}

				if (data->sceneDynModel)
				{
					buf->align(3);
					buf->write(data->sceneDynModel, data->dpvsDyn.dynEntClientCount[0]);
					ZoneBuffer::clear_pointer(&dest->sceneDynModel);
				}

				if (data->sceneDynBrush)
				{
					buf->align(3);
					buf->write(data->sceneDynBrush, data->dpvsDyn.dynEntClientCount[1]);
					ZoneBuffer::clear_pointer(&dest->sceneDynBrush);
				}

				if (data->primaryLightEntityShadowVis)
				{
					buf->align(3);
					buf->write(data->primaryLightEntityShadowVis,
						(data->primaryLightCount - data->sunPrimaryLightIndex - 1) << 15);
					ZoneBuffer::clear_pointer(&dest->primaryLightEntityShadowVis);
				}

				if (data->primaryLightDynEntShadowVis[0])
				{
					buf->align(3);
					buf->write(data->primaryLightDynEntShadowVis[0],
						data->dpvsDyn.dynEntClientCount[0] * (data->primaryLightCount - data->sunPrimaryLightIndex -
							1));
					ZoneBuffer::clear_pointer(&dest->primaryLightDynEntShadowVis[0]);
				}

				if (data->primaryLightDynEntShadowVis[1])
				{
					buf->align(3);
					buf->write(data->primaryLightDynEntShadowVis[1],
						data->dpvsDyn.dynEntClientCount[1] * (data->primaryLightCount - data->sunPrimaryLightIndex -
							1));
					ZoneBuffer::clear_pointer(&dest->primaryLightDynEntShadowVis[1]);
				}

				if (data->primaryLightForModelDynEnt)
				{
					buf->align(0);
					buf->write(data->primaryLightForModelDynEnt, data->dpvsDyn.dynEntClientCount[0]);
					ZoneBuffer::clear_pointer(&dest->primaryLightForModelDynEnt);
				}
				buf->pop_stream();

				if (data->shadowGeom)
				{
					buf->align(3);
					auto shadow_geometry = buf->write(data->shadowGeom, data->primaryLightCount);

					for (std::int32_t i = 0; i < data->primaryLightCount; i++)
					{
						if (data->shadowGeom[i].sortedSurfIndex)
						{
							buf->align(1);
							buf->write_p(data->shadowGeom[i].sortedSurfIndex, data->shadowGeom[i].surfaceCount);
							ZoneBuffer::clear_pointer(&shadow_geometry[i].sortedSurfIndex);
						}
						if (data->shadowGeom[i].smodelIndex)
						{
							buf->align(1);
							buf->write_p(data->shadowGeom[i].smodelIndex, data->shadowGeom[i].smodelCount);
							ZoneBuffer::clear_pointer(&shadow_geometry[i].smodelIndex);
						}
					}

					ZoneBuffer::clear_pointer(&dest->shadowGeom);
				}

				if (data->lightRegion)
				{
					buf->align(3);
					auto light_region = buf->write(data->lightRegion, data->primaryLightCount);

					for (std::int32_t i = 0; i < data->primaryLightCount; i++)
					{
						if (data->lightRegion[i].hulls)
						{
							buf->align(3);
							auto light_region_hull = buf->write(data->lightRegion[i].hulls, data->lightRegion[i].hullCount);

							for (std::uint32_t i2 = 0; i2 < data->lightRegion[i].hullCount; i2++)
							{
								if (data->lightRegion[i].hulls[i2].axis)
								{
									buf->align(3);
									buf->write(data->lightRegion[i].hulls[i2].axis,
										data->lightRegion[i].hulls[i2].axisCount);
									ZoneBuffer::clear_pointer(&light_region_hull[i2].axis);
								}
							}

							ZoneBuffer::clear_pointer(&light_region[i].hulls);
						}
					}

					ZoneBuffer::clear_pointer(&dest->lightRegion);
				}

				buf->push_stream(2);
				if (data->dpvs.smodelVisData[0])
				{
					buf->align(0);
					buf->write(data->dpvs.smodelVisData[0], data->dpvs.smodelCount);
					ZoneBuffer::clear_pointer(&dest->dpvs.smodelVisData[0]);
				}

				if (data->dpvs.smodelVisData[1])
				{
					buf->align(0);
					buf->write(data->dpvs.smodelVisData[1], data->dpvs.smodelCount);
					ZoneBuffer::clear_pointer(&dest->dpvs.smodelVisData[1]);
				}

				if (data->dpvs.smodelVisData[2])
				{
					buf->align(0);
					buf->write(data->dpvs.smodelVisData[2], data->dpvs.smodelCount);
					ZoneBuffer::clear_pointer(&dest->dpvs.smodelVisData[2]);
				}

				if (data->dpvs.surfaceVisData[0])
				{
					buf->align(0);
					buf->write(data->dpvs.surfaceVisData[0], data->dpvs.staticSurfaceCount);
					ZoneBuffer::clear_pointer(&dest->dpvs.surfaceVisData[0]);
				}

				if (data->dpvs.surfaceVisData[1])
				{
					buf->align(0);
					buf->write(data->dpvs.surfaceVisData[1], data->dpvs.staticSurfaceCount);
					ZoneBuffer::clear_pointer(&dest->dpvs.surfaceVisData[1]);
				}

				if (data->dpvs.surfaceVisData[2])
				{
					buf->align(0);
					buf->write(data->dpvs.surfaceVisData[2], data->dpvs.staticSurfaceCount);
					ZoneBuffer::clear_pointer(&dest->dpvs.surfaceVisData[2]);
				}
				buf->pop_stream();

				if (data->dpvs.sortedSurfIndex)
				{
					buf->align(1);
					buf->write_p(data->dpvs.sortedSurfIndex,
						data->dpvs.staticSurfaceCount + data->dpvs.staticSurfaceCountNoDecal);
					ZoneBuffer::clear_pointer(&dest->dpvs.sortedSurfIndex);
				}

				if (data->dpvs.smodelInsts)
				{
					buf->align(3);
					buf->write(data->dpvs.smodelInsts, data->dpvs.smodelCount);
					ZoneBuffer::clear_pointer(&dest->dpvs.smodelInsts);
				}

				if (data->dpvs.surfaces)
				{
					buf->align(3);
					auto surface = buf->write(data->dpvs.surfaces, data->indexCount);

					for (std::int32_t i = 0; i < data->indexCount; i++)
					{
						if (data->dpvs.surfaces[i].material)
						{
							surface[i].material = reinterpret_cast<Material*>(zone->get_asset_pointer(
								material, data->dpvs.surfaces[i].material->name));
						}
					}

					ZoneBuffer::clear_pointer(&dest->dpvs.surfaces);
				}

				if (data->dpvs.cullGroups)
				{
					buf->align(3);
					buf->write(data->dpvs.cullGroups, data->indexCount);
					ZoneBuffer::clear_pointer(&dest->dpvs.cullGroups);
				}

				if (data->dpvs.smodelDrawInsts)
				{
					buf->align(3);
					auto static_model_draw_inst = buf->write(data->dpvs.smodelDrawInsts, data->dpvs.smodelCount);

					for (std::uint32_t i = 0; i < data->dpvs.smodelCount; i++)
					{
						if (data->dpvs.smodelDrawInsts[i].model)
						{
							static_model_draw_inst[i].model = reinterpret_cast<XModel*>(zone->get_asset_pointer(
								xmodel, data->dpvs.smodelDrawInsts[i].model->name));
						}
					}

					ZoneBuffer::clear_pointer(&dest->dpvs.smodelDrawInsts);
				}

				buf->push_stream(2);
				if (data->dpvs.surfaceMaterials)
				{
					buf->align(7);
					buf->write(data->dpvs.surfaceMaterials, data->indexCount);
					ZoneBuffer::clear_pointer(&dest->dpvs.smodelDrawInsts);
				}

				if (data->dpvs.surfaceCastsSunShadow)
				{
					buf->align(127);
					buf->write(data->dpvs.surfaceCastsSunShadow, data->dpvs.surfaceVisDataCount);
					ZoneBuffer::clear_pointer(&dest->dpvs.surfaceCastsSunShadow);
				}

				if (data->dpvsDyn.dynEntCellBits[0])
				{
					buf->align(3);
					buf->write(data->dpvsDyn.dynEntCellBits[0],
						data->dpvsDyn.dynEntClientWordCount[0] * data->dpvsPlanes.cellCount);
					ZoneBuffer::clear_pointer(&dest->dpvsDyn.dynEntCellBits[0]);
				}

				if (data->dpvsDyn.dynEntCellBits[1])
				{
					buf->align(3);
					buf->write(data->dpvsDyn.dynEntCellBits[1],
						data->dpvsDyn.dynEntClientWordCount[1] * data->dpvsPlanes.cellCount);
					ZoneBuffer::clear_pointer(&dest->dpvsDyn.dynEntCellBits[1]);
				}

				if (data->dpvsDyn.dynEntVisData[0][0])
				{
					buf->align(15);
					buf->write(data->dpvsDyn.dynEntVisData[0][0], 32 * data->dpvsDyn.dynEntClientWordCount[0]);
					ZoneBuffer::clear_pointer(&dest->dpvsDyn.dynEntVisData[0][0]);
				}

				if (data->dpvsDyn.dynEntVisData[1][0])
				{
					buf->align(15);
					buf->write(data->dpvsDyn.dynEntVisData[1][0], 32 * data->dpvsDyn.dynEntClientWordCount[1]);
					ZoneBuffer::clear_pointer(&dest->dpvsDyn.dynEntVisData[1][0]);
				}

				if (data->dpvsDyn.dynEntVisData[0][1])
				{
					buf->align(15);
					buf->write(data->dpvsDyn.dynEntVisData[0][1], 32 * data->dpvsDyn.dynEntClientWordCount[0]);
					ZoneBuffer::clear_pointer(&dest->dpvsDyn.dynEntVisData[0][1]);
				}

				if (data->dpvsDyn.dynEntVisData[1][1])
				{
					buf->align(15);
					buf->write(data->dpvsDyn.dynEntVisData[1][1], 32 * data->dpvsDyn.dynEntClientWordCount[1]);
					ZoneBuffer::clear_pointer(&dest->dpvsDyn.dynEntVisData[1][1]);
				}

				if (data->dpvsDyn.dynEntVisData[0][2])
				{
					buf->align(15);
					buf->write(data->dpvsDyn.dynEntVisData[0][2], 32 * data->dpvsDyn.dynEntClientWordCount[0]);
					ZoneBuffer::clear_pointer(&dest->dpvsDyn.dynEntVisData[0][2]);
				}

				if (data->dpvsDyn.dynEntVisData[1][2])
				{
					buf->align(15);
					buf->write(data->dpvsDyn.dynEntVisData[1][2], 32 * data->dpvsDyn.dynEntClientWordCount[1]);
					ZoneBuffer::clear_pointer(&dest->dpvsDyn.dynEntVisData[1][2]);
				}
				buf->pop_stream();

				if (data->heroLights)
				{
					buf->align(3);
					buf->write(data->heroLights, data->heroLightCount);
					ZoneBuffer::clear_pointer(&dest->heroLights);
				}

				END_LOG_STREAM;
				buf->pop_stream();
			}
			else
			{
				// convert GfxWorld to alpha format
				alpha::GfxWorld alpha_world[2] = {};
				memset(alpha_world, 0, sizeof alpha_world);
				memcpy(alpha_world, this->asset_, sizeof GfxWorld);
				alpha_world->draw.vd.vertices = this->asset_->worldDraw.vd.vertices;
				alpha_world->draw.vertexLayerDataSize = this->asset_->worldDraw.vertexLayerDataSize;
				alpha_world->draw.vld.data = this->asset_->worldDraw.vld.data;
				alpha_world->draw.indexCount = this->asset_->worldDraw.indexCount;
				alpha_world->draw.indices = this->asset_->worldDraw.indices;
				memset(&alpha_world->draw.vd.worldVb, 0, sizeof alpha::D3DVertexBuffer);
				memset(&alpha_world->draw.vld.layerVb, 0, sizeof alpha::D3DVertexBuffer);
				memset(&alpha_world->draw.indexBuffer, 0, sizeof alpha::D3DIndexBuffer);
				memcpy(&alpha_world->lightGrid, &this->asset_->lightGrid, sizeof GfxWorld);
				memcpy(&alpha_world->dpvs.litOpaqueSurfsBegin, &this->asset_->dpvs.litOpaqueSurfsBegin, 100);
				memcpy(&alpha_world->dpvsDyn, &this->asset_->dpvsDyn, 70);



				//// DEBUGGING PURPOSES: CHECK WHATS BROKEN!
				//alpha::GfxWorldDraw tempDraw = {};
				//memcpy(&tempDraw, &alpha_world->draw, sizeof alpha::GfxWorldDraw);

				//// clear gfx world
				// memset(&alpha_world->dpvsPlanes, 0, sizeof alpha::GfxWorld);
				//alpha_world->aabbTreeCounts = nullptr;
				//alpha_world->aabbTrees = nullptr;
				alpha_world->cells = nullptr;

				//// restore draw data
				//memcpy(&alpha_world->draw, &tempDraw, sizeof alpha::GfxWorldDraw);

				// these need to be fixed
				alpha_world->draw.reflectionProbeTextures = 0;
				alpha_world->draw.lightmapPrimaryTextures = 0;
				alpha_world->draw.lightmapSecondaryTextures = 0;
								
				auto data = &alpha_world[0];
				auto dest = buf->write(data);

				buf->push_stream(3);
				START_LOG_STREAM;

				if (data->name)
				{
					dest->name = buf->write_str(this->name());
				}
				if (data->baseName)
				{
					dest->baseName = buf->write_str(this->name());
				}

				if (data->skies)
				{
					buf->align(3);
					auto skiesArray = buf->write(data->skies, data->skyCount);

					for (std::uint32_t i = 0; i < data->skyCount; i++)
					{
						if (data->skies[i].skyStartSurfs)
						{
							buf->align(3);
							auto destSkyStartSurfs = buf->write_p(data->skies[i].skyStartSurfs, data->skies[i].skySurfCount);
							ZoneBuffer::clear_pointer(&skiesArray[i].skyStartSurfs);

							for (auto i2 = 0; i2 < data->skies[i].skySurfCount; i2++)
							{
								endian_convert(&destSkyStartSurfs[i2]);
							}
						}

						if (data->skies[i].skyImage)
						{
							skiesArray[i].skyImage = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
								image, data->skies[i].skyImage->name));
						}

						endian_convert(&skiesArray[i].skySurfCount);
						endian_convert(&skiesArray[i].skyStartSurfs);
						endian_convert(&skiesArray[i].skyImage);
					}

					ZoneBuffer::clear_pointer(&dest->skies);
				}

				if (dest->dpvsPlanes.planes)
				{
					cplane_s* destCplanes = nullptr;
					dest->dpvsPlanes.planes = buf->write_s(3, data->dpvsPlanes.planes, data->planeCount, sizeof cplane_s, &destCplanes);

					if (dest->dpvsPlanes.planes == reinterpret_cast<cplane_s*>(-1))
					{
						for (int i = 0; i < data->planeCount; i++)
						{
							endian_convert(&destCplanes[i].normal[0]);
							endian_convert(&destCplanes[i].normal[1]);
							endian_convert(&destCplanes[i].normal[2]);
							endian_convert(&destCplanes[i].dist);
						}
					}
				}

				if (dest->dpvsPlanes.nodes)
				{
					buf->align(1);
					auto destNodes = buf->write_p(data->dpvsPlanes.nodes, data->nodeCount);
					ZoneBuffer::clear_pointer(&dest->dpvsPlanes.nodes);

					for (int i = 0; i < data->nodeCount; i++)
					{
						endian_convert(&destNodes[i]);
					}
				}

				buf->push_stream(2);
				if (dest->dpvsPlanes.sceneEntCellBits)
				{
					buf->align(3);
					buf->write(data->dpvsPlanes.sceneEntCellBits, data->dpvsPlanes.cellCount << 11);
					ZoneBuffer::clear_pointer(&dest->dpvsPlanes.sceneEntCellBits);
				}
				buf->pop_stream();

				if (data->aabbTreeCounts)
				{
					buf->align(3);
					auto destTrees = buf->write_p(data->aabbTreeCounts, data->dpvsPlanes.cellCount);
					ZoneBuffer::clear_pointer(&dest->aabbTreeCounts);

					for (int i = 0; i < data->dpvsPlanes.cellCount; i++)
					{
						endian_convert(&destTrees[i].aabbTreeCount);
					}
				}

				if (data->aabbTrees)
				{
					buf->align(127);
					auto cell_tree = buf->write_p(data->aabbTrees, data->dpvsPlanes.cellCount);

					for (std::int32_t i = 0; i < data->dpvsPlanes.cellCount; i++)
					{
						if (data->aabbTrees[i].aabbtree)
						{
							buf->align(3);
							auto gfx_aabb_tree = buf->write_p(data->aabbTrees[i].aabbtree,
								data->aabbTreeCounts[i].aabbTreeCount);

							for (std::int32_t i2 = 0; i2 < data->aabbTreeCounts[i].aabbTreeCount; i2++)
							{
								if (data->aabbTrees[i].aabbtree[i2].smodelIndexes)
								{
									unsigned short* destSmodelIndexes = nullptr;
									
									gfx_aabb_tree[i2].smodelIndexes = buf->write_s(
										1, data->aabbTrees[i].aabbtree[i2].smodelIndexes,
										data->aabbTrees[i].aabbtree[i2].smodelIndexCount, sizeof (unsigned short), &destSmodelIndexes);

									if (gfx_aabb_tree[i2].smodelIndexes == reinterpret_cast<unsigned short*>(-1))
									{
										for (auto i3 = 0; i3 < data->aabbTrees[i].aabbtree[i2].smodelIndexCount; i3++)
										{
											endian_convert(&destSmodelIndexes[i3]);
										}
									}
								}

								endian_convert(&gfx_aabb_tree[i2].bounds.halfSize[0]);
								endian_convert(&gfx_aabb_tree[i2].bounds.halfSize[1]);
								endian_convert(&gfx_aabb_tree[i2].bounds.halfSize[2]);
								endian_convert(&gfx_aabb_tree[i2].bounds.midPoint[0]);
								endian_convert(&gfx_aabb_tree[i2].bounds.midPoint[1]);
								endian_convert(&gfx_aabb_tree[i2].bounds.midPoint[2]);
								endian_convert(&gfx_aabb_tree[i2].unkn);
								endian_convert(&gfx_aabb_tree[i2].childCount);
								endian_convert(&gfx_aabb_tree[i2].surfaceCount);
								endian_convert(&gfx_aabb_tree[i2].startSurfIndex);
								endian_convert(&gfx_aabb_tree[i2].smodelIndexCount);
								endian_convert(&gfx_aabb_tree[i2].smodelIndexes);
								endian_convert(&gfx_aabb_tree[i2].childrenOffset);
							}

							ZoneBuffer::clear_pointer(&cell_tree[i].aabbtree);
						}

						endian_convert(&cell_tree[i].aabbtree);
					}

					ZoneBuffer::clear_pointer(&dest->aabbTrees);
				}

				if (data->cells)
				{
					buf->align(3);
					auto gfx_cell = buf->write(data->cells, data->dpvsPlanes.cellCount);

					for (std::int32_t i = 0; i < data->dpvsPlanes.cellCount; i++)
					{
						if (data->cells[i].portals)
						{
							buf->align(3);
							auto gfx_portal = buf->write(data->cells[i].portals, data->cells[i].portalCount);
							
							for (std::int32_t i2 = 0; i2 < data->cells[i].portalCount; i2++)
							{
								gfx_portal[i2].writable.queuedParent = nullptr;
								gfx_portal[i2].writable.hullPointCount = 0;
								gfx_portal[i2].writable.hullPoints = nullptr;
								
								if (data->cells[i].portals[i2].vertices)
								{
									buf->align(3);
									auto destVertices = buf->write(data->cells[i].portals[i2].vertices, data->cells[i].portals[i2].vertexCount);
									ZoneBuffer::clear_pointer(&gfx_portal[i2].vertices);

									for (auto i3 = 0; i3 < data->cells[i].portals[i2].vertexCount; i3++)
									{
										endian_convert(&destVertices[i3][0]);
										endian_convert(&destVertices[i3][1]);
										endian_convert(&destVertices[i3][2]);
									}
								}

								endian_convert(&gfx_portal[i2].vertices);
								endian_convert(&gfx_portal[i2].plane.coeffs[0]);
								endian_convert(&gfx_portal[i2].plane.coeffs[1]);
								endian_convert(&gfx_portal[i2].plane.coeffs[2]);
								endian_convert(&gfx_portal[i2].plane.coeffs[3]);
								endian_convert(&gfx_portal[i2].cellIndex);

								for (auto a = 0; a < 2; a++)
								{
									for (auto b = 0; b < 3; b++)
									{
										endian_convert(&gfx_portal[i2].hullAxis[a][b]);
									}
								}
							}

							ZoneBuffer::clear_pointer(&gfx_cell[i].portals);
						}

						if (data->cells[i].reflectionProbes)
						{
							buf->align(0);
							buf->write(data->cells[i].reflectionProbes, data->cells[i].reflectionProbeCount);
							ZoneBuffer::clear_pointer(&gfx_cell[i].reflectionProbes);
						}
						
						ZoneBuffer::clear_pointer(&dest->cells);

						endian_convert(&gfx_cell[i].bounds.halfSize[0]);
						endian_convert(&gfx_cell[i].bounds.halfSize[1]);
						endian_convert(&gfx_cell[i].bounds.halfSize[2]);
						endian_convert(&gfx_cell[i].bounds.midPoint[0]);
						endian_convert(&gfx_cell[i].bounds.midPoint[1]);
						endian_convert(&gfx_cell[i].bounds.midPoint[2]);
						endian_convert(&gfx_cell[i].portalCount);
						endian_convert(&gfx_cell[i].portals);
					}
				}

				if (data->draw.reflectionProbes)
				{
					buf->align(3);
					auto reflectionProbes = buf->write(data->draw.reflectionProbes, data->draw.reflectionProbeCount);

					for (std::uint64_t i = 0; i < data->draw.reflectionProbeCount; i++)
					{
						if (reflectionProbes[i])
						{
							reflectionProbes[i] = reinterpret_cast<alpha::GfxImage*>(zone->get_asset_pointer(
								image, reinterpret_cast<GfxImage*>(data->draw.reflectionProbes[i])->name));
						}

						endian_convert(&reflectionProbes[i]);
					}

					ZoneBuffer::clear_pointer(&dest->draw.reflectionProbes);
				}

				if (data->draw.reflectionProbeOrigins)
				{
					buf->align(3);
					auto destReflectionProbes = buf->write(data->draw.reflectionProbeOrigins, data->draw.reflectionProbeCount);
					ZoneBuffer::clear_pointer(&dest->draw.reflectionProbeOrigins);

					for (auto i = 0; i < data->draw.reflectionProbeCount; i++)
					{
						endian_convert(&destReflectionProbes[i].offset[0]);
						endian_convert(&destReflectionProbes[i].offset[1]);
						endian_convert(&destReflectionProbes[i].offset[2]);
					}
				}

				buf->push_stream(2);
				if (data->draw.reflectionProbeTextures)
				{
					buf->align(3);
					buf->write(data->draw.reflectionProbeTextures, data->draw.reflectionProbeCount);
					ZoneBuffer::clear_pointer(&dest->draw.reflectionProbeTextures);
				}
				buf->pop_stream();

				if (data->draw.lightmaps)
				{
					buf->align(3);
					auto gfx_lightmap_array = buf->write(data->draw.lightmaps, data->draw.lightmapCount);

					for (std::int32_t i = 0; i < data->draw.lightmapCount; i++)
					{
						if (data->draw.lightmaps[i].primary)
						{
							gfx_lightmap_array[i].primary = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
								image, data->draw.lightmaps[i].primary->name));
						}

						if (data->draw.lightmaps[i].secondary)
						{
							gfx_lightmap_array[i].secondary = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
								image, data->draw.lightmaps[i].secondary->name));
						}

						endian_convert(&gfx_lightmap_array[i].primary);
						endian_convert(&gfx_lightmap_array[i].secondary);
					}

					ZoneBuffer::clear_pointer(&dest->draw.lightmaps);
				}

				buf->push_stream(2);
				if (data->draw.lightmapPrimaryTextures)
				{
					buf->align(3);
					buf->write_p(data->draw.lightmapPrimaryTextures, data->draw.lightmapCount);
					ZoneBuffer::clear_pointer(&dest->draw.lightmapPrimaryTextures);
				}

				if (data->draw.lightmapSecondaryTextures)
				{
					buf->align(3);
					buf->write_p(data->draw.lightmapSecondaryTextures, data->draw.lightmapCount);
					ZoneBuffer::clear_pointer(&dest->draw.lightmapSecondaryTextures);
				}
				buf->pop_stream();

				if (data->draw.lightmapOverridePrimary)
				{
					dest->draw.lightmapOverridePrimary = reinterpret_cast<alpha::GfxImage*>(zone->get_asset_pointer(
						image, reinterpret_cast<GfxImage*>(data->draw.lightmapOverridePrimary)->name));
				}

				if (data->draw.lightmapOverrideSecondary)
				{
					dest->draw.lightmapOverrideSecondary = reinterpret_cast<alpha::GfxImage*>(zone->get_asset_pointer(
						image, reinterpret_cast<GfxImage*>(data->draw.lightmapOverrideSecondary)->name));
				}

				if (data->draw.vd.vertices)
				{
					buf->align(3);
					auto destVertices = buf->write_p(data->draw.vd.vertices, data->draw.vertexCount);
					ZoneBuffer::clear_pointer(&dest->draw.vd.vertices);

					for (auto i = 0u; i < data->draw.vertexCount; i++)
					{
						endian_convert(&destVertices[i].xyz[0]);
						endian_convert(&destVertices[i].xyz[1]);
						endian_convert(&destVertices[i].xyz[2]);
						endian_convert(&destVertices[i].binormalSign);
						endian_convert(&destVertices[i].color.packed);
						endian_convert(&destVertices[i].texCoord[0]);
						endian_convert(&destVertices[i].texCoord[1]);
						endian_convert(&destVertices[i].lmapCoord[0]);
						endian_convert(&destVertices[i].lmapCoord[1]);
						endian_convert(&destVertices[i].normal.packed);
						endian_convert(&destVertices[i].tangent.packed);
					}
				}

				if (data->draw.vld.data)
				{
					buf->align(0);
					buf->write_p(data->draw.vld.data, data->draw.vertexLayerDataSize);
					ZoneBuffer::clear_pointer(&dest->draw.vld.data);
				}

				if (data->draw.indices)
				{
					buf->align(1);
					auto destIndices = buf->write_p(data->draw.indices, data->draw.indexCount);
					ZoneBuffer::clear_pointer(&dest->draw.indices);

					for (auto i = 0; i < data->draw.indexCount; i++)
					{
						endian_convert(&destIndices[i]);
					}
				}

				if (data->lightGrid.rowDataStart)
				{
					buf->align(1);
					auto destRowData = buf->write_p(data->lightGrid.rowDataStart,
						data->lightGrid.maxs[data->lightGrid.rowAxis] - data->lightGrid.mins[data
						->lightGrid.rowAxis] +
						1);
					ZoneBuffer::clear_pointer(&dest->lightGrid.rowDataStart);

					for (auto i = 0u; i < data->lightGrid.maxs[data->lightGrid.rowAxis] - data->lightGrid.mins[data->lightGrid.rowAxis] + 1; i++)
					{
						endian_convert(&destRowData[i]);
					}
				}

				if (data->lightGrid.rawRowData)
				{
					buf->align(0);
					buf->write_p(data->lightGrid.rawRowData, data->lightGrid.rawRowDataSize);
					ZoneBuffer::clear_pointer(&dest->lightGrid.rawRowData);
				}

				if (data->lightGrid.entries)
				{
					buf->align(3);
					auto destEntries = buf->write(data->lightGrid.entries, data->lightGrid.entryCount);
					ZoneBuffer::clear_pointer(&dest->lightGrid.entries);

					for (auto i = 0u; i < data->lightGrid.entryCount; i++)
					{
						endian_convert(&destEntries[i].colorsIndex);
					}
				}

				if (data->lightGrid.colors)
				{
					buf->align(3);
					buf->write(data->lightGrid.colors, data->lightGrid.colorCount);
					ZoneBuffer::clear_pointer(&dest->lightGrid.colors);
				}

				if (data->models)
				{
					buf->align(3);
					buf->write(data->models, data->modelCount);
					ZoneBuffer::clear_pointer(&dest->models);
				}

				if (data->materialMemory)
				{
					buf->align(3);
					auto memory = buf->write(data->materialMemory, data->materialMemoryCount);

					for (std::int32_t i = 0; i < data->materialMemoryCount; i++)
					{
						memory[i].material = reinterpret_cast<Material*>(zone->get_asset_pointer(
							material, data->materialMemory[i].material->name));

						endian_convert(&memory[i].material);
						endian_convert(&memory[i].memory);
					}

					ZoneBuffer::clear_pointer(&dest->materialMemory);
				}

				if (data->sun.spriteMaterial)
				{
					dest->sun.spriteMaterial = reinterpret_cast<Material*>(zone->get_asset_pointer(
						material, data->sun.spriteMaterial->name));
				}
				if (data->sun.flareMaterial)
				{
					dest->sun.flareMaterial = reinterpret_cast<Material*>(zone->get_asset_pointer(
						material, data->sun.flareMaterial->name));
				}

				if (data->outdoorImage)
				{
					dest->outdoorImage = reinterpret_cast<alpha::GfxImage*>(zone->get_asset_pointer(image, reinterpret_cast<GfxImage*>(data->outdoorImage)->name)
						);
				}

				buf->push_stream(2);
				if (data->cellCasterBits)
				{
					buf->align(3);
					auto destCellCasterBits = buf->write(data->cellCasterBits,
						data->dpvsPlanes.cellCount * ((data->dpvsPlanes.cellCount + 31) >> 5));
					ZoneBuffer::clear_pointer(&dest->cellCasterBits);

					for (auto i = 0; i < data->dpvsPlanes.cellCount * ((data->dpvsPlanes.cellCount + 31) >> 5); i++)
					{
						endian_convert(&destCellCasterBits[i]);
					}
				}

				if (data->cellHasSunLitSurfsBits)
				{
					buf->align(3);
					auto destCellHasSunLitSurfsBits = buf->write(data->cellHasSunLitSurfsBits, (data->dpvsPlanes.cellCount + 31) >> 5);
					ZoneBuffer::clear_pointer(&dest->cellHasSunLitSurfsBits);

					for (auto i = 0; i < (data->dpvsPlanes.cellCount + 31) >> 5; i++)
					{
						endian_convert(&destCellHasSunLitSurfsBits[i]);
					}
				}

				if (data->sceneDynModel)
				{
					buf->align(3);
					buf->write(data->sceneDynModel, data->dpvsDyn.dynEntClientCount[0]);
					ZoneBuffer::clear_pointer(&dest->sceneDynModel);
				}

				if (data->sceneDynBrush)
				{
					buf->align(3);
					buf->write(data->sceneDynBrush, data->dpvsDyn.dynEntClientCount[1]);
					ZoneBuffer::clear_pointer(&dest->sceneDynBrush);
				}

				if (data->primaryLightEntityShadowVis)
				{
					buf->align(3);
					buf->write(data->primaryLightEntityShadowVis,
						(data->primaryLightCount - data->lastSunPrimaryLightIndex - 1) << 15);
					ZoneBuffer::clear_pointer(&dest->primaryLightEntityShadowVis);
				}

				if (data->primaryLightDynEntShadowVis[0])
				{
					buf->align(3);
					buf->write(data->primaryLightDynEntShadowVis[0],
						data->dpvsDyn.dynEntClientCount[0] * (data->primaryLightCount - data->lastSunPrimaryLightIndex -
							1));
					ZoneBuffer::clear_pointer(&dest->primaryLightDynEntShadowVis[0]);
				}

				if (data->primaryLightDynEntShadowVis[1])
				{
					buf->align(3);
					buf->write(data->primaryLightDynEntShadowVis[1],
						data->dpvsDyn.dynEntClientCount[1] * (data->primaryLightCount - data->lastSunPrimaryLightIndex -
							1));
					ZoneBuffer::clear_pointer(&dest->primaryLightDynEntShadowVis[1]);
				}

				if (data->nonSunPrimaryLightForModelDynEnt)
				{
					buf->align(0);
					buf->write(data->nonSunPrimaryLightForModelDynEnt, data->dpvsDyn.dynEntClientCount[0]);
					ZoneBuffer::clear_pointer(&dest->nonSunPrimaryLightForModelDynEnt);
				}
				buf->pop_stream();

				if (data->shadowGeom)
				{
					buf->align(3);
					auto shadow_geometry = buf->write(data->shadowGeom, data->primaryLightCount);

					for (std::int32_t i = 0; i < data->primaryLightCount; i++)
					{
						if (data->shadowGeom[i].sortedSurfIndex)
						{
							buf->align(1);
							auto destSurf = buf->write_p(data->shadowGeom[i].sortedSurfIndex, data->shadowGeom[i].surfaceCount);
							ZoneBuffer::clear_pointer(&shadow_geometry[i].sortedSurfIndex);

							for (auto i2 = 0; i2 < data->shadowGeom[i].surfaceCount; i2++)
							{
								endian_convert(&destSurf[i2]);
							}
						}
						if (data->shadowGeom[i].smodelIndex)
						{
							buf->align(1);
							auto destIndex = buf->write_p(data->shadowGeom[i].smodelIndex, data->shadowGeom[i].smodelCount);
							ZoneBuffer::clear_pointer(&shadow_geometry[i].smodelIndex);

							for (auto i2 = 0; i2 < data->shadowGeom[i].smodelCount; i2++)
							{
								endian_convert(&destIndex[i2]);
							}
						}

						endian_convert(&shadow_geometry[i].surfaceCount);
						endian_convert(&shadow_geometry[i].smodelCount);
						endian_convert(&shadow_geometry[i].sortedSurfIndex);
						endian_convert(&shadow_geometry[i].smodelIndex);
					}

					ZoneBuffer::clear_pointer(&dest->shadowGeom);
				}

				if (data->lightRegion)
				{
					buf->align(3);
					auto light_region = buf->write(data->lightRegion, data->primaryLightCount);

					for (std::int32_t i = 0; i < data->primaryLightCount; i++)
					{
						if (data->lightRegion[i].hulls)
						{
							buf->align(3);
							auto light_region_hull = buf->write(data->lightRegion[i].hulls, data->lightRegion[i].hullCount);

							for (std::uint32_t i2 = 0; i2 < data->lightRegion[i].hullCount; i2++)
							{
								if (data->lightRegion[i].hulls[i2].axis)
								{
									buf->align(3);
									auto destAxis = buf->write(data->lightRegion[i].hulls[i2].axis,
										data->lightRegion[i].hulls[i2].axisCount);

									for (auto i3 = 0u; i3 < data->lightRegion[i].hulls[i2].axisCount; i3++)
									{
										endian_convert(&destAxis[i3].dir[0]);
										endian_convert(&destAxis[i3].dir[1]);
										endian_convert(&destAxis[i3].dir[2]);
										endian_convert(&destAxis[i3].halfSize);
										endian_convert(&destAxis[i3].midPoint);
									}
									
									ZoneBuffer::clear_pointer(&light_region_hull[i2].axis);
								}

								endian_convert(&light_region_hull[i2].axis);
								endian_convert(&light_region_hull[i2].axisCount);
								for (auto a = 0; a < 9; a++)
								{
									endian_convert(&light_region_hull[i2].kdopMidPoint[a]);
									endian_convert(&light_region_hull[i2].kdopHalfSize[a]);
								}
							}
							
							ZoneBuffer::clear_pointer(&light_region[i].hulls);
						}

						endian_convert(&light_region[i].hullCount);
						endian_convert(&light_region[i].hulls);
					}

					ZoneBuffer::clear_pointer(&dest->lightRegion);
				}

				buf->push_stream(2);
				if (data->dpvs.smodelVisData[0])
				{
					buf->align(0);
					buf->write(data->dpvs.smodelVisData[0], data->dpvs.smodelCount);
					ZoneBuffer::clear_pointer(&dest->dpvs.smodelVisData[0]);
				}

				if (data->dpvs.smodelVisData[1])
				{
					buf->align(0);
					buf->write(data->dpvs.smodelVisData[1], data->dpvs.smodelCount);
					ZoneBuffer::clear_pointer(&dest->dpvs.smodelVisData[1]);
				}

				if (data->dpvs.smodelVisData[2])
				{
					buf->align(0);
					buf->write(data->dpvs.smodelVisData[2], data->dpvs.smodelCount);
					ZoneBuffer::clear_pointer(&dest->dpvs.smodelVisData[2]);
				}

				if (data->dpvs.surfaceVisData[0])
				{
					buf->align(0);
					buf->write(data->dpvs.surfaceVisData[0], data->dpvs.staticSurfaceCount);
					ZoneBuffer::clear_pointer(&dest->dpvs.surfaceVisData[0]);
				}

				if (data->dpvs.surfaceVisData[1])
				{
					buf->align(0);
					buf->write(data->dpvs.surfaceVisData[1], data->dpvs.staticSurfaceCount);
					ZoneBuffer::clear_pointer(&dest->dpvs.surfaceVisData[1]);
				}

				if (data->dpvs.surfaceVisData[2])
				{
					buf->align(0);
					buf->write(data->dpvs.surfaceVisData[2], data->dpvs.staticSurfaceCount);
					ZoneBuffer::clear_pointer(&dest->dpvs.surfaceVisData[2]);
				}
				buf->pop_stream();

				if (data->dpvs.sortedSurfIndex)
				{
					buf->align(1);
					buf->write_p(data->dpvs.sortedSurfIndex,
						data->dpvs.staticSurfaceCount + data->dpvs.staticSurfaceCount);
					ZoneBuffer::clear_pointer(&dest->dpvs.sortedSurfIndex);
				}

				if (data->dpvs.smodelInsts)
				{
					buf->align(3);
					buf->write(data->dpvs.smodelInsts, data->dpvs.smodelCount);
					ZoneBuffer::clear_pointer(&dest->dpvs.smodelInsts);
				}

				if (data->dpvs.surfaces)
				{
					buf->align(3);
					auto surface = buf->write(data->dpvs.surfaces, data->surfaceCount);

					for (std::int32_t i = 0; i < data->surfaceCount; i++)
					{
						if (data->dpvs.surfaces[i].material)
						{
							surface[i].material = reinterpret_cast<Material*>(zone->get_asset_pointer(
								material, data->dpvs.surfaces[i].material->name));
						}

						endian_convert(&surface[i].tris.vertexLayerData);
						endian_convert(&surface[i].tris.firstVertex);
						endian_convert(&surface[i].tris.vertexCount);
						endian_convert(&surface[i].tris.triCount);
						endian_convert(&surface[i].tris.baseIndex);
						endian_convert(&surface[i].material);
					}

					ZoneBuffer::clear_pointer(&dest->dpvs.surfaces);
				}

				if (data->dpvs.surfacesBounds)
				{
					buf->align(3);
					buf->write(data->dpvs.surfacesBounds, data->surfaceCount);
					ZoneBuffer::clear_pointer(&dest->dpvs.surfacesBounds);
				}

				if (data->dpvs.smodelDrawInsts)
				{
					buf->align(3);
					auto static_model_draw_inst = buf->write(data->dpvs.smodelDrawInsts, data->dpvs.smodelCount);

					for (std::uint32_t i = 0; i < data->dpvs.smodelCount; i++)
					{
						if (data->dpvs.smodelDrawInsts[i].model)
						{
							static_model_draw_inst[i].model = reinterpret_cast<XModel*>(zone->get_asset_pointer(
								xmodel, data->dpvs.smodelDrawInsts[i].model->name));
						}

						endian_convert(&static_model_draw_inst[i].model);
						/*endian_convert(&static_model_draw_inst[i].smodelCacheIndex[0]);
						endian_convert(&static_model_draw_inst[i].smodelCacheIndex[1]);
						endian_convert(&static_model_draw_inst[i].smodelCacheIndex[2]);
						endian_convert(&static_model_draw_inst[i].smodelCacheIndex[3]);*/
						endian_convert(&static_model_draw_inst[i].cullDist);
						endian_convert(&static_model_draw_inst[i].lightingHandle);
						endian_convert(&static_model_draw_inst[i].placement.origin[0]);
						endian_convert(&static_model_draw_inst[i].placement.origin[1]);
						endian_convert(&static_model_draw_inst[i].placement.origin[2]);
						endian_convert(&static_model_draw_inst[i].placement.scale);
						for (auto a = 0; a < 3; a++)
						{
							for (auto b = 0; b < 3; b++)
							{
								endian_convert(&static_model_draw_inst[i].placement.axis[a][b]);
							}
						}
					}

					ZoneBuffer::clear_pointer(&dest->dpvs.smodelDrawInsts);
				}

				buf->push_stream(2);
				if (data->dpvs.surfaceMaterials)
				{
					buf->align(7);
					auto destmaterials = buf->write(data->dpvs.surfaceMaterials, data->surfaceCount);
					ZoneBuffer::clear_pointer(&dest->dpvs.smodelDrawInsts);

					for (int i = 0; i < data->surfaceCount; i++)
					{
						endian_convert(&destmaterials[i].packed);
					}
				}

				if (data->dpvs.surfaceCastsSunShadow)
				{
					buf->align(127);
					buf->write(data->dpvs.surfaceCastsSunShadow, data->dpvs.surfaceVisDataCount);
					ZoneBuffer::clear_pointer(&dest->dpvs.surfaceCastsSunShadow);
				}

				if (data->dpvsDyn.dynEntCellBits[0])
				{
					buf->align(3);
					buf->write(data->dpvsDyn.dynEntCellBits[0],
						data->dpvsDyn.dynEntClientWordCount[0] * data->dpvsPlanes.cellCount);
					ZoneBuffer::clear_pointer(&dest->dpvsDyn.dynEntCellBits[0]);
				}

				if (data->dpvsDyn.dynEntCellBits[1])
				{
					buf->align(3);
					buf->write(data->dpvsDyn.dynEntCellBits[1],
						data->dpvsDyn.dynEntClientWordCount[1] * data->dpvsPlanes.cellCount);
					ZoneBuffer::clear_pointer(&dest->dpvsDyn.dynEntCellBits[1]);
				}

				if (data->dpvsDyn.dynEntVisData[0][0])
				{
					buf->align(15);
					buf->write(data->dpvsDyn.dynEntVisData[0][0], 32 * data->dpvsDyn.dynEntClientWordCount[0]);
					ZoneBuffer::clear_pointer(&dest->dpvsDyn.dynEntVisData[0][0]);
				}

				if (data->dpvsDyn.dynEntVisData[1][0])
				{
					buf->align(15);
					buf->write(data->dpvsDyn.dynEntVisData[1][0], 32 * data->dpvsDyn.dynEntClientWordCount[1]);
					ZoneBuffer::clear_pointer(&dest->dpvsDyn.dynEntVisData[1][0]);
				}

				if (data->dpvsDyn.dynEntVisData[0][1])
				{
					buf->align(15);
					buf->write(data->dpvsDyn.dynEntVisData[0][1], 32 * data->dpvsDyn.dynEntClientWordCount[0]);
					ZoneBuffer::clear_pointer(&dest->dpvsDyn.dynEntVisData[0][1]);
				}

				if (data->dpvsDyn.dynEntVisData[1][1])
				{
					buf->align(15);
					buf->write(data->dpvsDyn.dynEntVisData[1][1], 32 * data->dpvsDyn.dynEntClientWordCount[1]);
					ZoneBuffer::clear_pointer(&dest->dpvsDyn.dynEntVisData[1][1]);
				}

				if (data->dpvsDyn.dynEntVisData[0][2])
				{
					buf->align(15);
					buf->write(data->dpvsDyn.dynEntVisData[0][2], 32 * data->dpvsDyn.dynEntClientWordCount[0]);
					ZoneBuffer::clear_pointer(&dest->dpvsDyn.dynEntVisData[0][2]);
				}

				if (data->dpvsDyn.dynEntVisData[1][2])
				{
					buf->align(15);
					buf->write(data->dpvsDyn.dynEntVisData[1][2], 32 * data->dpvsDyn.dynEntClientWordCount[1]);
					ZoneBuffer::clear_pointer(&dest->dpvsDyn.dynEntVisData[1][2]);
				}
				buf->pop_stream();

				if (data->heroOnlyLights)
				{
					buf->align(3);
					buf->write(data->heroOnlyLights, data->heroOnlyLightCount);
					ZoneBuffer::clear_pointer(&dest->heroOnlyLights);
				}

				END_LOG_STREAM;
				buf->pop_stream();

				endian_convert(&dest->name);
				endian_convert(&dest->baseName);
				endian_convert(&dest->planeCount);
				endian_convert(&dest->nodeCount);
				endian_convert(&dest->surfaceCount);
				endian_convert(&dest->skyCount);
				endian_convert(&dest->skies);
				endian_convert(&dest->lastSunPrimaryLightIndex);
				endian_convert(&dest->primaryLightCount);
				endian_convert(&dest->sortKeyLitDecal);
				endian_convert(&dest->sortKeyEffectDecal);
				endian_convert(&dest->sortKeyEffectAuto);
				endian_convert(&dest->sortKeyDistortion);

				endian_convert(&dest->dpvsPlanes.cellCount);
				endian_convert(&dest->dpvsPlanes.planes);
				endian_convert(&dest->dpvsPlanes.nodes);
				endian_convert(&dest->dpvsPlanes.sceneEntCellBits);

				endian_convert(&dest->aabbTreeCounts);
				endian_convert(&dest->aabbTrees);
				endian_convert(&dest->cells);

				endian_convert(&dest->draw.reflectionProbeCount);
				endian_convert(&dest->draw.reflectionProbes);
				endian_convert(&dest->draw.reflectionProbeOrigins);
				endian_convert(&dest->draw.reflectionProbeTextures);
				endian_convert(&dest->draw.lightmapCount);
				endian_convert(&dest->draw.lightmaps);
				endian_convert(&dest->draw.lightmapPrimaryTextures);
				endian_convert(&dest->draw.lightmapSecondaryTextures);
				endian_convert(&dest->draw.lightmapOverridePrimary);
				endian_convert(&dest->draw.lightmapOverrideSecondary);
				endian_convert(&dest->draw.vertexCount);
				endian_convert(&dest->draw.vd.vertices);
				endian_convert(&dest->draw.vertexLayerDataSize);
				endian_convert(&dest->draw.vld.data);
				endian_convert(&dest->draw.indexCount);
				endian_convert(&dest->draw.indices);

				endian_convert(&dest->lightGrid.sunPrimaryLightIndex);
				endian_convert(&dest->lightGrid.mins[0]);
				endian_convert(&dest->lightGrid.mins[1]);
				endian_convert(&dest->lightGrid.mins[2]);
				endian_convert(&dest->lightGrid.maxs[0]);
				endian_convert(&dest->lightGrid.maxs[1]);
				endian_convert(&dest->lightGrid.maxs[2]);
				endian_convert(&dest->lightGrid.rowAxis);
				endian_convert(&dest->lightGrid.colAxis);
				endian_convert(&dest->lightGrid.rowDataStart);
				endian_convert(&dest->lightGrid.rawRowDataSize);
				endian_convert(&dest->lightGrid.rawRowData);
				endian_convert(&dest->lightGrid.entryCount);
				endian_convert(&dest->lightGrid.entries);
				endian_convert(&dest->lightGrid.colorCount);
				endian_convert(&dest->lightGrid.colors);

				endian_convert(&dest->modelCount);
				endian_convert(&dest->models);
				endian_convert(&dest->bounds.halfSize);
				endian_convert(&dest->bounds.midPoint);
				endian_convert(&dest->checksum);
				endian_convert(&dest->materialMemoryCount);
				endian_convert(&dest->materialMemory);

				endian_convert(&dest->sun.spriteMaterial);
				endian_convert(&dest->sun.flareMaterial);
				endian_convert(&dest->sun.spriteSize);
				endian_convert(&dest->sun.flareMinSize);
				endian_convert(&dest->sun.flareMinDot);
				endian_convert(&dest->sun.flareMaxSize);
				endian_convert(&dest->sun.flareMaxDot);
				endian_convert(&dest->sun.flareMaxAlpha);
				endian_convert(&dest->sun.flareFadeInTime);
				endian_convert(&dest->sun.flareFadeOutTime);
				endian_convert(&dest->sun.blindMinDot);
				endian_convert(&dest->sun.blindMaxDot);
				endian_convert(&dest->sun.blindMaxDarken);
				endian_convert(&dest->sun.blindFadeInTime);
				endian_convert(&dest->sun.blindFadeOutTime);
				endian_convert(&dest->sun.glareMinDot);
				endian_convert(&dest->sun.glareMaxDot);
				endian_convert(&dest->sun.glareMaxLighten);
				endian_convert(&dest->sun.glareFadeInTime);
				endian_convert(&dest->sun.glareFadeOutTime);
				endian_convert(&dest->sun.sunFxPosition[0]);
				endian_convert(&dest->sun.sunFxPosition[1]);
				endian_convert(&dest->sun.sunFxPosition[2]);

				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						endian_convert(&dest->outdoorLookupMatrix[i][j]);
					}
				}

				endian_convert(&dest->outdoorImage);
				endian_convert(&dest->cellCasterBits);
				endian_convert(&dest->cellHasSunLitSurfsBits);
				endian_convert(&dest->sceneDynModel);
				endian_convert(&dest->sceneDynBrush);
				endian_convert(&dest->primaryLightEntityShadowVis);
				endian_convert(&dest->primaryLightDynEntShadowVis[0]);
				endian_convert(&dest->primaryLightDynEntShadowVis[1]);
				endian_convert(&dest->nonSunPrimaryLightForModelDynEnt);
				endian_convert(&dest->shadowGeom);
				endian_convert(&dest->lightRegion);

				endian_convert(&dest->dpvs.smodelCount);
				endian_convert(&dest->dpvs.staticSurfaceCount);
				endian_convert(&dest->dpvs.litOpaqueSurfsBegin);
				endian_convert(&dest->dpvs.litOpaqueSurfsEnd);
				endian_convert(&dest->dpvs.litTransSurfsBegin);
				endian_convert(&dest->dpvs.litTransSurfsEnd);
				endian_convert(&dest->dpvs.shadowCasterSurfsBegin);
				endian_convert(&dest->dpvs.shadowCasterSurfsEnd);
				endian_convert(&dest->dpvs.emissiveSurfsBegin);
				endian_convert(&dest->dpvs.emissiveSurfsEnd);
				endian_convert(&dest->dpvs.smodelVisDataCount);
				endian_convert(&dest->dpvs.surfaceVisDataCount);
				endian_convert(&dest->dpvs.smodelVisData[0]);
				endian_convert(&dest->dpvs.smodelVisData[1]);
				endian_convert(&dest->dpvs.smodelVisData[2]);
				endian_convert(&dest->dpvs.surfaceVisData[0]);
				endian_convert(&dest->dpvs.surfaceVisData[1]);
				endian_convert(&dest->dpvs.surfaceVisData[2]);
				endian_convert(&dest->dpvs.sortedSurfIndex);
				endian_convert(&dest->dpvs.smodelInsts);
				endian_convert(&dest->dpvs.surfaces);
				endian_convert(&dest->dpvs.surfacesBounds);
				endian_convert(&dest->dpvs.smodelDrawInsts);
				endian_convert(&dest->dpvs.surfaceMaterials);
				endian_convert(&dest->dpvs.surfaceCastsSunShadow);
				endian_convert(&dest->dpvs.usageCount);

				endian_convert(&dest->dpvsDyn.dynEntClientWordCount[0]);
				endian_convert(&dest->dpvsDyn.dynEntClientWordCount[1]);
				endian_convert(&dest->dpvsDyn.dynEntClientCount[0]);
				endian_convert(&dest->dpvsDyn.dynEntClientCount[1]);
				endian_convert(&dest->dpvsDyn.dynEntCellBits[0]);
				endian_convert(&dest->dpvsDyn.dynEntCellBits[1]);
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 3; j++)
					{
						endian_convert(&dest->dpvsDyn.dynEntVisData[i][j]);
					}
				}

				endian_convert(&dest->mapVtxChecksum);
				endian_convert(&dest->heroOnlyLightCount);
				endian_convert(&dest->heroOnlyLights);

				sizeof GfxWorld;
				sizeof alpha::GfxWorld;
			}

#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif
		}

		void IGfxWorld::dump(GfxWorld* asset)
		{
			// convert asset to IW5 format
			auto* iw5_asset = new IW5::GfxWorld;
			memset(iw5_asset, 0, sizeof IW5::GfxWorld);

			// copy struct data
			memcpy(&iw5_asset->name, &asset->name, Difference(&iw5_asset->cells, &iw5_asset->name));

			// allocate cells
			iw5_asset->cells = new IW5::GfxCell[iw5_asset->dpvsPlanes.cellCount];
			memset(iw5_asset->cells, 0, sizeof(IW5::GfxCell) * iw5_asset->dpvsPlanes.cellCount);

			// copy cell data
			for (int i = 0; i < iw5_asset->dpvsPlanes.cellCount; i++)
			{
				memcpy(&iw5_asset->cells[i], &asset->cells[i], sizeof GfxCell);
			}

			// copy worldDraw data
			memcpy(iw5_asset->worldDraw._portpad0, asset->worldDraw._portpad0, 16);
			memcpy(iw5_asset->worldDraw._portpad1, asset->worldDraw._portpad1, 56);

			// copy remaining GfxWorld data
			memcpy(&iw5_asset->lightGrid, &asset->lightGrid,
			       Difference(&iw5_asset->fogTypesAllowed + 1, &iw5_asset->lightGrid));

			// dump asset
			IW5::IGfxWorld::dump(iw5_asset);

			// free memory_
			delete[] iw5_asset->cells;
			delete iw5_asset;
		}
	}
}
