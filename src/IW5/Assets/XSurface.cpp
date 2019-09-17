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
		ModelSurface* IXSurface::parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			sizeof XSurface;

			AssetReader read(mem);

			if (!read.Open("XSurface\\" + name + ".xse"))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing xmodel surface \"%s\"...", name.c_str());

			auto asset = read.Array<ModelSurface>();
			asset->name = read.String();

			asset->xSurficies = mem->Alloc<XSurface>(asset->xSurficiesCount);

			for (int i = 0; i < asset->xSurficiesCount; i++)
			{
				asset->xSurficies[i].tileMode = read.Int();
				asset->xSurficies[i].deformed = read.Int();
				asset->xSurficies[i].baseTriIndex = read.Int();
				asset->xSurficies[i].baseVertIndex = read.Int();

				for (int j = 0; j < 6; j++)
				{
					asset->xSurficies[i].partBits[j] = read.Int();
				}

				// vertex
				auto vertexInfo = read.Array<XSurfaceVertexInfo>();
				memcpy(&asset->xSurficies[i].vertexInfo, vertexInfo, sizeof XSurfaceVertexInfo);
				asset->xSurficies[i].vertexInfo.vertsBlend = read.Array<unsigned short>();

				// verticies
				asset->xSurficies[i].vertCount = read.Int();
				asset->xSurficies[i].verticies = read.Array<GfxPackedVertex>();

				// triangles
				asset->xSurficies[i].triCount = read.Int();
				asset->xSurficies[i].triIndices = read.Array<Face>();

				// rigidVertLists
				asset->xSurficies[i].vertListCount = read.Int();
				asset->xSurficies[i].rigidVertLists = read.Array<XRigidVertList>();
				for (int vert = 0; vert < asset->xSurficies[i].vertListCount; vert++)
				{
					if (asset->xSurficies[i].rigidVertLists[vert].collisionTree)
					{
						asset->xSurficies[i].rigidVertLists[vert].collisionTree = read.Array<XSurfaceCollisionTree>();

						if (asset->xSurficies[i].rigidVertLists[vert].collisionTree->leafs)
						{
							asset->xSurficies[i].rigidVertLists[vert].collisionTree->leafs = read.Array<
								XSurfaceCollisionLeaf>();
						}

						if (asset->xSurficies[i].rigidVertLists[vert].collisionTree->nodes)
						{
							asset->xSurficies[i].rigidVertLists[vert].collisionTree->nodes = read.Array<
								XSurfaceCollisionNode>();
						}
					}
				}
			}

			read.Close();

			return asset;
		}

		void IXSurface::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			this->m_name = name;
			this->m_asset = this->parse(name, mem);

			if (!this->m_asset)
			{
				this->m_asset = DB_FindXAssetHeader(this->type(), this->m_name.data(), 1).xsurface;
			}
		}

		void IXSurface::init(void* asset, std::shared_ptr<ZoneMemory>& mem)
		{
			this->m_asset = reinterpret_cast<ModelSurface*>(asset);
			this->m_name = this->m_asset->name;
		}

		void IXSurface::prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
		{
		}

		void IXSurface::load_depending(IZone* zone)
		{
		}

		std::string IXSurface::name()
		{
			return this->m_name;
		}

		std::int32_t IXSurface::type()
		{
			return xmodelsurfs;
		}

		void IXSurface::write_xsurfices(IZone* zone, std::shared_ptr<ZoneBuffer>& buf, XSurface* data,
		                                std::int16_t count)
		{
			assert(sizeof Face, 6);
			assert(sizeof XRigidVertList, 12);
			assert(sizeof XSurfaceCollisionTree, 40);

			auto dest = buf->write(data, count);

			for (std::int16_t surf = 0; surf < count; surf++)
			{
				if (data[surf].vertexInfo.vertsBlend)
				{
					dest[surf].vertexInfo.vertsBlend = buf->write_s(1, data[surf].vertexInfo.vertsBlend,
					                                                data[surf].vertexInfo.vertCount[0] +
					                                                (data[surf].vertexInfo.vertCount[1] * 3) + (data[
						                                                surf].vertexInfo.vertCount[2] * 5) + (data[
						                                                surf].vertexInfo.vertCount[3] * 7));
				}

				buf->push_stream(6);
				if (data[surf].verticies)
				{
					dest[surf].verticies = buf->write_s(15, data[surf].verticies, data[surf].vertCount);
				}
				buf->pop_stream();

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
										entry->nodes = buf->write_s(15, entry->nodes, entry->nodeCount);
									}

									if (entry->leafs)
									{
										entry->leafs = buf->write_s(1, entry->leafs, entry->leafCount);
									}
								}
							}
						}
					}
				}

				buf->push_stream(7);
				if (data[surf].triIndices)
				{
					dest[surf].triIndices = buf->write_s(15, data[surf].triIndices, data[surf].triCount);
				}
				buf->pop_stream();
			}
		}

		void IXSurface::write(IZone* zone, std::shared_ptr<ZoneBuffer>& buf)
		{
			assert(sizeof ModelSurface, 36);

			auto dest = buf->at<ModelSurface>();
			auto data = this->m_asset;

			buf->write<ModelSurface>(data);
			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->xSurficies)
			{
				buf->align(3);
				this->write_xsurfices(zone, buf, data->xSurficies, data->xSurficiesCount);
				ZoneBuffer::ClearPointer(&dest->xSurficies);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IXSurface::dump(ModelSurface* asset)
		{
			AssetDumper dump;
			dump.Open("XSurface\\"s + asset->name + ".xse");

			dump.Array(asset, 1);
			dump.String(asset->name);

			for (int i = 0; i < asset->xSurficiesCount; i++)
			{
				dump.Int(asset->xSurficies[i].tileMode);
				dump.Int(asset->xSurficies[i].deformed);
				dump.Int(asset->xSurficies[i].baseTriIndex);
				dump.Int(asset->xSurficies[i].baseVertIndex);

				for (int j = 0; j < 6; j++)
				{
					dump.Int(asset->xSurficies[i].partBits[j]);
				}

				// vertex bs
				dump.Array(&asset->xSurficies[i].vertexInfo, 1);
				dump.Array(asset->xSurficies[i].vertexInfo.vertsBlend,
				           asset->xSurficies[i].vertexInfo.vertCount[0] +
				           (asset->xSurficies[i].vertexInfo.vertCount[1] * 3) +
				           (asset->xSurficies[i].vertexInfo.vertCount[2] * 5) +
				           (asset->xSurficies[i].vertexInfo.vertCount[3] * 7)
				);

				dump.Int(asset->xSurficies[i].vertCount);
				dump.Array(asset->xSurficies[i].verticies, asset->xSurficies[i].vertCount);

				dump.Int(asset->xSurficies[i].triCount);
				dump.Array(asset->xSurficies[i].triIndices, asset->xSurficies[i].triCount);

				dump.Int(asset->xSurficies[i].vertListCount);
				dump.Array(asset->xSurficies[i].rigidVertLists, asset->xSurficies[i].vertListCount);
				for (int vert = 0; vert < asset->xSurficies[i].vertListCount; vert++)
				{
					if (asset->xSurficies[i].rigidVertLists[vert].collisionTree)
					{
						dump.Array(asset->xSurficies[i].rigidVertLists[vert].collisionTree, 1);

						if (asset->xSurficies[i].rigidVertLists[vert].collisionTree->leafs)
						{
							dump.Array(asset->xSurficies[i].rigidVertLists[vert].collisionTree->leafs,
							           asset->xSurficies[i].rigidVertLists[vert].collisionTree->leafCount);
						}

						if (asset->xSurficies[i].rigidVertLists[vert].collisionTree->nodes)
						{
							dump.Array(asset->xSurficies[i].rigidVertLists[vert].collisionTree->nodes,
							           asset->xSurficies[i].rigidVertLists[vert].collisionTree->nodeCount);
						}
					}
				}
			}

			dump.Close();
		}

		IXSurface::IXSurface()
		{
		}

		IXSurface::~IXSurface()
		{
		}
	}
}
