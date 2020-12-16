// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: momo5502 (https://github.com/momo5502)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "../IW4/Assets/ClipMap.hpp"
#include "ClipMap.hpp"

namespace ZoneTool
{
	namespace IW3
	{
		void IClipMap::dump(clipMap_t* asset, ZoneMemory* mem)
		{
			if (!asset) return;

			auto* iw4_clipmap = mem->Alloc<IW4::clipMap_t>();
			memset(iw4_clipmap, 0, sizeof IW4::clipMap_t);

			// convert clipmap to IW4 format
			iw4_clipmap->name = asset->name;
			iw4_clipmap->isInUse = asset->isInUse;

			iw4_clipmap->numCPlanes = asset->planeCount;
			iw4_clipmap->cPlanes = (IW4::cplane_s*)asset->planes;

			iw4_clipmap->numStaticModels = (int)asset->numStaticModels;
			iw4_clipmap->staticModelList = mem->Alloc<IW4::cStaticModel_s>(iw4_clipmap->numStaticModels);
			for (unsigned int i = 0; i < asset->numStaticModels; ++i)
			{
				std::memcpy(&iw4_clipmap->staticModelList[i], &asset->staticModelList[i].xmodel,
				            sizeof(IW4::cStaticModel_s));
				iw4_clipmap->staticModelList[i].absBounds.compute();
			}

			iw4_clipmap->numMaterials = (int)asset->numMaterials;
			iw4_clipmap->materials = mem->Alloc<IW4::dmaterial_t>(iw4_clipmap->numMaterials);
			for (auto i = 0u; i < iw4_clipmap->numMaterials; i++)
			{
				iw4_clipmap->materials[i].material = mem->StrDup(asset->materials[i].material);
				iw4_clipmap->materials[i].contentFlags = asset->materials[i].contentFlags;
				iw4_clipmap->materials[i].surfaceFlags = asset->materials[i].surfaceFlags;
			}

			iw4_clipmap->numCBrushSides = (int)asset->numBrushSides;
			iw4_clipmap->cBrushSides = mem->Alloc<IW4::cbrushside_t>(iw4_clipmap->numCBrushSides);

			std::unordered_map<cbrushside_t*, IW4::cbrushside_t*> mapped_brush_sides;
			
			for (unsigned int i = 0; i < asset->numBrushSides; ++i)
			{
				mapped_brush_sides[&asset->brushsides[i]] = &iw4_clipmap->cBrushSides[i];
				
				iw4_clipmap->cBrushSides[i].plane = (IW4::cplane_s*)asset->brushsides[i].plane;
				iw4_clipmap->cBrushSides[i].materialNum = asset->brushsides[i].materialNum;
				iw4_clipmap->cBrushSides[i].firstAdjacentSideOffset = (char)asset->brushsides[i].firstAdjacentSideOffset;
				iw4_clipmap->cBrushSides[i].edgeCount = asset->brushsides[i].edgeCount;
			}

			iw4_clipmap->numCBrushEdges = (int)asset->numBrushEdges;
			iw4_clipmap->cBrushEdges = (IW4::cbrushedge_t*)asset->brushEdges;

			iw4_clipmap->numCNodes = (int)asset->numNodes;
			iw4_clipmap->cNodes = (IW4::cNode_t*)asset->nodes;

			iw4_clipmap->numCLeaf = (int)asset->numLeafs;
			iw4_clipmap->cLeaf = mem->Alloc<IW4::cLeaf_t>(iw4_clipmap->numCLeaf);
			for (unsigned int i = 0; i < asset->numLeafs; ++i)
			{
				std::memcpy(&iw4_clipmap->cLeaf[i], &asset->leafs[i], sizeof(IW4::cLeaf_t));
				iw4_clipmap->cLeaf[i].bounds.compute();
			}

			iw4_clipmap->numCLeafBrushNodes = (int)asset->leafbrushNodesCount;
			iw4_clipmap->cLeafBrushNodes = (IW4::cLeafBrushNode_s*)asset->leafbrushNodes;

			iw4_clipmap->numLeafBrushes = (int)asset->numLeafBrushes;
			iw4_clipmap->leafBrushes = (short*)asset->leafbrushes;

			iw4_clipmap->numLeafSurfaces = (int)asset->numLeafSurfaces;
			iw4_clipmap->leafSurfaces = (int*)asset->leafsurfaces;

			iw4_clipmap->numVerts = (int)asset->vertCount;
			iw4_clipmap->verts = (IW4::VecInternal<3>*)asset->verts;

			iw4_clipmap->numTriIndices = asset->triCount;
			iw4_clipmap->triIndices = (short*)asset->triIndices;
			iw4_clipmap->triEdgeIsWalkable = asset->triEdgeIsWalkable;

			iw4_clipmap->numCollisionBorders = asset->borderCount;
			iw4_clipmap->collisionBorders = (IW4::CollisionBorder*)asset->borders;

			iw4_clipmap->numCollisionPartitions = asset->partitionCount;
			iw4_clipmap->collisionPartitions = (IW4::CollisionPartition*)asset->partitions;

			iw4_clipmap->numCollisionAABBTrees = asset->aabbTreeCount;
			iw4_clipmap->collisionAABBTrees = mem->Alloc<IW4::CollisionAabbTree>(iw4_clipmap->numCollisionAABBTrees);
			for (int i = 0; i < asset->aabbTreeCount; ++i)
			{
				std::memcpy(&iw4_clipmap->collisionAABBTrees[i].origin, &asset->aabbTrees[i].origin, 12);
				std::memcpy(&iw4_clipmap->collisionAABBTrees[i].halfSize, &asset->aabbTrees[i].halfSize, 12);
				iw4_clipmap->collisionAABBTrees[i].materialIndex = asset->aabbTrees[i].materialIndex;
				iw4_clipmap->collisionAABBTrees[i].childCount = asset->aabbTrees[i].childCount;
				iw4_clipmap->collisionAABBTrees[i].u.firstChildIndex = asset->aabbTrees[i].u.firstChildIndex;
			}

			// cmodels!
			iw4_clipmap->numCModels = (int)asset->numSubModels;
			iw4_clipmap->cModels = mem->Alloc<IW4::cmodel_t>(iw4_clipmap->numCModels);
			for (unsigned int i = 0; i < asset->numSubModels; ++i)
			{
				std::memcpy(&iw4_clipmap->cModels[i], &asset->cmodels[i], sizeof(IW4::cmodel_t));
				iw4_clipmap->cModels[i].bounds.compute();
				iw4_clipmap->cModels[i].leaf.bounds.compute();
			}

			iw4_clipmap->numBrushes = (short)asset->numBrushes;
			iw4_clipmap->brushes = mem->Alloc<IW4::cbrush_t>(iw4_clipmap->numBrushes);
			iw4_clipmap->brushBounds = mem->Alloc<IW4::Bounds>(iw4_clipmap->numBrushes);
			iw4_clipmap->brushContents = mem->Alloc<int>(iw4_clipmap->numBrushes);
			for (unsigned int i = 0; i < asset->numBrushes; ++i)
			{
				std::memcpy(&iw4_clipmap->brushes[i].axialMaterialNum, &asset->brushes[i].axialMaterialNum,
				            sizeof(iw4_clipmap->brushes[i].axialMaterialNum));
				std::memcpy(&iw4_clipmap->brushes[i].firstAdjacentSideOffsets,
				            &asset->brushes[i].firstAdjacentSideOffsets,
				            sizeof(iw4_clipmap->brushes[i].firstAdjacentSideOffsets));
				std::memcpy(&iw4_clipmap->brushes[i].edgeCount, &asset->brushes[i].edgeCount,
				            sizeof(iw4_clipmap->brushes[i].edgeCount));

				iw4_clipmap->brushes[i].numsides = asset->brushes[i].numsides;
				iw4_clipmap->brushes[i].sides = mapped_brush_sides.find(asset->brushes[i].sides)->second;
				iw4_clipmap->brushes[i].edge = asset->brushes[i].baseAdjacentSide;
				iw4_clipmap->brushes[i].numsides = asset->brushes[i].numsides;

				iw4_clipmap->brushBounds[i].compute(asset->brushes[i].mins, asset->brushes[i].maxs);

				iw4_clipmap->brushContents[i] = asset->brushes[i].contents;
			}

			iw4_clipmap->smodelNodeCount = 1;
			iw4_clipmap->smodelNodes = mem->Alloc<IW4::SModelAabbNode>();
			if (asset->numStaticModels == 0)
			{
				iw4_clipmap->smodelNodes[0].bounds.halfSize[0] = -131072.000f;
				iw4_clipmap->smodelNodes[0].bounds.halfSize[1] = -131072.000f;
				iw4_clipmap->smodelNodes[0].bounds.halfSize[2] = -131072.000f;
			}
			else
			{
				float maxs[3];
				float mins[3];

				maxs[0] = asset->staticModelList[0].absmax[0];
				maxs[1] = asset->staticModelList[1].absmax[1];
				maxs[2] = asset->staticModelList[2].absmax[2];

				mins[0] = asset->staticModelList[0].absmin[0];
				mins[1] = asset->staticModelList[1].absmin[1];
				mins[2] = asset->staticModelList[2].absmin[2];

				for (unsigned int i = 1; i < asset->numStaticModels; i++)
				{
					maxs[0] = max(maxs[0], asset->staticModelList[i].absmax[0]);
					maxs[1] = max(maxs[1], asset->staticModelList[i].absmax[1]);
					maxs[2] = max(maxs[2], asset->staticModelList[i].absmax[2]);

					mins[0] = min(mins[0], asset->staticModelList[i].absmin[0]);
					mins[1] = min(mins[1], asset->staticModelList[i].absmin[1]);
					mins[2] = min(mins[2], asset->staticModelList[i].absmin[2]);
				}

				iw4_clipmap->smodelNodes[0].bounds.compute(mins, maxs);
				iw4_clipmap->smodelNodes[0].childCount = static_cast<short>(asset->numStaticModels);
				iw4_clipmap->smodelNodes[0].firstChild = 0;
			}

			iw4_clipmap->mapEnts = mem->Alloc<IW4::MapEnts>(); //  asset->mapEnts;
			memcpy(iw4_clipmap->mapEnts, asset->mapEnts, sizeof MapEnts);

			iw4_clipmap->mapEnts->stageCount = 1;
			iw4_clipmap->mapEnts->stageNames = mem->Alloc<IW4::Stage>();
			iw4_clipmap->mapEnts->stageNames[0].stageName = mem->StrDup("stage 0");
			iw4_clipmap->mapEnts->stageNames[0].triggerIndex = 0x400;
			iw4_clipmap->mapEnts->stageNames[0].sunPrimaryLightIndex = 0x1;

			iw4_clipmap->dynEntCount[0] = asset->dynEntCount[0];
			iw4_clipmap->dynEntCount[1] = asset->dynEntCount[1];

			iw4_clipmap->dynEntDefList[0] = (IW4::DynEntityDef*)asset->dynEntDefList[0];
			iw4_clipmap->dynEntDefList[1] = (IW4::DynEntityDef*)asset->dynEntDefList[1];

			iw4_clipmap->dynEntPoseList[0] = (IW4::DynEntityPose*)asset->dynEntPoseList[0];
			iw4_clipmap->dynEntPoseList[1] = (IW4::DynEntityPose*)asset->dynEntPoseList[1];

			iw4_clipmap->dynEntClientList[0] = (IW4::DynEntityClient*)asset->dynEntClientList[0];
			iw4_clipmap->dynEntClientList[1] = (IW4::DynEntityClient*)asset->dynEntClientList[1];

			iw4_clipmap->dynEntCollList[0] = (IW4::DynEntityColl*)asset->dynEntCollList[0];
			iw4_clipmap->dynEntCollList[1] = (IW4::DynEntityColl*)asset->dynEntCollList[1];
			
			iw4_clipmap->checksum = asset->checksum;

			//iw4_clipmap->stageCount = 1;
			//iw4_clipmap-> = mem->Alloc<IW4::Stage>();


			IW4::IClipMap::dump(iw4_clipmap);
		}
	}
}
