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
#ifdef USE_CPP_POINTERS
	class ZoneMemPtrInternal {};
	template <typename T> class ZoneMemPtr : public ZoneMemPtrInternal
	{
	private:
		std::shared_ptr < T >	m_ptr;

	public:
		ZoneMemPtr()
		{
			this->m_ptr = std::make_shared < T >();
		}
		template <typename ...Args> ZoneMemPtr(Args&&... args)
		{
			this->m_ptr = std::make_shared < T >(args...);
		}
		std::shared_ptr < T >& ToShared()
		{
			return this->m_ptr;
		}
	};
	template <typename T> class ZoneMemPtrArr : public ZoneMemPtrInternal
	{
	private:
		std::shared_ptr < T >	m_ptr;

	public:
		ZoneMemPtrArr(std::size_t size)
		{
			this->m_ptr = std::shared_ptr < T >(new T[size], std::default_delete<T[]>());
		}
		std::shared_ptr < T >& ToShared()
		{
			return this->m_ptr;
		}
	};

	class ZoneMemory
	{
	private:
		std::vector < ZoneMemPtrInternal >	m_pointers;
		std::mutex							mutex_;

	public:
		ZoneMemory()
		{
		}

		char* StrDup(const char* name)
		{
			std::lock_guard < std::mutex > g(this->mutex_);

			// dup string
			auto pointer = ZoneMemPtrArr < char >(strlen(name) + 1);
			strcpy(
				reinterpret_cast<char*>(pointer.ToShared().get()), 
				name);
			this->m_pointers.push_back(pointer);

			// return pointer
			return reinterpret_cast<char*>(pointer.ToShared().get());
		}
		char* StrDup(const std::string &name)
		{
			return this->StrDup(name.data());
		}

		template <typename T> T* Alloc(std::size_t count)
		{
			std::lock_guard < std::mutex > g(this->mutex_);

			// alloc pointer and zero it out
			auto pointer = ZoneMemPtrArr < T >(count);
			memset(
				reinterpret_cast<T*>(pointer.ToShared().get()), 
				0, 
				sizeof T * count);

			// add pointer to array
			this->m_pointers.push_back(pointer);

			// return pointer
			return reinterpret_cast<T*>(pointer.ToShared().get());
		}

		template <typename T> T* Alloc()
		{
			std::lock_guard < std::mutex > g(this->mutex_);

			// alloc pointer and zero it out
			auto pointer = ZoneMemPtr < T >();
			memset(
				reinterpret_cast<T*>(pointer.ToShared().get()),
				0,
				sizeof T);

			// add pointer to array
			this->m_pointers.push_back(pointer);

			// return pointer
			return reinterpret_cast<T*>(pointer.ToShared().get());
		}

		template <typename T> T* ManualAlloc(std::size_t size, std::size_t count = 1)
		{
			std::lock_guard < std::mutex > g(this->mutex_);

			// alloc pointer and zero it out
			auto pointer = ZoneMemPtrArr < char >(size * count); // reinterpret_cast<T*>(malloc(size * count));
			memset(
				reinterpret_cast<char*>(pointer.ToShared().get()), 
				0, 
				size * count);

			// add pointer to array
			this->m_pointers.push_back(pointer);

			// return pointer
			return reinterpret_cast<T*>(pointer.ToShared().get());
		}
	};
#else
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

			printf("ZoneTool memory statistics: build used %ub of ram (%fmb).\n", mem_pos_,
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
#endif
}
