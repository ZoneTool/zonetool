// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "../iw5/Assets/ClipMap.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		clipMap_t* IClipMap::parse(const std::string& name, ZoneMemory* mem)
		{
			auto iw5_colmap = IW5::IClipMap::parse(name, mem);

			if (!iw5_colmap)
			{
				return nullptr;
			}

			// allocate collision map
			auto colmap = mem->Alloc<clipMap_t>();

			// copy data from IW5 to IW4
			colmap->name = iw5_colmap->name;
			colmap->isInUse = iw5_colmap->isInUse;

			colmap->numCPlanes = iw5_colmap->info.numCPlanes;
			colmap->cPlanes = (cplane_s*)iw5_colmap->info.cPlanes;

			colmap->numStaticModels = iw5_colmap->numStaticModels;
			colmap->staticModelList = (cStaticModel_s*)iw5_colmap->staticModelList;

			colmap->numMaterials = iw5_colmap->info.numMaterials;
			colmap->materials = (dmaterial_t*)iw5_colmap->info.materials;

			colmap->numCBrushSides = iw5_colmap->info.numCBrushSides;
			colmap->cBrushSides = (cbrushside_t*)iw5_colmap->info.cBrushSides;

			colmap->numCBrushEdges = iw5_colmap->info.numCBrushEdges;
			colmap->cBrushEdges = (cbrushedge_t*)iw5_colmap->info.cBrushEdges;

			colmap->numCNodes = iw5_colmap->numCNodes;
			colmap->cNodes = (cNode_t*)iw5_colmap->cNodes;

			colmap->numCLeaf = iw5_colmap->numCLeaf;
			colmap->cLeaf = (cLeaf_t*)iw5_colmap->cLeaf;

			colmap->numCLeafBrushNodes = iw5_colmap->info.numCLeafBrushNodes;
			colmap->cLeafBrushNodes = (cLeafBrushNode_s*)iw5_colmap->info.cLeafBrushNodes;

			colmap->numLeafBrushes = iw5_colmap->info.numLeafBrushes;
			colmap->leafBrushes = iw5_colmap->info.leafBrushes;

			// leafSurfaces todo!

			colmap->numVerts = iw5_colmap->numVerts;
			colmap->verts = (VecInternal<3>*)iw5_colmap->verts;

			colmap->numTriIndices = iw5_colmap->numTriIndices;
			colmap->triIndices = iw5_colmap->triIndices;
			colmap->triEdgeIsWalkable = iw5_colmap->triEdgeIsWalkable;

			colmap->numCollisionBorders = iw5_colmap->numCollisionBorders;
			colmap->collisionBorders = (CollisionBorder*)iw5_colmap->collisionBorders;

			colmap->numCollisionPartitions = iw5_colmap->numCollisionPartitions;
			colmap->collisionPartitions = (CollisionPartition*)iw5_colmap->collisionPartitions;

			colmap->numCollisionAABBTrees = iw5_colmap->numCollisionAABBTrees;
			colmap->collisionAABBTrees = (CollisionAabbTree*)iw5_colmap->collisionAABBTrees;

			// cmodels!
			colmap->numCModels = iw5_colmap->numCModels;
			colmap->cModels = new cmodel_t[colmap->numCModels];
			memset(colmap->cModels, 0, sizeof(cmodel_t) * colmap->numCModels);

			for (int i = 0; i < colmap->numCModels; i++)
			{
				memcpy(colmap->cModels[i]._portpad0, iw5_colmap->cModels[i]._portpad0, 28);
				memcpy(colmap->cModels[i]._portpad1, iw5_colmap->cModels[i]._portpad1, 40);
			}

			colmap->numBrushes = iw5_colmap->info.numBrushes;
			colmap->brushes = (cbrush_t*)iw5_colmap->info.brushes;
			colmap->brushBounds = (Bounds*)iw5_colmap->info.brushBounds;
			colmap->brushContents = iw5_colmap->info.brushContents;

			colmap->mapEnts = (MapEnts*)iw5_colmap->mapEnts;

			colmap->smodelNodeCount = iw5_colmap->smodelNodeCount;
			colmap->smodelNodes = (SModelAabbNode*)iw5_colmap->smodelNodes;

			// return converted colmap
			return colmap;
		}

		IClipMap::IClipMap()
		{
		}

		IClipMap::~IClipMap()
		{
		}

		void IClipMap::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = "maps/mp/" + currentzone + ".d3dbsp"; // name;
			this->asset_ = this->parse(name, mem);
			this->m_filename = name;

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), name.data()).clipmap;
			}
		}

		void IClipMap::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IClipMap::load_depending(IZone* zone)
		{
#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW4::IClipMap::load_depending");
#endif

			auto data = this->asset_;

			if (data->staticModelList)
			{
				for (int i = 0; i < data->numStaticModels; i++)
				{
					if (data->staticModelList[i].xmodel)
					{
						zone->add_asset_of_type(xmodel, data->staticModelList[i].xmodel->name);
					}
				}
			}

			if (data->dynEntDefList[0])
			{
				for (int i = 0; i < data->dynEntCount[0]; i++)
				{
					if (data->dynEntDefList[0][i].xModel)
					{
						zone->add_asset_of_type(xmodel, data->dynEntDefList[0][i].xModel->name);
					}
					if (data->dynEntDefList[0][i].destroyFx)
					{
						// zone->AddAssetOfType(fx, data->dynEntDefList[0][i].destroyFx->name);
					}
					if (data->dynEntDefList[0][i].physPreset)
					{
						zone->add_asset_of_type(physpreset, data->dynEntDefList[0][i].physPreset->name);
					}
				}
			}

			if (data->dynEntDefList[1])
			{
				for (int i = 0; i < data->dynEntCount[1]; i++)
				{
					if (data->dynEntDefList[1][i].xModel)
					{
						zone->add_asset_of_type(xmodel, data->dynEntDefList[1][i].xModel->name);
					}
					if (data->dynEntDefList[1][i].destroyFx)
					{
						// zone->AddAssetOfType(fx, data->dynEntDefList[1][i].destroyFx->name);
					}
					if (data->dynEntDefList[1][i].physPreset)
					{
						zone->add_asset_of_type(physpreset, data->dynEntDefList[1][i].physPreset->name);
					}
				}
			}

			if (data->mapEnts)
			{
				zone->add_asset_of_type(map_ents, this->asset_->mapEnts->name);
			}

#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif
		}

		std::string IClipMap::name()
		{
			return this->name_;
		}

		std::int32_t IClipMap::type()
		{
			return col_map_mp;
		}

		void IClipMap::write(IZone* zone, ZoneBuffer* buf)
		{
#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW4::IClipMap::write");
#endif

			auto data = this->asset_;
			auto offset = buf->get_zone_offset();
			auto dest = buf->write<clipMap_t>(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->cPlanes)
			{
				dest->cPlanes = buf->write_s(3, data->cPlanes, data->numCPlanes);
			}

			if (data->staticModelList)
			{
				buf->align(3);
				auto static_model = buf->write(data->staticModelList, data->numStaticModels);

				for (std::int32_t i = 0; i < data->numStaticModels; i++)
				{
					if (data->staticModelList[i].xmodel)
					{
						static_model[i].xmodel = reinterpret_cast<XModel*>(zone->get_asset_pointer(
							xmodel, data->staticModelList[i].xmodel->name));
					}
				}

				ZoneBuffer::ClearPointer(&data->staticModelList);
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
			}

			if (data->cBrushEdges)
			{
				dest->cBrushEdges = buf->write_s(0, data->cBrushEdges, data->numCBrushEdges);
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

			if (data->leafBrushes)
			{
				dest->leafBrushes = buf->write_s(1, data->leafBrushes, data->numLeafBrushes);
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
				ZoneBuffer::ClearPointer(&dest->cModels);
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
			}

			// brushBounds
			if (data->brushBounds)
			{
				dest->brushBounds = buf->write_s(127, data->brushBounds, data->numBrushes);
			}

			// brushContents
			if (data->brushContents)
			{
				dest->brushContents = buf->write_s(3, data->brushContents, data->numBrushes);
			}

			if (data->smodelNodes)
			{
				buf->align(3);
				buf->write(data->smodelNodes, data->smodelNodeCount);
				ZoneBuffer::ClearPointer(&dest->smodelNodes);
			}

			if (data->mapEnts)
			{
				dest->mapEnts = reinterpret_cast<MapEnts*>(zone->get_asset_pointer(map_ents, this->name()));
			}

			if (data->dynEntDefList[0])
			{
				buf->align(3);
				auto dyn_entity_def = buf->write(data->dynEntDefList[0], data->dynEntCount[0]);

				for (std::uint16_t i = 0; i < data->dynEntCount[0]; i++)
				{
					if (data->dynEntDefList[0][i].xModel)
					{
						dyn_entity_def[i].xModel = reinterpret_cast<XModel*>(zone->get_asset_pointer(
							xmodel, data->dynEntDefList[0][i].xModel->name));
					}
					if (data->dynEntDefList[0][i].destroyFx)
					{
						// dyn_entity_def[i].destroyFx = reinterpret_cast<FxEffectDef*>(zone->GetAssetPointer(fx, data->dynEntDefList[0][i].destroyFx->name));
					}
					if (data->dynEntDefList[0][i].physPreset)
					{
						dyn_entity_def[i].physPreset = reinterpret_cast<PhysPreset*>(zone->get_asset_pointer(
							physpreset, data->dynEntDefList[0][i].physPreset->name));
					}

					/*if (data->dynEntDefList[0][i].hinge)
					{
						dyn_entity_def[i].hinge = buf->write_s(3, dyn_entity_def[i].hinge, 1);
					}*/
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
						dyn_entity_def[i].xModel = reinterpret_cast<XModel*>(zone->get_asset_pointer(
							xmodel, data->dynEntDefList[1][i].xModel->name));
					}
					if (data->dynEntDefList[1][i].destroyFx)
					{
						// dyn_entity_def[i].destroyFx = reinterpret_cast<FxEffectDef*>(zone->GetAssetPointer(fx, data->dynEntDefList[1][i].destroyFx->name));
					}
					if (data->dynEntDefList[1][i].physPreset)
					{
						dyn_entity_def[i].physPreset = reinterpret_cast<PhysPreset*>(zone->get_asset_pointer(
							physpreset, data->dynEntDefList[1][i].physPreset->name));
					}

					/*if (data->dynEntDefList[1][i].hinge)
					{
						dyn_entity_def[i].hinge = buf->write_s(3, dyn_entity_def[i].hinge, 1);
					}*/
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

#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif
		}

		void IClipMap::dump(clipMap_t* asset)
		{
			auto iw5_clipmap = new IW5::clipMap_t;
			memset(iw5_clipmap, 0, sizeof IW5::clipMap_t);

			// convert clipmap to IW5 format
			iw5_clipmap->name = asset->name;
			iw5_clipmap->isInUse = asset->isInUse;

			iw5_clipmap->info.numCPlanes = asset->numCPlanes;
			iw5_clipmap->info.cPlanes = (IW5::cplane_s*)asset->cPlanes;

			iw5_clipmap->numStaticModels = asset->numStaticModels;
			iw5_clipmap->staticModelList = (IW5::cStaticModel_s*)asset->staticModelList;

			iw5_clipmap->info.numMaterials = asset->numMaterials;
			iw5_clipmap->info.materials = (IW5::dmaterial_t*)asset->materials;

			iw5_clipmap->info.numCBrushSides = asset->numCBrushSides;
			iw5_clipmap->info.cBrushSides = (IW5::cbrushside_t*)asset->cBrushSides;

			iw5_clipmap->info.numCBrushEdges = asset->numCBrushEdges;
			iw5_clipmap->info.cBrushEdges = (IW5::cbrushedge_t*)asset->cBrushEdges;

			iw5_clipmap->numCNodes = asset->numCNodes;
			iw5_clipmap->cNodes = (IW5::cNode_t*)asset->cNodes;

			iw5_clipmap->numCLeaf = asset->numCLeaf;
			iw5_clipmap->cLeaf = (IW5::cLeaf_t*)asset->cLeaf;

			iw5_clipmap->info.numCLeafBrushNodes = asset->numCLeafBrushNodes;
			iw5_clipmap->info.cLeafBrushNodes = (IW5::cLeafBrushNode_s*)asset->cLeafBrushNodes;

			iw5_clipmap->info.numLeafBrushes = asset->numLeafBrushes;
			iw5_clipmap->info.leafBrushes = asset->leafBrushes;

			// leafSurfaces todo!

			iw5_clipmap->numVerts = asset->numVerts;
			iw5_clipmap->verts = (IW5::VecInternal<3>*)asset->verts;

			iw5_clipmap->numTriIndices = asset->numTriIndices;
			iw5_clipmap->triIndices = asset->triIndices;
			iw5_clipmap->triEdgeIsWalkable = asset->triEdgeIsWalkable;

			iw5_clipmap->numCollisionBorders = asset->numCollisionBorders;
			iw5_clipmap->collisionBorders = (IW5::CollisionBorder*)asset->collisionBorders;

			iw5_clipmap->numCollisionPartitions = asset->numCollisionPartitions;
			iw5_clipmap->collisionPartitions = (IW5::CollisionPartition*)asset->collisionPartitions;

			iw5_clipmap->numCollisionAABBTrees = asset->numCollisionAABBTrees;
			iw5_clipmap->collisionAABBTrees = (IW5::CollisionAabbTree*)asset->collisionAABBTrees;

			// cmodels!
			iw5_clipmap->numCModels = asset->numCModels;
			iw5_clipmap->cModels = new IW5::cmodel_t[iw5_clipmap->numCModels];
			memset(iw5_clipmap->cModels, 0, sizeof(IW5::cmodel_t) * iw5_clipmap->numCModels);

			for (int i = 0; i < iw5_clipmap->numCModels; i++)
			{
				memcpy(iw5_clipmap->cModels[i]._portpad0, asset->cModels[i]._portpad0, 28);
				memcpy(iw5_clipmap->cModels[i]._portpad1, asset->cModels[i]._portpad1, 40);
			}

			iw5_clipmap->info.numBrushes = asset->numBrushes;
			iw5_clipmap->info.brushes = (IW5::cbrush_t*)asset->brushes;
			iw5_clipmap->info.brushBounds = (IW5::Bounds*)asset->brushBounds;
			iw5_clipmap->info.brushContents = asset->brushContents;

			iw5_clipmap->mapEnts = (IW5::MapEnts*)asset->mapEnts;
			iw5_clipmap->stageCount = asset->mapEnts->stageCount;
			iw5_clipmap->stages = (IW5::Stage*)asset->mapEnts->stageNames;

			iw5_clipmap->smodelNodeCount = asset->smodelNodeCount;
			iw5_clipmap->smodelNodes = (IW5::SModelAabbNode*)asset->smodelNodes;

			// dump clipmap
			IW5::IClipMap::dump(iw5_clipmap);

			// free memory_
			delete[] iw5_clipmap->cModels;
			delete[] iw5_clipmap;
		}
	}
}
