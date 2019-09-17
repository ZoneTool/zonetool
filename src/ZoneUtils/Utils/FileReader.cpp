#include "stdafx.hpp"

namespace ZoneTool
{
	FileReader::FileReader(std::uint8_t* buffer, std::size_t size)
	{
		this->m_buf.resize(size);
		this->m_pos = 0;

		memcpy(&this->m_buf[0], buffer, size);
	}

	FileReader::FileReader(std::vector<std::uint8_t>& buffer)
	{
		this->m_buf = buffer;
		this->m_pos = 0;
	}
}
