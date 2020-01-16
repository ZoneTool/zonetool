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
	namespace IW4
	{
		XModelSurfs* IXSurface::parse(const std::string& name, ZoneMemory* mem)
		{
			AssetReader read(mem);
			
			if (!read.open("XSurface\\" + name + ".xse"))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing xmodel surface \"%s\"...", name.c_str());

			auto asset = read.read_array<XModelSurfs>();
			asset->name = read.read_string();

			asset->surfs = mem->Alloc<XSurface>(asset->numsurfs);

			for (int i = 0; i < asset->numsurfs; i++)
			{
				asset->surfs[i].tileMode = read.read_int();
				asset->surfs[i].deformed = read.read_int();
				asset->surfs[i].baseTriIndex = read.read_int();
				asset->surfs[i].baseVertIndex = read.read_int();

				for (int j = 0; j < 6; j++)
				{
					asset->surfs[i].partBits[j] = read.read_int();
				}

				// vertex
				auto vertexInfo = read.read_array<XSurfaceVertexInfo>();
				memcpy(&asset->surfs[i].vertexInfo, vertexInfo, sizeof XSurfaceVertexInfo);
				asset->surfs[i].vertexInfo.vertsBlend = read.read_array<unsigned short>();

				// verticies
				asset->surfs[i].vertCount = read.read_int();
				asset->surfs[i].verticies = read.read_array<GfxPackedVertex>();

				// triangles
				asset->surfs[i].triCount = read.read_int();
				asset->surfs[i].triIndices = read.read_array<Face>();

				// rigidVertLists
				asset->surfs[i].vertListCount = read.read_int();
				asset->surfs[i].rigidVertLists = read.read_array<XRigidVertList>();
				for (int vert = 0; vert < asset->surfs[i].vertListCount; vert++)
				{
					if (asset->surfs[i].rigidVertLists[vert].collisionTree)
					{
						asset->surfs[i].rigidVertLists[vert].collisionTree = read.read_array<XSurfaceCollisionTree>();

						if (asset->surfs[i].rigidVertLists[vert].collisionTree->leafs)
						{
							asset->surfs[i].rigidVertLists[vert].collisionTree->leafs = read.read_array<
								XSurfaceCollisionLeaf>();
						}

						if (asset->surfs[i].rigidVertLists[vert].collisionTree->nodes)
						{
							asset->surfs[i].rigidVertLists[vert].collisionTree->nodes = read.read_array<
								XSurfaceCollisionNode>();
						}
					}
					else
					{
						asset->surfs[i].rigidVertLists[vert].collisionTree = nullptr;
					}
				}
			}

			read.close();

			return asset;
		}

		void IXSurface::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name_.data()).xsurface;
			}
		}

		void IXSurface::init(void* asset, ZoneMemory* mem)
		{
			this->asset_ = reinterpret_cast<XModelSurfs*>(asset);
			this->name_ = this->asset_->name;
		}

		void IXSurface::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IXSurface::load_depending(IZone* zone)
		{
		}

		std::string IXSurface::name()
		{
			return this->name_;
		}

		std::int32_t IXSurface::type()
		{
			return xmodelsurfs;
		}

		template <typename T> void write_xsurfaces(IZone* zone, ZoneBuffer* buf, T* data, T* dest, std::uint16_t count, bool is_console)
		{
			assert(sizeof XSurface, 64);
			assert(sizeof Face, 6);
			assert(sizeof XRigidVertList, 12);
			assert(sizeof XSurfaceCollisionTree, 40);
			assert(sizeof XSurfaceCollisionLeaf, 2);
			assert(sizeof XSurfaceCollisionAabb, 12);
			assert(sizeof XSurfaceCollisionNode, 16);
			assert(sizeof GfxPackedVertex, 32);
			assert(sizeof XSurfaceVertexInfo, 12);

			for (auto surf = 0u; surf < count; surf++)
			{
				if (data[surf].vertexInfo.vertsBlend)
				{
					unsigned __int16* destverts = nullptr;
					const auto vertcount = data[surf].vertexInfo.vertCount[0] +
						(data[surf].vertexInfo.vertCount[1] * 3) + 
						(data[surf].vertexInfo.vertCount[2] * 5) + 
						(data[surf].vertexInfo.vertCount[3] * 7);
					
					dest[surf].vertexInfo.vertsBlend = buf->write_s(1, data[surf].vertexInfo.vertsBlend, count, sizeof (unsigned __int16), &destverts);

					if (is_console && dest[surf].vertexInfo.vertsBlend == reinterpret_cast<unsigned __int16*>(-1))
					{
						for (auto a = 0u; a < vertcount; a++)
						{
							endian_convert(&destverts[a]);
						}
					}
				}

				if (!is_console)
				{
					buf->push_stream(6);
				}
				
				if (data[surf].verticies)
				{
					GfxPackedVertex* destverticies = nullptr;
					dest[surf].verticies = buf->write_s(15, data[surf].verticies, data[surf].vertCount, sizeof GfxPackedVertex, &destverticies);

					if (is_console && dest[surf].verticies == reinterpret_cast<GfxPackedVertex*>(-1))
					{
						for (auto a = 0; a < data[surf].vertCount; a++)
						{
							endian_convert(&destverticies[a].xyz[0]);
							endian_convert(&destverticies[a].xyz[1]);
							endian_convert(&destverticies[a].xyz[2]);
							endian_convert(&destverticies[a].binormalSign);
							// packed data probably consists of single bytes?
							//endian_convert(&destverticies[a].texCoord.packed);
							//endian_convert(&destverticies[a].normal.packed);
							//endian_convert(&destverticies[a].tangent.packed);
						}
					}
				}
				
				if (!is_console)
				{
					buf->pop_stream();
				}

				if (data[surf].rigidVertLists)
				{
					XRigidVertList* ct = nullptr;
					dest[surf].rigidVertLists = buf->write_s(3, data[surf].rigidVertLists, data[surf].vertListCount,
					                                         sizeof XRigidVertList, &ct);

					if (dest[surf].rigidVertLists == reinterpret_cast<XRigidVertList*>(-1))
					{
						for (int k = 0; k < data[surf].vertListCount; k++)
						{
							if (ct[k].collisionTree)
							{
								XSurfaceCollisionTree* entry = nullptr;
								ct[k].collisionTree = buf->write_s(3, ct[k].collisionTree, 1,
								                                   sizeof XSurfaceCollisionTree, &entry);

								if (ct[k].collisionTree == reinterpret_cast<XSurfaceCollisionTree*>(-1))
								{
									if (entry->nodes)
									{
										XSurfaceCollisionNode* destnode = nullptr;
										entry->nodes = buf->write_s(15, entry->nodes, entry->nodeCount, sizeof (XSurfaceCollisionNode), &destnode);

										if (entry->nodes == reinterpret_cast<XSurfaceCollisionNode*>(-1) && is_console)
										{
											for (auto a = 0u; a < entry->leafCount; a++)
											{
												for (auto b = 0; b < 3; b++)
												{
													endian_convert(&destnode[a].aabb.mins[b]);
													endian_convert(&destnode[a].aabb.maxs[b]);
												}

												endian_convert(&destnode[a].childBeginIndex);
												endian_convert(&destnode[a].childCount);
											}
										}
									}

									if (entry->leafs)
									{
										XSurfaceCollisionLeaf* destleaf = nullptr;
										entry->leafs = buf->write_s(1, entry->leafs, entry->leafCount, sizeof (XSurfaceCollisionLeaf), &destleaf);

										if (entry->leafs == reinterpret_cast<XSurfaceCollisionLeaf *>(-1) && is_console)
										{
											for (auto a = 0u; a < entry->leafCount; a++)
											{
												endian_convert(&destleaf[a].triangleBeginIndex);
											}
										}
									}

									if (is_console)
									{
										for (auto a = 0; a < 3; a++)
										{
											endian_convert(&entry->trans[a]);
											endian_convert(&entry->scale[a]);
										}
										endian_convert(&entry->nodeCount);
										endian_convert(&entry->nodes);
										endian_convert(&entry->leafCount);
										endian_convert(&entry->leafs);
									}
								}
							}

							if (is_console)
							{
								endian_convert(&ct[k].boneOffset);
								endian_convert(&ct[k].vertCount);
								endian_convert(&ct[k].triOffset);
								endian_convert(&ct[k].triCount);
								endian_convert(&ct[k].collisionTree);
							}
						}
					}
				}

				if (!is_console)
				{
					buf->push_stream(7);
				}
				
				if (data[surf].triIndices)
				{
					Face* dest_tris = nullptr;
					dest[surf].triIndices = buf->write_s(15, data[surf].triIndices, data[surf].triCount, sizeof(unsigned __int16), &dest_tris);

					if (is_console && dest[surf].triIndices == reinterpret_cast<Face*>(-1))
					{
						for (auto a = 0u; a < data[surf].triCount; a++)
						{
							endian_convert(&dest_tris[a].v1);
							endian_convert(&dest_tris[a].v2);
							endian_convert(&dest_tris[a].v3);
						}
					}
				}

				if (!is_console)
				{
					buf->pop_stream();
				}

				if (is_console)
				{
					endian_convert(&dest[surf].vertCount);
					endian_convert(&dest[surf].triCount);
					endian_convert(&dest[surf].triIndices);
					endian_convert(&dest[surf].vertexInfo.vertCount[0]);
					endian_convert(&dest[surf].vertexInfo.vertCount[1]);
					endian_convert(&dest[surf].vertexInfo.vertCount[2]);
					endian_convert(&dest[surf].vertexInfo.vertCount[3]);
					endian_convert(&dest[surf].verticies);
					endian_convert(&dest[surf].vertListCount);
					endian_convert(&dest[surf].rigidVertLists);
					for (auto a = 0; a < 5; a++)
					{
						endian_convert(&dest[surf].partBits[a]);
					}
				}
			}
		}

		void IXSurface::write(IZone* zone, ZoneBuffer* buf)
		{
			if (zone->get_target() == zone_target::pc)
			{
				auto data = this->asset_;
				auto dest = buf->write<XModelSurfs>(data);

				assert(sizeof XModelSurfs, 36);

				buf->push_stream(3);
				START_LOG_STREAM;

				dest->name = buf->write_str(this->name());

				if (data->surfs)
				{
					buf->align(3);
					auto destsurfaces = buf->write(data->surfs, data->numsurfs);
					write_xsurfaces<XSurface>(zone, buf, data->surfs, destsurfaces, data->numsurfs, false);
					ZoneBuffer::ClearPointer(&dest->surfs);
				}

				END_LOG_STREAM;
				buf->pop_stream();
			}
			else
			{
				alpha::XModelSurfs alpha_surfs = {};
				auto data = &alpha_surfs;
				auto dest = buf->write(data);

				assert(sizeof alpha::XModelSurfs, 32);

				buf->push_stream(3);
				START_LOG_STREAM;

				dest->name = buf->write_str(this->name());

				if (data->surfs)
				{
					buf->align(3);
					auto destsurfaces = buf->write(data->surfs, data->numsurfs);
					write_xsurfaces<alpha::XSurface>(zone, buf, data->surfs, destsurfaces, data->numsurfs, true);
					ZoneBuffer::ClearPointer(&dest->surfs);
				}

				END_LOG_STREAM;
				buf->pop_stream();

				endian_convert(&dest->name);
				endian_convert(&dest->surfs);
				endian_convert(&dest->numsurfs);
				for (auto a = 0; a < 5; a++)
				{
					endian_convert(&dest->partBits[a]);
				}
			}
			
		}

		void IXSurface::dump(XModelSurfs* asset)
		{
			AssetDumper dump;
			dump.open("XSurface\\"s + asset->name + ".xse");

			if (asset->name == "tntbomb_mp_low10"s)
			{
				return;
			}

			dump.dump_array(asset, 1);
			dump.dump_string(asset->name);

			for (int i = 0; i < asset->numsurfs; i++)
			{
				dump.dump_int(asset->surfs[i].tileMode);
				dump.dump_int(asset->surfs[i].deformed);
				dump.dump_int(asset->surfs[i].baseTriIndex);
				dump.dump_int(asset->surfs[i].baseVertIndex);

				for (int j = 0; j < 6; j++)
				{
					dump.dump_int(asset->surfs[i].partBits[j]);
				}

				// vertex bs
				dump.dump_array(&asset->surfs[i].vertexInfo, 1);
				dump.dump_array(asset->surfs[i].vertexInfo.vertsBlend,
				           asset->surfs[i].vertexInfo.vertCount[0] +
				           (asset->surfs[i].vertexInfo.vertCount[1] * 3) +
				           (asset->surfs[i].vertexInfo.vertCount[2] * 5) +
				           (asset->surfs[i].vertexInfo.vertCount[3] * 7)
				);

				dump.dump_int(asset->surfs[i].vertCount);
				dump.dump_array(asset->surfs[i].verticies, asset->surfs[i].vertCount);

				dump.dump_int(asset->surfs[i].triCount);
				dump.dump_array(asset->surfs[i].triIndices, asset->surfs[i].triCount);

				dump.dump_int(asset->surfs[i].vertListCount);
				dump.dump_array(asset->surfs[i].rigidVertLists, asset->surfs[i].vertListCount);
				for (int vert = 0; vert < asset->surfs[i].vertListCount; vert++)
				{
					if (asset->surfs[i].rigidVertLists[vert].collisionTree)
					{
						dump.dump_array(asset->surfs[i].rigidVertLists[vert].collisionTree, 1);

						if (asset->surfs[i].rigidVertLists[vert].collisionTree->leafs)
						{
							dump.dump_array(asset->surfs[i].rigidVertLists[vert].collisionTree->leafs,
							           asset->surfs[i].rigidVertLists[vert].collisionTree->leafCount);
						}

						if (asset->surfs[i].rigidVertLists[vert].collisionTree->nodes)
						{
							dump.dump_array(asset->surfs[i].rigidVertLists[vert].collisionTree->nodes,
							           asset->surfs[i].rigidVertLists[vert].collisionTree->nodeCount);
						}
					}
				}
			}

			dump.close();
		}
	}
}
