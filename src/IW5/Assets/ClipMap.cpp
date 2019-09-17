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
#define INFO info->

		void parse_info(ClipInfo* info, AssetReader& read, std::shared_ptr<ZoneMemory>& mem)
		{
			INFO numCPlanes = read.Int();
			INFO cPlanes = read.Array<cplane_s>();

			INFO numMaterials = read.Int();
			INFO materials = read.Array<dmaterial_t>();
			for (auto i = 0; i < INFO numMaterials; i++)
			{
				if (INFO materials[i].material)
				{
					INFO materials[i].material = read.String();
				}
			}

			INFO numCBrushSides = read.Int();
			INFO cBrushSides = read.Array<cbrushside_t>();
			for (auto i = 0; i < INFO numCBrushSides; i++)
			{
				if (INFO cBrushSides[i].plane)
				{
					INFO cBrushSides[i].plane = read.Array<cplane_s>();
				}
			}

			INFO numCBrushEdges = read.Int();
			INFO cBrushEdges = read.Array<cbrushedge_t>();

			INFO numLeafBrushes = read.Int();
			INFO leafBrushes = read.Array<short>();

			INFO numCLeafBrushNodes = read.Int();
			INFO cLeafBrushNodes = read.Array<cLeafBrushNode_s>();
			for (auto i = 0; i < INFO numCLeafBrushNodes; i++)
			{
				if (INFO cLeafBrushNodes[i].leafBrushCount > 0)
				{
					INFO cLeafBrushNodes[i].data.leaf.brushes = read.Array<unsigned short>();
				}
			}

			INFO numBrushes = read.Int();
			INFO brushes = read.Array<cbrush_t>();
			for (auto i = 0; i < INFO numBrushes; i++)
			{
				if (INFO brushes[i].sides)
				{
					INFO brushes[i].sides = read.Array<cbrushside_t>();

					if (INFO brushes[i].sides->plane)
					{
						INFO brushes[i].sides->plane = read.Array<cplane_s>();
					}
				}
				if (INFO brushes[i].edge)
				{
					INFO brushes[i].edge = read.Array<cbrushedge_t>();
				}
			}

			INFO brushBounds = read.Array<Bounds>();
			INFO brushContents = read.Array<int>();
		}

#undef INFO
#define INFO info.

		clipMap_t* IClipMap::parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			auto path = name + ".colmap";

			if (!FileSystem::FileExists(path))
			{
				return nullptr;
			}

			AssetReader read(mem);

			if (!read.Open(path))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing colmap \"%s\"...", name.c_str());

#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW5::IClipMap::parse");
#endif

			auto colmap = mem->Alloc<clipMap_t>();

			colmap->name = read.String();
			colmap->isInUse = read.Int();

			parse_info(&colmap->info, read, mem);

			colmap->numStaticModels = read.Int();
			colmap->staticModelList = read.Array<cStaticModel_s>();
			for (auto i = 0; i < colmap->numStaticModels; i++)
			{
				if (colmap->staticModelList[i].xmodel)
				{
					colmap->staticModelList[i].xmodel = read.Asset<XModel>();
				}
			}

			colmap->numCNodes = read.Int();
			colmap->cNodes = read.Array<cNode_t>();
			for (auto i = 0; i < colmap->numCNodes; i++)
			{
				if (colmap->cNodes[i].plane)
				{
					colmap->cNodes[i].plane = read.Array<cplane_s>();
				}
			}

			colmap->numCLeaf = read.Int();
			colmap->cLeaf = read.Array<cLeaf_t>();

			colmap->numVerts = read.Int();
			colmap->verts = read.Array<VecInternal<3>>();

			colmap->numTriIndices = read.Int();
			colmap->triIndices = read.Array<short>();
			colmap->triEdgeIsWalkable = read.Array<char>();

			colmap->numCollisionBorders = read.Int();
			colmap->collisionBorders = read.Array<CollisionBorder>();

			colmap->numCollisionPartitions = read.Int();
			colmap->collisionPartitions = read.Array<CollisionPartition>();
			for (auto i = 0; i < colmap->numCollisionPartitions; i++)
			{
				if (colmap->collisionPartitions[i].borders)
				{
					colmap->collisionPartitions[i].borders = read.Array<CollisionBorder>();
				}
			}

			colmap->numCollisionAABBTrees = read.Int();
			colmap->collisionAABBTrees = read.Array<CollisionAabbTree>();

			colmap->numCModels = read.Int();
			colmap->cModels = read.Array<cmodel_t>();

			for (auto i = 0; i < colmap->numCModels; i++)
			{
				if (colmap->cModels[i].info)
				{
					colmap->cModels[i].info = mem->Alloc<ClipInfo>();
					parse_info(colmap->cModels[i].info, read, mem);
				}
			}

			colmap->mapEnts = read.Asset<MapEnts>();
			colmap->mapEnts->name = colmap->name;
			colmap->smodelNodeCount = read.Int();
			colmap->smodelNodes = read.Array<SModelAabbNode>();

#ifdef IW4
			colmap->dynEntCount[0] = read.Int();
			DynEntityDef_IW5* newEntDef = read.Array<DynEntityDef_IW5>();
			colmap->dynEntDefList[0] = new DynEntityDef[colmap->dynEntCount[0]];
#define dynEntDefCopy(_num1, _num2, _item) \
colmap->dynEntDefList[_num1][_num2]._item = newEntDef[_num2]._item;

			for (Int i = 0; i < colmap->dynEntCount[0]; i++)
			{
				dynEntDefCopy(0, i, type);
				dynEntDefCopy(0, i, pose.quat[0]);
				dynEntDefCopy(0, i, pose.quat[1]);
				dynEntDefCopy(0, i, pose.quat[2]);
				dynEntDefCopy(0, i, pose.quat[3]);
				dynEntDefCopy(0, i, pose.origin[0]);
				dynEntDefCopy(0, i, pose.origin[1]);
				dynEntDefCopy(0, i, pose.origin[2]);
				dynEntDefCopy(0, i, health);
				dynEntDefCopy(0, i, mass.centerOfMass[0]);
				dynEntDefCopy(0, i, mass.centerOfMass[1]);
				dynEntDefCopy(0, i, mass.centerOfMass[2]);
				dynEntDefCopy(0, i, mass.momentsOfInertia[0]);
				dynEntDefCopy(0, i, mass.momentsOfInertia[1]);
				dynEntDefCopy(0, i, mass.momentsOfInertia[2]);
				dynEntDefCopy(0, i, mass.productsOfInertia[0]);
				dynEntDefCopy(0, i, mass.productsOfInertia[1]);
				dynEntDefCopy(0, i, mass.productsOfInertia[2]);
				dynEntDefCopy(0, i, mass.contents);
				dynEntDefCopy(0, i, xModel);
				dynEntDefCopy(0, i, physPreset);
				colmap->dynEntDefList[0][i].destroyFx = nullptr;
				if (colmap->dynEntDefList[0][i].xModel)
				{
					colmap->dynEntDefList[0][i].xModel = read.Asset<XModel>();
				}
				if (colmap->dynEntDefList[0][i].physPreset)
				{
					colmap->dynEntDefList[0][i].physPreset = read.Asset<PhysPreset>();
				}
			}

			colmap->dynEntCount[1] = read.Int();
			newEntDef = read.Array<DynEntityDef_IW5>();
			colmap->dynEntDefList[1] = new DynEntityDef[colmap->dynEntCount[1]];
			for (Int i = 0; i < colmap->dynEntCount[1]; i++)
			{
				dynEntDefCopy(1, i, type);
				dynEntDefCopy(1, i, pose.quat[0]);
				dynEntDefCopy(1, i, pose.quat[1]);
				dynEntDefCopy(1, i, pose.quat[2]);
				dynEntDefCopy(1, i, pose.quat[3]);
				dynEntDefCopy(1, i, pose.origin[0]);
				dynEntDefCopy(1, i, pose.origin[1]);
				dynEntDefCopy(1, i, pose.origin[2]);
				dynEntDefCopy(1, i, health);
				dynEntDefCopy(1, i, mass.centerOfMass[0]);
				dynEntDefCopy(1, i, mass.centerOfMass[1]);
				dynEntDefCopy(1, i, mass.centerOfMass[2]);
				dynEntDefCopy(1, i, mass.momentsOfInertia[0]);
				dynEntDefCopy(1, i, mass.momentsOfInertia[1]);
				dynEntDefCopy(1, i, mass.momentsOfInertia[2]);
				dynEntDefCopy(1, i, mass.productsOfInertia[0]);
				dynEntDefCopy(1, i, mass.productsOfInertia[1]);
				dynEntDefCopy(1, i, mass.productsOfInertia[2]);
				dynEntDefCopy(1, i, mass.contents);
				colmap->dynEntDefList[1][i].destroyFx = nullptr;
				if (colmap->dynEntDefList[1][i].xModel)
				{
					colmap->dynEntDefList[1][i].xModel = read.Asset<XModel>();
				}
				if (colmap->dynEntDefList[1][i].physPreset)
				{
					colmap->dynEntDefList[1][i].physPreset = read.Asset<PhysPreset>();
				}
			}

			colmap->dynEntPoseList[0] = new DynEntityPose[colmap->dynEntCount[0]];
			colmap->dynEntPoseList[1] = new DynEntityPose[colmap->dynEntCount[1]];
			colmap->dynEntClientList[0] = new DynEntityClient[colmap->dynEntCount[0]];
			colmap->dynEntClientList[1] = new DynEntityClient[colmap->dynEntCount[1]];
			colmap->dynEntCollList[0] = new DynEntityColl[colmap->dynEntCount[0]];
			colmap->dynEntCollList[1] = new DynEntityColl[colmap->dynEntCount[1]];
#endif

			// parse stages
			AssetReader stageReader(mem);
			if (stageReader.Open(name + ".ents.stages"))
			{
				ZONETOOL_INFO("Parsing entity stages...");

				colmap->stageCount = stageReader.Int();
				if (colmap->stageCount)
				{
					colmap->stages = stageReader.Array<Stage>();

					for (int i = 0; i < colmap->stageCount; i++)
					{
						colmap->stages[i].name = stageReader.String();
					}
				}
			}
			stageReader.Close();

			// copy info into pInfo
			colmap->pInfo = &colmap->info;
			/*colmap->pInfo = mem->Alloc<ClipInfo>();
			memcpy(colmap->pInfo, &colmap->info, sizeof ClipInfo);*/

#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif

			return colmap;
		}

		IClipMap::IClipMap()
		{
		}

		IClipMap::~IClipMap()
		{
		}

		void IClipMap::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			this->m_name = "maps/mp/" + currentzone + ".d3dbsp"; // name;
			this->m_asset = this->parse(name, mem);
			this->m_filename = name;

			if (!this->m_asset)
			{
				this->m_asset = DB_FindXAssetHeader(this->type(), name.data(), 1).clipmap;
			}
		}

		void IClipMap::prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
		{
		}

		void IClipMap::load_depending(IZone* zone)
		{
#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW5::IClipMap::load_depending");
#endif

			auto data = this->m_asset;

			if (data->staticModelList)
			{
				for (int i = 0; i < data->numStaticModels; i++)
				{
					if (data->staticModelList[i].xmodel)
					{
						zone->AddAssetOfType(xmodel, data->staticModelList[i].xmodel->name);
					}
				}
			}

			if (data->dynEntDefList[0])
			{
				for (int i = 0; i < data->dynEntCount[0]; i++)
				{
					if (data->dynEntDefList[0][i].xModel)
					{
						zone->AddAssetOfType(xmodel, data->dynEntDefList[0][i].xModel->name);
					}
					if (data->dynEntDefList[0][i].destroyFx)
					{
						zone->AddAssetOfType(fx, data->dynEntDefList[0][i].destroyFx->name);
					}
					if (data->dynEntDefList[0][i].physPreset)
					{
						zone->AddAssetOfType(physpreset, data->dynEntDefList[0][i].physPreset->name);
					}
				}
			}

			if (data->dynEntDefList[1])
			{
				for (int i = 0; i < data->dynEntCount[1]; i++)
				{
					if (data->dynEntDefList[1][i].xModel)
					{
						zone->AddAssetOfType(xmodel, data->dynEntDefList[1][i].xModel->name);
					}
					if (data->dynEntDefList[1][i].destroyFx)
					{
						zone->AddAssetOfType(fx, data->dynEntDefList[1][i].destroyFx->name);
					}
					if (data->dynEntDefList[1][i].physPreset)
					{
						zone->AddAssetOfType(physpreset, data->dynEntDefList[1][i].physPreset->name);
					}
				}
			}

			if (data->mapEnts)
			{
				zone->AddAssetOfType(map_ents, this->m_asset->mapEnts->name);
			}

#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif
		}

		std::string IClipMap::name()
		{
			return this->m_name;
		}

		std::int32_t IClipMap::type()
		{
			return col_map_mp;
		}

		void IClipMap::write_info(IZone* zone, std::shared_ptr<ZoneBuffer>& buf, ClipInfo* data, ClipInfo* dest)
		{
#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW5::IClipMap::write_info");
#endif

			if (data->cPlanes)
			{
				dest->cPlanes = buf->write_s(3, data->cPlanes, data->numCPlanes);
				ZONETOOL_INFO("cPlanes pointer is 0x%08X", dest->cPlanes);
			}

			if (data->materials)
			{
				dmaterial_t* dmaterial;
				dest->materials = buf->write_s(3, data->materials, data->numMaterials, sizeof dmaterial_t, &dmaterial);

				if (dest->materials == reinterpret_cast<dmaterial_t*>(-1))
				{
					for (std::int32_t i = 0; i < data->numMaterials; i++)
					{
						if (data->materials[i].material)
						{
							dmaterial[i].material = buf->write_str(data->materials[i].material);
						}
					}
				}
				ZONETOOL_INFO("materials pointer is 0x%08X", dest->materials);
			}

			if (data->cBrushSides)
			{
				cbrushside_t* brush_side;
				dest->cBrushSides = buf->write_s(3, data->cBrushSides, data->numCBrushSides, sizeof cbrushside_t,
				                                 &brush_side);

				if (dest->cBrushSides == reinterpret_cast<cbrushside_t*>(-1))
				{
					for (std::int32_t i = 0; i < data->numCBrushSides; i++)
					{
						if (data->cBrushSides[i].plane)
						{
							brush_side[i].plane = buf->write_s(3, data->cBrushSides[i].plane);
						}
					}
				}
				ZONETOOL_INFO("cBrushSides pointer is 0x%08X", dest->cBrushSides);
			}

			if (data->cBrushEdges)
			{
				/*buf->write_p(data->cBrushEdges, data->numCBrushEdges);
				ZoneBuffer::ClearPointer(&);*/
				dest->cBrushEdges = buf->write_s(0, data->cBrushEdges, data->numCBrushEdges);
				ZONETOOL_INFO("cBrushEdges pointer is 0x%08X", dest->cBrushEdges);
			}

			if (data->cLeafBrushNodes)
			{
				cLeafBrushNode_s* leaf_brush_node = nullptr;
				dest->cLeafBrushNodes = buf->write_s(3, data->cLeafBrushNodes, data->numCLeafBrushNodes,
				                                     sizeof cLeafBrushNode_s, &leaf_brush_node);

				if (dest->cLeafBrushNodes == reinterpret_cast<cLeafBrushNode_s*>(-1))
				{
					for (std::int32_t i = 0; i < data->numCLeafBrushNodes; i++)
					{
						if (data->cLeafBrushNodes[i].leafBrushCount > 0 && data->cLeafBrushNodes[i].data.leaf.brushes)
						{
							leaf_brush_node[i].data.leaf.brushes = buf->write_s(
								1, data->cLeafBrushNodes[i].data.leaf.brushes, data->cLeafBrushNodes[i].leafBrushCount);
						}
					}
				}
				ZONETOOL_INFO("cLeafBrushNodes pointer is 0x%08X", dest->cLeafBrushNodes);
			}

			if (data->leafBrushes)
			{
				/*buf->align(1);
				buf->write(data->leafBrushes, data->numLeafBrushes);
				ZoneBuffer::ClearPointer(&dest->leafBrushes);*/
				dest->leafBrushes = buf->write_s(1, data->leafBrushes, data->numLeafBrushes);
				ZONETOOL_INFO("leafBrushes pointer is 0x%08X", dest->leafBrushes);
			}

			// brushes
			if (data->brushes)
			{
				cbrush_t* brush = nullptr;
				dest->brushes = buf->write_s(127, data->brushes, data->numBrushes, sizeof cbrush_t, &brush);

				if (dest->brushes == reinterpret_cast<cbrush_t*>(-1))
				{
					for (int i = 0; i < data->numBrushes; i++)
					{
						if (data->brushes[i].sides)
						{
							cbrushside_t* side = nullptr;
							brush[i].sides = buf->write_s(3, data->brushes[i].sides, 1, sizeof cbrushside_t, &side);

							if (brush[i].sides == (cbrushside_t*)-1 && side)
							{
								if (side->plane)
								{
									side->plane = buf->write_s(3, side->plane);
								}
							}
						}

						if (data->brushes[i].edge)
						{
							brush[i].edge = buf->write_s(0, data->brushes[i].edge);
						}
					}
				}
				ZONETOOL_INFO("brushes pointer is 0x%08X", dest->brushes);
			}

			// brushBounds
			if (data->brushBounds)
			{
				/*buf->align(127);
				buf->write(data->brushBounds, data->numBrushes);
				ZoneBuffer::ClearPointer(&dest->brushBounds);*/
				dest->brushBounds = buf->write_s(127, data->brushBounds, data->numBrushes);
				ZONETOOL_INFO("brushBounds pointer is 0x%08X", dest->brushBounds);
			}

			// brushContents
			if (data->brushContents)
			{
				/*buf->align(3);
				buf->write(data->brushContents, data->numBrushes);
				ZoneBuffer::ClearPointer(&dest->brushContents);*/
				dest->brushContents = buf->write_s(3, data->brushContents, data->numBrushes);
				ZONETOOL_INFO("brushContents pointer is 0x%08X", dest->brushContents);
			}

#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif
		}

		void IClipMap::write(IZone* zone, std::shared_ptr<ZoneBuffer>& buf)
		{
#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW5::IClipMap::write");
#endif

			auto data = this->m_asset;
			auto offset = buf->get_zone_offset();
			auto dest = buf->write<clipMap_t>(data);

			sizeof clipMap_t;

			dest->isPlutoniumMap = 0x13370420;

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			this->write_info(zone, buf, &data->info, &dest->info);

			if (data->pInfo)
			{
				buf->align(3);
				auto destInfo = buf->write(data->pInfo);
				this->write_info(zone, buf, data->pInfo, destInfo);
				ZoneBuffer::ClearPointer(&dest->pInfo);
			}

			if (data->staticModelList)
			{
				buf->align(3);
				auto static_model = buf->write(data->staticModelList, data->numStaticModels);

				for (std::int32_t i = 0; i < data->numStaticModels; i++)
				{
					if (data->staticModelList[i].xmodel)
					{
						static_model[i].xmodel = reinterpret_cast<XModel*>(zone->GetAssetPointer(
							xmodel, data->staticModelList[i].xmodel->name));
					}
				}

				ZoneBuffer::ClearPointer(&data->staticModelList);
			}

			if (data->cNodes)
			{
				buf->align(3);
				auto node = buf->write(data->cNodes, data->numCNodes);

				for (std::int32_t i = 0; i < data->numCNodes; i++)
				{
					if (data->cNodes[i].plane)
					{
						node[i].plane = buf->write_s(3, data->cNodes[i].plane);
					}
				}

				ZoneBuffer::ClearPointer(&dest->cNodes);
			}

			if (data->cLeaf)
			{
				buf->align(3);
				buf->write(data->cLeaf, data->numCLeaf);
				ZoneBuffer::ClearPointer(&dest->cLeaf);
			}

			if (data->verts)
			{
				buf->align(3);
				buf->write(data->verts, data->numVerts);
				ZoneBuffer::ClearPointer(&dest->verts);
			}

			if (data->triIndices)
			{
				buf->align(1);
				buf->write(data->triIndices, data->numTriIndices * 3);
				ZoneBuffer::ClearPointer(&dest->triIndices);
			}

			if (data->triEdgeIsWalkable)
			{
				buf->align(0);
				buf->write(data->triEdgeIsWalkable, 4 * ((3 * data->numTriIndices + 31) >> 5));
				ZoneBuffer::ClearPointer(&dest->triEdgeIsWalkable);
			}

			if (data->collisionBorders)
			{
				buf->align(3);
				buf->write_p(data->collisionBorders, data->numCollisionBorders);
				ZoneBuffer::ClearPointer(&dest->collisionBorders);
			}

			if (data->collisionPartitions)
			{
				buf->align(3);
				auto collision_partition = buf->write(data->collisionPartitions, data->numCollisionPartitions);

				for (std::int32_t i = 0; i < data->numCollisionPartitions; i++)
				{
					if (data->collisionPartitions[i].borders)
					{
						collision_partition[i].borders = buf->write_s(3, data->collisionPartitions[i].borders);
					}
				}

				ZoneBuffer::ClearPointer(&dest->collisionPartitions);
			}

			if (data->collisionAABBTrees)
			{
				buf->align(15);
				buf->write(data->collisionAABBTrees, data->numCollisionAABBTrees);
				ZoneBuffer::ClearPointer(&dest->collisionAABBTrees);
			}

			if (data->cModels)
			{
				buf->align(3);
				auto destCModels = buf->write(data->cModels, data->numCModels);

				for (std::int32_t i = 0; i < data->numCModels; i++)
				{
					buf->push_stream(0);

					if (data->cModels[i].info)
					{
						ClipInfo* destInfo = nullptr;
						destCModels[i].info = buf->write_s(3, data->cModels[i].info, 1, sizeof ClipInfo, &destInfo);

						if (destCModels[i].info == reinterpret_cast<ClipInfo*>(-1))
						{
							this->write_info(zone, buf, data->cModels[i].info, destInfo);
						}
					}

					buf->pop_stream();
				}

				ZoneBuffer::ClearPointer(&dest->cModels);
			}

			if (data->smodelNodes)
			{
				buf->align(3);
				buf->write(data->smodelNodes, data->smodelNodeCount);
				ZoneBuffer::ClearPointer(&dest->smodelNodes);
			}

			if (data->mapEnts)
			{
				dest->mapEnts = reinterpret_cast<MapEnts*>(zone->GetAssetPointer(map_ents, this->name()));
			}

			if (data->stages)
			{
				buf->align(3);
				auto destStages = buf->write(data->stages, data->stageCount);

				for (std::uint8_t i = 0; i < data->stageCount; i++)
				{
					if (data->stages[i].name)
					{
						buf->write_str(data->stages[i].name);
						ZoneBuffer::ClearPointer(&destStages[i].name);
					}
				}

				ZoneBuffer::ClearPointer(&dest->stages);
			}

			// copy trigger data from mapents
			memcpy(&dest->trigger, &data->mapEnts->trigger, sizeof MapTriggers);

			// write triggers
			IMapEnts::write_triggers(zone, buf, &dest->trigger);

			if (data->dynEntDefList[0])
			{
				buf->align(3);
				auto dyn_entity_def = buf->write(data->dynEntDefList[0], data->dynEntCount[0]);

				for (std::uint16_t i = 0; i < data->dynEntCount[0]; i++)
				{
					if (data->dynEntDefList[0][i].xModel)
					{
						dyn_entity_def[i].xModel = reinterpret_cast<XModel*>(zone->GetAssetPointer(
							xmodel, data->dynEntDefList[0][i].xModel->name));
					}
					if (data->dynEntDefList[0][i].destroyFx)
					{
						dyn_entity_def[i].destroyFx = reinterpret_cast<FxEffectDef*>(zone->GetAssetPointer(
							fx, data->dynEntDefList[0][i].destroyFx->name));
					}
					if (data->dynEntDefList[0][i].physPreset)
					{
						dyn_entity_def[i].physPreset = reinterpret_cast<PhysPreset*>(zone->GetAssetPointer(
							physpreset, data->dynEntDefList[0][i].physPreset->name));
					}

					if (data->dynEntDefList[0][i].hinge)
					{
						dyn_entity_def[i].hinge = buf->write_s(3, dyn_entity_def[i].hinge, 1);
					}
				}

				ZoneBuffer::ClearPointer(&dest->dynEntDefList[0]);
			}

			if (data->dynEntDefList[1])
			{
				buf->align(3);
				auto dyn_entity_def = buf->write(data->dynEntDefList[1], data->dynEntCount[1]);

				for (std::uint16_t i = 0; i < data->dynEntCount[1]; i++)
				{
					if (data->dynEntDefList[1][i].xModel)
					{
						dyn_entity_def[i].xModel = reinterpret_cast<XModel*>(zone->GetAssetPointer(
							xmodel, data->dynEntDefList[1][i].xModel->name));
					}
					if (data->dynEntDefList[1][i].destroyFx)
					{
						dyn_entity_def[i].destroyFx = reinterpret_cast<FxEffectDef*>(zone->GetAssetPointer(
							fx, data->dynEntDefList[1][i].destroyFx->name));
					}
					if (data->dynEntDefList[1][i].physPreset)
					{
						dyn_entity_def[i].physPreset = reinterpret_cast<PhysPreset*>(zone->GetAssetPointer(
							physpreset, data->dynEntDefList[1][i].physPreset->name));
					}

					if (data->dynEntDefList[1][i].hinge)
					{
						dyn_entity_def[i].hinge = buf->write_s(3, dyn_entity_def[i].hinge, 1);
					}
				}

				ZoneBuffer::ClearPointer(&dest->dynEntDefList[1]);
			}

			buf->push_stream(2);

			if (data->dynEntPoseList[0])
			{
				buf->align(3);
				buf->write(data->dynEntPoseList[0], data->dynEntCount[0]);
				ZoneBuffer::ClearPointer(&dest->dynEntPoseList[0]);
			}

			if (data->dynEntPoseList[1])
			{
				buf->align(3);
				buf->write(data->dynEntPoseList[1], data->dynEntCount[1]);
				ZoneBuffer::ClearPointer(&dest->dynEntPoseList[1]);
			}

			if (data->dynEntClientList[0])
			{
				buf->align(3);
				buf->write(data->dynEntClientList[0], data->dynEntCount[0]);
				ZoneBuffer::ClearPointer(&dest->dynEntClientList[0]);
			}

			if (data->dynEntClientList[1])
			{
				buf->align(3);
				buf->write(data->dynEntClientList[1], data->dynEntCount[1]);
				ZoneBuffer::ClearPointer(&dest->dynEntClientList[1]);
			}

			if (data->dynEntCollList[0])
			{
				buf->align(3);
				buf->write(data->dynEntCollList[0], data->dynEntCount[0]);
				ZoneBuffer::ClearPointer(&dest->dynEntCollList[0]);
			}

			if (data->dynEntCollList[1])
			{
				buf->align(3);
				buf->write(data->dynEntCollList[1], data->dynEntCount[1]);
				ZoneBuffer::ClearPointer(&dest->dynEntCollList[1]);
			}

			buf->pop_stream();

			END_LOG_STREAM;
			buf->pop_stream();

			encrypt_data(dest, sizeof clipMap_t);

#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif
		}

#undef INFO
#define INFO info->

		void dump_info(ClipInfo* info, AssetDumper& write)
		{
			write.Int(INFO numCPlanes);
			write.Array(INFO cPlanes, INFO numCPlanes);

			write.Int(INFO numMaterials);
			write.Array(INFO materials, INFO numMaterials);

			for (auto i = 0; i < INFO numMaterials; i++)
			{
				if (INFO materials[i].material)
				{
					write.String(INFO materials[i].material);
				}
			}

			write.Int(INFO numCBrushSides);
			write.Array(INFO cBrushSides, INFO numCBrushSides);

			for (auto i = 0; i < INFO numCBrushSides; i++)
			{
				if (INFO cBrushSides[i].plane)
				{
					write.Array(INFO cBrushSides[i].plane, 1);
				}
			}

			write.Int(INFO numCBrushEdges);
			write.Array(INFO cBrushEdges, INFO numCBrushEdges);

			write.Int(INFO numLeafBrushes);
			write.Array(INFO leafBrushes, INFO numLeafBrushes);

			write.Int(INFO numCLeafBrushNodes);
			write.Array(INFO cLeafBrushNodes, INFO numCLeafBrushNodes);

			for (auto i = 0; i < INFO numCLeafBrushNodes; i++)
			{
				if (INFO cLeafBrushNodes[i].leafBrushCount > 0)
				{
					write.Array(INFO cLeafBrushNodes[i].data.leaf.brushes, INFO cLeafBrushNodes[i].leafBrushCount);
				}
			}

			write.Int(INFO numBrushes);
			write.Array(INFO brushes, INFO numBrushes);

			for (auto i = 0; i < INFO numBrushes; i++)
			{
				if (INFO brushes[i].sides)
				{
					write.Array(INFO brushes[i].sides, 1);

					if (INFO brushes[i].sides->plane)
					{
						write.Array(INFO brushes[i].sides->plane, 1);
					}
				}
				if (INFO brushes[i].edge)
				{
					write.Array(INFO brushes[i].edge, 1);
				}
			}

			write.Array(INFO brushBounds, INFO numBrushes);
			write.Array(INFO brushContents, INFO numBrushes);
		}

		void IClipMap::dump(clipMap_t* asset)
		{
			AssetDumper write;
			if (!write.Open(asset->name + ".colmap"s))
			{
				return;
			}

			write.String(asset->name);
			write.Int(asset->isInUse);

			dump_info(&asset->info, write);

			write.Int(asset->numStaticModels);
			write.Array(asset->staticModelList, asset->numStaticModels);

			for (auto i = 0; i < asset->numStaticModels; i++)
			{
				if (asset->staticModelList[i].xmodel)
				{
					write.Asset(asset->staticModelList[i].xmodel);
				}
			}

			write.Int(asset->numCNodes);
			write.Array(asset->cNodes, asset->numCNodes);

			for (auto i = 0; i < asset->numCNodes; i++)
			{
				if (asset->cNodes[i].plane)
				{
					write.Array(asset->cNodes[i].plane, 1);
				}
			}

			write.Int(asset->numCLeaf);
			write.Array(asset->cLeaf, asset->numCLeaf);

			write.Int(asset->numVerts);
			write.Array(asset->verts, asset->numVerts);

			write.Int(asset->numTriIndices);
			write.Array(asset->triIndices, asset->numTriIndices * 3);
			write.Array(asset->triEdgeIsWalkable, 4 * ((3 * asset->numTriIndices + 31) >> 5));

			write.Int(asset->numCollisionBorders);
			write.Array(asset->collisionBorders, asset->numCollisionBorders);

			write.Int(asset->numCollisionPartitions);
			write.Array(asset->collisionPartitions, asset->numCollisionPartitions);

			for (auto i = 0; i < asset->numCollisionPartitions; i++)
			{
				if (asset->collisionPartitions[i].borders)
				{
					write.Array(asset->collisionPartitions[i].borders, asset->collisionPartitions[i].borderCount);
				}
			}

			write.Int(asset->numCollisionAABBTrees);
			write.Array(asset->collisionAABBTrees, asset->numCollisionAABBTrees);

			write.Int(asset->numCModels);
			write.Array(asset->cModels, asset->numCModels);

			for (int i = 0; i < asset->numCModels; i++)
			{
				if (asset->cModels[i].info)
				{
					dump_info(asset->cModels[i].info, write);
				}
			}

			write.Asset(asset->mapEnts);

			write.Int(asset->smodelNodeCount);
			write.Array(asset->smodelNodes, asset->smodelNodeCount);

			// save file to disk
			write.Close();

			// dump stages
			if (asset->stages)
			{
				AssetDumper stageDumper;

				if (stageDumper.Open(asset->name + ".ents.stages"s))
				{
					stageDumper.Int(asset->stageCount);
					if (asset->stageCount)
					{
						stageDumper.Array(asset->stages, asset->stageCount);

						for (int i = 0; i < asset->stageCount; i++)
						{
							stageDumper.String(asset->stages[i].name);
						}
					}
				}

				stageDumper.Close();
			}
		}
	}
}
