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
		std::mutex							m_mutex;

	public:
		ZoneMemory()
		{
		}

		char* StrDup(const char* name)
		{
			std::lock_guard < std::mutex > g(this->m_mutex);

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
			std::lock_guard < std::mutex > g(this->m_mutex);

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
			std::lock_guard < std::mutex > g(this->m_mutex);

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
			std::lock_guard < std::mutex > g(this->m_mutex);

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
		LPVOID m_memoryPool;
		std::size_t m_memorySize;
		std::size_t m_memPos;
		std::recursive_mutex m_mutex;

	public:
		ZoneMemory(const ZoneMemory& mem) : m_memoryPool(mem.m_memoryPool), m_memorySize(mem.m_memorySize),
		                                    m_memPos(mem.m_memPos)
		{
		}

		ZoneMemory(const std::size_t& size)
		{
			m_memPos = 0;
			m_memorySize = size;
			m_memoryPool = VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

			if (!m_memoryPool)
			{
				char buffer[256];
				_snprintf(buffer, sizeof buffer,
				          "ZoneTool just went out of memory, and has to be closed. Error code is %u (0x%08X).",
				          GetLastError(), GetLastError());

				MessageBoxA(nullptr, buffer, "ZoneTool: Out of Memory", NULL);
				std::exit(0);
			}
		}

		~ZoneMemory()
		{
			std::lock_guard<std::recursive_mutex> g(this->m_mutex);
			VirtualFree(m_memoryPool, m_memorySize, MEM_DECOMMIT | MEM_RELEASE);

			printf("ZoneTool post-build memory statistics: build used %ub of ram (%fmb).\n", m_memPos,
			       static_cast<float>(m_memPos) / 1024 / 1024);

			m_memoryPool = nullptr;
			m_memorySize = 0;
			m_memPos = 0;
		}

		char* StrDup(const char* name)
		{
			std::lock_guard<std::recursive_mutex> g(this->m_mutex);

			// get string length
			auto len = strlen(name) + 1;
			auto pointer = this->ManualAlloc<char>(len);
			memcpy(pointer, name, len);

			// return pointer
			return pointer;
		}

		char* StrDup(const std::string& name)
		{
			std::lock_guard<std::recursive_mutex> g(this->m_mutex);
			return this->StrDup(name.data());
		}

		template <typename T>
		T* Alloc(std::size_t count)
		{
			std::lock_guard<std::recursive_mutex> g(this->m_mutex);
			return this->ManualAlloc<T>(sizeof T, count);
		}

		template <typename T>
		T* Alloc()
		{
			std::lock_guard<std::recursive_mutex> g(this->m_mutex);
			return this->Alloc<T>(1);
		}

		template <typename T>
		T* ManualAlloc(std::size_t size, std::size_t count = 1)
		{
			std::lock_guard<std::recursive_mutex> g(this->m_mutex);

			// alloc pointer and zero it out
			auto pointer = reinterpret_cast<char*>(m_memoryPool) + m_memPos;
			memset(pointer, 0, size * count);
			m_memPos += size * count;

			// return pointer
			return reinterpret_cast<T*>(pointer);
		}
	};
#endif
}
