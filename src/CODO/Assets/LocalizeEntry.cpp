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

	void ILocalizeEntry::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = name;
		this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data()).localize;
	}

	void ILocalizeEntry::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void ILocalizeEntry::load_depending(IZone* zone)
	{
	}

	std::string ILocalizeEntry::name()
	{
		return this->name_;
	}

	std::int32_t ILocalizeEntry::type()
	{
		return localize;
	}

	void ILocalizeEntry::write(IZone* zone, ZoneBuffer* buf)
	{
		auto data = this->asset_;
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
