// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

namespace ZoneTool
{
	namespace IW4
	{
		enum FxElemType : char
		{
			FX_ELEM_TYPE_SPRITE_BILLBOARD = 0x0,
			FX_ELEM_TYPE_SPRITE_ORIENTED = 0x1,
			FX_ELEM_TYPE_TAIL = 0x2,
			FX_ELEM_TYPE_TRAIL = 0x3,
			FX_ELEM_TYPE_CLOUD = 0x4,
			FX_ELEM_TYPE_SPARKCLOUD = 0x5,
			FX_ELEM_TYPE_SPARKFOUNTAIN = 0x6,
			FX_ELEM_TYPE_MODEL = 0x7,
			FX_ELEM_TYPE_OMNI_LIGHT = 0x8,
			FX_ELEM_TYPE_SPOT_LIGHT = 0x9,
			FX_ELEM_TYPE_SOUND = 0xA,
			FX_ELEM_TYPE_DECAL = 0xB,
			FX_ELEM_TYPE_RUNNER = 0xC,
			FX_ELEM_TYPE_COUNT = 0xD,
			FX_ELEM_TYPE_LAST_SPRITE = 0x3,
			FX_ELEM_TYPE_LAST_DRAWN = 0x9,
		};

		enum XAssetType : std::int32_t
		{
			physpreset,
			phys_collmap,
			xanim,
			xmodelsurfs,
			xmodel,
			material,
			pixelshader,
			vertexshader,
			vertexdecl,
			techset,
			image,
			sound,
			sndcurve,
			loaded_sound,
			col_map_sp,
			col_map_mp,
			com_map,
			game_map_sp,
			game_map_mp,
			map_ents,
			fx_map,
			gfx_map,
			lightdef,
			ui_map,
			// not used
			font,
			menufile,
			menu,
			localize,
			weapon,
			snddriverglobals,
			// not used
			fx,
			impactfx,
			aitype,
			// not used
			mptype,
			// not used
			character,
			// not used
			xmodelalias,
			// not used
			rawfile,
			stringtable,
			leaderboarddef,
			structureddatadef,
			tracer,
			vehicle,
			addon_map_ents,
			max,
		};

		typedef float vec4_t[4];
		typedef float vec3_t[3];
		typedef float vec2_t[2];

		template <std::size_t N>
		struct VecInternal
		{
			float data[N];
		};

		struct RawFile
		{
			const char* name;
			int compressedLen;
			int len;
			const char* buffer;
		};

#pragma pack(push, 4)
		struct PhysPreset
		{
			const char* name;
			int type;
			float mass;
			float bounce;
			float friction;
			float bulletForceScale;
			float explosiveForceScale;
			const char* sndAliasPrefix;
			float piecesSpreadFraction;
			float piecesUpwardVelocity;
			bool tempDefaultToCylinder;
		};
#pragma pack(pop)


		struct XModelAngle
		{
			short x;
			short y;
			short z;
			short base;
		};

		struct XModelTagPos
		{
			float x;
			float y;
			float z;
		};

		struct DObjAnimMat
		{
			float quat[4];
			float trans[3];
			float transWeight;
		};

#pragma pack(push, 4)
		struct MaterialStreamRouting
		{
			char source;
			char dest;
		};

		struct VertexDecl
		{
			const char* name;
			char streamCount;
			bool hasOptionalSource;
			char pad[2];
			MaterialStreamRouting streams[13];
			void* declarations[16];
		};
#pragma pack(pop)

		struct PixelShader
		{
			const char* name;
			void* shader;
			DWORD* bytecode;
			short codeLen;
		};

		struct VertexShader
		{
			const char* name;
			void* shader;
			DWORD* bytecode;
			short codeLen;
		};

		struct MaterialArgumentCodeConst
		{
			unsigned __int16 index;
			char firstRow;
			char rowCount;
		};

		union MaterialArgumentDef
		{
			float* literalConst;
			MaterialArgumentCodeConst codeConst;
			unsigned int codeSampler;
			unsigned int nameHash;
		};

		struct ShaderArgumentDef
		{
			short type;
			short dest;
			MaterialArgumentDef u;
		};
#pragma pack(push, 4)

		struct MaterialPass
		{
			VertexDecl* vertexDecl;
			VertexShader* vertexShader;
			PixelShader* pixelShader;
			char perPrimArgCount;
			char perObjArgCount;
			char stableArgCount;
			char customSamplerFlags;
			ShaderArgumentDef* argumentDef;
		};
#pragma pack(pop)
		struct MaterialTechniqueHeader
		{
			const char* name;
			unsigned __int16 flags;
			unsigned __int16 passCount;
		};

		struct MaterialTechnique
		{
			MaterialTechniqueHeader hdr;
			MaterialPass pass[1];
		};

		struct MaterialTechniqueSet
		{
			const char* name;
			int pad;
			MaterialTechniqueSet* remappedTechniques;
			MaterialTechnique* techniques[48];
		};

		struct MaterialConstantDef
		{
			unsigned int nameHash;
			char name[12];
			float literal[4];
		};

		struct GfxStateBits
		{
			unsigned int loadBits[2];
		};

		struct GfxImageLoadDef // actually a IDirect3DTexture* but this is easier
		{
			char mipLevels;
			char flags;
			short dimensions[3];
			int format; // usually the compression Magic
			int dataSize; // set to zero to load from IWD
			char* texture; // texture
		};

		struct GfxImage
		{
			GfxImageLoadDef* texture;
			char mapType; // 5 is cube, 4 is 3d, 3 is 2d
			char semantic;
			char category;
			char flags;
			int cardMemory;
			int dataLen1;
			int dataLen2;
			short height;
			short width;
			short depth;
			bool loaded;
			char pad;
			char* name;
		};

		struct MaterialImage
		{
			unsigned int typeHash; // asset hash of type
			char firstCharacter; // first character of image name
			char secondLastCharacter; // second-last character of image name (maybe only in CoD4?!)
			char sampleState;
			char semantic;
			GfxImage* image; // Image* actually
		};

		namespace alpha
		{
			enum MaterialTechniqueType
			{
				TECHNIQUE_DEPTH_PREPASS = 0x0,
				TECHNIQUE_BUILD_FLOAT_Z = 0x1,
				TECHNIQUE_BUILD_SHADOWMAP_DEPTH = 0x2,
				TECHNIQUE_BUILD_SHADOWMAP_COLOR = 0x3,
				TECHNIQUE_UNLIT = 0x4,
				TECHNIQUE_EMISSIVE = 0x5,
				TECHNIQUE_EMISSIVE_DFOG = 0x6,
				TECHNIQUE_EMISSIVE_SHADOW = 0x7,
				TECHNIQUE_EMISSIVE_SHADOW_DFOG = 0x8,
				TECHNIQUE_LIT_BEGIN = 0x9,
				TECHNIQUE_LIT = 0x9,
				TECHNIQUE_LIT_DFOG = 0xA,
				TECHNIQUE_LIT_SUN = 0xB,
				TECHNIQUE_LIT_SUN_DFOG = 0xC,
				TECHNIQUE_LIT_SUN_SHADOW = 0xD,
				TECHNIQUE_LIT_SUN_SHADOW_DFOG = 0xE,
				TECHNIQUE_LIT_SPOT = 0xF,
				TECHNIQUE_LIT_SPOT_DFOG = 0x10,
				TECHNIQUE_LIT_SPOT_SHADOW = 0x11,
				TECHNIQUE_LIT_SPOT_SHADOW_DFOG = 0x12,
				TECHNIQUE_LIT_OMNI = 0x13,
				TECHNIQUE_LIT_OMNI_DFOG = 0x14,
				TECHNIQUE_LIT_OMNI_SHADOW = 0x15,
				TECHNIQUE_LIT_OMNI_SHADOW_DFOG = 0x16,
				TECHNIQUE_LIT_END = 0x17,
				TECHNIQUE_LIGHT_SPOT = 0x17,
				TECHNIQUE_LIGHT_OMNI = 0x18,
				TECHNIQUE_LIGHT_SPOT_SHADOW = 0x19,
				TECHNIQUE_FAKELIGHT_NORMAL = 0x1A,
				TECHNIQUE_FAKELIGHT_VIEW = 0x1B,
				TECHNIQUE_SUNLIGHT_PREVIEW = 0x1C,
				TECHNIQUE_CASE_TEXTURE = 0x1D,
				TECHNIQUE_WIREFRAME_SOLID = 0x1E,
				TECHNIQUE_WIREFRAME_SHADED = 0x1F,
				TECHNIQUE_DEBUG_BUMPMAP = 0x20,
				TECHNIQUE_COUNT = 0x21,
				TECHNIQUE_TOTAL_COUNT = 0x22,
				TECHNIQUE_NONE = 0x23,
			};

			struct MaterialPass
			{
				VertexDecl* vertexDecl;
				VertexShader* vertexShaderArray[15];
				VertexShader* vertexShader;
				PixelShader* pixelShader;
				char perPrimArgCount;
				char perObjArgCount;
				char stableArgCount;
				char customSamplerFlags;
				char precompiledIndex;
				ShaderArgumentDef* args;
			};
			
			struct MaterialTechnique
			{
				const char* name;
				unsigned __int16 flags;
				unsigned __int16 passCount;
				MaterialPass passArray[1];
			};

#pragma pack(push, 4)
			struct MaterialTechniqueSet
			{
				const char* name;
				char worldVertFormat;
				char unused[2];
				MaterialTechniqueSet* remappedTechniqueSet;
				MaterialTechnique* techniques[33];
			};
#pragma pack(pop)
			
			struct D3DTexture
			{
				//DWORD Common;
				//DWORD Fence;
				//DWORD BaseFlush;
				//DWORD MipFlush;
				//DWORD Format;
				char unk[52];
			};
			
			struct CardMemory
			{
				int platform[1];
			};
			
			struct GfxImageStreamData
			{
				unsigned __int16 width;
				unsigned __int16 height;
				unsigned int pixelSize;
			};

#pragma pack(push, 4)
			struct GfxImage
			{
				D3DTexture texture;
				int format;
				char mapType;
				char semantic;
				char category;
				CardMemory cardMemory;
				unsigned __int16 width;
				unsigned __int16 height;
				unsigned __int16 depth;
				char levelCount;
				char cached;
				char* pixels;
				GfxImageStreamData streams[4];
				const char* name;
			};
#pragma pack(pop)

			struct GfxDrawSurfFields
			{
				unsigned __int64 objectId : 16;
				unsigned __int64 reflectionProbeIndex : 8;
				unsigned __int64 hasGfxEntIndex : 1;
				unsigned __int64 customIndex : 5;
				unsigned __int64 materialSortedIndex : 12;
				unsigned __int64 prepass : 2;
				unsigned __int64 useHeroLighting : 1;
				unsigned __int64 sceneLightIndex : 8;
				unsigned __int64 surfType : 4;
				unsigned __int64 primarySortKey : 6;
				unsigned __int64 unused : 1;
			};

			union GfxDrawSurf
			{
				GfxDrawSurfFields fields;
				unsigned __int64 packed;
			};
			
			struct __declspec(align(8)) Material
			{
				const char* name;
				char gameFlags;
				char sortKey;
				char textureAtlasRowCount;
				char textureAtlasColumnCount;
				GfxDrawSurf drawSurf;
				unsigned int surfaceTypeBits;
				char stateBitsEntry[33];
				char textureCount;
				char constantCount;
				char stateBitsCount;
				char stateFlags;
				char cameraRegion;
				char layerCount;
				MaterialTechniqueSet* techniqueSet;
				MaterialImage* textureTable;
				MaterialConstantDef* constantTable;
				GfxStateBits* stateBitsTable;
				const char** subMaterials;
			};
		}

		struct Material
		{
			const char* name; // 0
			char gameFlags;
			char sortKey;
			unsigned char animationX; // 6 // amount of animation frames in X
			unsigned char animationY; // 7 // amount of animation frames in Y
			unsigned int subRendererIndex; // 0x00 //+8
			unsigned int rendererIndex; // 12 // only for 3D models
			int unknown;
			unsigned int surfaceTypeBits; //+20
			char stateBitsEntry[48]; // 32 // 0xFF
			char numMaps;
			char constantCount;
			char stateBitsCount;
			char stateFlags; // 0x03
			unsigned short cameraRegion; // 0x04
			MaterialTechniqueSet* techniqueSet; // '2d' techset
			MaterialImage* maps; // map references
			MaterialConstantDef* constantTable;
			GfxStateBits* stateMap; // might be NULL, need to test
		};

		//vec3_t should be from idTech3, has to do with camera angles
		typedef float vec_t;
		typedef vec_t vec3_t[3];

		struct Bounds
		{
			vec3_t midPoint;
			vec3_t halfSize;

			void compute()
			{
				compute(midPoint, halfSize);
			}

			void compute(vec3_t mins, vec3_t maxs)
			{
				for (int i = 0; i < 3; ++i)
				{
					this->halfSize[i] = (maxs[i] - mins[i]) / 2;
					this->midPoint[i] = this->halfSize[i] + mins[i];
				}
			}
		};

		struct XBoneInfo
		{
			union
			{
				Bounds packedBounds;
				float bounds[2][3];
			};

			float radiusSquared;
		};

		struct Face
		{
			unsigned short v1;
			unsigned short v2;
			unsigned short v3;
		};

		// XModel
		struct XSurfaceVertexInfo
		{
			__int16 vertCount[4];
			unsigned __int16* vertsBlend;

			/*
			Count... Ok, here we go...

			(((vertCount[2] << 2) + vertCount[2]) +
			((vertCount[3] << 3) - vertCount[3]) +
			((vertCount[1] << 1) + vertCount[1])
			+ vertCount[0]) << 1*/
		};

		union GfxColor
		{
			unsigned int packed;
			char array[4];
		};

		union PackedTexCoords
		{
			unsigned int packed;
		};

		union PackedUnitVec
		{
			unsigned int packed;
		};

		struct GfxPackedVertex
		{
			float xyz[3];
			float binormalSign;
			GfxColor color;
			PackedTexCoords texCoord;
			PackedUnitVec normal;
			PackedUnitVec tangent;
		};

		struct XSurfaceCollisionAabb
		{
			unsigned __int16 mins[3];
			unsigned __int16 maxs[3];
		};

		struct XSurfaceCollisionNode
		{
			XSurfaceCollisionAabb aabb;
			unsigned __int16 childBeginIndex;
			unsigned __int16 childCount;
		};

		struct XSurfaceCollisionLeaf
		{
			unsigned __int16 triangleBeginIndex;
		};

		struct XSurfaceCollisionTree
		{
			float trans[3];
			float scale[3];
			unsigned int nodeCount;
			XSurfaceCollisionNode* nodes;
			unsigned int leafCount;
			XSurfaceCollisionLeaf* leafs;
		};

		struct XRigidVertList
		{
			unsigned __int16 boneOffset;
			unsigned __int16 vertCount;
			unsigned __int16 triOffset;
			unsigned __int16 triCount;
			XSurfaceCollisionTree* collisionTree;
		};

		struct XSurface
		{
			char tileMode;
			bool deformed;
			unsigned short vertCount;
			unsigned short triCount;
			unsigned char streamHandle;
			char zoneHandle;
			unsigned __int16 baseTriIndex;
			unsigned __int16 baseVertIndex;
			Face* triIndices;
			XSurfaceVertexInfo vertexInfo;
			GfxPackedVertex* verticies;
			int vertListCount;
			XRigidVertList* rigidVertLists;
			int partBits[6];
		};

		namespace alpha
		{
			struct D3DResource
			{
				unsigned int Common;
				unsigned int ReferenceCount;
				unsigned int Fence;
				unsigned int ReadFence;
				unsigned int Identifier;
				unsigned int BaseFlush;
			};

			struct D3DIndexBuffer : D3DResource
			{
				unsigned int Address;
				unsigned int Size;
			};

			union GPUVERTEX_FETCH_CONSTANT
			{
				unsigned int dword[2];
			};

			struct D3DVertexBuffer : D3DResource
			{
				GPUVERTEX_FETCH_CONSTANT Format;
			};
			
			struct XSurface
			{
				char tileMode;
				char deformed;
				unsigned __int16 vertCount;
				unsigned __int16 triCount;
				Face* triIndices;
				XSurfaceVertexInfo vertexInfo;
				GfxPackedVertex* verticies;
				D3DVertexBuffer vb0;
				unsigned int vertListCount;
				XRigidVertList* rigidVertLists;
				D3DIndexBuffer indexBuffer;
				int partBits[5];
			};

#pragma pack(push, 4)
			struct XModelSurfs
			{
				const char* name;
				XSurface* surfs;
				unsigned __int16 numsurfs;
				int partBits[5];
			};
#pragma pack(pop)

			struct XModelLodInfo
			{
				float dist;
				unsigned __int16 numSurfacesInLod;
				unsigned __int16 surfIndex;
				XModelSurfs* surfaces;
				int partBits[5];
				XSurface* surfs;
			};

			struct XModelCollSurf_s
			{
				Bounds bounds;
				int boneIdx;
				int contents;
				int surfFlags;
			};
		}

		struct XModelSurfs
		{
			const char* name;
			XSurface* surfs;
			unsigned __int16 numsurfs;
			unsigned __int16 pad;
			int partBits[6];
		};

		struct XModelLodInfo
		{
			float dist;
			short numSurfacesInLod;
			short surfIndex;
			XModelSurfs* surfaces;
			int partBits[6];
			XSurface* surfs;
			char lod;
			char smcBaseIndexPlusOne;
			char smcSubIndexMask;
			char smcBucket;
		};

		struct XModelCollTri_s
		{
			float plane[4];
			float svec[4];
			float tvec[4];
		};

		struct XModelCollSurf_s
		{
			XModelCollTri_s* tris;
			int numCollTris;
			Bounds bounds;
			int boneIdx;
			int contents;
			int surfFlags;
		};

		struct PhysCollmap;

		struct XModel
		{
			char* name;
			char numBones;
			char numRootBones;
			unsigned char numSurfaces;
			char lodRampType;
			float scale;
			unsigned int noScalePartBits[6];
			short* boneNames;
			unsigned char* parentList;
			XModelAngle* tagAngles;
			XModelTagPos* tagPositions;
			char* partClassification;
			DObjAnimMat* animMatrix;
			Material** materials;
			XModelLodInfo lods[4];
			char maxLoadedLod;
			char numLods;
			char collLod;
			char flags;
			XModelCollSurf_s* colSurf;
			int numColSurfs;
			int contents;
			XBoneInfo* boneInfo;
			float radius;
			Bounds bounds;
			int memUsage;
			bool bad;
			char pad[3];
			PhysPreset* physPreset;
			PhysCollmap* physCollmap;
		}; // total size 304

		namespace alpha
		{
#pragma pack(push, 4)
			struct XModel
			{
				const char* name;
				char numBones;
				char numRootBones;
				char numSurfaces;
				float scale;
				unsigned int noScalePartBits[5];
				__int16* boneNames;
				unsigned char* parentList;
				XModelAngle* tagAngles;
				XModelTagPos* tagPositions;
				char* partClassification;
				DObjAnimMat* animMatrix;
				Material** materials;
				XModelLodInfo lods[4];
				char maxLoadedLod;
				char numLods;
				char collLod;
				char flags;
				XModelCollSurf_s* colSurf;
				int numColSurfs;
				int contents;
				XBoneInfo* boneInfo;
				float radius;
				Bounds bounds;
				unsigned __int16* invHighMipRadius;
				int memUsage;
				PhysPreset* physPreset;
				PhysCollmap* physCollmap;
			};
#pragma pack(pop)
		}

		enum weapFireType_t : int
		{
			WEAPON_FIRETYPE_FULLAUTO = 0x0,
			WEAPON_FIRETYPE_SINGLESHOT = 0x1,
			WEAPON_FIRETYPE_BURSTFIRE2 = 0x2,
			WEAPON_FIRETYPE_BURSTFIRE3 = 0x3,
			WEAPON_FIRETYPE_BURSTFIRE4 = 0x4,
			WEAPON_FIRETYPE_DOUBLE_BARREL = 0x5,
			WEAPON_FIRETYPE_MAX
		};

		enum weapInventoryType_t : int
		{
			WEAPINVENTORY_PRIMARY = 0,
			WEAPINVENTORY_OFFHAND = 1,
			WEAPINVENTORY_ITEM = 2,
			WEAPINVENTORY_ALTMODE = 3,
			WEAPINVENTORY_EXCLUSIVE = 4,
			WEAPINVENTORY_SCAVENGER = 5,
			WEAPINVENTORY_MAX
		};

		enum PenetrateType
		{
			PENETRATE_TYPE_NONE = 0x0,
			PENETRATE_TYPE_SMALL = 0x1,
			PENETRATE_TYPE_MEDIUM = 0x2,
			PENETRATE_TYPE_LARGE = 0x3,
			PENETRATE_TYPE_COUNT = 0x4
		};

		enum activeReticleType_t : int
		{
			VEH_ACTIVE_RETICLE_NONE = 0,
			VEH_ACTIVE_RETICLE_PIP_ON_A_STICK = 1,
			VEH_ACTIVE_RETICLE_BOUNCING_DIAMOND = 2,
			VEH_ACTIVE_RETICLE_MAX
		};

		enum weapType_t : int
		{
			WEAPTYPE_BULLET = 0,
			WEAPTYPE_GRENADE = 1,
			WEAPTYPE_PROJECTILE = 2,
			WEAPTYPE_RIOTSHIELD = 3,
			WEAPTYPE_MAX
		};

		enum weapClass_t : int
		{
			WEAPCLASS_RIFLE = 0,
			WEAPCLASS_SNIPER = 1,
			WEAPCLASS_MG = 2,
			WEAPCLASS_SMG = 3,
			WEAPCLASS_SPREAD = 4,
			WEAPCLASS_PISTOL = 5,
			WEAPCLASS_GRENADE = 6,
			WEAPCLASS_ROCKETLAUNCHER = 7,
			WEAPCLASS_TURRET = 8,
			WEAPCLASS_THROWINGKNIFE = 9,
			WEAPCLASS_NON_PLAYER = 10,
			WEAPCLASS_ITEM = 11,
			WEAPCLASS_MAX
		};

		enum OffhandClass : int
		{
			OFFHAND_CLASS_NONE = 0,
			OFFHAND_CLASS_FRAG_GRENADE = 1,
			OFFHAND_CLASS_SMOKE_GRENADE = 2,
			OFFHAND_CLASS_FLASH_GRENADE = 3,
			OFFHAND_CLASS_MAX
		};

		enum playerAnimType_t : int
		{
			PLAER_ANIM_TYPE_NONE = 0x0,
			PLAER_ANIM_TYPE_OTHER = 0x1,
			PLAER_ANIM_TYPE_PISTOL = 0x2,
			PLAER_ANIM_TYPE_SMG = 0x3,
			PLAER_ANIM_TYPE_AUTORIFLE = 0x4,
			PLAER_ANIM_TYPE_MG = 0x5,
			PLAER_ANIM_TYPE_SNIPER = 0x6,
			PLAER_ANIM_TYPE_ROCKETLAUNCHER = 0x7,
			PLAER_ANIM_TYPE_EXPLOSIVE = 0x8,
			PLAER_ANIM_TYPE_GRENADE = 0x9,
			PLAER_ANIM_TYPE_TURRET = 0xA,
			PLAER_ANIM_TYPE_C4 = 0xB,
			PLAER_ANIM_TYPE_M203 = 0xC,
			PLAER_ANIM_TYPE_HOLD = 0xD,
			PLAER_ANIM_TYPE_BRIEFCASE = 0xE,
			PLAER_ANIM_TYPE_RIOTSHIELD = 0xF,
			PLAER_ANIM_TYPE_LAPTOP = 0x10,
			PLAER_ANIM_TYPE_THROWINGKNIFE = 0x11
		};

		enum weapProjExplosion_t
		{
			WEAPPROJEXP_GRENADE = 0x0,
			WEAPPROJEXP_ROCKET = 0x1,
			WEAPPROJEXP_FLASHBANG = 0x2,
			WEAPPROJEXP_NONE = 0x3,
			WEAPPROJEXP_DUD = 0x4,
			WEAPPROJEXP_SMOKE = 0x5,
			WEAPPROJEXP_HEAVY = 0x6,
			WEAPPROJEXP_NUM = 0x7
		};

		enum WeapStickinessType
		{
			WEAPSTICKINESS_NONE = 0x0,
			WEAPSTICKINESS_ALL = 0x1,
			WEAPSTICKINESS_ALL_ORIENT = 0x2,
			WEAPSTICKINESS_GROUND = 0x3,
			WEAPSTICKINESS_GROUND_WITH_YAW = 0x4,
			WEAPSTICKINESS_KNIFE = 0x5,
			WEAPSTICKINESS_COUNT = 0x6
		};

		enum weaponIconRatioType_t
		{
			WEAPON_ICON_RATIO_1TO1 = 0x0,
			WEAPON_ICON_RATIO_2TO1 = 0x1,
			WEAPON_ICON_RATIO_4TO1 = 0x2,
			WEAPON_ICON_RATIO_COUNT = 0x3
		};

		enum ammoCounterClipType_t
		{
			AMMO_COUNTER_CLIP_NONE = 0x0,
			AMMO_COUNTER_CLIP_MAGAZINE = 0x1,
			AMMO_COUNTER_CLIP_SHORTMAGAZINE = 0x2,
			AMMO_COUNTER_CLIP_SHOTGUN = 0x3,
			AMMO_COUNTER_CLIP_ROCKET = 0x4,
			AMMO_COUNTER_CLIP_BELTFED = 0x5,
			AMMO_COUNTER_CLIP_ALTWEAPON = 0x6,
			AMMO_COUNTER_CLIP_COUNT = 0x7
		};

		enum weapOverlayReticle_t
		{
			WEAPOVERLAYRETICLE_NONE = 0x0,
			WEAPOVERLAYRETICLE_CROSSHAIR = 0x1,
			WEAPOVERLAYRETICLE_NUM = 0x2
		};

		enum weapOverlayInterface_t
		{
			WEAPOVERLAYINTERFACE_NONE = 0x0,
			WEAPOVERLAYINTERFACE_JAVELIN = 0x1,
			WEAPOVERLAYINTERFACE_TURRETSCOPE = 0x2,
			WEAPOVERLAYINTERFACECOUNT = 0x3
		};

		enum weapStance_t
		{
			WEAPSTANCE_STAND = 0x0,
			WEAPSTANCE_DUCK = 0x1,
			WEAPSTANCE_PRONE = 0x2,
			WEAPSTANCE_NUM = 0x3
		};

		enum ImpactType
		{
			IMPACT_TYPE_NONE = 0,
			IMPACT_TYPE_BULLET_SMALL = 1,
			IMPACT_TYPE_BULLET_LARGE = 2,
			IMPACT_TYPE_BULLET_AP = 3,
			IMPACT_TYPE_SHOTGUN_FMJ = 4,
			IMPACT_TYPE_SHOTGUN = 5,
			IMPACT_TYPE_GRENADE_BOUNCE = 7,
			IMPACT_TYPE_GRENADE_EXPLODE = 8,
			IMPACT_TYPE_ROCKET_EXPLODE = 9,
			IMPACT_TYPE_PROJECTILE_DUD = 10,
			IMPACT_TYPE_MAX
		};

		enum guidedMissileType_t
		{
			MISSILE_GUIDANCE_NONE = 0x0,
			MISSILE_GUIDANCE_SIDEWINDER = 0x1,
			MISSILE_GUIDANCE_HELLFIRE = 0x2,
			MISSILE_GUIDANCE_JAVELIN = 0x3,
			MISSILE_GUIDANCE_MAX
		};

		// Check knots in FF later!
#pragma pack(push, 4)
		struct SndCurve
		{
			const char* filename;
			unsigned __int16 knotCount;
			vec2_t knots[16];
		};
#pragma pack(pop)

        struct _AILSOUNDINFO
        {
            int format;
            const void* data_ptr;
            unsigned int data_len;
            unsigned int rate;
            int bits;
            int channels;
            unsigned int samples;
            unsigned int block_size;
            const void* initial_ptr;
        };

		// Loaded sound
#pragma pack(push, 4)
		struct MssSound
		{
            _AILSOUNDINFO info;
            char* data;
		};
#pragma pack(pop)

		struct LoadedSound
		{
			const char* name;
            MssSound sound;
		};

		// Sounds
		struct SpeakerLevels
		{
			int speaker;
			int numLevels;
			float levels[2];
		};

		struct ChannelMap
		{
			int entryCount; // how many entries are used
			SpeakerLevels speakers[6];
		};

		struct SpeakerMap
		{
			bool isDefault;
			char _pad[3];
			const char* name;
			ChannelMap channelMaps[2][2];
		};

		enum snd_alias_type_t : char
		{
			SAT_UNKNOWN = 0x0,
			SAT_LOADED = 0x1,
			SAT_STREAMED = 0x2,
			SAT_PRIMED = 0x3,
			SAT_COUNT = 0x4,
		};

		struct StreamFileNamePacked
		{
			unsigned __int64 offset;
			unsigned __int64 length;
		};

		struct StreamFileNameRaw
		{
			const char* dir;
			const char* name;
		};

		union StreamFileInfo
		{
			StreamFileNameRaw raw;
			StreamFileNamePacked packed;
		};

		struct StreamFileName
		{
			unsigned __int16 isLocalized;
			unsigned __int16 fileIndex;
			StreamFileInfo info;
		};

		/*struct StreamedSound
		{
		StreamFileName filename;
		unsigned int totalMsec;
		};*/
		struct StreamedSound
		{
			const char* dir;
			const char* name;
		};

		struct PrimedSound
		{
			StreamFileName filename;
			LoadedSound* loadedPart;
			int dataOffset;
			int totalSize;
			unsigned int primedCrc;
		};

		union SoundData
		{
			LoadedSound* loadSnd; // SoundFile->type == SAT_LOADED
			StreamedSound streamSnd; // SoundFile->type == SAT_STREAMED
			//PrimedSound primedSnd;	// SoundFile->type == SAT_PRIMED
		};

		struct SoundFile // 0x10
		{
			char type;
			bool exists;
			char _pad[2];
			SoundData sound;
		};
#pragma pack(push, 4)
		union $C8D87EB0090687D323381DFB7A82089C
		{
			float slavePercentage;
			float masterPercentage;
		};
		
		struct snd_alias_t
		{
			const char* aliasName;
			const char* subtitle;
			const char* secondaryAliasName;
			const char* chainAliasName;
			const char* mixerGroup;
			SoundFile* soundFile;
			int sequence;
			float volMin;
			float volMax;
			float pitchMin;
			float pitchMax;
			float distMin;
			float distMax;
			float velocityMin;
			int flags;
			$C8D87EB0090687D323381DFB7A82089C ___u15;
			float probability;
			float lfePercentage;
			float centerPercentage;
			int startDelay;
			SndCurve* volumeFalloffCurve;
			float envelopMin;
			float envelopMax;
			float envelopPercentage;
			SpeakerMap* speakerMap;
		};
#pragma pack(pop)
		struct snd_alias_list_t
		{
			union
			{
				const char* aliasName;
				const char* name;
			};

			snd_alias_t* head;
			int count;
		};

		union snd_alias_list_name
		{
			const char* name;
			snd_alias_list_t* asset;
		};

		// Tracers
		struct TracerDef
		{
			const char* name;
			Material* material;
			unsigned int drawInterval;
			float speed;
			float beamLength;
			float beamWidth;
			float screwRadius;
			float screwDist;
			float colors[5][4];
		};

		struct WeaponDef
		{
			const char* szOverlayName;
			XModel** gunXModel;
			XModel* handXModel;
			const char** szXAnimsRightHanded;
			const char** szXAnimsLeftHanded;
			const char* szModeName;
			unsigned __int16* notetrackSoundMapKeys;
			unsigned __int16* notetrackSoundMapValues;
			unsigned __int16* notetrackRumbleMapKeys;
			unsigned __int16* notetrackRumbleMapValues;
			int playerAnimType;
			weapType_t weapType;
			weapClass_t weapClass;
			PenetrateType penetrateType;
			weapInventoryType_t inventoryType;
			weapFireType_t fireType;
			OffhandClass offhandClass;
			weapStance_t stance;
			void* viewFlashEffect;
			void* worldFlashEffect;
			union
			{
				struct
				{
					snd_alias_list_t* pickupSound;
					snd_alias_list_t* pickupSoundPlayer;
					snd_alias_list_t* ammoPickupSound;
					snd_alias_list_t* ammoPickupSoundPlayer;
					snd_alias_list_t* projectileSound;
					snd_alias_list_t* pullbackSound;
					snd_alias_list_t* pullbackSoundPlayer;
					snd_alias_list_t* fireSound;
					snd_alias_list_t* fireSoundPlayer;
					snd_alias_list_t* fireSoundPlayerAkimbo;
					snd_alias_list_t* fireLoopSound;
					snd_alias_list_t* fireLoopSoundPlayer;
					snd_alias_list_t* fireStopSound;
					snd_alias_list_t* fireStopSoundPlayer;
					snd_alias_list_t* fireLastSound;
					snd_alias_list_t* fireLastSoundPlayer;
					snd_alias_list_t* emptyFireSound;
					snd_alias_list_t* emptyFireSoundPlayer;
					snd_alias_list_t* meleeSwipeSound;
					snd_alias_list_t* meleeSwipeSoundPlayer;
					snd_alias_list_t* meleeHitSound;
					snd_alias_list_t* meleeMissSound;
					snd_alias_list_t* rechamberSound;
					snd_alias_list_t* rechamberSoundPlayer;
					snd_alias_list_t* reloadSound;
					snd_alias_list_t* reloadSoundPlayer;
					snd_alias_list_t* reloadEmptySound;
					snd_alias_list_t* reloadEmptySoundPlayer;
					snd_alias_list_t* reloadStartSound;
					snd_alias_list_t* reloadStartSoundPlayer;
					snd_alias_list_t* reloadEndSound;
					snd_alias_list_t* reloadEndSoundPlayer;
					snd_alias_list_t* detonateSound;
					snd_alias_list_t* detonateSoundPlayer;
					snd_alias_list_t* nightVisionWearSound;
					snd_alias_list_t* nightVisionWearSoundPlayer;
					snd_alias_list_t* nightVisionRemoveSound;
					snd_alias_list_t* nightVisionRemoveSoundPlayer;
					snd_alias_list_t* altSwitchSound;
					snd_alias_list_t* altSwitchSoundPlayer;
					snd_alias_list_t* raiseSound;
					snd_alias_list_t* raiseSoundPlayer;
					snd_alias_list_t* firstRaiseSound;
					snd_alias_list_t* firstRaiseSoundPlayer;
					snd_alias_list_t* putawaySound;
					snd_alias_list_t* putawaySoundPlayer;
					snd_alias_list_t* scanSound;
				};
				snd_alias_list_t* sounds[47];
			};
			snd_alias_list_t** bounceSound;
			void* viewShellEjectEffect;
			void* worldShellEjectEffect;
			void* viewLastShotEjectEffect;
			void* worldLastShotEjectEffect;
			Material* reticleCenter;
			Material* reticleSide;
			int iReticleCenterSize;
			int iReticleSideSize;
			int iReticleMinOfs;
			activeReticleType_t activeReticleType;
			float vStandMove[3];
			float vStandRot[3];
			float strafeMove[3];
			float strafeRot[3];
			float vDuckedOfs[3];
			float vDuckedMove[3];
			float vDuckedRot[3];
			float vProneOfs[3];
			float vProneMove[3];
			float vProneRot[3];
			float fPosMoveRate;
			float fPosProneMoveRate;
			float fStandMoveMinSpeed;
			float fDuckedMoveMinSpeed;
			float fProneMoveMinSpeed;
			float fPosRotRate;
			float fPosProneRotRate;
			float fStandRotMinSpeed;
			float fDuckedRotMinSpeed;
			float fProneRotMinSpeed;
			XModel** worldModel;
			XModel* worldClipModel;
			XModel* rocketModel;
			XModel* knifeModel;
			XModel* worldKnifeModel;
			Material* hudIcon;
			weaponIconRatioType_t hudIconRatio;
			Material* pickupIcon;
			weaponIconRatioType_t pickupIconRatio;
			Material* ammoCounterIcon;
			weaponIconRatioType_t ammoCounterIconRatio;
			ammoCounterClipType_t ammoCounterClip;
			int iStartAmmo;
			const char* szAmmoName;
			int iAmmoIndex;
			const char* szClipName;
			int iClipIndex;
			int iMaxAmmo;
			int shotCount;
			const char* szSharedAmmoCapName;
			int iSharedAmmoCapIndex;
			int iSharedAmmoCap;
			int damage;
			int playerDamage;
			int iMeleeDamage;
			int iDamageType;
			int iFireDelay;
			int iMeleeDelay;
			int meleeChargeDelay;
			int iDetonateDelay;
			int iRechamberTime;
			int rechamberTimeOneHanded;
			int iRechamberBoltTime;
			int iHoldFireTime;
			int iDetonateTime;
			int iMeleeTime;
			int meleeChargeTime;
			int iReloadTime;
			int reloadShowRocketTime;
			int iReloadEmptyTime;
			int iReloadAddTime;
			int iReloadStartTime;
			int iReloadStartAddTime;
			int iReloadEndTime;
			int iDropTime;
			int iRaiseTime;
			int iAltDropTime;
			int quickDropTime;
			int quickRaiseTime;
			int iBreachRaiseTime;
			int iEmptyRaiseTime;
			int iEmptyDropTime;
			int sprintInTime;
			int sprintLoopTime;
			int sprintOutTime;
			int stunnedTimeBegin;
			int stunnedTimeLoop;
			int stunnedTimeEnd;
			int nightVisionWearTime;
			int nightVisionWearTimeFadeOutEnd;
			int nightVisionWearTimePowerUp;
			int nightVisionRemoveTime;
			int nightVisionRemoveTimePowerDown;
			int nightVisionRemoveTimeFadeInStart;
			int fuseTime;
			int aiFuseTime;
			float autoAimRange;
			float aimAssistRange;
			float aimAssistRangeAds;
			float aimPadding;
			float enemyCrosshairRange;
			float moveSpeedScale;
			float adsMoveSpeedScale;
			float sprintDurationScale;
			float fAdsZoomInFrac;
			float fAdsZoomOutFrac;
			Material* overlayMaterial;
			Material* overlayMaterialLowRes;
			Material* overlayMaterialEMP;
			Material* overlayMaterialEMPLowRes;
			weapOverlayReticle_t overlayReticle;
			int overlayInterface;
			float overlayWidth;
			float overlayHeight;
			float overlayWidthSplitscreen;
			float overlayHeightSplitscreen;
			float fAdsBobFactor;
			float fAdsViewBobMult;
			float fHipSpreadStandMin;
			float fHipSpreadDuckedMin;
			float fHipSpreadProneMin;
			float hipSpreadStandMax;
			float hipSpreadDuckedMax;
			float hipSpreadProneMax;
			float fHipSpreadDecayRate;
			float fHipSpreadFireAdd;
			float fHipSpreadTurnAdd;
			float fHipSpreadMoveAdd;
			float fHipSpreadDuckedDecay;
			float fHipSpreadProneDecay;
			float fHipReticleSidePos;
			float fAdsIdleAmount;
			float fHipIdleAmount;
			float adsIdleSpeed;
			float hipIdleSpeed;
			float fIdleCrouchFactor;
			float fIdleProneFactor;
			float fGunMaxPitch;
			float fGunMaxYaw;
			float swayMaxAngle;
			float swayLerpSpeed;
			float swayPitchScale;
			float swayYawScale;
			float swayHorizScale;
			float swayVertScale;
			float swayShellShockScale;
			float adsSwayMaxAngle;
			float adsSwayLerpSpeed;
			float adsSwayPitchScale;
			float adsSwayYawScale;
			float adsSwayHorizScale;
			float adsSwayVertScale;
			float adsViewErrorMin;
			float adsViewErrorMax;
			PhysCollmap* physCollmap;
			float dualWieldViewModelOffset;
			weaponIconRatioType_t killIconRatio;
			int iReloadAmmoAdd;
			int iReloadStartAdd;
			int ammoDropStockMin;
			int ammoDropClipPercentMin;
			int ammoDropClipPercentMax;
			int iExplosionRadius;
			int iExplosionRadiusMin;
			int iExplosionInnerDamage;
			int iExplosionOuterDamage;
			float damageConeAngle;
			float bulletExplDmgMult;
			float bulletExplRadiusMult;
			int iProjectileSpeed;
			int iProjectileSpeedUp;
			int iProjectileSpeedForward;
			int iProjectileActivateDist;
			float projLifetime;
			float timeToAccelerate;
			float projectileCurvature;
			XModel* projectileModel;
			int projExplosion;
			void* projExplosionEffect;
			void* projDudEffect;
			snd_alias_list_t* projExplosionSound;
			snd_alias_list_t* projDudSound;
			WeapStickinessType stickiness;
			float lowAmmoWarningThreshold;
			float ricochetChance;
			float* parallelBounce;
			float* perpendicularBounce;
			void* projTrailEffect;
			void* projBeaconEffect;
			float vProjectileColor[3];
			guidedMissileType_t guidedMissileType;
			float maxSteeringAccel;
			int projIgnitionDelay;
			void* projIgnitionEffect;
			snd_alias_list_t* projIgnitionSound;
			float fAdsAimPitch;
			float fAdsCrosshairInFrac;
			float fAdsCrosshairOutFrac;
			int adsGunKickReducedKickBullets;
			float adsGunKickReducedKickPercent;
			float fAdsGunKickPitchMin;
			float fAdsGunKickPitchMax;
			float fAdsGunKickYawMin;
			float fAdsGunKickYawMax;
			float fAdsGunKickAccel;
			float fAdsGunKickSpeedMax;
			float fAdsGunKickSpeedDecay;
			float fAdsGunKickStaticDecay;
			float fAdsViewKickPitchMin;
			float fAdsViewKickPitchMax;
			float fAdsViewKickYawMin;
			float fAdsViewKickYawMax;
			float fAdsViewScatterMin;
			float fAdsViewScatterMax;
			float fAdsSpread;
			int hipGunKickReducedKickBullets;
			float hipGunKickReducedKickPercent;
			float fHipGunKickPitchMin;
			float fHipGunKickPitchMax;
			float fHipGunKickYawMin;
			float fHipGunKickYawMax;
			float fHipGunKickAccel;
			float fHipGunKickSpeedMax;
			float fHipGunKickSpeedDecay;
			float fHipGunKickStaticDecay;
			float fHipViewKickPitchMin;
			float fHipViewKickPitchMax;
			float fHipViewKickYawMin;
			float fHipViewKickYawMax;
			float fHipViewScatterMin;
			float fHipViewScatterMax;
			float fightDist;
			float maxDist;
			const char* accuracyGraphName[2];
			float(*originalAccuracyGraphKnots[2])[2];
			unsigned __int16 originalAccuracyGraphKnotCount[2];
			int iPositionReloadTransTime;
			float leftArc;
			float rightArc;
			float topArc;
			float bottomArc;
			float accuracy;
			float aiSpread;
			float playerSpread;
			float minTurnSpeed[2];
			float maxTurnSpeed[2];
			float pitchConvergenceTime;
			float yawConvergenceTime;
			float suppressTime;
			float maxRange;
			float fAnimHorRotateInc;
			float fPlayerPositionDist;
			const char* szUseHintString;
			const char* dropHintString;
			int iUseHintStringIndex;
			int dropHintStringIndex;
			float horizViewJitter;
			float vertViewJitter;
			float scanSpeed;
			float scanAccel;
			int scanPauseTime;
			const char* szScript;
			float fOOPosAnimLength[2];
			int minDamage;
			int minPlayerDamage;
			float fMaxDamageRange;
			float fMinDamageRange;
			float destabilizationRateTime;
			float destabilizationCurvatureMax;
			int destabilizeDistance;
			float* locationDamageMultipliers;
			const char* fireRumble;
			const char* meleeImpactRumble;
			TracerDef* tracerType;
			float turretScopeZoomRate;
			float turretScopeZoomMin;
			float turretScopeZoomMax;
			float turretOverheatUpRate;
			float turretOverheatDownRate;
			float turretOverheatPenalty;
			snd_alias_list_t* turretOverheatSound;
			void* turretOverheatEffect;
			const char* turretBarrelSpinRumble;
			float turretBarrelSpinSpeed;
			float turretBarrelSpinUpTime;
			float turretBarrelSpinDownTime;
			snd_alias_list_t* turretBarrelSpinMaxSnd;
			snd_alias_list_t* turretBarrelSpinUpSnd[4];
			snd_alias_list_t* turretBarrelSpinDownSnd[4];
			snd_alias_list_t* missileConeSoundAlias;
			snd_alias_list_t* missileConeSoundAliasAtBase;
			float missileConeSoundRadiusAtTop;
			float missileConeSoundRadiusAtBase;
			float missileConeSoundHeight;
			float missileConeSoundOriginOffset;
			float missileConeSoundVolumescaleAtCore;
			float missileConeSoundVolumescaleAtEdge;
			float missileConeSoundVolumescaleCoreSize;
			float missileConeSoundPitchAtTop;
			float missileConeSoundPitchAtBottom;
			float missileConeSoundPitchTopSize;
			float missileConeSoundPitchBottomSize;
			float missileConeSoundCrossfadeTopSize;
			float missileConeSoundCrossfadeBottomSize;
			bool sharedAmmo;
			bool lockonSupported;
			bool requireLockonToFire;
			bool bigExplosion;
			bool noAdsWhenMagEmpty;
			bool avoidDropCleanup;
			bool inheritsPerks;
			bool crosshairColorChange;
			bool bRifleBullet;
			bool armorPiercing;
			bool bBoltAction;
			bool aimDownSight;
			bool bRechamberWhileAds;
			bool bBulletExplosiveDamage;
			bool bCookOffHold;
			bool bClipOnly;
			bool noAmmoPickup;
			bool adsFireOnly;
			bool cancelAutoHolsterWhenEmpty;
			bool disableSwitchToWhenEmpty;
			bool suppressAmmoReserveDisplay;
			bool laserSightDuringNightvision;
			bool markableViewmodel;
			bool noDualWield;
			bool flipKillIcon;
			bool bNoPartialReload;
			bool bSegmentedReload;
			bool blocksProne;
			bool silenced;
			bool isRollingGrenade;
			bool projExplosionEffectForceNormalUp;
			bool bProjImpactExplode;
			bool stickToPlayers;
			bool hasDetonator;
			bool disableFiring;
			bool timedDetonation;
			bool rotate;
			bool holdButtonToThrow;
			bool freezeMovementWhenFiring;
			bool thermalScope;
			bool altModeSameWeapon;
			bool turretBarrelSpinEnabled;
			bool missileConeSoundEnabled;
			bool missileConeSoundPitchshiftEnabled;
			bool missileConeSoundCrossfadeEnabled;
			bool offhandHoldIsCancelable;
		};

#pragma pack(push, 4)
		struct WeaponCompleteDef
		{
			const char* szInternalName;
			WeaponDef* weapDef;
			const char* szDisplayName;
			unsigned __int16* hideTags;
			const char** szXAnims;
			float fAdsZoomFov;
			int iAdsTransInTime;
			int iAdsTransOutTime;
			int iClipSize;
			ImpactType impactType;
			int iFireTime;
			weaponIconRatioType_t dpadIconRatio;
			float penetrateMultiplier;
			float fAdsViewKickCenterSpeed;
			float fHipViewKickCenterSpeed;
			const char* szAltWeaponName;
			unsigned int altWeaponIndex;
			int iAltRaiseTime;
			Material* killIcon;
			Material* dpadIcon;
			int fireAnimLength;
			int iFirstRaiseTime;
			int ammoDropStockMax;
			float adsDofStart;
			float adsDofEnd;
			unsigned __int16 accuracyGraphKnotCount[2];
			float(*accuracyGraphKnots[2])[2];
			bool motionTracker;
			bool enhanced;
			bool dpadIconShowsAmmo;
		};
#pragma pack(pop)

		struct Glyph
		{
			unsigned __int16 letter;
			char x0;
			char y0;
			char dx;
			char pixelWidth;
			char pixelHeight;
			float s0;
			float t0;
			float s1;
			float t1;
		};

#pragma pack(push, 4)
		struct cplane_s
		{
			float normal[3];
			float dist;
			char type;
			char signbits;
		};
#pragma pack(pop)

		/* 1003 */
		struct Font_s
		{
			const char* fontName;
			int pixelHeight;
			int glyphCount;
			Material* material;
			Material* glowMaterial;
			Glyph* glyphs;
		};

		struct cbrushside_t
		{
			cplane_s *plane;
			unsigned __int16 materialNum;
			char firstAdjacentSideOffset;
			char edgeCount;
		};

		// ClipMap
		typedef char cbrushedge_t;

		struct cbrush_t
		{
			unsigned __int16 numsides;
			unsigned __int16 glassPieceIndex;
			cbrushside_t* sides;
			cbrushedge_t* edge;
			__int16 axialMaterialNum[2][3];
			char firstAdjacentSideOffsets[2][3];
			char edgeCount[2][3];
		};

		struct BrushWrapper
		{
			float mins[3];
			float maxs[3];
			unsigned int numPlaneSide;
			cbrushside_t* side;
			char* edge;
			__int16 axialMaterialNum[2][3];
			__int16 firstAdjacentSideOffsets[2][3];
			int numEdge;
			cplane_s* plane;
		};

		//struct BrushWrapper
		//{
		//	Bounds bounds; // 24
		//	cbrush_t brush; // 36
		//	cbrushside_t *side; // 4
		//	int totalEdgeCount; // 4
		//	cplane_s *plane; // 4
		//	short numPlaneSide; // 2
		//	char *edge; // 4
		//	int numEdge; // 4
		//}; // 82 bytes total

		struct PhysGeomInfo
		{
			BrushWrapper* brush;
			int type;
			float orientation[3][3];
			Bounds bounds;
		};

		struct PhysMass
		{
			float centerOfMass[3];
			float momentsOfInertia[3];
			float productsOfInertia[3];
			// int contents;
		};

		struct PhysCollmap
		{
			const char* name;
			unsigned int numInfo;
			PhysGeomInfo* info;
			PhysMass mass;
			Bounds bounds;
		};

		struct G_GlassName
		{
			char* nameStr;
			unsigned __int16 name;
			unsigned __int16 pieceCount;
			unsigned __int16* pieceIndices;
		};

#pragma pack(push, 2)
		struct G_GlassPiece
		{
			unsigned __int16 damageTaken;
			unsigned __int16 collapseTime;
			int lastStateChangeTime;
			char impactDir;
			char impactPos[2];
		};
#pragma pack(pop)

		struct G_GlassData
		{
			G_GlassPiece* glassPieces;
			unsigned int pieceCount;
			unsigned __int16 damageToWeaken;
			unsigned __int16 damageToDestroy;
			unsigned int glassNameCount;
			G_GlassName* glassNames;
			char pad[108];
		};

		struct GameWorldMp
		{
			const char* name;
			G_GlassData* g_glassData;
		};

		struct GameWorldSp
		{
			const char* name;
			char useless_sp_shit[48];
			G_GlassData* g_glassData;
		};

		// FxWorld
#pragma pack(push, 4)

		struct FxGlassDef
		{
			float halfThickness;
			float texVecs[2][2];
			GfxColor color;
			Material* material;
			Material* materialShattered;
			PhysPreset* physPreset;
		};

		struct FxSpatialFrame
		{
			float quat[4];
			float origin[3];
		};

		union FxGlassPiecePlace
		{
			struct
			{
				FxSpatialFrame frame;
				float radius;
			};

			unsigned int nextFree;
		};

		struct FxGlassPieceState
		{
			float texCoordOrigin[2];
			unsigned int supportMask;
			unsigned __int16 initIndex;
			unsigned __int16 geoDataStart;
			unsigned __int16 lightingIndex;
			char defIndex;
			char pad[3];
			char vertCount;
			char holeDataCount;
			char crackDataCount;
			char fanDataCount;
			unsigned __int16 flags;
			float areaX2;
		};

		struct FxGlassPieceDynamics
		{
			int fallTime;
			__int32 physObjId;
			__int32 physJointId;
			float vel[3];
			float avel[3];
		};

		struct FxGlassVertex
		{
			__int16 x;
			__int16 y;
		};

		struct FxGlassHoleHeader
		{
			unsigned __int16 uniqueVertCount;
			char touchVert;
			char pad[1];
		};

		struct FxGlassCrackHeader
		{
			unsigned __int16 uniqueVertCount;
			char beginVertIndex;
			char endVertIndex;
		};

		union FxGlassGeometryData
		{
			FxGlassVertex vert;
			FxGlassHoleHeader hole;
			FxGlassCrackHeader crack;
			char asBytes[4];
			__int16 anonymous[2];
		};

		struct FxGlassInitPieceState //Note, on MW3 this is missing 4 bytes, just not sure whats missing yet
		{
			FxSpatialFrame frame;
			float radius;
			float texCoordOrigin[2];
			unsigned int supportMask;
			//float areaX2; // Commented out a random thing so the size fits. Most probably wrong since it was random.
			unsigned __int16 lightingIndex;
			char defIndex;
			char vertCount;
			char fanDataCount;
			char pad[1];
		};

		struct FxGlassSystem
		{
			int time; // 4
			int prevTime; // 4
			unsigned int defCount; // 4
			unsigned int pieceLimit; // 4
			unsigned int pieceWordCount; // 4
			unsigned int initPieceCount; // 4
			unsigned int cellCount; // 4
			unsigned int activePieceCount; // 4
			unsigned int firstFreePiece; // 4
			unsigned int geoDataLimit; // 4
			unsigned int geoDataCount; // 4
			unsigned int initGeoDataCount; // 4
			FxGlassDef* defs; // 4
			FxGlassPiecePlace* piecePlaces; // 4
			FxGlassPieceState* pieceStates; // 4
			FxGlassPieceDynamics* pieceDynamics; // 4
			FxGlassGeometryData* geoData; // 4
			unsigned int* isInUse; // 4
			unsigned int* cellBits; // 4
			char* visData; // 4
			VecInternal<3>* linkOrg;
			float* halfThickness; // 4
			unsigned __int16* lightingHandles; // 4
			FxGlassInitPieceState* initPieceStates; // 4
			FxGlassGeometryData* initGeoData; // 4
			bool needToCompactData; // 1
			char initCount;
			short pad;
			float effectChanceAccum; // 4
			int lastPieceDeletionTime; // 4
		};

		struct FxWorld
		{
			char* name;
			FxGlassSystem glassSys;
		};
#pragma pack(pop)

		// MapEnts
		struct TriggerModel
		{
			int contents;
			unsigned short hullCount;
			unsigned short firstHull;
		};

		struct TriggerHull
		{
			Bounds bounds;
			int contents;
			unsigned short slabCount;
			unsigned short firstSlab;
		};

		struct TriggerSlab
		{
			vec3_t dir;
			float midPoint;
			float halfSize;
		};

		struct MapTriggers
		{
			int modelCount;
			TriggerModel* models; // sizeof 8
			int hullCount;
			TriggerHull* hulls; // sizeof 32
			int slabCount;
			TriggerSlab* slabs; // sizeof 20
		};
#pragma pack(push, 1)
		struct Stage
		{
			char* stageName;
			float offset[3];
			unsigned __int16 triggerIndex;
			char sunPrimaryLightIndex;
			char pad;
		};

		struct MapEnts
		{
			const char* name; // 0
			const char* entityString; // 4
			int numEntityChars; // 8
			MapTriggers trigger; // 12
			Stage* stageNames; // 36
			char stageCount; // 40
			char pad[3];
		};
#pragma pack(pop)

#pragma pack(push, 1)
		struct ComPrimaryLight
		{
			union
			{
				char _portpad0[28];

				struct
				{
					char type;
					char canUseShadowMap;
					char exponent;
					char unused;
					float color[3];
					float dir[3];
				};
			};

			union
			{
				char _portpad1[40];

				struct
				{
					float origin[3];
					float radius;
					float cosHalfFovOuter;
					float cosHalfFovInner;
					float cosHalfFovExpanded;
					float rotationLimit;
					float translationLimit;
					const char* defName;
				};
			};
		};
#pragma pack(pop)

		struct ComWorld
		{
			const char* name;
			int isInUse;
			unsigned int primaryLightCount;
			ComPrimaryLight* primaryLights;
		};


		union XAnimIndices
		{
			char* _1;
			unsigned __int16* _2;
			void* data;
		};

		union XAnimDynamicFrames
		{
			char (*_1)[3];
			unsigned __int16 (*_2)[3];
		};

		union XAnimDynamicIndices
		{
			char _1[1];
			unsigned __int16 _2[1];
		};

		struct XAnimPartTransFrames
		{
			float mins[3];
			float size[3];
			XAnimDynamicFrames frames;
			XAnimDynamicIndices indices;
		};

		union XAnimPartTransData
		{
			XAnimPartTransFrames frames;
			float frame0[3];
		};

		struct XAnimPartTrans
		{
			unsigned __int16 size;
			char smallTrans;
			__declspec(align(2)) XAnimPartTransData u;
		};

#pragma pack(push, 4)
		struct XAnimDeltaPartQuatDataFrames2
		{
			__int16(*frames)[2];
			XAnimDynamicIndices indices;
		};

		union XAnimDeltaPartQuatData2
		{
			XAnimDeltaPartQuatDataFrames2 frames;
			__int16 frame0[2];
		};

		struct XAnimDeltaPartQuat2
		{
			unsigned __int16 size;
			XAnimDeltaPartQuatData2 u;
		};

		struct XAnimDeltaPartQuatDataFrames
		{
			__int16(*frames)[4];
			XAnimDynamicIndices indices;
		};

		union XAnimDeltaPartQuatData
		{
			XAnimDeltaPartQuatDataFrames frames;
			__int16 frame0[4];
		};

		struct XAnimDeltaPartQuat
		{
			unsigned __int16 size;
			XAnimDeltaPartQuatData u;
		};

		struct XAnimDeltaPart
		{
			XAnimPartTrans* trans;
			XAnimDeltaPartQuat2* quat2;
			XAnimDeltaPartQuat* quat;
		};

		struct XAnimNotifyInfo
		{
			short name;
			float time;
		};

        enum XAnimPartsFlags
        {
            ANIM_LOOP = 0x1,
            ANIM_DELTA = 0x2,
            ANIM_DELTA_3D = 0x4,
        };

		struct XAnimParts
		{
			char* name; // 0
			unsigned short dataByteCount; // 4
			unsigned short dataShortCount; // 6
			unsigned short dataIntCount; // 8
			unsigned short randomDataByteCount; // 10 - 0xA
			unsigned short randomDataIntCount; // 12 - 0xC
			unsigned short framecount; // 14 - 0xE
			char flags; // 16
			unsigned char boneCount[10]; // 17
			char notifyCount; // 27
			char assetType; // 30
			bool isDefault; // 31
			unsigned int randomDataShortCount; // 32 - 0x20
			unsigned int indexcount; // 36 - 0x24
			float framerate; // 40 - 0x28
			float frequency; // 44 - 0x2C
			unsigned short* tagnames; // 48 - 0x30
			char* dataByte; // 52 - 0x34
			short* dataShort; // 56 - 0x38
			int* dataInt; // 60 - 0x3C
			short* randomDataShort; // 64 - 0x40
			char* randomDataByte; // 68 - 0x44
			int* randomDataInt; // 72 - 0x48
			XAnimIndices indices; // 76 - 0x4C
			XAnimNotifyInfo* notify; // 80 - 0x50
			XAnimDeltaPart* delta; // 84 - 0x54
		};
#pragma pack(pop)

		// Localized Strings
		struct LocalizeEntry
		{
			const char* localizedString;
			const char* name;
		};

		// Stringtables
		struct StringTableCell
		{
			char* string; // 0
			int hash; // 4
		};

		struct StringTable
		{
			const char* name; // 0
			int columns; // 4
			int rows; // 8
			StringTableCell* strings; // 12
		};

		// Fx
		struct FxEffectDef;

		/*	struct FxElemMarkVisuals
			{
				Material *materials[2];
			};*/
		struct FxImpactEntry
		{
			FxEffectDef* nonflesh[31];
			FxEffectDef* flesh[4];
		};

		union FxEffectDefRef
		{
			FxEffectDef* handle;
			const char* name;
		};

		union FxElemVisuals
		{
			const void* anonymous;
			Material* material;
			XModel* xmodel;
			FxEffectDefRef* effectDef;
			const char* soundName;
		};

		typedef Material* FxElemMarkVisuals[2];

		union FxElemDefVisuals
		{
			FxElemVisuals instance;
			FxElemVisuals* array;
			FxElemMarkVisuals* markArray;
		};

		struct FxTrailVertex
		{
			float pos[2];
			float normal[2];
			float texCoord;
		};

		struct FxTrailDef
		{
			int scrollTimeMsec;
			int repeatDist;
			float invSplitDist;
			float invSplitArcDist;
			float invSplitTime;
			int vertCount;
			FxTrailVertex* verts;
			int indCount;
			unsigned __int16* inds;
		};

		struct FxSparkFountainDef
		{
			float gravity;
			float bounceFrac;
			float bounceRand;
			float sparkSpacing;
			float sparkLength;
			int sparkCount;
			float loopTime;
			float velMin;
			float velMax;
			float velConeFrac;
			float restSpeed;
			float boostTime;
			float boostFactor;
		};

		union FxElemExtendedDefPtr
		{
			FxTrailDef* trailDef;
			FxSparkFountainDef* sparkFountain;
			char* unknownDef;
		};

		struct FxSpawnDefLooping
		{
			int intervalMsec;
			int count;
		};

		struct FxIntRange
		{
			int base;
			int amplitude;
		};

		struct FxFloatRange
		{
			float base;
			float amplitude;
		};

		struct FxSpawnDefOneShot
		{
			FxIntRange count;
		};

		union FxSpawnDef
		{
			FxSpawnDefLooping looping;
			FxSpawnDefOneShot oneShot;
		};

		struct FxElemAtlas
		{
			char behavior;
			char index;
			char fps;
			char loopCount;
			char colIndexBits;
			char rowIndexBits;
			__int16 entryCount;
		};

		struct FxElemVec3Range
		{
			float base[3];
			float amplitude[3];
		};

		struct FxElemVelStateInFrame
		{
			FxElemVec3Range velocity;
			FxElemVec3Range totalDelta;
		};

		const struct FxElemVelStateSample
		{
			FxElemVelStateInFrame local;
			FxElemVelStateInFrame world;
		};

		struct FxElemVisualState
		{
			char color[4];
			float rotationDelta;
			float rotationTotal;
			float size[2];
			float scale;
		};

		const struct FxElemVisStateSample
		{
			FxElemVisualState base;
			FxElemVisualState amplitude;
		};

		struct FxElemDef
		{
			int flags;
			FxSpawnDef spawn;
			FxFloatRange spawnRange;
			FxFloatRange fadeInRange;
			FxFloatRange fadeOutRange;
			float spawnFrustumCullRadius;
			FxIntRange spawnDelayMsec;
			FxIntRange lifeSpanMsec;
			FxFloatRange spawnOrigin[3];
			FxFloatRange spawnOffsetRadius;
			FxFloatRange spawnOffsetHeight;
			FxFloatRange spawnAngles[3];
			FxFloatRange angularVelocity[3];
			FxFloatRange initialRotation;
			FxFloatRange gravity;
			FxFloatRange reflectionFactor;
			FxElemAtlas atlas;
			char elemType;
			char visualCount;
			char velIntervalCount;
			char visStateIntervalCount;
			FxElemVelStateSample* velSamples;
			FxElemVisStateSample* visSamples;
			FxElemDefVisuals visuals;
			Bounds collBounds;
			FxEffectDefRef* effectOnImpact;
			FxEffectDefRef* effectOnDeath;
			FxEffectDefRef* effectEmitted;
			FxFloatRange emitDist;
			FxFloatRange emitDistVariance;
			FxElemExtendedDefPtr extended;
			char sortOrder;
			char lightingFrac;
			char useItemClip;
			char fadeInfo;
		};

		struct FxEffectDef
		{
			const char* name;
			int flags;
			int totalSize;
			int msecLoopingLife;
			int elemDefCountLooping;
			int elemDefCountOneShot;
			int elemDefCountEmission;
			FxElemDef* elemDefs;
		};

		// GfxWorld
#pragma pack(push, 4)
		struct GfxLightImage
		{
			GfxImage* image;
			char samplerState;
		};
#pragma pack(pop)

		struct GfxLightDef
		{
			const char* name;
			GfxLightImage attenuation;
			int lmapLookupStart;
		};

		struct GfxSky
		{
			int skySurfCount;
			std::uint32_t* skyStartSurfs;
			GfxImage* skyImage;
			char skySamplerState;
			char pad[3];
		};

		struct GfxWorldDpvsPlanes
		{
			int cellCount;
			cplane_s* planes;
			unsigned __int16* nodes;
			unsigned char* sceneEntCellBits; //Size = cellCount << 11
		};

		struct GfxAabbTree
		{
			union
			{
				Bounds bounds;

				struct
				{
					float mins[3]; // 12
					float maxs[3]; // 12
				};
			};

			int unkn;
			unsigned __int16 childCount; // 2
			unsigned __int16 surfaceCount; // 2
			unsigned __int16 startSurfIndex; // 2
			unsigned __int16 smodelIndexCount; // 2
			unsigned __int16* smodelIndexes; // 4
			int childrenOffset; // 4
		}; // Size: 0x2C

		struct GfxCellTree
		{
			// Best struct ever
			GfxAabbTree* aabbtree;
		};

#pragma pack(push, 4)
		struct GfxPortal;
		struct GfxPortalWritable
		{
			bool isQueued;
			bool isAncestor;
			char recursionDepth;
			char hullPointCount;
			float(*hullPoints)[2];
			GfxPortal* queuedParent;
		};

		struct DpvsPlane
		{
			float coeffs[4];
		};

		struct GfxPortal
		{
			GfxPortalWritable writable;
			DpvsPlane plane;
			float(*vertices)[3];
			unsigned __int16 cellIndex;
			char vertexCount;
			float hullAxis[2][3];
		};
#pragma pack(pop)

#pragma pack(push, 4)
		struct GfxCell
		{
			union
			{
				Bounds bounds;

				struct
				{
					float mins[3];
					float maxs[3];
				};
			};

			int portalCount;
			GfxPortal* portals;
			char reflectionProbeCount;
			char* reflectionProbes;
		};
#pragma pack(pop)

		struct GfxCell_IW5
		{
			float mins[3];
			float maxs[3];
			int portalCount;
			GfxPortal* portals;
			char reflectionProbeCount;
			char* reflectionProbes;
			char reflectionProbeReferenceCount;
			char* reflectionProbeReferences;
		};

		struct GfxReflectionProbe
		{
			float offset[3];
		};

		typedef char GfxTexture[0x04];

		struct GfxLightmapArray
		{
			GfxImage* primary;
			GfxImage* secondary;
		};

		struct GfxWorldVertex
		{
			float xyz[3];
			float binormalSign;
			GfxColor color;
			float texCoord[2];
			float lmapCoord[2];
			PackedUnitVec normal;
			PackedUnitVec tangent;
		};

		struct GfxWorldVertexData
		{
			GfxWorldVertex* vertices;
			void* worldVb; // D3DVertexBuffer
		};

		struct GfxWorldVertexLayerData
		{
			char* data;
			void* layerVb; // D3DVertexBuffer
		};

		struct GfxWorldDraw
		{
			union
			{
				char _portpad0[16];

				struct
				{
					unsigned int reflectionProbeCount; // 4
					GfxImage* * reflectionImages; // 4
					GfxReflectionProbe* reflectionProbes; // 4
					GfxTexture* reflectionProbeTextures; //Count = reflectionProbeCount // 4
				};
			};

			union
			{
				char _portpad1[56];

				struct
				{
					int lightmapCount; // 4
					GfxLightmapArray* lightmaps; // 4
					GfxTexture* lightmapPrimaryTextures; //Count = lightmapCount // 4
					GfxTexture* lightmapSecondaryTextures; //Count = lightmapCount // 4
					GfxImage* skyImage; // 4
					GfxImage* outdoorImage; // 4
					unsigned int vertexCount; // 4
					GfxWorldVertexData vd;
					unsigned int vertexLayerDataSize;
					GfxWorldVertexLayerData vld;
					int indexCount;
					unsigned __int16* indices;
				};
			};
		};

		struct GfxWorldDraw_IW5
		{
			union
			{
				char _portpad0[16];

				struct
				{
					unsigned int reflectionProbeCount; // 4
					GfxImage* * reflectionImages; // 4
					GfxReflectionProbe* reflectionProbes; // 4
					GfxTexture* reflectionProbeTextures; //Count = reflectionProbeCount // 4
				};
			};

			char cancer[12];

			union
			{
				char _portpad1[56];

				struct
				{
					int lightmapCount; // 4
					GfxLightmapArray* lightmaps; // 4
					GfxTexture* lightmapPrimaryTextures; //Count = lightmapCount // 4
					GfxTexture* lightmapSecondaryTextures; //Count = lightmapCount // 4
					GfxImage* skyImage; // 4
					GfxImage* outdoorImage; // 4
					unsigned int vertexCount; // 4
					GfxWorldVertexData vd;
					unsigned int vertexLayerDataSize;
					GfxWorldVertexLayerData vld;
					int indexCount;
					unsigned __int16* indices;
				};
			};
		};

		struct GfxLightGridEntry
		{
			unsigned __int16 colorsIndex;
			char primaryLightIndex;
			char needsTrace;
		};

		struct GfxLightGridColors
		{
			char rgb[56][3];
		};

		struct GfxLightGrid
		{
			bool hasLightRegions; // 4
			unsigned int sunPrimaryLightIndex; // 4
			unsigned __int16 mins[3]; // 6
			unsigned __int16 maxs[3]; // 6
			unsigned int rowAxis; // 4
			unsigned int colAxis; // 4
			unsigned __int16* rowDataStart;
			// Size: (varGfxLightGrid->maxs[varGfxLightGrid->rowAxis] - varGfxLightGrid->mins[varGfxLightGrid->rowAxis] + 1) * 2
			unsigned int rawRowDataSize;
			char* rawRowData;
			unsigned int entryCount;
			GfxLightGridEntry* entries;
			unsigned int colorCount;
			GfxLightGridColors* colors;
		};

		struct GfxBrushModelWritable
		{
			Bounds bounds;
		};

		struct GfxBrushModel
		{
			GfxBrushModelWritable writable;
			Bounds bounds;
			float radius;
			unsigned short surfaceCount;
			unsigned short startSurfIndex;
			unsigned short surfaceCountNoDecal;
		};
		
		struct MaterialMemory
		{
			Material* material;
			int memory;
		};

		struct sunflare_t
		{
			bool hasValidData;
			Material* spriteMaterial;
			Material* flareMaterial;
			float spriteSize;
			float flareMinSize;
			float flareMinDot;
			float flareMaxSize;
			float flareMaxDot;
			float flareMaxAlpha;
			int flareFadeInTime;
			int flareFadeOutTime;
			float blindMinDot;
			float blindMaxDot;
			float blindMaxDarken;
			int blindFadeInTime;
			int blindFadeOutTime;
			float glareMinDot;
			float glareMaxDot;
			float glareMaxLighten;
			int glareFadeInTime;
			int glareFadeOutTime;
			float sunFxPosition[3];
		};

		struct XModelDrawInfo
		{
			unsigned __int16 lod;
			unsigned __int16 surfId;
		};

		struct GfxSceneDynModel
		{
			XModelDrawInfo info;
			unsigned __int16 dynEntId;
		};

		struct BModelDrawInfo
		{
			unsigned __int16 surfId;
		};

		struct GfxSceneDynBrush
		{
			BModelDrawInfo info;
			unsigned __int16 dynEntId;
		};

		struct GfxShadowGeometry
		{
			unsigned __int16 surfaceCount;
			unsigned __int16 smodelCount;
			unsigned __int16* sortedSurfIndex;
			unsigned __int16* smodelIndex;
		};

		struct GfxLightRegionAxis
		{
			float dir[3];
			float midPoint;
			float halfSize;
		};

		struct GfxLightRegionHull
		{
			float kdopMidPoint[9];
			float kdopHalfSize[9];
			unsigned int axisCount;
			GfxLightRegionAxis* axis;
		};

		struct GfxLightRegion
		{
			unsigned int hullCount;
			GfxLightRegionHull* hulls;
		};

		struct GfxStaticModelInst
		{
			union
			{
				Bounds bounds;

				struct
				{
					float mins[3];
					float maxs[3];
				};
			};
			float lightingOrigin[3];
		};

		struct srfTriangles_t
		{
			unsigned int vertexLayerData;
			unsigned int firstVertex;
			unsigned __int16 vertexCount;
			unsigned __int16 triCount;
			unsigned int baseIndex;
		};

		struct GfxSurface
		{
			srfTriangles_t tris;
			Material* material;
			char lightmapIndex;
			char reflectionProbeIndex;
			char primaryLightIndex;
			char flags;
		};

		struct GfxCullGroup
		{
			union
			{
				Bounds bounds;

				struct
				{
					float mins[3];
					float maxs[3];
				};
			};
		};

		struct GfxDrawSurfFields
		{
			unsigned __int64 objectId : 16;
			unsigned __int64 reflectionProbeIndex : 8;
			unsigned __int64 hasGfxEntIndex : 1;
			unsigned __int64 customIndex : 5;
			unsigned __int64 materialSortedIndex : 12;
			unsigned __int64 prepass : 2;
			unsigned __int64 useHeroLighting : 1;
			unsigned __int64 sceneLightIndex : 8;
			unsigned __int64 surfType : 4;
			unsigned __int64 primarySortKey : 6;
			unsigned __int64 unused : 1;
		};

		union GfxDrawSurf
		{
			GfxDrawSurfFields fields;
			unsigned __int64 packed;
		};
		static_assert(sizeof(GfxDrawSurf) == 8);

#pragma pack(push, 4)
		struct GfxPackedPlacement
		{
			float origin[3];
			float axis[3][3];
			float scale;
		};

		struct GfxStaticModelDrawInst
		{
			GfxPackedPlacement placement;
			XModel *model;
			unsigned __int16 cullDist;
			unsigned __int16 lightingHandle;
			char reflectionProbeIndex;
			char primaryLightIndex;
			char flags;
			char firstMtlSkinIndex;
			GfxColor groundLighting;
			unsigned __int16 cacheId[4];
		};

		struct GfxWorldDpvsDynamic
		{
			unsigned int dynEntClientWordCount[2];
			unsigned int dynEntClientCount[2];
			unsigned int* dynEntCellBits[2];
			char* dynEntVisData[2][3];
		};

#pragma pack(pop)

		struct GfxWorldDpvsStatic
		{
			unsigned int smodelCount;
			unsigned int staticSurfaceCount;
			unsigned int staticSurfaceCountNoDecal;
			unsigned int litOpaqueSurfsBegin;
			unsigned int litOpaqueSurfsEnd;
			unsigned int litTransSurfsBegin;
			unsigned int litTransSurfsEnd;
			unsigned int shadowCasterSurfsBegin;
			unsigned int shadowCasterSurfsEnd;
			unsigned int emissiveSurfsBegin;
			unsigned int emissiveSurfsEnd;
			unsigned int smodelVisDataCount;
			unsigned int surfaceVisDataCount;
			char* smodelVisData[3];
			char* surfaceVisData[3];
			unsigned __int16* sortedSurfIndex;
			GfxStaticModelInst* smodelInsts;
			GfxSurface* surfaces;
			GfxCullGroup* surfacesBounds;
			GfxStaticModelDrawInst* smodelDrawInsts;
			GfxDrawSurf* surfaceMaterials;
			unsigned int* surfaceCastsSunShadow;
			volatile int usageCount;
		};

		struct GfxHeroLight
		{
			char type;
			char pad[3];
			float color[3];
			float dir[3];
			float origin[3];
			float radius;
			float cosHalfFovOuter;
			float cosHalfFovInner;
			int exponent;
		};

		struct GfxCellTreeCount
		{
			int aabbTreeCount;
		};

#pragma pack(push, 4)
		struct GfxWorld
		{
			const char* name; // 4
			const char* baseName; // 4
			int planeCount; // 4
			int nodeCount; // 4 // = 16
			int surfaceCount; // 4
			unsigned int skyCount; // 4
			GfxSky* skies; // 4
			int sunPrimaryLightIndex; // 4 // = 32
			int primaryLightCount; // 4
			unsigned int sortKeyLitDecal;
			unsigned int sortKeyEffectDecal;
			unsigned int sortKeyEffectAuto;
			unsigned int sortKeyDistortion;
			GfxWorldDpvsPlanes dpvsPlanes; // 16
			GfxCellTreeCount* aabbTreeCounts; // Size: 4 * dpvsPlanes.cellCount // 4
			GfxCellTree* aabbTree; // 4
			GfxCell* cells; // 4  // = 80
			GfxWorldDraw draw; // 72
			GfxLightGrid lightGrid; // 56 // = 208
			int modelCount; // 4
			GfxBrushModel* models; // 4 // = 216
			union
			{
				Bounds bounds;

				struct
				{
					float mins[3]; // 12
					float maxs[3]; // 12
				};
			};

			unsigned int checksum; // 4
			int materialMemoryCount; // 4 // = 248
			MaterialMemory* materialMemory; // 4
			sunflare_t sun; // 96 // = 348
			float outdoorLookupMatrix[4][4]; // 64
			GfxImage* outdoorImage; // 4 // = 416
			unsigned int* cellCasterBits[2]; // 8
			GfxSceneDynModel* sceneDynModel; // 4
			GfxSceneDynBrush* sceneDynBrush; // 4 // = 432
			unsigned char* primaryLightEntityShadowVis;
			unsigned int* primaryLightDynEntShadowVis[2];
			char* primaryLightForModelDynEnt;
			GfxShadowGeometry* shadowGeom;
			GfxLightRegion* lightRegion;
			GfxWorldDpvsStatic dpvs;
			GfxWorldDpvsDynamic dpvsDyn;
			unsigned int mapVtxChecksum;
			unsigned int heroLightCount;
			GfxHeroLight* heroLights;
			char fogTypesAllowed;
			char pad2[3];
		};
#pragma pack(pop)

		namespace alpha
		{			
			struct GfxWorldVertexData
			{
				GfxWorldVertex* vertices;
				D3DVertexBuffer worldVb;
			};

			struct GfxWorldVertexLayerData
			{
				char* data;
				D3DVertexBuffer layerVb;
			};
			
			struct GfxWorldDraw
			{
				unsigned int reflectionProbeCount;
				GfxImage** reflectionProbes;
				GfxReflectionProbe* reflectionProbeOrigins;
				GfxTexture* reflectionProbeTextures;
				int lightmapCount;
				GfxLightmapArray* lightmaps;
				GfxTexture* lightmapPrimaryTextures;
				GfxTexture* lightmapSecondaryTextures;
				GfxImage* lightmapOverridePrimary;
				GfxImage* lightmapOverrideSecondary;
				unsigned int vertexCount;
				GfxWorldVertexData vd;
				unsigned int vertexLayerDataSize;
				GfxWorldVertexLayerData vld;
				unsigned int indexCount;
				unsigned __int16* indices;
				D3DIndexBuffer indexBuffer;
			};

			struct GfxWorldDpvsStatic
			{
				unsigned int smodelCount;
				unsigned int staticSurfaceCount;
				unsigned int litOpaqueSurfsBegin;
				unsigned int litOpaqueSurfsEnd;
				unsigned int litTransSurfsBegin;
				unsigned int litTransSurfsEnd;
				unsigned int shadowCasterSurfsBegin;
				unsigned int shadowCasterSurfsEnd;
				unsigned int emissiveSurfsBegin;
				unsigned int emissiveSurfsEnd;
				unsigned int smodelVisDataCount;
				unsigned int surfaceVisDataCount;
				char* smodelVisData[3];
				char* surfaceVisData[3];
				unsigned __int16* sortedSurfIndex;
				GfxStaticModelInst* smodelInsts;
				GfxSurface* surfaces;
				GfxCullGroup* surfacesBounds;
				GfxStaticModelDrawInst* smodelDrawInsts;
				GfxDrawSurf* surfaceMaterials;
				unsigned int* surfaceCastsSunShadow;
				volatile int usageCount;
			};
			
			struct __declspec(align(4)) GfxWorld
			{
				const char* name;
				const char* baseName;
				int planeCount;
				int nodeCount;
				unsigned int surfaceCount;
				int skyCount;
				GfxSky* skies;
				unsigned int lastSunPrimaryLightIndex;
				unsigned int primaryLightCount;
				unsigned int sortKeyLitDecal;
				unsigned int sortKeyEffectDecal;
				unsigned int sortKeyEffectAuto;
				unsigned int sortKeyDistortion;
				GfxWorldDpvsPlanes dpvsPlanes;
				GfxCellTreeCount* aabbTreeCounts;
				GfxCellTree* aabbTrees;
				GfxCell* cells;
				GfxWorldDraw draw;
				GfxLightGrid lightGrid;
				int modelCount;
				GfxBrushModel* models;
				Bounds bounds;
				unsigned int checksum;
				int materialMemoryCount;
				MaterialMemory* materialMemory;
				sunflare_t sun;
				float outdoorLookupMatrix[4][4];
				GfxImage* outdoorImage;
				unsigned int* cellCasterBits;
				unsigned int* cellHasSunLitSurfsBits;
				GfxSceneDynModel* sceneDynModel;
				GfxSceneDynBrush* sceneDynBrush;
				unsigned int* primaryLightEntityShadowVis;
				unsigned int* primaryLightDynEntShadowVis[2];
				char* nonSunPrimaryLightForModelDynEnt;
				GfxShadowGeometry* shadowGeom;
				GfxLightRegion* lightRegion;
				GfxWorldDpvsStatic dpvs;
				GfxWorldDpvsDynamic dpvsDyn;
				unsigned int mapVtxChecksum;
				unsigned int heroOnlyLightCount;
				GfxHeroLight* heroOnlyLights;
				char fogTypesAllowed;
			};
		}
		
#pragma pack(push, 4)
		struct cStaticModel_s
		{
			XModel* xmodel;
			float origin[3];
			float invScaledAxis[3][3];
			Bounds absBounds;
		};

		struct dmaterial_t
		{
			char* material;
			int surfaceFlags;
			int contentFlags;
		};

		struct cNode_t
		{
			cplane_s* plane;
			__int16 children[2];
		};

		struct cLeaf_t
		{
			unsigned __int16 firstCollAabbIndex;
			unsigned __int16 collAabbCount;
			int brushContents;
			int terrainContents;
			Bounds bounds;
			int leafBrushNode;
		};

		struct cLeafBrushNodeLeaf_t
		{
			unsigned __int16* brushes;
		};

		struct cLeafBrushNodeChildren_t
		{
			unsigned __int16 childOffset[6];
		};

		union cLeafBrushNodeData_t
		{
			cLeafBrushNodeLeaf_t leaf;
			cLeafBrushNodeChildren_t children;
		};

		struct cLeafBrushNode_s
		{
			char axis;
			short leafBrushCount;
			int contents;
			cLeafBrushNodeData_t data;
		};

		struct CollisionBorder
		{
			float distEq[3];
			float zBase;
			float zSlope;
			float start;
			float length;
		};

		struct CollisionPartition
		{
			char triCount;
			char borderCount;
			int firstTri;
			CollisionBorder* borders;
		};

		union CollisionAabbTreeIndex
		{
			int firstChildIndex;
			int partitionIndex;
		};

		struct CollisionAabbTree
		{
			float origin[3];
			unsigned __int16 materialIndex;
			unsigned __int16 childCount;
			float halfSize[3];
			CollisionAabbTreeIndex u;
		};

		enum DynEntityType
		{
			DYNENT_TYPE_INVALID = 0x0,
			DYNENT_TYPE_CLUTTER = 0x1,
			DYNENT_TYPE_DESTRUCT = 0x2,
			DYNENT_TYPE_COUNT = 0x3,
		};

		struct GfxPlacement
		{
			float quat[4];
			float origin[3];
		};

		struct DynEntityDef
		{
			DynEntityType type;
			GfxPlacement pose;
			XModel* xModel;
			unsigned __int16 brushModel;
			unsigned __int16 physicsBrushModel;
			void* destroyFx; // FxEffectDef
			PhysPreset* physPreset;
			int health;
			PhysMass mass;
			//char *unknown;
		};

		struct DynEntityPose
		{
			GfxPlacement pose;
			float radius;
		};

		struct DynEntityClient
		{
			int physObjId;
			unsigned __int16 flags;
			unsigned __int16 lightingHandle;
			int health;
		};

		struct DynEntityColl
		{
			unsigned __int16 sector;
			unsigned __int16 nextEntInSector;
			float linkMins[2];
			float linkMaxs[2];
		};

		struct unknownInternalClipMapStruct1
		{
			int planeCount;
			cplane_s* planes;
			unsigned int numMaterials;
			dmaterial_t* materials;
			unsigned int numBrushSides;
			cbrushside_t* brushsides;
			unsigned int numBrushEdges;
			cbrushedge_t* brushEdges;
			unsigned int leafbrushNodesCount;
			cLeafBrushNode_s* leafbrushNodes;
			unsigned int numLeafBrushes;
			unsigned __int16* leafbrushes;
			unsigned __int16 numBrushes;
			cbrush_t* brushes;
			char* unknown1; //Size = ((numBrushes << 1) + numBrushes) << 3
			unsigned int* leafsurfaces; //Count = numBrushes
		};

		struct unknownInternalClipMapStruct2
		{
			char* unknownString;
			char unknown[0x10];
		};

		struct unknownInternalClipMapStruct3
		{
			char _pad[28];
			unknownInternalClipMapStruct1* unkArrayPtr;
			char _pad2[40];
		};

		struct cmodel_t
		{
			union
			{
				char _portpad0[28];

				struct
				{
					Bounds bounds;
					float radius;
				};
			};

			union
			{
				char _portpad1[40];

				struct
				{
					cLeaf_t leaf;
				};
			};
		};

		struct SModelAabbNode
		{
			Bounds bounds;
			short firstChild;
			short childCount;
		};

		struct DynEntityDef_IW5
		{
			DynEntityType type;
			GfxPlacement pose;
			XModel* xModel;
			unsigned __int16 brushModel;
			unsigned __int16 physicsBrushModel;
			void* destroyFx; // FxEffectDef
			PhysPreset* physPreset;
			int health;
			void* hinge;
			PhysMass mass;
		};

		struct cmodel_t_IW5
		{
			Bounds bounds;
			float radius;
			void* info;
			cLeaf_t leaf;
		};

		struct clipMap_t
		{
			const char* name; // 4
			int isInUse; // 4
			int numCPlanes; // +8
			cplane_s* cPlanes; // sizeof 20, +12
			int numStaticModels; // +16
			cStaticModel_s* staticModelList; // sizeof 76, +20
			int numMaterials; // +24
			dmaterial_t* materials; // sizeof 12 with a string (possibly name?), +28
			int numCBrushSides; // +32
			cbrushside_t* cBrushSides; // sizeof 8, +36
			int numCBrushEdges; // +40			NOT USED IN T5
			cbrushedge_t* cBrushEdges; // +44			NOT USED IN T5
			int numCNodes; // +48
			cNode_t* cNodes; // sizeof 8, +52
			int numCLeaf; // +56
			cLeaf_t* cLeaf; // +60
			int numCLeafBrushNodes; // +64
			cLeafBrushNode_s* cLeafBrushNodes; // +68
			int numLeafBrushes; // +72
			short* leafBrushes; // +76
			int numLeafSurfaces; // +80
			int* leafSurfaces; // +84
			int numVerts; // +88
			VecInternal<3>* verts; // +92
			int numTriIndices; // +96
			short* triIndices; // +100
			char* triEdgeIsWalkable; // +104
			int numCollisionBorders; // +108
			CollisionBorder* collisionBorders; // sizeof 28, +112
			int numCollisionPartitions; // +116
			CollisionPartition* collisionPartitions; // sizeof 12, +120
			int numCollisionAABBTrees; // +124
			CollisionAabbTree* collisionAABBTrees; // sizeof 32, +128
			int numCModels; // +132
			cmodel_t* cModels; // sizeof 68, +136
			short numBrushes; // +140
			short pad2; // +142
			cbrush_t* brushes; // sizeof 36, +144
			Bounds* brushBounds; // same count as cBrushes, +148
			int* brushContents; // same count as cBrushes, +152
			MapEnts* mapEnts; // +156
			short smodelNodeCount; // +160
			short pad3;
			SModelAabbNode* smodelNodes; // +164
			unsigned __int16 dynEntCount[2];
			DynEntityDef* dynEntDefList[2];
			DynEntityPose* dynEntPoseList[2];
			DynEntityClient* dynEntClientList[2];
			DynEntityColl* dynEntCollList[2];
			unsigned int checksum;
			char unknown5[48];
		}; // +256
#pragma pack(pop)

				// StructuredDataDef
		enum StructuredDataTypeCategory
		{
			DATA_INT = 0x0,
			DATA_BYTE = 0x1,
			DATA_BOOL = 0x2,
			DATA_STRING = 0x3,
			DATA_ENUM = 0x4,
			DATA_STRUCT = 0x5,
			DATA_INDEXED_ARRAY = 0x6,
			DATA_ENUM_ARRAY = 0x7,
			DATA_FLOAT = 0x8,
			DATA_SHORT = 0x9,
			DATA_COUNT = 0xA,
		};
#pragma pack(push,4)
		struct StructuredDataEnumEntry
		{
			const char* name;
			unsigned __int16 index;
		};
#pragma pack(pop)
		struct StructuredDataEnum
		{
			int entryCount;
			int reservedEntryCount;
			StructuredDataEnumEntry* entries;
		};

		union StructuredDataTypeUnion
		{
			unsigned int stringDataLength;
			int enumIndex;
			int structIndex;
			int indexedArrayIndex;
			int enumedArrayIndex;
		};

		struct StructuredDataType
		{
			StructuredDataTypeCategory type;
			StructuredDataTypeUnion u;
		};
#pragma pack(push,4)
		struct StructuredDataStructProperty
		{
			const char* name;
			StructuredDataType item;
			int offset;
		};
#pragma pack(pop)
		struct StructuredDataStruct
		{
			int propertyCount;
			StructuredDataStructProperty* properties;
			int size;
			unsigned int bitOffset;
		};

		struct StructuredDataIndexedArray
		{
			int arraySize;
			StructuredDataType elementType;
			unsigned int elementSize;
		};

		struct StructuredDataEnumedArray
		{
			int enumIndex;
			StructuredDataType elementType;
			unsigned int elementSize;
		};

		struct StructuredDataDef
		{
			int version;
			unsigned int formatChecksum;
			int enumCount;
			StructuredDataEnum* enums;
			int structCount;
			StructuredDataStruct* structs;
			int indexedArrayCount;
			StructuredDataIndexedArray* indexedArrays;
			int enumedArrayCount;
			StructuredDataEnumedArray* enumedArrays;
			StructuredDataType rootType;
			unsigned int size;
		};

		struct StructuredDataDefSet
		{
			const char* name;
			unsigned int defCount;
			StructuredDataDef* defs;
		};

		struct AddonMapEnts
		{
			const char* name;
			char* entityString;
			int numEntityChars;
			MapTriggers trigger;
		};

		union XAssetHeader
		{
			RawFile* rawfile;
			VertexDecl* vertexdecl;
			PixelShader* pixelshader;
			VertexShader* vertexshader;
			MaterialTechniqueSet* techset;
			GfxImage* gfximage;
			GfxImage* image;
			Material* material;
			PhysPreset* physpreset;
			PhysCollmap* physcollmap;
			PhysCollmap* phys_collmap;
			XAnimParts* xanimparts;
			XAnimParts* xanim;
			XModelSurfs* xsurface;
			XModelSurfs* xmodelsurfs;
			clipMap_t* clipmap;
			clipMap_t* col_map_mp;
			GfxWorld* gfxworld;
			GfxWorld* gfx_map;
			GameWorldMp* gameworldmp;
			GameWorldSp* gameworldsp;
			GameWorldMp* game_map_mp;
			GameWorldSp* game_map_sp;
			FxWorld* fxworld;
			FxWorld* fx_map;
			MapEnts* mapents;
			MapEnts* map_ents;
			XModel* xmodel;
			StringTable* stringtable;
			ComWorld* comworld;
			ComWorld* com_map;
			LocalizeEntry* localize;
			SndCurve* soundcurve;
			SndCurve* sndcurve;
			TracerDef* tracer;
			Font_s* font;
			WeaponCompleteDef* weapon;
			FxEffectDef* fx;
			snd_alias_list_t* sound;
			LoadedSound* loadedsound;
			LoadedSound* loaded_sound;
			StructuredDataDefSet*	structureddatadef;
			GfxLightDef* lightdef;
			AddonMapEnts* addon_map_ents;
			void* data;
		};

		enum MaterialTechniqueType
		{
			TECHNIQUE_DEPTH_PREPASS = 0x0,
			TECHNIQUE_BUILD_FLOAT_Z = 0x1,
			TECHNIQUE_BUILD_SHADOWMAP_DEPTH = 0x2,
			TECHNIQUE_BUILD_SHADOWMAP_COLOR = 0x3,
			TECHNIQUE_UNLIT = 0x4,
			TECHNIQUE_EMISSIVE = 0x5,
			TECHNIQUE_EMISSIVE_DFOG = 0x6,
			TECHNIQUE_EMISSIVE_SHADOW = 0x7,
			TECHNIQUE_EMISSIVE_SHADOW_DFOG = 0x8,
			TECHNIQUE_LIT_BEGIN = 0x9,
			TECHNIQUE_LIT = 0x9,
			TECHNIQUE_LIT_DFOG = 0xA,
			TECHNIQUE_LIT_SUN = 0xB,
			TECHNIQUE_LIT_SUN_DFOG = 0xC,
			TECHNIQUE_LIT_SUN_SHADOW = 0xD,
			TECHNIQUE_LIT_SUN_SHADOW_DFOG = 0xE,
			TECHNIQUE_LIT_SPOT = 0xF,
			TECHNIQUE_LIT_SPOT_DFOG = 0x10,
			TECHNIQUE_LIT_SPOT_SHADOW = 0x11,
			TECHNIQUE_LIT_SPOT_SHADOW_DFOG = 0x12,
			TECHNIQUE_LIT_OMNI = 0x13,
			TECHNIQUE_LIT_OMNI_DFOG = 0x14,
			TECHNIQUE_LIT_OMNI_SHADOW = 0x15,
			TECHNIQUE_LIT_OMNI_SHADOW_DFOG = 0x16,
			TECHNIQUE_LIT_INSTANCED = 0x17,
			TECHNIQUE_LIT_INSTANCED_DFOG = 0x18,
			TECHNIQUE_LIT_INSTANCED_SUN = 0x19,
			TECHNIQUE_LIT_INSTANCED_SUN_DFOG = 0x1A,
			TECHNIQUE_LIT_INSTANCED_SUN_SHADOW = 0x1B,
			TECHNIQUE_LIT_INSTANCED_SUN_SHADOW_DFOG = 0x1C,
			TECHNIQUE_LIT_INSTANCED_SPOT = 0x1D,
			TECHNIQUE_LIT_INSTANCED_SPOT_DFOG = 0x1E,
			TECHNIQUE_LIT_INSTANCED_SPOT_SHADOW = 0x1F,
			TECHNIQUE_LIT_INSTANCED_SPOT_SHADOW_DFOG = 0x20,
			TECHNIQUE_LIT_INSTANCED_OMNI = 0x21,
			TECHNIQUE_LIT_INSTANCED_OMNI_DFOG = 0x22,
			TECHNIQUE_LIT_INSTANCED_OMNI_SHADOW = 0x23,
			TECHNIQUE_LIT_INSTANCED_OMNI_SHADOW_DFOG = 0x24,
			TECHNIQUE_LIT_END = 0x25,
			TECHNIQUE_LIGHT_SPOT = 0x25,
			TECHNIQUE_LIGHT_OMNI = 0x26,
			TECHNIQUE_LIGHT_SPOT_SHADOW = 0x27,
			TECHNIQUE_FAKELIGHT_NORMAL = 0x28,
			TECHNIQUE_FAKELIGHT_VIEW = 0x29,
			TECHNIQUE_SUNLIGHT_PREVIEW = 0x2A,
			TECHNIQUE_CASE_TEXTURE = 0x2B,
			TECHNIQUE_WIREFRAME_SOLID = 0x2C,
			TECHNIQUE_WIREFRAME_SHADED = 0x2D,
			TECHNIQUE_DEBUG_BUMPMAP = 0x2E,
			TECHNIQUE_DEBUG_BUMPMAP_INSTANCED = 0x2F,
			TECHNIQUE_COUNT = 0x30,
			TECHNIQUE_TOTAL_COUNT = 0x31,
			TECHNIQUE_NONE = 0x32,
		};

		enum $949C82572B6A70952C455E749D3B3D56
		{
			CONST_SRC_CODE_MAYBE_DIRTY_PS_BEGIN = 0x0,
			CONST_SRC_CODE_LIGHT_POSITION = 0x0,
			CONST_SRC_CODE_LIGHT_DIFFUSE = 0x1,
			CONST_SRC_CODE_LIGHT_SPECULAR = 0x2,
			CONST_SRC_CODE_LIGHT_SPOTDIR = 0x3,
			CONST_SRC_CODE_LIGHT_SPOTFACTORS = 0x4,
			CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT = 0x5,
			CONST_SRC_CODE_PARTICLE_CLOUD_COLOR = 0x6,
			CONST_SRC_CODE_GAMETIME = 0x7,
			CONST_SRC_CODE_MAYBE_DIRTY_PS_END = 0x8,
			CONST_SRC_CODE_ALWAYS_DIRTY_PS_BEGIN = 0x8,
			CONST_SRC_CODE_PIXEL_COST_FRACS = 0x8,
			CONST_SRC_CODE_PIXEL_COST_DECODE = 0x9,
			CONST_SRC_CODE_FILTER_TAP_0 = 0xA,
			CONST_SRC_CODE_FILTER_TAP_1 = 0xB,
			CONST_SRC_CODE_FILTER_TAP_2 = 0xC,
			CONST_SRC_CODE_FILTER_TAP_3 = 0xD,
			CONST_SRC_CODE_FILTER_TAP_4 = 0xE,
			CONST_SRC_CODE_FILTER_TAP_5 = 0xF,
			CONST_SRC_CODE_FILTER_TAP_6 = 0x10,
			CONST_SRC_CODE_FILTER_TAP_7 = 0x11,
			CONST_SRC_CODE_COLOR_MATRIX_R = 0x12,
			CONST_SRC_CODE_COLOR_MATRIX_G = 0x13,
			CONST_SRC_CODE_COLOR_MATRIX_B = 0x14,
			CONST_SRC_CODE_SHADOWMAP_POLYGON_OFFSET = 0x15,
			CONST_SRC_CODE_RENDER_TARGET_SIZE = 0x16,
			CONST_SRC_CODE_ALWAYS_DIRTY_PS_END = 0x17,
			CONST_SRC_CODE_FIXED_PS_BEGIN = 0x17,
			CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR = 0x17,
			CONST_SRC_CODE_DOF_EQUATION_SCENE = 0x18,
			CONST_SRC_CODE_DOF_LERP_SCALE = 0x19,
			CONST_SRC_CODE_DOF_LERP_BIAS = 0x1A,
			CONST_SRC_CODE_DOF_ROW_DELTA = 0x1B,
			CONST_SRC_CODE_MOTION_MATRIX_X = 0x1C,
			CONST_SRC_CODE_MOTION_MATRIX_Y = 0x1D,
			CONST_SRC_CODE_MOTION_MATRIX_W = 0x1E,
			CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION = 0x1F,
			CONST_SRC_CODE_SHADOWMAP_SCALE = 0x20,
			CONST_SRC_CODE_ZNEAR = 0x21,
			CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE = 0x22,
			CONST_SRC_CODE_DEBUG_BUMPMAP = 0x23,
			CONST_SRC_CODE_MATERIAL_COLOR = 0x24,
			CONST_SRC_CODE_FOG = 0x25,
			CONST_SRC_CODE_FOG_COLOR_LINEAR = 0x26,
			CONST_SRC_CODE_FOG_COLOR_GAMMA = 0x27,
			CONST_SRC_CODE_FOG_SUN_CONSTS = 0x28,
			CONST_SRC_CODE_FOG_SUN_COLOR_LINEAR = 0x29,
			CONST_SRC_CODE_FOG_SUN_COLOR_GAMMA = 0x2A,
			CONST_SRC_CODE_FOG_SUN_DIR = 0x2B,
			CONST_SRC_CODE_GLOW_SETUP = 0x2C,
			CONST_SRC_CODE_GLOW_APPLY = 0x2D,
			CONST_SRC_CODE_COLOR_BIAS = 0x2E,
			CONST_SRC_CODE_COLOR_TINT_BASE = 0x2F,
			CONST_SRC_CODE_COLOR_TINT_DELTA = 0x30,
			CONST_SRC_CODE_COLOR_TINT_QUADRATIC_DELTA = 0x31,
			CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS = 0x32,
			CONST_SRC_CODE_ENVMAP_PARMS = 0x33,
			CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST = 0x34,
			CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST = 0x35,
			CONST_SRC_CODE_COMPOSITE_FX_DISTORTION = 0x36,
			CONST_SRC_CODE_POSTFX_FADE_EFFECT = 0x37,
			CONST_SRC_CODE_VIEWPORT_DIMENSIONS = 0x38,
			CONST_SRC_CODE_FRAMEBUFFER_READ = 0x39,
			CONST_SRC_CODE_FIXED_PS_END = 0x3A,
			CONST_SRC_CODE_NON_PS_BEGIN = 0x3A,
			CONST_SRC_CODE_BASE_LIGHTING_COORDS = 0x3A,
			CONST_SRC_CODE_LIGHT_PROBE_AMBIENT = 0x3B,
			CONST_SRC_CODE_NEARPLANE_ORG = 0x3C,
			CONST_SRC_CODE_NEARPLANE_DX = 0x3D,
			CONST_SRC_CODE_NEARPLANE_DY = 0x3E,
			CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE = 0x3F,
			CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET = 0x40,
			CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX0 = 0x41,
			CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX1 = 0x42,
			CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX2 = 0x43,
			CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR0 = 0x44,
			CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR1 = 0x45,
			CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR2 = 0x46,
			CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM0 = 0x47,
			CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM1 = 0x48,
			CONST_SRC_CODE_DEPTH_FROM_CLIP = 0x49,
			CONST_SRC_CODE_CODE_MESH_ARG_0 = 0x4A,
			CONST_SRC_CODE_CODE_MESH_ARG_1 = 0x4B,
			CONST_SRC_CODE_CODE_MESH_ARG_LAST = 0x4B,
			CONST_SRC_CODE_NON_PS_END = 0x4C,
			CONST_SRC_CODE_COUNT_FLOAT4 = 0x4C,
			CONST_SRC_FIRST_CODE_MATRIX = 0x4C,
			CONST_SRC_CODE_VIEW_MATRIX = 0x4C,
			CONST_SRC_CODE_INVERSE_VIEW_MATRIX = 0x4D,
			CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX = 0x4E,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX = 0x4F,
			CONST_SRC_CODE_PROJECTION_MATRIX = 0x50,
			CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX = 0x51,
			CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX = 0x52,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX = 0x53,
			CONST_SRC_CODE_VIEW_PROJECTION_MATRIX = 0x54,
			CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX = 0x55,
			CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0x56,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0x57,
			CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX = 0x58,
			CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX = 0x59,
			CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0x5A,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0x5B,
			CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x5C,
			CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x5D,
			CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x5E,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x5F,
			CONST_SRC_CODE_WORLD_MATRIX0 = 0x60,
			CONST_SRC_CODE_INVERSE_WORLD_MATRIX0 = 0x61,
			CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0 = 0x62,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX0 = 0x63,
			CONST_SRC_CODE_WORLD_VIEW_MATRIX0 = 0x64,
			CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX0 = 0x65,
			CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX0 = 0x66,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX0 = 0x67,
			CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX0 = 0x68,
			CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX0 = 0x69,
			CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0 = 0x6A,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0 = 0x6B,
			CONST_SRC_CODE_WORLD_MATRIX1 = 0x6C,
			CONST_SRC_CODE_INVERSE_WORLD_MATRIX1 = 0x6D,
			CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX1 = 0x6E,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX1 = 0x6F,
			CONST_SRC_CODE_WORLD_VIEW_MATRIX1 = 0x70,
			CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX1 = 0x71,
			CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX1 = 0x72,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX1 = 0x73,
			CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX1 = 0x74,
			CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX1 = 0x75,
			CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1 = 0x76,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1 = 0x77,
			CONST_SRC_CODE_WORLD_MATRIX2 = 0x78,
			CONST_SRC_CODE_INVERSE_WORLD_MATRIX2 = 0x79,
			CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX2 = 0x7A,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX2 = 0x7B,
			CONST_SRC_CODE_WORLD_VIEW_MATRIX2 = 0x7C,
			CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX2 = 0x7D,
			CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX2 = 0x7E,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX2 = 0x7F,
			CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX2 = 0x80,
			CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX2 = 0x81,
			CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2 = 0x82,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2 = 0x83,
			CONST_SRC_TOTAL_COUNT = 0x84,
			CONST_SRC_NONE = 0x85,
		};

		struct XAsset
		{
			XAssetType type;
			XAssetHeader ptr;
		};
	}

	namespace IW3
	{
		enum MaterialTechniqueType
		{
			TECHNIQUE_DEPTH_PREPASS = 0x0,
			TECHNIQUE_BUILD_FLOAT_Z = 0x1,
			TECHNIQUE_BUILD_SHADOWMAP_DEPTH = 0x2,
			TECHNIQUE_BUILD_SHADOWMAP_COLOR = 0x3,
			TECHNIQUE_UNLIT = 0x4,
			TECHNIQUE_EMISSIVE = 0x5,
			TECHNIQUE_EMISSIVE_SHADOW = 0x6,
			TECHNIQUE_LIT_BEGIN = 0x7,
			TECHNIQUE_LIT = 0x7,
			TECHNIQUE_LIT_SUN = 0x8,
			TECHNIQUE_LIT_SUN_SHADOW = 0x9,
			TECHNIQUE_LIT_SPOT = 0xA,
			TECHNIQUE_LIT_SPOT_SHADOW = 0xB,
			TECHNIQUE_LIT_OMNI = 0xC,
			TECHNIQUE_LIT_OMNI_SHADOW = 0xD,
			TECHNIQUE_LIT_INSTANCED = 0xE,
			TECHNIQUE_LIT_INSTANCED_SUN = 0xF,
			TECHNIQUE_LIT_INSTANCED_SUN_SHADOW = 0x10,
			TECHNIQUE_LIT_INSTANCED_SPOT = 0x11,
			TECHNIQUE_LIT_INSTANCED_SPOT_SHADOW = 0x12,
			TECHNIQUE_LIT_INSTANCED_OMNI = 0x13,
			TECHNIQUE_LIT_INSTANCED_OMNI_SHADOW = 0x14,
			TECHNIQUE_LIT_END = 0x15,
			TECHNIQUE_LIGHT_SPOT = 0x15,
			TECHNIQUE_LIGHT_OMNI = 0x16,
			TECHNIQUE_LIGHT_SPOT_SHADOW = 0x17,
			TECHNIQUE_FAKELIGHT_NORMAL = 0x18,
			TECHNIQUE_FAKELIGHT_VIEW = 0x19,
			TECHNIQUE_SUNLIGHT_PREVIEW = 0x1A,
			TECHNIQUE_CASE_TEXTURE = 0x1B,
			TECHNIQUE_WIREFRAME_SOLID = 0x1C,
			TECHNIQUE_WIREFRAME_SHADED = 0x1D,
			TECHNIQUE_SHADOWCOOKIE_CASTER = 0x1E,
			TECHNIQUE_SHADOWCOOKIE_RECEIVER = 0x1F,
			TECHNIQUE_DEBUG_BUMPMAP = 0x20,
			TECHNIQUE_DEBUG_BUMPMAP_INSTANCED = 0x21,
			TECHNIQUE_COUNT = 0x22,
			TECHNIQUE_TOTAL_COUNT = 0x23,
			TECHNIQUE_NONE = 0x24,
		};

		enum $D8CAED6A392807092A83572483F14017
		{
			CONST_SRC_CODE_MAYBE_DIRTY_PS_BEGIN = 0x0,
			CONST_SRC_CODE_LIGHT_POSITION = 0x0,
			CONST_SRC_CODE_LIGHT_DIFFUSE = 0x1,
			CONST_SRC_CODE_LIGHT_SPECULAR = 0x2,
			CONST_SRC_CODE_LIGHT_SPOTDIR = 0x3,
			CONST_SRC_CODE_LIGHT_SPOTFACTORS = 0x4,
			CONST_SRC_CODE_NEARPLANE_ORG = 0x5,
			CONST_SRC_CODE_NEARPLANE_DX = 0x6,
			CONST_SRC_CODE_NEARPLANE_DY = 0x7,
			CONST_SRC_CODE_SHADOW_PARMS = 0x8,
			CONST_SRC_CODE_SHADOWMAP_POLYGON_OFFSET = 0x9,
			CONST_SRC_CODE_RENDER_TARGET_SIZE = 0xA,
			CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT = 0xB,
			CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR = 0xC,
			CONST_SRC_CODE_DOF_EQUATION_SCENE = 0xD,
			CONST_SRC_CODE_DOF_LERP_SCALE = 0xE,
			CONST_SRC_CODE_DOF_LERP_BIAS = 0xF,
			CONST_SRC_CODE_DOF_ROW_DELTA = 0x10,
			CONST_SRC_CODE_PARTICLE_CLOUD_COLOR = 0x11,
			CONST_SRC_CODE_GAMETIME = 0x12,
			CONST_SRC_CODE_MAYBE_DIRTY_PS_END = 0x13,
			CONST_SRC_CODE_ALWAYS_DIRTY_PS_BEGIN = 0x13,
			CONST_SRC_CODE_PIXEL_COST_FRACS = 0x13,
			CONST_SRC_CODE_PIXEL_COST_DECODE = 0x14,
			CONST_SRC_CODE_FILTER_TAP_0 = 0x15,
			CONST_SRC_CODE_FILTER_TAP_1 = 0x16,
			CONST_SRC_CODE_FILTER_TAP_2 = 0x17,
			CONST_SRC_CODE_FILTER_TAP_3 = 0x18,
			CONST_SRC_CODE_FILTER_TAP_4 = 0x19,
			CONST_SRC_CODE_FILTER_TAP_5 = 0x1A,
			CONST_SRC_CODE_FILTER_TAP_6 = 0x1B,
			CONST_SRC_CODE_FILTER_TAP_7 = 0x1C,
			CONST_SRC_CODE_COLOR_MATRIX_R = 0x1D,
			CONST_SRC_CODE_COLOR_MATRIX_G = 0x1E,
			CONST_SRC_CODE_COLOR_MATRIX_B = 0x1F,
			CONST_SRC_CODE_ALWAYS_DIRTY_PS_END = 0x20,
			CONST_SRC_CODE_NEVER_DIRTY_PS_BEGIN = 0x20,
			CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION = 0x20,
			CONST_SRC_CODE_SHADOWMAP_SCALE = 0x21,
			CONST_SRC_CODE_ZNEAR = 0x22,
			CONST_SRC_CODE_SUN_POSITION = 0x23,
			CONST_SRC_CODE_SUN_DIFFUSE = 0x24,
			CONST_SRC_CODE_SUN_SPECULAR = 0x25,
			CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE = 0x26,
			CONST_SRC_CODE_DEBUG_BUMPMAP = 0x27,
			CONST_SRC_CODE_MATERIAL_COLOR = 0x28,
			CONST_SRC_CODE_FOG = 0x29,
			CONST_SRC_CODE_FOG_COLOR = 0x2A,
			CONST_SRC_CODE_GLOW_SETUP = 0x2B,
			CONST_SRC_CODE_GLOW_APPLY = 0x2C,
			CONST_SRC_CODE_COLOR_BIAS = 0x2D,
			CONST_SRC_CODE_COLOR_TINT_BASE = 0x2E,
			CONST_SRC_CODE_COLOR_TINT_DELTA = 0x2F,
			CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS = 0x30,
			CONST_SRC_CODE_ENVMAP_PARMS = 0x31,
			CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST = 0x32,
			CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE = 0x33,
			CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET = 0x34,
			CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX = 0x35,
			CONST_SRC_CODE_DEPTH_FROM_CLIP = 0x36,
			CONST_SRC_CODE_CODE_MESH_ARG_0 = 0x37,
			CONST_SRC_CODE_CODE_MESH_ARG_1 = 0x38,
			CONST_SRC_CODE_CODE_MESH_ARG_LAST = 0x38,
			CONST_SRC_CODE_BASE_LIGHTING_COORDS = 0x39,
			CONST_SRC_CODE_NEVER_DIRTY_PS_END = 0x3A,
			CONST_SRC_CODE_COUNT_FLOAT4 = 0x3A,
			CONST_SRC_FIRST_CODE_MATRIX = 0x3A,
			CONST_SRC_CODE_WORLD_MATRIX = 0x3A,
			CONST_SRC_CODE_INVERSE_WORLD_MATRIX = 0x3B,
			CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX = 0x3C,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX = 0x3D,
			CONST_SRC_CODE_VIEW_MATRIX = 0x3E,
			CONST_SRC_CODE_INVERSE_VIEW_MATRIX = 0x3F,
			CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX = 0x40,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX = 0x41,
			CONST_SRC_CODE_PROJECTION_MATRIX = 0x42,
			CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX = 0x43,
			CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX = 0x44,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX = 0x45,
			CONST_SRC_CODE_WORLD_VIEW_MATRIX = 0x46,
			CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX = 0x47,
			CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX = 0x48,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX = 0x49,
			CONST_SRC_CODE_VIEW_PROJECTION_MATRIX = 0x4A,
			CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX = 0x4B,
			CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0x4C,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0x4D,
			CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX = 0x4E,
			CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX = 0x4F,
			CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX = 0x50,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX = 0x51,
			CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX = 0x52,
			CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX = 0x53,
			CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0x54,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0x55,
			CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x56,
			CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x57,
			CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x58,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x59,
			CONST_SRC_TOTAL_COUNT = 0x5A,
			CONST_SRC_NONE = 0x5B,
		};
	}
}
