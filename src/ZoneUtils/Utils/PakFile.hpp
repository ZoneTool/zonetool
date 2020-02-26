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
	class PakFile
	{
	public:
		PakFile(const std::uint32_t version);

		[[nodiscard]] std::pair<std::uint32_t, std::uint32_t> add_entry(const std::uint8_t* pixels, const std::size_t size) const;
		[[nodiscard]] std::pair<std::uint32_t, std::uint32_t> add_entry(const std::vector<std::uint8_t>& pixels) const;
		void save(const std::string& filename);
		std::size_t size();
		
	private:
		std::shared_ptr<ZoneBuffer> buffer_;
		
	};
}
