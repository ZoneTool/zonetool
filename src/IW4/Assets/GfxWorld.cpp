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

			// copy draw data
			memcpy(gfxmap->draw._portpad0, iw5_gfxmap->worldDraw._portpad0, 16);
			memcpy(gfxmap->draw._portpad1, iw5_gfxmap->worldDraw._portpad1, 56);

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
			auto* data = this->asset_;
			
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
			if (data->draw.reflectionImages)
			{
				for (unsigned int i = 0; i < data->draw.reflectionProbeCount; i++)
				{
					if (data->draw.reflectionImages[i])
					{
						zone->add_asset_of_type(image, data->draw.reflectionImages[i]->name);
					}
				}
			}

			// Lightmaps
			if (data->draw.lightmaps)
			{
				for (int i = 0; i < data->draw.lightmapCount; i++)
				{
					if (data->draw.lightmaps[i].primary)
					{
						zone->add_asset_of_type(image, data->draw.lightmaps[i].primary->name);
					}

					if (data->draw.lightmaps[i].secondary)
					{
						zone->add_asset_of_type(image, data->draw.lightmaps[i].secondary->name);
					}
				}
			}

			// SkyImage (Unused?)
			if (data->draw.skyImage)
			{
				zone->add_asset_of_type(image, data->draw.skyImage->name);
			}

			// OutdoorImage (Unused?)
			if (data->draw.outdoorImage)
			{
				zone->add_asset_of_type(image, data->draw.outdoorImage->name);
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
				for (int i = 0; i < data->surfaceCount; i++)
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
			auto* data = this->asset_;
			auto* dest = buf->write(data);

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

			if (data->draw.reflectionImages)
			{
				buf->align(3);
				auto reflectionProbes = buf->write(data->draw.reflectionImages,
					data->draw.reflectionProbeCount);

				for (std::uint64_t i = 0; i < data->draw.reflectionProbeCount; i++)
				{
					if (reflectionProbes[i])
					{
						reflectionProbes[i] = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
							image, data->draw.reflectionImages[i]->name));
					}
				}

				ZoneBuffer::clear_pointer(&dest->draw.reflectionImages);
			}

			if (data->draw.reflectionProbes)
			{
				buf->align(3);
				buf->write(data->draw.reflectionProbes, data->draw.reflectionProbeCount);
				ZoneBuffer::clear_pointer(&dest->draw.reflectionProbes);
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

			if (data->draw.skyImage)
			{
				dest->draw.skyImage = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
					image, data->draw.skyImage->name));
			}

			if (data->draw.outdoorImage)
			{
				dest->draw.outdoorImage = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
					image, data->draw.outdoorImage->name));
			}

			if (data->draw.vd.vertices)
			{
				buf->align(3);
				buf->write_p(data->draw.vd.vertices, data->draw.vertexCount);
				ZoneBuffer::clear_pointer(&dest->draw.vd.vertices);
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
				buf->write_p(data->draw.indices, data->draw.indexCount);
				ZoneBuffer::clear_pointer(&dest->draw.indices);
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
				auto surface = buf->write(data->dpvs.surfaces, data->surfaceCount);

				for (std::int32_t i = 0; i < data->surfaceCount; i++)
				{
					if (data->dpvs.surfaces[i].material)
					{
						surface[i].material = reinterpret_cast<Material*>(zone->get_asset_pointer(
							material, data->dpvs.surfaces[i].material->name));
					}
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
				}

				ZoneBuffer::clear_pointer(&dest->dpvs.smodelDrawInsts);
			}

			buf->push_stream(2);
			if (data->dpvs.surfaceMaterials)
			{
				buf->align(7);
				buf->write(data->dpvs.surfaceMaterials, data->surfaceCount);
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

			// copy draw data
			memcpy(iw5_asset->worldDraw._portpad0, asset->draw._portpad0, 16);
			memcpy(iw5_asset->worldDraw._portpad1, asset->draw._portpad1, 56);

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
