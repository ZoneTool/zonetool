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
	class ZoneMemory
	{
	private:
		LPVOID memory_pool_;
		std::size_t memory_size_;
		std::size_t mem_pos_;
		std::recursive_mutex mutex_;

	public:
		ZoneMemory(const ZoneMemory& mem) : memory_pool_(mem.memory_pool_), memory_size_(mem.memory_size_),
		                                    mem_pos_(mem.mem_pos_)
		{
		}

		ZoneMemory(const std::size_t& size)
		{
			mem_pos_ = 0;
			memory_size_ = size;
			memory_pool_ = VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

			if (!memory_pool_)
			{
				char buffer[256];
				_snprintf(buffer, sizeof buffer,
				          "ZoneTool just went out of memory, and has to be closed. Error code is %u (0x%08X).",
				          GetLastError(), GetLastError());

				MessageBoxA(nullptr, buffer, "ZoneTool: Out of Memory", NULL);
				std::exit(0);
			}
		}

		void Free()
		{
			std::lock_guard<std::recursive_mutex> g(this->mutex_);
			VirtualFree(memory_pool_, memory_size_, MEM_DECOMMIT | MEM_RELEASE);

			printf("ZoneTool memory statistics: used %ub of ram (%fmb).\n", mem_pos_,
				static_cast<float>(mem_pos_) / 1024 / 1024);

			memory_pool_ = nullptr;
			memory_size_ = 0;
			mem_pos_ = 0;
		}
		
		~ZoneMemory()
		{
			this->Free();
		}

		char* StrDup(const char* name)
		{
			std::lock_guard<std::recursive_mutex> g(this->mutex_);

			// get string length
			auto len = strlen(name) + 1;
			auto pointer = this->ManualAlloc<char>(len);
			memcpy(pointer, name, len);

			// return pointer
			return pointer;
		}

		char* StrDup(const std::string& name)
		{
			std::lock_guard<std::recursive_mutex> g(this->mutex_);
			return this->StrDup(name.data());
		}

		template <typename T>
		T* Alloc(std::size_t count)
		{
			std::lock_guard<std::recursive_mutex> g(this->mutex_);
			return this->ManualAlloc<T>(sizeof T, count);
		}

		template <typename T>
		T* Alloc()
		{
			std::lock_guard<std::recursive_mutex> g(this->mutex_);
			return this->Alloc<T>(1);
		}

		template <typename T>
		T* ManualAlloc(std::size_t size, std::size_t count = 1)
		{
			std::lock_guard<std::recursive_mutex> g(this->mutex_);

			// alloc pointer and zero it out
			auto pointer = reinterpret_cast<char*>(memory_pool_) + mem_pos_;
			memset(pointer, 0, size * count);
			mem_pos_ += size * count;

			// return pointer
			return reinterpret_cast<T*>(pointer);
		}
	};
}
