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
		ISoundCurve::ISoundCurve()
		{
		}

		ISoundCurve::~ISoundCurve()
		{
		}

		void ISoundCurve::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data(), 1).soundcurve;
		}

		void ISoundCurve::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void ISoundCurve::load_depending(IZone* zone)
		{
		}

		std::string ISoundCurve::name()
		{
			return this->name_;
		}

		std::int32_t ISoundCurve::type()
		{
			return sndcurve;
		}

		void ISoundCurve::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->asset_;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->filename = buf->write_str(this->name());

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void ISoundCurve::dump(SndCurve* asset)
		{
		}
	}
}
