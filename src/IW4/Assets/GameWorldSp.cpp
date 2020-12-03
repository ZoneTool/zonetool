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
			this->name_ = "maps/"s + (currentzone.substr(0, 3) == "mp_" ? "mp/" : "") + currentzone + ".d3dbsp";
			auto* mp_asset = IGameWorldMp::parse(name, mem);

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
			auto* data = this->asset_;
			auto* dest = buf->write(data);

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

				auto* glass_data = data->g_glassData;
				auto* dest_glass_data = buf->write(glass_data);

				if (glass_data->glassPieces)
				{
					buf->align(3);
					buf->write(glass_data->glassPieces, glass_data->pieceCount);
					ZoneBuffer::clear_pointer(&dest_glass_data->glassPieces);
				}
				if (glass_data->glassNames)
				{
					buf->align(3);
					const auto namedest = buf->write(glass_data->glassNames, glass_data->glassNameCount);

					for (unsigned int i = 0; i < glass_data->glassNameCount; i++)
					{
						namedest[i].nameStr = buf->write_str(glass_data->glassNames[i].nameStr);

						if (glass_data->glassNames[i].pieceCount)
						{
							buf->align(1);
							buf->write(glass_data->glassNames[i].pieceIndices, glass_data->glassNames[i].pieceCount);
							ZoneBuffer::clear_pointer(&glass_data->glassNames[i].pieceIndices);
						}
					}

					ZoneBuffer::clear_pointer(&dest_glass_data->glassNames);
				}

				ZoneBuffer::clear_pointer(&dest->g_glassData);
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
			auto* mp_asset = new GameWorldMp;
			memset(mp_asset, 0, sizeof GameWorldMp);

			mp_asset->name = asset->name;
			mp_asset->g_glassData = asset->g_glassData;

			IGameWorldMp::dump(mp_asset);

			delete mp_asset;
		}
	}
}
