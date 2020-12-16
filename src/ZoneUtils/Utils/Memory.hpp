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
	class Memory
	{
	public:
		struct Instructions
		{
			static std::uint8_t nop_;
			static std::uint8_t call_;
			static std::uint8_t jump_;
		};

		// Constructors
		Memory(std::uintptr_t address) // used for stuff like nops
		{
			this->address_ = address;
			this->function_ = 0;
		}

		Memory(Memory* memoryptr)
		{
			this->address_ = memoryptr->address_;
			this->function_ = memoryptr->function_;
		}

		// Memory function
		template <typename T>
		static std::function<T> func(std::uintptr_t function)
		{
			return std::function<T>(reinterpret_cast<T*>(function));
		}

		// Returns a pointer to the current Hook instance, incase you want to uninstall your hook later on.
		Memory* initialize();
		Memory* install();

		// Restores the original data of the address
		void restore();

		// Address only patches
		template <typename T>
		void set(T value)
		{
			this->unprotect(sizeof(T));
			*reinterpret_cast<T*>(address_) = value;
			this->protect(sizeof(T));
		}

		void nop(std::size_t size);

		// The following functions change the memory_
		template <typename T>
		void call(T func, std::size_t size = 5)
		{
			this->function_ = (std::uintptr_t)func;
			this->set_original_data(size);
			this->unprotect(size);

#ifndef _WIN64
			this->original_address = *reinterpret_cast<std::uint32_t*>(this->address_ + 1);
			this->original_address += this->address_;
			this->original_address += 5;

			// 32bit .exe call
			*reinterpret_cast<std::uint8_t*>(this->address_) = Instructions::call_;
			*reinterpret_cast<std::uint32_t*>(this->address_ + 1) = (this->function_ - this->address_ - 5);
#else
			this->original_address = *reinterpret_cast<std::uint32_t*>(this->address_ + 1);
			this->original_address += this->address_;
			this->original_address += 5;

			// 64bit .exe call
			*reinterpret_cast<std::uint8_t*>(this->address_) = Memory::Instructions::Call;
			*reinterpret_cast<std::uint32_t*>(this->address_ + 1) = (this->function_ - this->address_ - 5);
#endif

			this->protect(size);
		}

		template <typename T>
		void jump(T func, std::size_t size = 5)
		{
			this->function_ = (std::uintptr_t)func;
			this->set_original_data(size);
			this->unprotect(size);

#ifndef _WIN64
			this->original_address = *reinterpret_cast<std::uint32_t*>(this->address_ + 1);
			this->original_address += this->address_;
			this->original_address += 5;

			// 32bit .exe jump
			*reinterpret_cast<std::uint8_t*>(this->address_) = Instructions::jump_;
			*reinterpret_cast<std::uint32_t*>(this->address_ + 1) = (this->function_ - this->address_ - 5);
#else
			this->original_address = *reinterpret_cast<std::uint32_t*>(this->address_ + 1);
			this->original_address += this->address_;
			this->original_address += 5;

			// 64bit .exe call
			*reinterpret_cast<std::uint8_t*>(this->address_) = Memory::Instructions::Jump;
			*reinterpret_cast<std::uint32_t*>(this->address_ + 1) = (this->function_ - this->address_ - 5);
#endif

			this->protect(size);
		}

		void write_string(const std::string& str);

		template <typename T>
		void write(T* data)
		{
			this->write<T>(data, 1, 0);
		}

		template <typename T>
		void write(T* data, std::size_t count = 1, std::size_t size = 0)
		{
			if (!size)
			{
				size = sizeof(T);
			}
			size *= count;

			this->set_original_data(size);

			this->unprotect(size);
			memcpy(reinterpret_cast<void*>(this->address_), data, size);
			this->protect(size);
		}

		// This holds the original jump/call location
		std::uintptr_t original_address;

	private:
		// Sets the original data for opcode restoration
		void set_original_data(std::size_t size);

		// Protects / Unprotects the address.
		void unprotect(std::size_t size);
		void protect(std::size_t size);

		// Original data, for restoring purposes.
		std::vector<std::uint8_t> original_data_;

		// Other hooking information
		std::uint32_t protect_;
		std::uintptr_t address_;
		std::uintptr_t function_;
	};
}
