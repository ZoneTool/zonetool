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
		GfxWorld* IGfxWorld::parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			if (!FileSystem::FileExists(name + ".gfxmap"))
			{
				return nullptr;
			}

			// alloc GfxWorld
			auto asset = mem->Alloc<GfxWorld>();
			memset(asset, 0, sizeof(GfxWorld));

			AssetReader read(mem);
			if (!read.Open(name + ".gfxmap"))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing GfxWorld \"%s\"...", name.data());

#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW5::IGfxWorld::parse");
#endif

			asset->name = read.String();
			asset->baseName = read.String();

			asset->planeCount = read.Int();
			asset->nodeCount = read.Int();
			asset->indexCount = read.Int();
			asset->skyCount = read.Int();

			asset->skies = read.Array<GfxSky>();
			for (unsigned int i = 0; i < asset->skyCount; i++)
			{
				asset->skies[i].skyStartSurfs = read.Array<std::uint32_t>();
				asset->skies[i].skyImage = read.Asset<GfxImage>();
			}

			asset->sunPrimaryLightIndex = read.Int();
			asset->primaryLightCount = read.Int();
			asset->primaryLightEnvCount = read.Int();

			char* unknown1 = read.Array<char>();
			memcpy(asset->unknown1, unknown1, 12);

			// dpvsplanes
			asset->dpvsPlanes.cellCount = read.Int();
			asset->dpvsPlanes.planes = read.Array<cplane_s>();
			asset->dpvsPlanes.nodes = read.Array<unsigned short>();
			asset->dpvsPlanes.sceneEntCellBits = read.Array<unsigned char>();

			// dpvs
			GfxWorldDpvsStatic* dpvs = read.Array<GfxWorldDpvsStatic>();
			memcpy(&asset->dpvs, dpvs, sizeof(GfxWorldDpvsStatic));

			asset->dpvs.smodelVisData[0] = read.Array<char>();
			asset->dpvs.smodelVisData[1] = read.Array<char>();
			asset->dpvs.smodelVisData[2] = read.Array<char>();
			asset->dpvs.surfaceVisData[0] = read.Array<char>();
			asset->dpvs.surfaceVisData[1] = read.Array<char>();
			asset->dpvs.surfaceVisData[2] = read.Array<char>();
			asset->dpvs.sortedSurfIndex = read.Array<unsigned short>();
			asset->dpvs.smodelInsts = read.Array<GfxStaticModelInst>();
			asset->dpvs.surfaces = read.Array<GfxSurface>();
			for (int i = 0; i < asset->indexCount; i++)
			{
				asset->dpvs.surfaces[i].material = read.Asset<Material>();
			}
			asset->dpvs.cullGroups = read.Array<GfxCullGroup>();
			asset->dpvs.smodelDrawInsts = read.Array<GfxStaticModelDrawInst>();
			for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
			{
				asset->dpvs.smodelDrawInsts[i].model = read.Asset<XModel>();
			}
			asset->dpvs.surfaceMaterials = read.Array<GfxDrawSurf>();
			asset->dpvs.surfaceCastsSunShadow = read.Array<unsigned int>();

			// dpvsDyn
			GfxWorldDpvsDynamic* dpvsDyn = read.Array<GfxWorldDpvsDynamic>();
			memcpy(&asset->dpvsDyn, dpvsDyn, sizeof(GfxWorldDpvsDynamic));

			asset->dpvsDyn.dynEntCellBits[0] = read.Array<unsigned int>();
			asset->dpvsDyn.dynEntCellBits[1] = read.Array<unsigned int>();
			asset->dpvsDyn.dynEntVisData[0][0] = read.Array<char>();
			asset->dpvsDyn.dynEntVisData[1][0] = read.Array<char>();
			asset->dpvsDyn.dynEntVisData[0][1] = read.Array<char>();
			asset->dpvsDyn.dynEntVisData[1][1] = read.Array<char>();
			asset->dpvsDyn.dynEntVisData[0][2] = read.Array<char>();
			asset->dpvsDyn.dynEntVisData[1][2] = read.Array<char>();

			// aabbTreeCount
			asset->aabbTreeCounts = read.Array<GfxCellTreeCount>();

			// GfxCellTree
			asset->aabbTree = read.Array<GfxCellTree>();
			for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
			{
				asset->aabbTree[i].aabbtree = read.Array<GfxAabbTree>();

				for (int j = 0; j < asset->aabbTreeCounts[i].aabbTreeCount; j++)
				{
					asset->aabbTree[i].aabbtree[j].smodelIndexes = read.Array<unsigned short>();
				}
			}

			// read GFX cells
			asset->cells = read.Array<GfxCell>();

			for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
			{
				asset->cells[i].portals = read.Array<GfxPortal>();
				for (int j = 0; j < asset->cells[i].portalCount; j++)
				{
					asset->cells[i].portals[j].vertices = read.Array<float[3]>();
				}
				asset->cells[i].reflectionProbes = read.Array<char>();
				asset->cells[i].reflectionProbeReferences = read.Array<char>();
			}
			
			auto worldDraw = read.Array<GfxWorldDraw>();
			memcpy(&asset->worldDraw, worldDraw, sizeof GfxWorldDraw);

			asset->worldDraw.reflectionImages = mem->Alloc<GfxImage*>(asset->worldDraw.reflectionProbeCount);
			// new GfxImage*[asset->worldDraw.reflectionProbeCount];
			memset(asset->worldDraw.reflectionImages, 0, sizeof(GfxImage*) * asset->worldDraw.reflectionProbeCount);

			for (unsigned int i = 0; i < asset->worldDraw.reflectionProbeCount; i++)
			{
				asset->worldDraw.reflectionImages[i] = read.Asset<GfxImage>();
			}

			asset->worldDraw.reflectionProbes = read.Array<GfxReflectionProbe>();

			asset->worldDraw.reflectionProbeReferences = read.Array<char>();
			asset->worldDraw.reflectionProbeReferenceOrigins = read.Array<GfxReflectionProbeReferenceOrigin>();

			asset->worldDraw.reflectionProbeTextures = read.Array<GfxTexture>();
			asset->worldDraw.lightmaps = read.Array<GfxLightmapArray>();
			for (int i = 0; i < asset->worldDraw.lightmapCount; i++)
			{
				asset->worldDraw.lightmaps[i].primary = read.Asset<GfxImage>();
				asset->worldDraw.lightmaps[i].secondary = read.Asset<GfxImage>();
			}
			asset->worldDraw.lightmapPrimaryTextures = read.Array<GfxTexture>();
			asset->worldDraw.lightmapSecondaryTextures = read.Array<GfxTexture>();
			asset->worldDraw.skyImage = read.Asset<GfxImage>();
			asset->worldDraw.outdoorImage = read.Asset<GfxImage>();
			asset->worldDraw.vd.vertices = read.Array<GfxWorldVertex>();
			asset->worldDraw.vld.data = read.Array<char>();
			asset->worldDraw.indices = read.Array<unsigned short>();


			// GfxLightGrid
			GfxLightGrid* lightGrid = read.Array<GfxLightGrid>();
			memcpy(&asset->lightGrid, lightGrid, sizeof(GfxLightGrid));

			asset->lightGrid.rowDataStart = read.Array<unsigned short>();
			asset->lightGrid.rawRowData = read.Array<char>();
			asset->lightGrid.entries = read.Array<GfxLightGridEntry>();
			asset->lightGrid.colors = read.Array<GfxLightGridColors>();

			// models
			asset->modelCount = read.Int();
			asset->models = read.Array<GfxBrushModel>();

			// mins/maxs
			float* mins = read.Array<float>();
			memcpy(asset->mins, mins, sizeof(float) * 3);

			float* maxs = read.Array<float>();
			memcpy(asset->maxs, maxs, sizeof(float) * 3);

			asset->checksum = read.Int();

			// materialmemory
			asset->materialMemoryCount = read.Int();
			asset->materialMemory = read.Array<MaterialMemory>();
			for (int i = 0; i < asset->materialMemoryCount; i++)
			{
				asset->materialMemory[i].material = read.Asset<Material>();
			}

			// sun data
			sunflare_t* sun = read.Array<sunflare_t>();
			memcpy(&asset->sun, sun, sizeof(sunflare_t));

			asset->sun.spriteMaterial = read.Asset<Material>();
			asset->sun.flareMaterial = read.Asset<Material>();

			// outdoor shit
			auto lookupMatrix = read.Array<VecInternal<4>>();
			memcpy(&asset->outdoorLookupMatrix, lookupMatrix, sizeof(VecInternal<4>) * 4);

			asset->outdoorImage = read.Asset<GfxImage>();

			// CellcasterBits
			asset->cellCasterBits[0] = read.Array<unsigned int>();
			asset->cellCasterBits[1] = read.Array<unsigned int>();

			// SceneDynModel
			asset->sceneDynModel = read.Array<GfxSceneDynModel>();

			// SceneDynBrush
			asset->sceneDynBrush = read.Array<GfxSceneDynBrush>();

			// PrimaryLightEntityShadowVis
			asset->primaryLightEntityShadowVis = read.Array<unsigned char>();

			// PrimaryLightDynEntShadowVis
			asset->primaryLightDynEntShadowVis[0] = read.Array<unsigned int>();
			asset->primaryLightDynEntShadowVis[1] = read.Array<unsigned int>();

			// PrimaryLightForModelDynEnt
			asset->primaryLightForModelDynEnt = read.Array<char>();

			// GfxShadowGeometry
			asset->shadowGeom = read.Array<GfxShadowGeometry>();
			for (int i = 0; i < asset->primaryLightCount; i++)
			{
				asset->shadowGeom[i].sortedSurfIndex = read.Array<unsigned short>();
				asset->shadowGeom[i].smodelIndex = read.Array<unsigned short>();
			}

			// GfxLightRegion
			asset->lightRegion = read.Array<GfxLightRegion>();
			for (int i = 0; i < asset->primaryLightCount; i++)
			{
				asset->lightRegion[i].hulls = read.Array<GfxLightRegionHull>();
				for (unsigned int j = 0; j < asset->lightRegion[i].hullCount; j++)
				{
					asset->lightRegion[i].hulls[j].axis = read.Array<GfxLightRegionAxis>();
				}
			}

			asset->mapVtxChecksum = read.Int();

			// heroLights
			asset->heroLightCount = 0;
			asset->heroLights = nullptr;

			asset->fogTypesAllowed = read.Int();

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

		void IGfxWorld::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			this->m_name = "maps/mp/" + currentzone + ".d3dbsp"; // name;
			this->m_asset = this->parse(name, mem);

			if (!this->m_asset)
			{
				this->m_asset = DB_FindXAssetHeader(this->type(), name.data(), 1).gfxworld;
			}
		}

		void IGfxWorld::prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
		{
		}

		void IGfxWorld::load_depending(IZone* zone)
		{
#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW5::IGfxWorld::load_depending");
#endif

			auto data = this->m_asset;

			// Skies
			if (data->skyCount)
			{
				for (unsigned int i = 0; i < data->skyCount; i++)
				{
					if (data->skies[i].skyImage)
					{
						zone->AddAssetOfType(image, data->skies[i].skyImage->name);
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
						zone->AddAssetOfType(image, data->worldDraw.reflectionImages[i]->name);
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
						zone->AddAssetOfType(image, data->worldDraw.lightmaps[i].primary->name);
					}

					if (data->worldDraw.lightmaps[i].secondary)
					{
						zone->AddAssetOfType(image, data->worldDraw.lightmaps[i].secondary->name);
					}
				}
			}

			// SkyImage (Unused?)
			if (data->worldDraw.skyImage)
			{
				zone->AddAssetOfType(image, data->worldDraw.skyImage->name);
			}

			// OutdoorImage (Unused?)
			if (data->worldDraw.outdoorImage)
			{
				zone->AddAssetOfType(image, data->worldDraw.outdoorImage->name);
			}

			// MaterialMemory

			if (data->materialMemory)
			{
				for (int i = 0; i < data->materialMemoryCount; i++)
				{
					if (data->materialMemory[i].material)
					{
						zone->AddAssetOfType(material, data->materialMemory[i].material->name);
					}
				}
			}

			// Sunflare_t
			if (data->sun.spriteMaterial)
			{
				zone->AddAssetOfType(material, data->sun.spriteMaterial->name);
			}

			if (data->sun.flareMaterial)
			{
				zone->AddAssetOfType(material, data->sun.flareMaterial->name);
			}

			// OutdoorImage
			if (data->outdoorImage)
			{
				zone->AddAssetOfType(image, data->outdoorImage->name);
			}

			// Dpvs.Surfaces
			if (data->dpvs.surfaces)
			{
				for (int i = 0; i < data->indexCount; i++)
				{
					if (data->dpvs.surfaces[i].material)
					{
						zone->AddAssetOfType(material, data->dpvs.surfaces[i].material->name);
					}
				}
			}

			if (data->dpvs.smodelDrawInsts)
			{
				for (unsigned int i = 0; i < data->dpvs.smodelCount; i++)
				{
					if (data->dpvs.smodelDrawInsts[i].model)
					{
						zone->AddAssetOfType(xmodel, data->dpvs.smodelDrawInsts[i].model->name);
					}
				}
			}

#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif
		}

		std::string IGfxWorld::name()
		{
			return this->m_name;
		}

		std::int32_t IGfxWorld::type()
		{
			return gfx_map;
		}

		void IGfxWorld::write(IZone* zone, std::shared_ptr<ZoneBuffer>& buf)
		{
#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW5::IGfxWorld::write");
#endif

			auto data = this->m_asset;
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
						ZoneBuffer::ClearPointer(&skiesArray[i].skyStartSurfs);
					}

					if (data->skies[i].skyImage)
					{
						skiesArray[i].skyImage = reinterpret_cast<GfxImage*>(zone->GetAssetPointer(
							image, data->skies[i].skyImage->name));
					}
				}

				ZoneBuffer::ClearPointer(&dest->skies);
			}

			if (dest->dpvsPlanes.planes)
			{
				dest->dpvsPlanes.planes = buf->write_s(3, data->dpvsPlanes.planes, data->planeCount);
			}

			if (dest->dpvsPlanes.nodes)
			{
				buf->align(1);
				buf->write_p(data->dpvsPlanes.nodes, data->nodeCount);
				ZoneBuffer::ClearPointer(&dest->dpvsPlanes.nodes);
			}

			buf->push_stream(2);
			if (dest->dpvsPlanes.sceneEntCellBits)
			{
				buf->align(3);
				buf->write(data->dpvsPlanes.sceneEntCellBits, data->dpvsPlanes.cellCount << 11);
				ZoneBuffer::ClearPointer(&dest->dpvsPlanes.sceneEntCellBits);
			}
			buf->pop_stream();

			if (data->aabbTreeCounts)
			{
				buf->align(3);
				buf->write_p(data->aabbTreeCounts, data->dpvsPlanes.cellCount);
				ZoneBuffer::ClearPointer(&dest->aabbTreeCounts);
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

						ZoneBuffer::ClearPointer(&cell_tree[i].aabbtree);
					}
				}

				ZoneBuffer::ClearPointer(&dest->aabbTree);
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
								ZoneBuffer::ClearPointer(&gfx_portal[i2].vertices);
							}
						}

						ZoneBuffer::ClearPointer(&gfx_cell[i].portals);
					}

					if (data->cells[i].reflectionProbes)
					{
						buf->align(0);
						buf->write(data->cells[i].reflectionProbes, data->cells[i].reflectionProbeCount);
						ZoneBuffer::ClearPointer(&gfx_cell[i].reflectionProbes);
					}
					if (data->cells[i].reflectionProbeReferences)
					{
						buf->align(0);
						buf->write(data->cells[i].reflectionProbeReferences,
						           data->cells[i].reflectionProbeReferenceCount);
						ZoneBuffer::ClearPointer(&gfx_cell[i].reflectionProbeReferences);
					}

					ZoneBuffer::ClearPointer(&dest->cells);
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
						reflectionProbes[i] = reinterpret_cast<GfxImage*>(zone->GetAssetPointer(
							image, data->worldDraw.reflectionImages[i]->name));
					}
				}

				ZoneBuffer::ClearPointer(&dest->worldDraw.reflectionImages);
			}

			if (data->worldDraw.reflectionProbes)
			{
				buf->align(3);
				buf->write(data->worldDraw.reflectionProbes, data->worldDraw.reflectionProbeCount);
				ZoneBuffer::ClearPointer(&dest->worldDraw.reflectionProbes);
			}

			buf->push_stream(2);
			if (data->worldDraw.reflectionProbeTextures)
			{
				buf->align(3);
				buf->write(data->worldDraw.reflectionProbeTextures, data->worldDraw.reflectionProbeCount);
				ZoneBuffer::ClearPointer(&dest->worldDraw.reflectionProbeTextures);
			}
			buf->pop_stream();

			if (data->worldDraw.reflectionProbeReferenceOrigins)
			{
				buf->align(3);
				buf->write(data->worldDraw.reflectionProbeReferenceOrigins,
				           data->worldDraw.reflectionProbeReferenceCount);
				ZoneBuffer::ClearPointer(&dest->worldDraw.reflectionProbeReferenceOrigins);
			}
			if (data->worldDraw.reflectionProbeReferences)
			{
				buf->align(0);
				buf->write(data->worldDraw.reflectionProbeReferences, data->worldDraw.reflectionProbeReferenceCount);
				ZoneBuffer::ClearPointer(&dest->worldDraw.reflectionProbeReferences);
			}

			if (data->worldDraw.lightmaps)
			{
				buf->align(3);
				auto gfx_lightmap_array = buf->write(data->worldDraw.lightmaps, data->worldDraw.lightmapCount);

				for (std::int32_t i = 0; i < data->worldDraw.lightmapCount; i++)
				{
					if (data->worldDraw.lightmaps[i].primary)
					{
						gfx_lightmap_array[i].primary = reinterpret_cast<GfxImage*>(zone->GetAssetPointer(
							image, data->worldDraw.lightmaps[i].primary->name));
					}

					if (data->worldDraw.lightmaps[i].secondary)
					{
						gfx_lightmap_array[i].secondary = reinterpret_cast<GfxImage*>(zone->GetAssetPointer(
							image, data->worldDraw.lightmaps[i].secondary->name));
					}
				}

				ZoneBuffer::ClearPointer(&dest->worldDraw.lightmaps);
			}

			buf->push_stream(2);
			if (data->worldDraw.lightmapPrimaryTextures)
			{
				buf->align(3);
				buf->write_p(data->worldDraw.lightmapPrimaryTextures, data->worldDraw.lightmapCount);
				ZoneBuffer::ClearPointer(&dest->worldDraw.lightmapPrimaryTextures);
			}

			if (data->worldDraw.lightmapSecondaryTextures)
			{
				buf->align(3);
				buf->write_p(data->worldDraw.lightmapSecondaryTextures, data->worldDraw.lightmapCount);
				ZoneBuffer::ClearPointer(&dest->worldDraw.lightmapSecondaryTextures);
			}
			buf->pop_stream();

			if (data->worldDraw.skyImage)
			{
				dest->worldDraw.skyImage = reinterpret_cast<GfxImage*>(zone->GetAssetPointer(
					image, data->worldDraw.skyImage->name));
			}

			if (data->worldDraw.outdoorImage)
			{
				dest->worldDraw.outdoorImage = reinterpret_cast<GfxImage*>(zone->GetAssetPointer(
					image, data->worldDraw.outdoorImage->name));
			}

			if (data->worldDraw.vd.vertices)
			{
				buf->align(3);
				buf->write_p(data->worldDraw.vd.vertices, data->worldDraw.vertexCount);
				ZoneBuffer::ClearPointer(&dest->worldDraw.vd.vertices);
			}

			if (data->worldDraw.vld.data)
			{
				buf->align(0);
				buf->write_p(data->worldDraw.vld.data, data->worldDraw.vertexLayerDataSize);
				ZoneBuffer::ClearPointer(&dest->worldDraw.vld.data);
			}

			if (data->worldDraw.indices)
			{
				buf->align(1);
				buf->write_p(data->worldDraw.indices, data->worldDraw.indexCount);
				ZoneBuffer::ClearPointer(&dest->worldDraw.indices);
			}

			if (data->lightGrid.rowDataStart)
			{
				buf->align(1);
				buf->write_p(data->lightGrid.rowDataStart,
				             data->lightGrid.maxs[data->lightGrid.rowAxis] - data->lightGrid.mins[data
				                                                                                  ->lightGrid.rowAxis] +
				             1);
				ZoneBuffer::ClearPointer(&dest->lightGrid.rowDataStart);
			}

			if (data->lightGrid.rawRowData)
			{
				buf->align(0);
				buf->write_p(data->lightGrid.rawRowData, data->lightGrid.rawRowDataSize);
				ZoneBuffer::ClearPointer(&dest->lightGrid.rawRowData);
			}

			if (data->lightGrid.entries)
			{
				buf->align(3);
				buf->write(data->lightGrid.entries, data->lightGrid.entryCount);
				ZoneBuffer::ClearPointer(&dest->lightGrid.entries);
			}

			if (data->lightGrid.colors)
			{
				buf->align(3);
				buf->write(data->lightGrid.colors, data->lightGrid.colorCount);
				ZoneBuffer::ClearPointer(&dest->lightGrid.colors);
			}

			if (data->models)
			{
				buf->align(3);
				buf->write(data->models, data->modelCount);
				ZoneBuffer::ClearPointer(&dest->models);
			}

			if (data->materialMemory)
			{
				buf->align(3);
				auto memory = buf->write(data->materialMemory, data->materialMemoryCount);

				for (std::int32_t i = 0; i < data->materialMemoryCount; i++)
				{
					memory[i].material = reinterpret_cast<Material*>(zone->GetAssetPointer(
						material, data->materialMemory[i].material->name));
				}

				ZoneBuffer::ClearPointer(&dest->materialMemory);
			}

			if (data->sun.spriteMaterial)
			{
				dest->sun.spriteMaterial = reinterpret_cast<Material*>(zone->GetAssetPointer(
					material, data->sun.spriteMaterial->name));
			}
			if (data->sun.flareMaterial)
			{
				dest->sun.flareMaterial = reinterpret_cast<Material*>(zone->GetAssetPointer(
					material, data->sun.flareMaterial->name));
			}

			if (data->outdoorImage)
			{
				dest->outdoorImage = reinterpret_cast<GfxImage*>(zone->GetAssetPointer(image, data->outdoorImage->name)
				);
			}

			buf->push_stream(2);
			if (data->cellCasterBits[0])
			{
				buf->align(3);
				buf->write(data->cellCasterBits[0],
				           data->dpvsPlanes.cellCount * ((data->dpvsPlanes.cellCount + 31) >> 5));
				ZoneBuffer::ClearPointer(&dest->cellCasterBits[0]);
			}

			if (data->cellCasterBits[1])
			{
				buf->align(3);
				buf->write(data->cellCasterBits[1], (data->dpvsPlanes.cellCount + 31) >> 5);
				ZoneBuffer::ClearPointer(&dest->cellCasterBits[1]);
			}

			if (data->sceneDynModel)
			{
				buf->align(3);
				buf->write(data->sceneDynModel, data->dpvsDyn.dynEntClientCount[0]);
				ZoneBuffer::ClearPointer(&dest->sceneDynModel);
			}

			if (data->sceneDynBrush)
			{
				buf->align(3);
				buf->write(data->sceneDynBrush, data->dpvsDyn.dynEntClientCount[1]);
				ZoneBuffer::ClearPointer(&dest->sceneDynBrush);
			}

			if (data->primaryLightEntityShadowVis)
			{
				buf->align(3);
				buf->write(data->primaryLightEntityShadowVis,
				           (data->primaryLightCount - data->sunPrimaryLightIndex - 1) << 15);
				ZoneBuffer::ClearPointer(&dest->primaryLightEntityShadowVis);
			}

			if (data->primaryLightDynEntShadowVis[0])
			{
				buf->align(3);
				buf->write(data->primaryLightDynEntShadowVis[0],
				           data->dpvsDyn.dynEntClientCount[0] * (data->primaryLightCount - data->sunPrimaryLightIndex -
					           1));
				ZoneBuffer::ClearPointer(&dest->primaryLightDynEntShadowVis[0]);
			}

			if (data->primaryLightDynEntShadowVis[1])
			{
				buf->align(3);
				buf->write(data->primaryLightDynEntShadowVis[1],
				           data->dpvsDyn.dynEntClientCount[1] * (data->primaryLightCount - data->sunPrimaryLightIndex -
					           1));
				ZoneBuffer::ClearPointer(&dest->primaryLightDynEntShadowVis[1]);
			}

			if (data->primaryLightForModelDynEnt)
			{
				buf->align(0);
				buf->write(data->primaryLightForModelDynEnt, data->dpvsDyn.dynEntClientCount[0]);
				ZoneBuffer::ClearPointer(&dest->primaryLightForModelDynEnt);
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
						ZoneBuffer::ClearPointer(&shadow_geometry[i].sortedSurfIndex);
					}
					if (data->shadowGeom[i].smodelIndex)
					{
						buf->align(1);
						buf->write_p(data->shadowGeom[i].smodelIndex, data->shadowGeom[i].smodelCount);
						ZoneBuffer::ClearPointer(&shadow_geometry[i].smodelIndex);
					}
				}

				ZoneBuffer::ClearPointer(&dest->shadowGeom);
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
								ZoneBuffer::ClearPointer(&light_region_hull[i2].axis);
							}
						}

						ZoneBuffer::ClearPointer(&light_region[i].hulls);
					}
				}

				ZoneBuffer::ClearPointer(&dest->lightRegion);
			}

			buf->push_stream(2);
			if (data->dpvs.smodelVisData[0])
			{
				buf->align(0);
				buf->write(data->dpvs.smodelVisData[0], data->dpvs.smodelCount);
				ZoneBuffer::ClearPointer(&dest->dpvs.smodelVisData[0]);
			}

			if (data->dpvs.smodelVisData[1])
			{
				buf->align(0);
				buf->write(data->dpvs.smodelVisData[1], data->dpvs.smodelCount);
				ZoneBuffer::ClearPointer(&dest->dpvs.smodelVisData[1]);
			}

			if (data->dpvs.smodelVisData[2])
			{
				buf->align(0);
				buf->write(data->dpvs.smodelVisData[2], data->dpvs.smodelCount);
				ZoneBuffer::ClearPointer(&dest->dpvs.smodelVisData[2]);
			}

			if (data->dpvs.surfaceVisData[0])
			{
				buf->align(0);
				buf->write(data->dpvs.surfaceVisData[0], data->dpvs.staticSurfaceCount);
				ZoneBuffer::ClearPointer(&dest->dpvs.surfaceVisData[0]);
			}

			if (data->dpvs.surfaceVisData[1])
			{
				buf->align(0);
				buf->write(data->dpvs.surfaceVisData[1], data->dpvs.staticSurfaceCount);
				ZoneBuffer::ClearPointer(&dest->dpvs.surfaceVisData[1]);
			}

			if (data->dpvs.surfaceVisData[2])
			{
				buf->align(0);
				buf->write(data->dpvs.surfaceVisData[2], data->dpvs.staticSurfaceCount);
				ZoneBuffer::ClearPointer(&dest->dpvs.surfaceVisData[2]);
			}
			buf->pop_stream();

			if (data->dpvs.sortedSurfIndex)
			{
				buf->align(1);
				buf->write_p(data->dpvs.sortedSurfIndex,
				             data->dpvs.staticSurfaceCount + data->dpvs.staticSurfaceCountNoDecal);
				ZoneBuffer::ClearPointer(&dest->dpvs.sortedSurfIndex);
			}

			if (data->dpvs.smodelInsts)
			{
				buf->align(3);
				buf->write(data->dpvs.smodelInsts, data->dpvs.smodelCount);
				ZoneBuffer::ClearPointer(&dest->dpvs.smodelInsts);
			}

			if (data->dpvs.surfaces)
			{
				buf->align(3);
				auto surface = buf->write(data->dpvs.surfaces, data->indexCount);

				for (std::int32_t i = 0; i < data->indexCount; i++)
				{
					if (data->dpvs.surfaces[i].material)
					{
						surface[i].material = reinterpret_cast<Material*>(zone->GetAssetPointer(
							material, data->dpvs.surfaces[i].material->name));
					}
				}

				ZoneBuffer::ClearPointer(&dest->dpvs.surfaces);
			}

			if (data->dpvs.cullGroups)
			{
				buf->align(3);
				buf->write(data->dpvs.cullGroups, data->indexCount);
				ZoneBuffer::ClearPointer(&dest->dpvs.cullGroups);
			}

			if (data->dpvs.smodelDrawInsts)
			{
				buf->align(3);
				auto static_model_draw_inst = buf->write(data->dpvs.smodelDrawInsts, data->dpvs.smodelCount);

				for (std::uint32_t i = 0; i < data->dpvs.smodelCount; i++)
				{
					if (data->dpvs.smodelDrawInsts[i].model)
					{
						static_model_draw_inst[i].model = reinterpret_cast<XModel*>(zone->GetAssetPointer(
							xmodel, data->dpvs.smodelDrawInsts[i].model->name));
					}
				}

				ZoneBuffer::ClearPointer(&dest->dpvs.smodelDrawInsts);
			}

			buf->push_stream(2);
			if (data->dpvs.surfaceMaterials)
			{
				buf->align(7);
				buf->write(data->dpvs.surfaceMaterials, data->indexCount);
				ZoneBuffer::ClearPointer(&dest->dpvs.smodelDrawInsts);
			}

			if (data->dpvs.surfaceCastsSunShadow)
			{
				buf->align(127);
				buf->write(data->dpvs.surfaceCastsSunShadow, data->dpvs.surfaceVisDataCount);
				ZoneBuffer::ClearPointer(&dest->dpvs.surfaceCastsSunShadow);
			}

			if (data->dpvsDyn.dynEntCellBits[0])
			{
				buf->align(3);
				buf->write(data->dpvsDyn.dynEntCellBits[0],
				           data->dpvsDyn.dynEntClientWordCount[0] * data->dpvsPlanes.cellCount);
				ZoneBuffer::ClearPointer(&dest->dpvsDyn.dynEntCellBits[0]);
			}

			if (data->dpvsDyn.dynEntCellBits[1])
			{
				buf->align(3);
				buf->write(data->dpvsDyn.dynEntCellBits[1],
				           data->dpvsDyn.dynEntClientWordCount[1] * data->dpvsPlanes.cellCount);
				ZoneBuffer::ClearPointer(&dest->dpvsDyn.dynEntCellBits[1]);
			}

			if (data->dpvsDyn.dynEntVisData[0][0])
			{
				buf->align(15);
				buf->write(data->dpvsDyn.dynEntVisData[0][0], 32 * data->dpvsDyn.dynEntClientWordCount[0]);
				ZoneBuffer::ClearPointer(&dest->dpvsDyn.dynEntVisData[0][0]);
			}

			if (data->dpvsDyn.dynEntVisData[1][0])
			{
				buf->align(15);
				buf->write(data->dpvsDyn.dynEntVisData[1][0], 32 * data->dpvsDyn.dynEntClientWordCount[1]);
				ZoneBuffer::ClearPointer(&dest->dpvsDyn.dynEntVisData[1][0]);
			}

			if (data->dpvsDyn.dynEntVisData[0][1])
			{
				buf->align(15);
				buf->write(data->dpvsDyn.dynEntVisData[0][1], 32 * data->dpvsDyn.dynEntClientWordCount[0]);
				ZoneBuffer::ClearPointer(&dest->dpvsDyn.dynEntVisData[0][1]);
			}

			if (data->dpvsDyn.dynEntVisData[1][1])
			{
				buf->align(15);
				buf->write(data->dpvsDyn.dynEntVisData[1][1], 32 * data->dpvsDyn.dynEntClientWordCount[1]);
				ZoneBuffer::ClearPointer(&dest->dpvsDyn.dynEntVisData[1][1]);
			}

			if (data->dpvsDyn.dynEntVisData[0][2])
			{
				buf->align(15);
				buf->write(data->dpvsDyn.dynEntVisData[0][2], 32 * data->dpvsDyn.dynEntClientWordCount[0]);
				ZoneBuffer::ClearPointer(&dest->dpvsDyn.dynEntVisData[0][2]);
			}

			if (data->dpvsDyn.dynEntVisData[1][2])
			{
				buf->align(15);
				buf->write(data->dpvsDyn.dynEntVisData[1][2], 32 * data->dpvsDyn.dynEntClientWordCount[1]);
				ZoneBuffer::ClearPointer(&dest->dpvsDyn.dynEntVisData[1][2]);
			}
			buf->pop_stream();

			if (data->heroLights)
			{
				buf->align(3);
				buf->write(data->heroLights, data->heroLightCount);
				ZoneBuffer::ClearPointer(&dest->heroLights);
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
			if (!write.Open(asset->name + ".gfxmap"s))
			{
				return;
			}

			write.String(asset->name);
			write.String(asset->baseName);

			write.Int(asset->planeCount);
			write.Int(asset->nodeCount);
			write.Int(asset->indexCount);
			write.Int(asset->skyCount);

			write.Array(asset->skies, asset->skyCount);
			for (unsigned int i = 0; i < asset->skyCount; i++)
			{
				write.Array(asset->skies[i].skyStartSurfs, asset->skies[i].skySurfCount);
				write.Asset(asset->skies[i].skyImage);
			}

			write.Int(asset->sunPrimaryLightIndex);
			write.Int(asset->primaryLightCount);
			write.Int(asset->primaryLightEnvCount);

			write.Array(asset->unknown1, 12);

			// dpvsplanes
			write.Int(asset->dpvsPlanes.cellCount);
			write.Array(asset->dpvsPlanes.planes, asset->planeCount);
			write.Array(asset->dpvsPlanes.nodes, asset->nodeCount);
			write.Array(asset->dpvsPlanes.sceneEntCellBits, asset->dpvsPlanes.cellCount << 11);

			// dpvs
			write.Array(&asset->dpvs, 1);
			write.Array(asset->dpvs.smodelVisData[0], asset->dpvs.smodelCount);
			write.Array(asset->dpvs.smodelVisData[1], asset->dpvs.smodelCount);
			write.Array(asset->dpvs.smodelVisData[2], asset->dpvs.smodelCount);
			write.Array(asset->dpvs.surfaceVisData[0], asset->dpvs.staticSurfaceCount);
			write.Array(asset->dpvs.surfaceVisData[1], asset->dpvs.staticSurfaceCount);
			write.Array(asset->dpvs.surfaceVisData[2], asset->dpvs.staticSurfaceCount);
			write.Array(asset->dpvs.sortedSurfIndex,
			            (asset->dpvs.staticSurfaceCount + asset->dpvs.staticSurfaceCountNoDecal));
			write.Array(asset->dpvs.smodelInsts, asset->dpvs.smodelCount);
			write.Array(asset->dpvs.surfaces, asset->indexCount);

			for (int i = 0; i < asset->indexCount; i++)
			{
				write.Asset(asset->dpvs.surfaces[i].material);
			}

			write.Array(asset->dpvs.cullGroups, asset->indexCount);
			write.Array(asset->dpvs.smodelDrawInsts, asset->dpvs.smodelCount);

			for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
			{
				write.Asset(asset->dpvs.smodelDrawInsts[i].model);
			}
			write.Array(asset->dpvs.surfaceMaterials, asset->indexCount);
			write.Array(asset->dpvs.surfaceCastsSunShadow, asset->dpvs.surfaceVisDataCount);

			// dpvsDyn
			write.Array(&asset->dpvsDyn, 1);
			write.Array(asset->dpvsDyn.dynEntCellBits[0],
			            asset->dpvsDyn.dynEntClientWordCount[0] * asset->dpvsPlanes.cellCount);
			write.Array(asset->dpvsDyn.dynEntCellBits[1],
			            asset->dpvsDyn.dynEntClientWordCount[1] * asset->dpvsPlanes.cellCount);
			write.Array(asset->dpvsDyn.dynEntVisData[0][0], asset->dpvsDyn.dynEntClientWordCount[0] * 32);
			write.Array(asset->dpvsDyn.dynEntVisData[1][0], asset->dpvsDyn.dynEntClientWordCount[1] * 32);
			write.Array(asset->dpvsDyn.dynEntVisData[0][1], asset->dpvsDyn.dynEntClientWordCount[0] * 32);
			write.Array(asset->dpvsDyn.dynEntVisData[1][1], asset->dpvsDyn.dynEntClientWordCount[1] * 32);
			write.Array(asset->dpvsDyn.dynEntVisData[0][2], asset->dpvsDyn.dynEntClientWordCount[0] * 32);
			write.Array(asset->dpvsDyn.dynEntVisData[1][2], asset->dpvsDyn.dynEntClientWordCount[1] * 32);

			// aabbTreeCount
			write.Array(asset->aabbTreeCounts, asset->dpvsPlanes.cellCount);

			// GfxCellTree
			write.Array(asset->aabbTree, asset->dpvsPlanes.cellCount);
			for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
			{
				write.Array(asset->aabbTree[i].aabbtree, asset->aabbTreeCounts[i].aabbTreeCount);

				for (int j = 0; j < asset->aabbTreeCounts[i].aabbTreeCount; j++)
				{
					write.Array(asset->aabbTree[i].aabbtree[j].smodelIndexes,
					            asset->aabbTree[i].aabbtree[j].smodelIndexCount);
				}
			}

			// read GFX cells
			write.Array(asset->cells, asset->dpvsPlanes.cellCount);

			for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
			{
				write.Array(asset->cells[i].portals, asset->cells[i].portalCount);
				for (int j = 0; j < asset->cells[i].portalCount; j++)
				{
					write.Array(asset->cells[i].portals[j].vertices, asset->cells[i].portals[j].vertexCount);
				}
				write.Array(asset->cells[i].reflectionProbes, asset->cells[i].reflectionProbeCount);
				write.Array(asset->cells[i].reflectionProbeReferences, asset->cells[i].reflectionProbeReferenceCount);
			}

			// worldDraw
			write.Array(&asset->worldDraw, 1);

			for (unsigned int i = 0; i < asset->worldDraw.reflectionProbeCount; i++)
			{
				write.Asset(asset->worldDraw.reflectionImages[i]);
			}

			write.Array(asset->worldDraw.reflectionProbes, asset->worldDraw.reflectionProbeCount);

			write.Array(asset->worldDraw.reflectionProbeReferences, asset->worldDraw.reflectionProbeReferenceCount);
			write.Array(asset->worldDraw.reflectionProbeReferenceOrigins,
			            asset->worldDraw.reflectionProbeReferenceCount);

			write.Array(asset->worldDraw.reflectionProbeTextures, asset->worldDraw.reflectionProbeCount);
			write.Array(asset->worldDraw.lightmaps, asset->worldDraw.lightmapCount);

			for (int i = 0; i < asset->worldDraw.lightmapCount; i++)
			{
				write.Asset(asset->worldDraw.lightmaps[i].primary);
				write.Asset(asset->worldDraw.lightmaps[i].secondary);
			}

			write.Array(asset->worldDraw.lightmapPrimaryTextures, asset->worldDraw.lightmapCount);
			write.Array(asset->worldDraw.lightmapSecondaryTextures, asset->worldDraw.lightmapCount);

			write.Asset(asset->worldDraw.skyImage);
			write.Asset(asset->worldDraw.outdoorImage);

			write.Array(asset->worldDraw.vd.vertices, asset->worldDraw.vertexCount);
			write.Array(asset->worldDraw.vld.data, asset->worldDraw.vertexLayerDataSize);
			write.Array(asset->worldDraw.indices, asset->worldDraw.indexCount);

			// GfxLightGrid
			write.Array(&asset->lightGrid, 1);
			write.Array(asset->lightGrid.rowDataStart,
			            (asset->lightGrid.maxs[asset->lightGrid.rowAxis] - asset->lightGrid.mins[asset
			                                                                                     ->lightGrid.rowAxis] +
				            1));
			write.Array(asset->lightGrid.rawRowData, asset->lightGrid.rawRowDataSize);
			write.Array(asset->lightGrid.entries, asset->lightGrid.entryCount);
			write.Array(asset->lightGrid.colors, asset->lightGrid.colorCount);

			// models
			write.Int(asset->modelCount);
			write.Array(asset->models, asset->modelCount);

			// mins/maxs
			write.Array(asset->mins, 3);
			write.Array(asset->maxs, 3);

			write.Int(asset->checksum);

			// materialmemory
			write.Int(asset->materialMemoryCount);
			write.Array(asset->materialMemory, asset->materialMemoryCount);
			for (int i = 0; i < asset->materialMemoryCount; i++)
			{
				write.Asset(asset->materialMemory[i].material);
			}

			// sun data
			write.Array(&asset->sun, 1);
			write.Asset(asset->sun.spriteMaterial);
			write.Asset(asset->sun.flareMaterial);

			// outdoor shit
			write.Array(asset->outdoorLookupMatrix, 4);
			write.Asset(asset->outdoorImage);

			// CellcasterBits
			write.Array(asset->cellCasterBits[0],
			            asset->dpvsPlanes.cellCount * ((asset->dpvsPlanes.cellCount + 31) >> 5));
			write.Array(asset->cellCasterBits[1], (asset->dpvsPlanes.cellCount + 31) >> 5);

			// SceneDynModel
			write.Array(asset->sceneDynModel, asset->dpvsDyn.dynEntClientCount[0]);

			// SceneDynBrush
			write.Array(asset->sceneDynBrush, asset->dpvsDyn.dynEntClientCount[1]);

			// PrimaryLightEntityShadowVis
			write.Array(asset->primaryLightEntityShadowVis,
			            (asset->primaryLightCount - asset->sunPrimaryLightIndex - 1) << 15);

			// PrimaryLightDynEntShadowVis
			write.Array(asset->primaryLightDynEntShadowVis[0],
			            asset->dpvsDyn.dynEntClientCount[0] * (asset->primaryLightCount - asset->sunPrimaryLightIndex -
				            1));
			write.Array(asset->primaryLightDynEntShadowVis[1],
			            asset->dpvsDyn.dynEntClientCount[1] * (asset->primaryLightCount - asset->sunPrimaryLightIndex -
				            1));

			// PrimaryLightForModelDynEnt
			write.Array(asset->primaryLightForModelDynEnt, asset->dpvsDyn.dynEntClientCount[0]);

			// GfxShadowGeometry
			write.Array(asset->shadowGeom, asset->primaryLightCount);
			for (int i = 0; i < asset->primaryLightCount; i++)
			{
				write.Array(asset->shadowGeom[i].sortedSurfIndex, asset->shadowGeom[i].surfaceCount);
				write.Array(asset->shadowGeom[i].smodelIndex, asset->shadowGeom[i].smodelCount);
			}

			// GfxLightRegion
			write.Array(asset->lightRegion, asset->primaryLightCount);
			for (int i = 0; i < asset->primaryLightCount; i++)
			{
				write.Array(asset->lightRegion[i].hulls, asset->lightRegion[i].hullCount);
				for (unsigned int j = 0; j < asset->lightRegion[i].hullCount; j++)
				{
					write.Array(asset->lightRegion[i].hulls[j].axis, asset->lightRegion[i].hulls[j].axisCount);
				}
			}

			write.Int(asset->mapVtxChecksum);
			write.Int(asset->fogTypesAllowed);

			// save file to disk!
			write.Close();
		}
	}
}
