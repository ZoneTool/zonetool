// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

namespace ZoneTool
{
	class FileReader
	{
	private:
		std::vector<std::uint8_t> m_buf;
		std::size_t m_pos;

	public:
		FileReader(std::vector<std::uint8_t>& buffer);
		FileReader(std::uint8_t* buffer, std::size_t size);

		template <typename T>
		T Read()
		{
			auto retval = *reinterpret_cast<T*>(&this->m_buf[this->m_pos]);
			this->m_pos += sizeof T;

			return retval;
		}

		template <typename T>
		T* ReadArray(std::size_t count = 1)
		{
			auto retval = reinterpret_cast<T*>(&this->m_buf[this->m_pos]);
			this->m_pos += sizeof T * count;

			return retval;
		}

		template <typename T>
		void ReadManual(T* ptr, std::size_t count = 1)
		{
			if (count)
			{
				memcpy(ptr,
				       &this->m_buf[this->m_pos],
				       count * sizeof T);
			}
		}

		template <typename T>
		void ReadManual(T* ptr, std::size_t count, std::size_t size)
		{
			if (count && size)
			{
				memcpy(ptr,
				       &this->m_buf[this->m_pos],
				       count * size);
			}
		}

		std::string ReadString()
		{
			auto retval = static_cast<std::string>(
				reinterpret_cast<const char*>(&this->m_buf[this->m_pos]));
			this->m_pos += retval.size() + 1;

			return retval;
		}
	};
}
