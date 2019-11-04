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
		ILeaderBoardDef::ILeaderBoardDef()
		{
		}

		ILeaderBoardDef::~ILeaderBoardDef()
		{
		}

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
			auto varLeaderboardDef = this->asset_;
			auto dest = buf->write(varLeaderboardDef);

			LeaderBoardDef;

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (varLeaderboardDef->columns)
			{
				buf->align(3);
				auto destColumns = buf->write(varLeaderboardDef->columns, varLeaderboardDef->columnCount);

				if (varLeaderboardDef->columnCount > 0)
				{
					for (int i = 0; i < varLeaderboardDef->columnCount; i++)
					{
						destColumns[i].title = buf->write_str(varLeaderboardDef->columns[i].title);
						destColumns[i].statName = buf->write_str(varLeaderboardDef->columns[i].statName);
					}
				}

				ZoneBuffer::ClearPointer(&dest->columns);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void ILeaderBoardDef::dump(LeaderBoardDef* asset)
		{
			Json& data = asset->ToJson();

			std::string path = "leaderboards\\"s + asset->name;
			std::string json = data.dump(4);

			auto file = FileSystem::FileOpen(path, "w"s);
			fwrite(json.data(), json.size(), 1, file);
			FileSystem::FileClose(file);
		}
	}
}
