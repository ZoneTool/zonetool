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
	// Instruction opcodes
	std::uint8_t Memory::Instructions::nop_ = 0x90;
	std::uint8_t Memory::Instructions::call_ = 0xE8;
	std::uint8_t Memory::Instructions::jump_ = 0xE9;

	void Memory::restore()
	{
		this->unprotect(this->original_data_.size());

		for (std::size_t i = 0; i < original_data_.size(); i++)
		{
			*reinterpret_cast<std::uint8_t*>(this->address_ + i) = original_data_[i];
		}

		this->protect(this->original_data_.size());
	}

	void Memory::nop(std::size_t size)
	{
		this->set_original_data(size);
		this->unprotect(size);

		for (std::size_t i = 0; i < size; i++)
		{
			*reinterpret_cast<std::uint8_t*>(this->address_ + i) = Instructions::nop_;
		}

		this->protect(size);
	}

	void Memory::write_string(const std::string& str)
	{
		this->set_original_data(str.size() + 1);

		this->unprotect(str.size() + 1);
		strncpy(reinterpret_cast<char*>(this->address_), str.data(), str.size() + 1);
		this->protect(str.size() + 1);
	}

	void Memory::set_original_data(std::size_t size)
	{
		this->original_data_.clear();

		for (std::size_t i = 0; i < size; i++)
		{
			original_data_.push_back(*reinterpret_cast<std::uint8_t*>(this->address_ + i));
		}
	}

	void Memory::unprotect(std::size_t size)
	{
		VirtualProtect(reinterpret_cast<void*>(this->address_), size, PAGE_EXECUTE_READWRITE,
		               reinterpret_cast<DWORD*>(&this->protect_));
	}

	void Memory::protect(std::size_t size)
	{
		VirtualProtect(reinterpret_cast<void*>(this->address_), size, this->protect_,
		               reinterpret_cast<DWORD*>(&this->protect_));
	}

	Memory* Memory::initialize()
	{
		return new Memory(this);
	}

	Memory* Memory::install()
	{
		return initialize();
	}
}
