#pragma once

namespace ZoneTool
{
	class Memory
	{
	public:
		struct Instructions
		{
			static std::uint8_t Nop;
			static std::uint8_t Call;
			static std::uint8_t Jump;
		};

		// Constructors
		Memory(std::uintptr_t address) // used for stuff like nops
		{
			this->m_address = address;
			this->m_function = 0;
		}

		Memory(Memory* memoryptr)
		{
			this->m_address = memoryptr->m_address;
			this->m_function = memoryptr->m_function;
		}

		// Memory function
		template <typename T>
		static std::function<T> Func(std::uintptr_t function)
		{
			return std::function<T>(reinterpret_cast<T*>(function));
		}

		// Returns a pointer to the current Hook instance, incase you want to uninstall your hook later on.
		Memory* Initialize();
		Memory* Install();

		// Restores the original data of the address
		void Restore();

		// Address only patches
		template <typename T>
		void Set(T value)
		{
			this->Unprotect(sizeof(T));
			*reinterpret_cast<T*>(m_address) = value;
			this->Protect(sizeof(T));
		}

		void Nop(std::size_t size);

		// The following functions change the memory
		template <typename T>
		void Call(T func, std::size_t size = 5)
		{
			this->m_function = (std::uintptr_t)func;
			this->SetOriginalData(size);
			this->Unprotect(size);

#ifndef _WIN64
			this->OriginalAddress = *reinterpret_cast<std::uint32_t*>(this->m_address + 1);
			this->OriginalAddress += this->m_address;
			this->OriginalAddress += 5;

			// 32bit .exe call
			*reinterpret_cast<std::uint8_t*>(this->m_address) = Instructions::Call;
			*reinterpret_cast<std::uint32_t*>(this->m_address + 1) = (this->m_function - this->m_address - 5);
#else
			this->OriginalAddress = *reinterpret_cast<std::uint32_t*>(this->m_address + 1);
			this->OriginalAddress += this->m_address;
			this->OriginalAddress += 5;

			// 64bit .exe call
			*reinterpret_cast<std::uint8_t*>(this->m_address) = Memory::Instructions::Call;
			*reinterpret_cast<std::uint32_t*>(this->m_address + 1) = (this->m_function - this->m_address - 5);
#endif

			this->Protect(size);
		}

		template <typename T>
		void Jump(T func, std::size_t size = 5)
		{
			this->m_function = (std::uintptr_t)func;
			this->SetOriginalData(size);
			this->Unprotect(size);

#ifndef _WIN64
			this->OriginalAddress = *reinterpret_cast<std::uint32_t*>(this->m_address + 1);
			this->OriginalAddress += this->m_address;
			this->OriginalAddress += 5;

			// 32bit .exe jump
			*reinterpret_cast<std::uint8_t*>(this->m_address) = Instructions::Jump;
			*reinterpret_cast<std::uint32_t*>(this->m_address + 1) = (this->m_function - this->m_address - 5);
#else
			this->OriginalAddress = *reinterpret_cast<std::uint32_t*>(this->m_address + 1);
			this->OriginalAddress += this->m_address;
			this->OriginalAddress += 5;

			// 64bit .exe call
			*reinterpret_cast<std::uint8_t*>(this->m_address) = Memory::Instructions::Jump;
			*reinterpret_cast<std::uint32_t*>(this->m_address + 1) = (this->m_function - this->m_address - 5);
#endif

			this->Protect(size);
		}

		void WriteString(std::string str);

		template <typename T>
		void Write(T* data)
		{
			this->Write<T>(data, 1, 0);
		}

		template <typename T>
		void Write(T* data, std::size_t count = 1, std::size_t size = 0)
		{
			if (!size)
			{
				size = sizeof(T);
			}
			size *= count;

			this->SetOriginalData(size);

			this->Unprotect(size);
			memcpy(reinterpret_cast<void*>(this->m_address), data, size);
			this->Protect(size);
		}

		// This holds the original jump/call location
		std::uintptr_t OriginalAddress;

	private:
		// Sets the original data for opcode restoration
		void SetOriginalData(std::size_t size);

		// Protects / Unprotects the address.
		void Unprotect(std::size_t size);
		void Protect(std::size_t size);

		// Original data, for restoring purposes.
		std::vector<std::uint8_t> m_originaldata;

		// Other hooking information
		std::uint32_t m_protect;
		std::uintptr_t m_address;
		std::uintptr_t m_function;
	};
}
