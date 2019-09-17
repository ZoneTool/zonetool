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
	std::uint8_t Memory::Instructions::Nop = 0x90;
	std::uint8_t Memory::Instructions::Call = 0xE8;
	std::uint8_t Memory::Instructions::Jump = 0xE9;

	void Memory::Restore()
	{
		this->Unprotect(this->m_originaldata.size());

		for (std::size_t i = 0; i < m_originaldata.size(); i++)
		{
			*reinterpret_cast<std::uint8_t*>(this->m_address + i) = m_originaldata[i];
		}

		this->Protect(this->m_originaldata.size());
	}

	void Memory::Nop(std::size_t size)
	{
		this->SetOriginalData(size);
		this->Unprotect(size);

		for (std::size_t i = 0; i < size; i++)
		{
			*reinterpret_cast<std::uint8_t*>(this->m_address + i) = Instructions::Nop;
		}

		this->Protect(size);
	}

	void Memory::WriteString(std::string str)
	{
		this->SetOriginalData(str.size() + 1);

		this->Unprotect(str.size() + 1);
		strncpy(reinterpret_cast<char*>(this->m_address), str.data(), str.size() + 1);
		this->Protect(str.size() + 1);
	}

	void Memory::SetOriginalData(std::size_t size)
	{
		this->m_originaldata.clear();

		for (std::size_t i = 0; i < size; i++)
		{
			m_originaldata.push_back(*reinterpret_cast<std::uint8_t*>(this->m_address + i));
		}
	}

	void Memory::Unprotect(std::size_t size)
	{
		VirtualProtect(reinterpret_cast<void*>(this->m_address), size, PAGE_EXECUTE_READWRITE,
		               reinterpret_cast<DWORD*>(&this->m_protect));
	}

	void Memory::Protect(std::size_t size)
	{
		VirtualProtect(reinterpret_cast<void*>(this->m_address), size, this->m_protect,
		               reinterpret_cast<DWORD*>(&this->m_protect));
	}

	Memory* Memory::Initialize()
	{
		return new Memory(this);
	}

	Memory* Memory::Install()
	{
		return Initialize();
	}
}
