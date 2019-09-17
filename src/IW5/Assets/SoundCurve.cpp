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

		void ISoundCurve::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			this->m_name = name;
			this->m_asset = DB_FindXAssetHeader(this->type(), this->name().data(), 1).soundcurve;
		}

		void ISoundCurve::prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
		{
		}

		void ISoundCurve::load_depending(IZone* zone)
		{
		}

		std::string ISoundCurve::name()
		{
			return this->m_name;
		}

		std::int32_t ISoundCurve::type()
		{
			return sndcurve;
		}

		void ISoundCurve::write(IZone* zone, std::shared_ptr<ZoneBuffer>& buf)
		{
			auto data = this->m_asset;
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
