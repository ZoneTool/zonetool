// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "../IW4/Assets/Xmodel.hpp"
#include "../IW4/Assets/XSurface.hpp"

namespace ZoneTool
{
	namespace IW3
	{
		IW4::XSurface* GenerateIW4Surface(XSurface* asset)
		{
			// allocate IW4 XSurface structure
			auto xsurface = new IW4::XSurface;
			memset(xsurface, 0, sizeof IW4::XSurface);

			xsurface->tileMode = asset->tileMode;
			xsurface->deformed = asset->deformed;
			xsurface->vertCount = asset->vertCount;
			xsurface->triCount = asset->triCount;
			xsurface->zoneHandle = asset->zoneHandle;
			xsurface->baseTriIndex = asset->baseTriIndex;
			xsurface->baseVertIndex = asset->baseVertIndex;
			xsurface->triIndices = (IW4::Face *)asset->triIndices;
			memcpy(&xsurface->vertexInfo, &asset->vertInfo, sizeof IW4::XSurfaceVertexInfo);
			xsurface->verticies = (IW4::GfxPackedVertex *)asset->verts0;
			xsurface->vertListCount = asset->vertListCount;
			xsurface->rigidVertLists = (IW4::XRigidVertList *)asset->vertList;
			memcpy(&xsurface->partBits, &asset->partBits, sizeof(int[4]));

			return xsurface;
		}

		IW4::XModel* GenerateIW4Model(XModel* asset)
		{
			// allocate IW4 XModel structure
			auto xmodel = new IW4::XModel;
			memset(xmodel, 0, sizeof IW4::XModel);

			// copy data over
			xmodel->name = (char*)asset->name;
			xmodel->numBones = asset->numBones;
			xmodel->numRootBones = asset->numRootBones;
			xmodel->numSurfaces = asset->numsurfs;
			xmodel->lodRampType = asset->lodRampType;
			xmodel->scale = 1.0f;
			memset(xmodel->noScalePartBits, 0, sizeof(int) * 6);
			xmodel->parentList = (unsigned char*)asset->parentList;
			xmodel->boneNames = (short*)asset->boneNames;

			xmodel->tagAngles = (IW4::XModelAngle*)asset->quats;
			xmodel->tagPositions = (IW4::XModelTagPos*)asset->trans;

			xmodel->partClassification = asset->partClassification;
			xmodel->animMatrix = (IW4::DObjAnimMat*)asset->baseMat;
			xmodel->materials = (IW4::Material**)asset->materialHandles;

			// convert level of detail data
			for (int i = 0; i < asset->numLods; i++)
			{
				xmodel->lods[i].dist = asset->lodInfo[i].dist;
				xmodel->lods[i].numSurfacesInLod = asset->lodInfo[i].numsurfs;
				xmodel->lods[i].surfIndex = asset->lodInfo[i].surfIndex;
				memcpy(xmodel->lods[i].partBits, asset->lodInfo[i].partBits, sizeof(int[4]));
				memcpy(&xmodel->lods[i].lod, &asset->lodInfo[i].lod, 3);
				
				// generate ModelSurface object
				xmodel->lods[i].surfaces = new IW4::XModelSurfs;
				memset(xmodel->lods[i].surfaces, 0, sizeof IW4::XModelSurfs);

				xmodel->lods[i].surfaces->name = _strdup(va("zonetool_%s_%u", xmodel->name, i).data());
				xmodel->lods[i].surfaces->numsurfs = xmodel->lods[i].numSurfacesInLod;
				memcpy(xmodel->lods[i].surfaces->partBits, asset->lodInfo[i].partBits, sizeof(int[4]));

				// allocate xsurficies
				xmodel->lods[i].surfaces->surfs = new IW4::XSurface[xmodel->lods[i].numSurfacesInLod];

				// loop through surfaces in current Level-of-Detail
				for (int surf = xmodel->lods[i].surfIndex; surf <
				     xmodel->lods[i].surfIndex + xmodel->lods[i].numSurfacesInLod; surf++)
				{
					// generate iw4 surface
					auto surface = GenerateIW4Surface(&asset->surfs[surf]);

					// copy XSurface into iw4 structure
					memcpy(
						&xmodel->lods[i].surfaces->surfs[surf - xmodel->lods[i].surfIndex],
						surface,
						sizeof IW4::XSurface
					);
				}
			}

			xmodel->numLods = asset->numLods;
			xmodel->collLod = asset->collLod;
			xmodel->flags = asset->flags;

			xmodel->colSurf = (IW4::XModelCollSurf_s*)asset->collSurfs;
			xmodel->numColSurfs = asset->numCollSurfs;
			xmodel->contents = asset->contents;

			// convert colsurf bounds
			for (int i = 0; i < xmodel->numColSurfs; i++)
			{
				xmodel->colSurf[i].bounds.compute();
			}

			// convert boneinfo
			xmodel->boneInfo = new IW4::XBoneInfo[xmodel->numBones];
			for (int i = 0; i < xmodel->numBones; i++)
			{
				memcpy(&xmodel->boneInfo[i].bounds, &asset->boneInfo[i].bounds, sizeof(Bounds));

				xmodel->boneInfo[i].packedBounds.compute();
				xmodel->boneInfo[i].radiusSquared = asset->boneInfo[i].radiusSquared;
			}

			xmodel->radius = asset->radius;
			xmodel->memUsage = asset->memUsage;
			xmodel->bad = asset->bad;
			xmodel->physPreset = (IW4::PhysPreset*)asset->physPreset;

			xmodel->bounds.compute(asset->mins, asset->maxs);

			return xmodel;
		}

		void IXModel::dump(XModel* asset)
		{
			// generate iw4 model
			auto iw4_model = GenerateIW4Model(asset);

			// dump iw4 model
			IW4::IXModel::dump(iw4_model, SL_ConvertToString);

			// dump all xsurfaces
			for (int i = 0; i < iw4_model->numLods; i++)
			{
				IW4::IXSurface::dump(iw4_model->lods[i].surfaces);
			}
		}
	}
}
