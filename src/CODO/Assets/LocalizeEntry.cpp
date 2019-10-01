// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"

namespace ZoneTool::CODO
{
	ILocalizeEntry::ILocalizeEntry()
	{
	}

	ILocalizeEntry::~ILocalizeEntry()
	{
	}

	void ILocalizeEntry::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
	{
		this->m_name = name;
		this->m_asset = DB_FindXAssetHeader(this->type(), this->name().data()).localize;
	}

	void ILocalizeEntry::prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
	{
	}

	void ILocalizeEntry::load_depending(IZone* zone)
	{
	}

	std::string ILocalizeEntry::name()
	{
		return this->m_name;
	}

	std::int32_t ILocalizeEntry::type()
	{
		return localize;
	}

	void ILocalizeEntry::write(IZone* zone, std::shared_ptr<ZoneBuffer>& buf)
	{
		auto data = this->m_asset;
		auto dest = buf->write(data);

		buf->push_stream(3);
		START_LOG_STREAM;

		dest->localizedString = buf->write_str(data->localizedString);
		dest->name = buf->write_str(data->name);

		END_LOG_STREAM;
		buf->pop_stream();
	}

	void ILocalizeEntry::dump(LocalizeEntry* asset)
	{
	}
}
