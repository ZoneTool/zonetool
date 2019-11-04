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
		void IGameWorldSp::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			const auto mp_asset = IGameWorldMp::parse(name, mem);

			if (!mp_asset)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data()).game_map_sp;
			}
			else
			{
				// generate sp asset based on mp one
				this->asset_ = mem->Alloc<GameWorldSp>();
				this->asset_->name = mp_asset->name;
				this->asset_->g_glassData = reinterpret_cast<G_GlassData*>(mp_asset->g_glassData);
			}
		}

		void IGameWorldSp::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IGameWorldSp::load_depending(IZone* zone)
		{
		}

		std::string IGameWorldSp::name()
		{
			return this->name_;
		}

		void IGameWorldSp::write(IZone* zone, ZoneBuffer* buf)
		{
			const auto data = this->asset_;
			const auto dest = buf->write(data);

			assert(sizeof GameWorldSp, 56);
			assert(sizeof G_GlassData, 128);
			assert(sizeof G_GlassPiece, 12);
			assert(sizeof G_GlassName, 12);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->g_glassData)
			{
				buf->align(3);

				const auto glassdata = data->g_glassData;
				const auto destdata = buf->write(glassdata);

				if (glassdata->glassPieces)
				{
					buf->align(3);
					buf->write(glassdata->glassPieces, glassdata->pieceCount);
					ZoneBuffer::ClearPointer(&destdata->glassPieces);
				}
				if (glassdata->glassNames)
				{
					buf->align(3);
					const auto namedest = buf->write(glassdata->glassNames, glassdata->glassNameCount);

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

		std::int32_t IGameWorldSp::type()
		{
			return game_map_sp;
		}

		void IGameWorldSp::dump(GameWorldSp* asset)
		{
			const auto mp_asset = new GameWorldMp;
			memset(mp_asset, 0, sizeof GameWorldMp);

			mp_asset->name = asset->name;
			mp_asset->g_glassData = asset->g_glassData;

			IGameWorldMp::dump(mp_asset);

			delete mp_asset;
		}
	}
}
