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
		/*legacy zonetool code, refactor me!*/
		GfxWorld* IGfxWorld::parse(const std::string& name, ZoneMemory* mem)
		{
			if (!FileSystem::FileExists(name + ".gfxmap"))
			{
				return nullptr;
			}

			// alloc GfxWorld
			auto asset = mem->Alloc<GfxWorld>();
			memset(asset, 0, sizeof(GfxWorld));

			AssetReader read(mem);
			if (!read.open(name + ".gfxmap"))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing GfxWorld \"%s\"...", name.data());

#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW5::IGfxWorld::parse");
#endif

			asset->name = read.read_string();
			asset->baseName = read.read_string();

			asset->planeCount = read.read_int();
			asset->nodeCount = read.read_int();
			asset->indexCount = read.read_int();
			asset->skyCount = read.read_int();

			asset->skies = read.read_array<GfxSky>();
			for (unsigned int i = 0; i < asset->skyCount; i++)
			{
				asset->skies[i].skyStartSurfs = read.read_array<std::uint32_t>();
				asset->skies[i].skyImage = read.read_asset<GfxImage>();
			}

			asset->sunPrimaryLightIndex = read.read_int();
			asset->primaryLightCount = read.read_int();
			asset->primaryLightEnvCount = read.read_int();

			char* unknown1 = read.read_array<char>();
			memcpy(asset->unknown1, unknown1, 12);

			// dpvsplanes
			asset->dpvsPlanes.cellCount = read.read_int();
			asset->dpvsPlanes.planes = read.read_array<cplane_s>();
			asset->dpvsPlanes.nodes = read.read_array<unsigned short>();
			asset->dpvsPlanes.sceneEntCellBits = read.read_array<unsigned char>();

			// dpvs
			GfxWorldDpvsStatic* dpvs = read.read_array<GfxWorldDpvsStatic>();
			memcpy(&asset->dpvs, dpvs, sizeof(GfxWorldDpvsStatic));

			asset->dpvs.smodelVisData[0] = read.read_array<char>();
			asset->dpvs.smodelVisData[1] = read.read_array<char>();
			asset->dpvs.smodelVisData[2] = read.read_array<char>();
			asset->dpvs.surfaceVisData[0] = read.read_array<char>();
			asset->dpvs.surfaceVisData[1] = read.read_array<char>();
			asset->dpvs.surfaceVisData[2] = read.read_array<char>();
			asset->dpvs.sortedSurfIndex = read.read_array<unsigned short>();
			asset->dpvs.smodelInsts = read.read_array<GfxStaticModelInst>();
			asset->dpvs.surfaces = read.read_array<GfxSurface>();
			for (int i = 0; i < asset->indexCount; i++)
			{
				asset->dpvs.surfaces[i].material = read.read_asset<Material>();
			}
			asset->dpvs.cullGroups = read.read_array<GfxCullGroup>();
			asset->dpvs.smodelDrawInsts = read.read_array<GfxStaticModelDrawInst>();
			for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
			{
				asset->dpvs.smodelDrawInsts[i].model = read.read_asset<XModel>();
			}
			asset->dpvs.surfaceMaterials = read.read_array<GfxDrawSurf>();
			asset->dpvs.surfaceCastsSunShadow = read.read_array<unsigned int>();

			// dpvsDyn
			GfxWorldDpvsDynamic* dpvsDyn = read.read_array<GfxWorldDpvsDynamic>();
			memcpy(&asset->dpvsDyn, dpvsDyn, sizeof(GfxWorldDpvsDynamic));

			asset->dpvsDyn.dynEntCellBits[0] = read.read_array<unsigned int>();
			asset->dpvsDyn.dynEntCellBits[1] = read.read_array<unsigned int>();
			asset->dpvsDyn.dynEntVisData[0][0] = read.read_array<char>();
			asset->dpvsDyn.dynEntVisData[1][0] = read.read_array<char>();
			asset->dpvsDyn.dynEntVisData[0][1] = read.read_array<char>();
			asset->dpvsDyn.dynEntVisData[1][1] = read.read_array<char>();
			asset->dpvsDyn.dynEntVisData[0][2] = read.read_array<char>();
			asset->dpvsDyn.dynEntVisData[1][2] = read.read_array<char>();

			// aabbTreeCount
			asset->aabbTreeCounts = read.read_array<GfxCellTreeCount>();

			// GfxCellTree
			asset->aabbTree = read.read_array<GfxCellTree>();
			for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
			{
				asset->aabbTree[i].aabbtree = read.read_array<GfxAabbTree>();

				for (int j = 0; j < asset->aabbTreeCounts[i].aabbTreeCount; j++)
				{
					asset->aabbTree[i].aabbtree[j].smodelIndexes = read.read_array<unsigned short>();
				}
			}

			// read GFX cells
			asset->cells = read.read_array<GfxCell>();

			for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
			{
				asset->cells[i].portals = read.read_array<GfxPortal>();
				for (int j = 0; j < asset->cells[i].portalCount; j++)
				{
					asset->cells[i].portals[j].vertices = read.read_array<float[3]>();
				}
				asset->cells[i].reflectionProbes = read.read_array<char>();
				asset->cells[i].reflectionProbeReferences = read.read_array<char>();
			}
			
			auto worldDraw = read.read_array<GfxWorldDraw>();
			memcpy(&asset->worldDraw, worldDraw, sizeof GfxWorldDraw);

			asset->worldDraw.reflectionImages = mem->Alloc<GfxImage*>(asset->worldDraw.reflectionProbeCount);
			// new GfxImage*[asset->draw.reflectionProbeCount];
			memset(asset->worldDraw.reflectionImages, 0, sizeof(GfxImage*) * asset->worldDraw.reflectionProbeCount);

			for (unsigned int i = 0; i < asset->worldDraw.reflectionProbeCount; i++)
			{
				asset->worldDraw.reflectionImages[i] = read.read_asset<GfxImage>();
			}

			asset->worldDraw.reflectionProbes = read.read_array<GfxReflectionProbe>();

			asset->worldDraw.reflectionProbeReferences = read.read_array<char>();
			asset->worldDraw.reflectionProbeReferenceOrigins = read.read_array<GfxReflectionProbeReferenceOrigin>();

			asset->worldDraw.reflectionProbeTextures = read.read_array<GfxTexture>();
			asset->worldDraw.lightmaps = read.read_array<GfxLightmapArray>();
			for (int i = 0; i < asset->worldDraw.lightmapCount; i++)
			{
				asset->worldDraw.lightmaps[i].primary = read.read_asset<GfxImage>();
				asset->worldDraw.lightmaps[i].secondary = read.read_asset<GfxImage>();
			}
			asset->worldDraw.lightmapPrimaryTextures = read.read_array<GfxTexture>();
			asset->worldDraw.lightmapSecondaryTextures = read.read_array<GfxTexture>();
			asset->worldDraw.skyImage = read.read_asset<GfxImage>();
			asset->worldDraw.outdoorImage = read.read_asset<GfxImage>();
			asset->worldDraw.vd.vertices = read.read_array<GfxWorldVertex>();
			asset->worldDraw.vld.data = read.read_array<char>();
			asset->worldDraw.indices = read.read_array<unsigned short>();


			// GfxLightGrid
			GfxLightGrid* lightGrid = read.read_array<GfxLightGrid>();
			memcpy(&asset->lightGrid, lightGrid, sizeof(GfxLightGrid));

			asset->lightGrid.rowDataStart = read.read_array<unsigned short>();
			asset->lightGrid.rawRowData = read.read_array<char>();
			asset->lightGrid.entries = read.read_array<GfxLightGridEntry>();
			asset->lightGrid.colors = read.read_array<GfxLightGridColors>();

			// models
			asset->modelCount = read.read_int();
			asset->models = read.read_array<GfxBrushModel>();

			// mins/maxs
			float* mins = read.read_array<float>();
			memcpy(asset->mins, mins, sizeof(float) * 3);

			float* maxs = read.read_array<float>();
			memcpy(asset->maxs, maxs, sizeof(float) * 3);

			asset->checksum = read.read_int();

			// materialmemory
			asset->materialMemoryCount = read.read_int();
			asset->materialMemory = read.read_array<MaterialMemory>();
			for (int i = 0; i < asset->materialMemoryCount; i++)
			{
				asset->materialMemory[i].material = read.read_asset<Material>();
			}

			// sun data
			sunflare_t* sun = read.read_array<sunflare_t>();
			memcpy(&asset->sun, sun, sizeof(sunflare_t));

			asset->sun.spriteMaterial = read.read_asset<Material>();
			asset->sun.flareMaterial = read.read_asset<Material>();

			// outdoor shit
			auto lookupMatrix = read.read_array<VecInternal<4>>();
			memcpy(&asset->outdoorLookupMatrix, lookupMatrix, sizeof(VecInternal<4>) * 4);

			asset->outdoorImage = read.read_asset<GfxImage>();

			// CellcasterBits
			asset->cellCasterBits[0] = read.read_array<unsigned int>();
			asset->cellCasterBits[1] = read.read_array<unsigned int>();

			// SceneDynModel
			asset->sceneDynModel = read.read_array<GfxSceneDynModel>();

			// SceneDynBrush
			asset->sceneDynBrush = read.read_array<GfxSceneDynBrush>();

			// PrimaryLightEntityShadowVis
			asset->primaryLightEntityShadowVis = read.read_array<unsigned char>();

			// PrimaryLightDynEntShadowVis
			asset->primaryLightDynEntShadowVis[0] = read.read_array<unsigned int>();
			asset->primaryLightDynEntShadowVis[1] = read.read_array<unsigned int>();

			// PrimaryLightForModelDynEnt
			asset->primaryLightForModelDynEnt = read.read_array<char>();

			// GfxShadowGeometry
			asset->shadowGeom = read.read_array<GfxShadowGeometry>();
			for (int i = 0; i < asset->primaryLightCount; i++)
			{
				asset->shadowGeom[i].sortedSurfIndex = read.read_array<unsigned short>();
				asset->shadowGeom[i].smodelIndex = read.read_array<unsigned short>();
			}

			// GfxLightRegion
			asset->lightRegion = read.read_array<GfxLightRegion>();
			for (int i = 0; i < asset->primaryLightCount; i++)
			{
				asset->lightRegion[i].hulls = read.read_array<GfxLightRegionHull>();
				for (unsigned int j = 0; j < asset->lightRegion[i].hullCount; j++)
				{
					asset->lightRegion[i].hulls[j].axis = read.read_array<GfxLightRegionAxis>();
				}
			}

			asset->mapVtxChecksum = read.read_int();

			// heroLights
			asset->heroLightCount = 0;
			asset->heroLights = nullptr;

			asset->fogTypesAllowed = read.read_int();

#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif

			return asset;
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
				this->asset_ = DB_FindXAssetHeader(this->type(), name.data(), 1).gfxworld;
			}
		}

		void IGfxWorld::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IGfxWorld::load_depending(IZone* zone)
		{
#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW5::IGfxWorld::load_depending");
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
			VMProtectBeginUltra("IW5::IGfxWorld::write");
#endif

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
					if (data->cells[i].reflectionProbeReferences)
					{
						buf->align(0);
						buf->write(data->cells[i].reflectionProbeReferences,
						           data->cells[i].reflectionProbeReferenceCount);
						ZoneBuffer::clear_pointer(&gfx_cell[i].reflectionProbeReferences);
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

			if (data->worldDraw.reflectionProbeReferenceOrigins)
			{
				buf->align(3);
				buf->write(data->worldDraw.reflectionProbeReferenceOrigins,
				           data->worldDraw.reflectionProbeReferenceCount);
				ZoneBuffer::clear_pointer(&dest->worldDraw.reflectionProbeReferenceOrigins);
			}
			if (data->worldDraw.reflectionProbeReferences)
			{
				buf->align(0);
				buf->write(data->worldDraw.reflectionProbeReferences, data->worldDraw.reflectionProbeReferenceCount);
				ZoneBuffer::clear_pointer(&dest->worldDraw.reflectionProbeReferences);
			}

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

					assert(surface[i].material != nullptr);
				}

				ZoneBuffer::clear_pointer(&dest->dpvs.surfaces);
			}
			assert(data->indexCount > 0);
			assert(data->dpvs.surfaces != nullptr);

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

			encrypt_data(dest, sizeof GfxWorld);
#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif
		}

		void IGfxWorld::dump(GfxWorld* asset)
		{
			// dump code
			AssetDumper write;
			if (!write.open(asset->name + ".gfxmap"s))
			{
				return;
			}

			write.dump_string(asset->name);
			write.dump_string(asset->baseName);

			write.dump_int(asset->planeCount);
			write.dump_int(asset->nodeCount);
			write.dump_int(asset->indexCount);
			write.dump_int(asset->skyCount);

			write.dump_array(asset->skies, asset->skyCount);
			for (unsigned int i = 0; i < asset->skyCount; i++)
			{
				write.dump_array(asset->skies[i].skyStartSurfs, asset->skies[i].skySurfCount);
				write.dump_asset(asset->skies[i].skyImage);
			}

			write.dump_int(asset->sunPrimaryLightIndex);
			write.dump_int(asset->primaryLightCount);
			write.dump_int(asset->primaryLightEnvCount);

			write.dump_array(asset->unknown1, 12);

			// dpvsplanes
			write.dump_int(asset->dpvsPlanes.cellCount);
			write.dump_array(asset->dpvsPlanes.planes, asset->planeCount);
			write.dump_array(asset->dpvsPlanes.nodes, asset->nodeCount);
			write.dump_array(asset->dpvsPlanes.sceneEntCellBits, asset->dpvsPlanes.cellCount << 11);

			// dpvs
			write.dump_array(&asset->dpvs, 1);
			write.dump_array(asset->dpvs.smodelVisData[0], asset->dpvs.smodelCount);
			write.dump_array(asset->dpvs.smodelVisData[1], asset->dpvs.smodelCount);
			write.dump_array(asset->dpvs.smodelVisData[2], asset->dpvs.smodelCount);
			write.dump_array(asset->dpvs.surfaceVisData[0], asset->dpvs.staticSurfaceCount);
			write.dump_array(asset->dpvs.surfaceVisData[1], asset->dpvs.staticSurfaceCount);
			write.dump_array(asset->dpvs.surfaceVisData[2], asset->dpvs.staticSurfaceCount);
			write.dump_array(asset->dpvs.sortedSurfIndex,
			            (asset->dpvs.staticSurfaceCount + asset->dpvs.staticSurfaceCountNoDecal));
			write.dump_array(asset->dpvs.smodelInsts, asset->dpvs.smodelCount);
			write.dump_array(asset->dpvs.surfaces, asset->indexCount);

			for (int i = 0; i < asset->indexCount; i++)
			{
				write.dump_asset(asset->dpvs.surfaces[i].material);
			}

			write.dump_array(asset->dpvs.cullGroups, asset->indexCount);
			write.dump_array(asset->dpvs.smodelDrawInsts, asset->dpvs.smodelCount);

			for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
			{
				write.dump_asset(asset->dpvs.smodelDrawInsts[i].model);
			}
			write.dump_array(asset->dpvs.surfaceMaterials, asset->indexCount);
			write.dump_array(asset->dpvs.surfaceCastsSunShadow, asset->dpvs.surfaceVisDataCount);

			// dpvsDyn
			write.dump_array(&asset->dpvsDyn, 1);
			write.dump_array(asset->dpvsDyn.dynEntCellBits[0],
			            asset->dpvsDyn.dynEntClientWordCount[0] * asset->dpvsPlanes.cellCount);
			write.dump_array(asset->dpvsDyn.dynEntCellBits[1],
			            asset->dpvsDyn.dynEntClientWordCount[1] * asset->dpvsPlanes.cellCount);
			write.dump_array(asset->dpvsDyn.dynEntVisData[0][0], asset->dpvsDyn.dynEntClientWordCount[0] * 32);
			write.dump_array(asset->dpvsDyn.dynEntVisData[1][0], asset->dpvsDyn.dynEntClientWordCount[1] * 32);
			write.dump_array(asset->dpvsDyn.dynEntVisData[0][1], asset->dpvsDyn.dynEntClientWordCount[0] * 32);
			write.dump_array(asset->dpvsDyn.dynEntVisData[1][1], asset->dpvsDyn.dynEntClientWordCount[1] * 32);
			write.dump_array(asset->dpvsDyn.dynEntVisData[0][2], asset->dpvsDyn.dynEntClientWordCount[0] * 32);
			write.dump_array(asset->dpvsDyn.dynEntVisData[1][2], asset->dpvsDyn.dynEntClientWordCount[1] * 32);

			// aabbTreeCount
			write.dump_array(asset->aabbTreeCounts, asset->dpvsPlanes.cellCount);

			// GfxCellTree
			write.dump_array(asset->aabbTree, asset->dpvsPlanes.cellCount);
			for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
			{
				write.dump_array(asset->aabbTree[i].aabbtree, asset->aabbTreeCounts[i].aabbTreeCount);

				for (int j = 0; j < asset->aabbTreeCounts[i].aabbTreeCount; j++)
				{
					write.dump_array(asset->aabbTree[i].aabbtree[j].smodelIndexes,
					            asset->aabbTree[i].aabbtree[j].smodelIndexCount);
				}
			}

			// read GFX cells
			write.dump_array(asset->cells, asset->dpvsPlanes.cellCount);

			for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
			{
				write.dump_array(asset->cells[i].portals, asset->cells[i].portalCount);
				for (int j = 0; j < asset->cells[i].portalCount; j++)
				{
					write.dump_array(asset->cells[i].portals[j].vertices, asset->cells[i].portals[j].vertexCount);
				}
				write.dump_array(asset->cells[i].reflectionProbes, asset->cells[i].reflectionProbeCount);
				write.dump_array(asset->cells[i].reflectionProbeReferences, asset->cells[i].reflectionProbeReferenceCount);
			}

			// draw
			write.dump_array(&asset->worldDraw, 1);

			for (unsigned int i = 0; i < asset->worldDraw.reflectionProbeCount; i++)
			{
				write.dump_asset(asset->worldDraw.reflectionImages[i]);
			}

			write.dump_array(asset->worldDraw.reflectionProbes, asset->worldDraw.reflectionProbeCount);

			write.dump_array(asset->worldDraw.reflectionProbeReferences, asset->worldDraw.reflectionProbeReferenceCount);
			write.dump_array(asset->worldDraw.reflectionProbeReferenceOrigins,
			            asset->worldDraw.reflectionProbeReferenceCount);

			write.dump_array(asset->worldDraw.reflectionProbeTextures, asset->worldDraw.reflectionProbeCount);
			write.dump_array(asset->worldDraw.lightmaps, asset->worldDraw.lightmapCount);

			for (int i = 0; i < asset->worldDraw.lightmapCount; i++)
			{
				write.dump_asset(asset->worldDraw.lightmaps[i].primary);
				write.dump_asset(asset->worldDraw.lightmaps[i].secondary);
			}

			write.dump_array(asset->worldDraw.lightmapPrimaryTextures, asset->worldDraw.lightmapCount);
			write.dump_array(asset->worldDraw.lightmapSecondaryTextures, asset->worldDraw.lightmapCount);

			write.dump_asset(asset->worldDraw.skyImage);
			write.dump_asset(asset->worldDraw.outdoorImage);

			write.dump_array(asset->worldDraw.vd.vertices, asset->worldDraw.vertexCount);
			write.dump_array(asset->worldDraw.vld.data, asset->worldDraw.vertexLayerDataSize);
			write.dump_array(asset->worldDraw.indices, asset->worldDraw.indexCount);

			// GfxLightGrid
			write.dump_array(&asset->lightGrid, 1);
			write.dump_array(asset->lightGrid.rowDataStart,
			            (asset->lightGrid.maxs[asset->lightGrid.rowAxis] - asset->lightGrid.mins[asset
			                                                                                     ->lightGrid.rowAxis] +
				            1));
			write.dump_array(asset->lightGrid.rawRowData, asset->lightGrid.rawRowDataSize);
			write.dump_array(asset->lightGrid.entries, asset->lightGrid.entryCount);
			write.dump_array(asset->lightGrid.colors, asset->lightGrid.colorCount);

			// models
			write.dump_int(asset->modelCount);
			write.dump_array(asset->models, asset->modelCount);

			// mins/maxs
			write.dump_array(asset->mins, 3);
			write.dump_array(asset->maxs, 3);

			write.dump_int(asset->checksum);

			// materialmemory
			write.dump_int(asset->materialMemoryCount);
			write.dump_array(asset->materialMemory, asset->materialMemoryCount);
			for (int i = 0; i < asset->materialMemoryCount; i++)
			{
				write.dump_asset(asset->materialMemory[i].material);
			}

			// sun data
			write.dump_array(&asset->sun, 1);
			write.dump_asset(asset->sun.spriteMaterial);
			write.dump_asset(asset->sun.flareMaterial);

			// outdoor shit
			write.dump_array(asset->outdoorLookupMatrix, 4);
			write.dump_asset(asset->outdoorImage);

			// CellcasterBits
			write.dump_array(asset->cellCasterBits[0],
			            asset->dpvsPlanes.cellCount * ((asset->dpvsPlanes.cellCount + 31) >> 5));
			write.dump_array(asset->cellCasterBits[1], (asset->dpvsPlanes.cellCount + 31) >> 5);

			// SceneDynModel
			write.dump_array(asset->sceneDynModel, asset->dpvsDyn.dynEntClientCount[0]);

			// SceneDynBrush
			write.dump_array(asset->sceneDynBrush, asset->dpvsDyn.dynEntClientCount[1]);

			// PrimaryLightEntityShadowVis
			write.dump_array(asset->primaryLightEntityShadowVis,
			            (asset->primaryLightCount - asset->sunPrimaryLightIndex - 1) << 15);

			// PrimaryLightDynEntShadowVis
			write.dump_array(asset->primaryLightDynEntShadowVis[0],
			            asset->dpvsDyn.dynEntClientCount[0] * (asset->primaryLightCount - asset->sunPrimaryLightIndex -
				            1));
			write.dump_array(asset->primaryLightDynEntShadowVis[1],
			            asset->dpvsDyn.dynEntClientCount[1] * (asset->primaryLightCount - asset->sunPrimaryLightIndex -
				            1));

			// PrimaryLightForModelDynEnt
			write.dump_array(asset->primaryLightForModelDynEnt, asset->dpvsDyn.dynEntClientCount[0]);

			// GfxShadowGeometry
			write.dump_array(asset->shadowGeom, asset->primaryLightCount);
			for (int i = 0; i < asset->primaryLightCount; i++)
			{
				write.dump_array(asset->shadowGeom[i].sortedSurfIndex, asset->shadowGeom[i].surfaceCount);
				write.dump_array(asset->shadowGeom[i].smodelIndex, asset->shadowGeom[i].smodelCount);
			}

			// GfxLightRegion
			write.dump_array(asset->lightRegion, asset->primaryLightCount);
			for (int i = 0; i < asset->primaryLightCount; i++)
			{
				write.dump_array(asset->lightRegion[i].hulls, asset->lightRegion[i].hullCount);
				for (unsigned int j = 0; j < asset->lightRegion[i].hullCount; j++)
				{
					write.dump_array(asset->lightRegion[i].hulls[j].axis, asset->lightRegion[i].hulls[j].axisCount);
				}
			}

			write.dump_int(asset->mapVtxChecksum);
			write.dump_int(asset->fogTypesAllowed);

			// save file to disk!
			write.close();
		}
	}
}
