#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW5
	{
		IGlassWorld::IGlassWorld()
		{
		}

		IGlassWorld::~IGlassWorld()
		{
		}

		GlassWorld* IGlassWorld::parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			auto path = name + ".glassmap";
			if (!FileSystem::FileExists(path))
			{
				return nullptr;
			}

			// alloc FxWorld
			GlassWorld* asset;

			ZONETOOL_INFO("Parsing GlassMap \"%s\" ...", name.data());

			AssetReader read(mem);
			if (!read.Open(path))
			{
				return nullptr;
			}

			asset = read.Array<GlassWorld>();
			asset->name = read.String();
			asset->g_glassData = read.Array<G_GlassData>();

			if (asset->g_glassData)
			{
				asset->g_glassData->glassPieces = read.Array<G_GlassPiece>();
				asset->g_glassData->glassNames = read.Array<G_GlassName>();

				for (int i = 0; i < asset->g_glassData->glassNameCount; i++)
				{
					asset->g_glassData->glassNames[i].nameStr = read.String();
					asset->g_glassData->glassNames[i].pieceIndices = read.Array<unsigned short>();
				}
			}

			read.Close();

			return asset;
		}

		void IGlassWorld::init(void* asset, std::shared_ptr<ZoneMemory>& mem)
		{
			this->m_asset = reinterpret_cast<GlassWorld*>(asset);
			this->m_name = "maps/mp/" + currentzone + ".d3dbsp"; // name;
		}

		void IGlassWorld::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			this->m_name = "maps/mp/" + currentzone + ".d3dbsp"; // name;
			this->m_asset = this->parse(name, mem);

			if (!this->m_asset)
			{
				this->m_asset = DB_FindXAssetHeader(this->type(), name.data(), 1).glassworld;
			}
		}

		void IGlassWorld::prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
		{
		}

		void IGlassWorld::load_depending(IZone* zone)
		{
		}

		std::string IGlassWorld::name()
		{
			return this->m_name;
		}

		std::int32_t IGlassWorld::type()
		{
			return glass_map;
		}

		void IGlassWorld::write(IZone* zone, std::shared_ptr<ZoneBuffer>& buf)
		{
			auto data = this->m_asset;
			auto dest = buf->write(data);

			assert(sizeof GlassWorld, 8);
			assert(sizeof G_GlassData, 128);
			assert(sizeof G_GlassPiece, 12);
			assert(sizeof G_GlassName, 12);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->g_glassData)
			{
				buf->align(3);

				auto glassdata = data->g_glassData;
				auto destdata = buf->write(glassdata);

				if (glassdata->glassPieces)
				{
					buf->align(3);
					buf->write(glassdata->glassPieces, glassdata->pieceCount);
					ZoneBuffer::ClearPointer(&destdata->glassPieces);
				}
				if (glassdata->glassNames)
				{
					buf->align(3);
					auto namedest = buf->write(glassdata->glassNames, glassdata->glassNameCount);

					for (unsigned int i = 0; i < glassdata->glassNameCount; i++)
					{
						namedest[i].nameStr = buf->write_str(glassdata->glassNames[i].nameStr);

						if (glassdata->glassNames[i].pieceCount)
						{
							buf->align(1);
							buf->write(glassdata->glassNames[i].pieceIndices, glassdata->glassNames[i].pieceCount);
							ZoneBuffer::ClearPointer(&glassdata->glassNames[i].pieceIndices);
						}
					}

					ZoneBuffer::ClearPointer(&destdata->glassNames);
				}

				ZoneBuffer::ClearPointer(&dest->g_glassData);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IGlassWorld::dump(GlassWorld* asset)
		{
			auto path = asset->name + ".glassmap"s;
			AssetDumper write;

			if (!write.Open(path))
			{
				return;
			}

			write.Array(asset, 1);
			write.String(asset->name);

			write.Array(asset->g_glassData, 1);

			if (asset->g_glassData)
			{
				write.Array(asset->g_glassData->glassPieces, asset->g_glassData->pieceCount);
				write.Array(asset->g_glassData->glassNames, asset->g_glassData->glassNameCount);

				for (int i = 0; i < asset->g_glassData->glassNameCount; i++)
				{
					write.String(asset->g_glassData->glassNames[i].nameStr);
					write.Array(asset->g_glassData->glassNames[i].pieceIndices,
					            asset->g_glassData->glassNames[i].pieceCount);
				}
			}

			write.Close();
		}
	}
}
