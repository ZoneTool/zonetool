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
		void ILeaderBoardDef::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data(), 1).leaderboard;
		}

		void ILeaderBoardDef::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void ILeaderBoardDef::load_depending(IZone* zone)
		{
		}

		std::string ILeaderBoardDef::name()
		{
			return this->name_;
		}

		std::int32_t ILeaderBoardDef::type()
		{
			return leaderboarddef;
		}

		void ILeaderBoardDef::write(IZone* zone, ZoneBuffer* buf)
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);
			
			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->columns)
			{
				buf->align(3);
				auto* dest_columns = buf->write(data->columns, data->columnCount);

				if (data->columnCount > 0)
				{
					for (auto i = 0; i < data->columnCount; i++)
					{
						dest_columns[i].title = buf->write_str(data->columns[i].title);
						dest_columns[i].statName = buf->write_str(data->columns[i].statName);
					}
				}

				ZoneBuffer::clear_pointer(&dest->columns);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void ILeaderBoardDef::dump(LeaderBoardDef* asset)
		{
			const auto data = asset->ToJson();

			const auto path = "leaderboards\\"s + asset->name;
			const auto json = data.dump(4);

			auto* file = FileSystem::FileOpen(path, "w"s);
			fwrite(json.data(), json.size(), 1, file);
			FileSystem::FileClose(file);
		}
	}
}
