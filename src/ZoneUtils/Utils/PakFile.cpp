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
	PakFile::PakFile(const std::uint32_t version)
	{
		this->buffer_ = std::make_shared<ZoneBuffer>(1024 * 1024 * 200);
		this->buffer_->init_streams(1);
		
		// write header
		this->buffer_->write_stream("IWffu100", 8, 1);
		auto dest_version = this->buffer_->write(&version);

		// endian convert version
		endian_convert(dest_version);
	}

	std::pair<std::uint32_t, std::uint32_t> PakFile::add_entry(const std::uint8_t* pixels, const std::size_t size, const bool is_compressed) const
	{
		// get start position
		auto start_location = this->buffer_->size();

		// write image data
		if (!is_compressed)
		{
			auto compressed = ZoneBuffer::compress_zlib(pixels, size);
			this->buffer_->write_stream(compressed.data(), compressed.size(), 1);
		}
		else
		{
			this->buffer_->write_stream(pixels, size, 1);
		}

		// get end position
		auto end_location = this->buffer_->size();

		return { start_location, end_location };
	}

	std::pair<std::uint32_t, std::uint32_t> PakFile::add_entry(const std::vector<std::uint8_t>& pixels) const
	{
		return add_entry(pixels.data(), pixels.size());
	}

	void PakFile::save(const std::string& filename)
	{
		this->buffer_->save(filename);
	}

	std::size_t PakFile::size()
	{
		return this->buffer_->size();
	}

}
