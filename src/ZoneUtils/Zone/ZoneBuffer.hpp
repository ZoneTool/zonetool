// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

#include <stack>
#include <bitset>
#include <fstream>
#include <iostream>
#include <unordered_map>

namespace ZoneTool
{
	class FileSystem;

	template <typename T>
	static void encrypt_data(T* _data, std::size_t _size)
	{
		return;
		
		auto fastfile = FileSystem::GetFastFile();

		auto encryptionKey = static_cast<std::string>(fastfile + ": This fastfile is property of the Plutonium Project."
		);
		auto dataptr = reinterpret_cast<char*>(_data);

		auto keyPos = 0;
		for (auto rounds = 0u; rounds < 4; rounds++)
		{
			for (auto i = 0u; i < _size; i++)
			{
				if (keyPos >= encryptionKey.size())
				{
					keyPos = 0;
				}

				dataptr[i] = 0xFF - dataptr[i];
				dataptr[i] = dataptr[i] ^ encryptionKey[keyPos];
				dataptr[i] = dataptr[i] ^ rounds;
				keyPos++;
			}
		}
	}

	template <typename T>
	static void decrypt_data(T* _data, std::size_t _size)
	{
		auto fastfile = static_cast<std::string>(*reinterpret_cast<const char**>(0x1294A00) + 4);
		// FileSystem::GetFastFile();

		auto encryptionKey = static_cast<std::string>(fastfile + ": This fastfile is property of the Plutonium Project."
		);
		auto dataptr = reinterpret_cast<char*>(_data);

		auto keyPos = 0;
		for (int rounds = 3; rounds >= 0; rounds--)
		{
			for (auto i = 0u; i < _size; i++)
			{
				if (keyPos >= encryptionKey.size())
				{
					keyPos = 0;
				}

				dataptr[i] = dataptr[i] ^ rounds;
				dataptr[i] = dataptr[i] ^ encryptionKey[keyPos];
				dataptr[i] = 0xFF - dataptr[i];
				keyPos++;
			}
		}
	}

	struct XZoneKey
	{
		std::uint32_t version;
		std::uint8_t pad1[16];
		std::uint8_t key[32];
		std::uint8_t pad2[16];
		std::uint8_t iv[16];
	};

	class ZoneBuffer
	{
	protected:
		std::vector<std::uint8_t> m_buf;
		std::size_t m_pos;
		std::size_t m_len;

		std::uint8_t m_stream;

		std::int32_t m_shiftsize;
		std::size_t m_numstreams;

		std::vector<std::uintptr_t> m_zonestreams;
		std::stack<std::uint8_t> m_streamstack;

		std::vector<char*> m_scriptstrings;

	private:
		class Offset
		{
		public:
			union
			{
				struct
				{
					std::uint32_t value : 28;
					std::uint32_t stream : 4;
				};

				std::uint32_t packed;
			};

			Offset() : packed(0)
			{
			};

			Offset(const Offset& offset) : value(offset.value), stream(offset.stream)
			{
			};

			Offset(std::uint32_t _stream, std::uint32_t _value) : value(_value), stream(_stream)
			{
			};

			// The game needs it to be incremented
			std::uint32_t getPackedValue() { return this->packed + 1; };
		};

		void write_data(const void* _data, std::size_t size, std::size_t count);
		void write_data(const void* _data, std::size_t size);

		std::unordered_map<std::uintptr_t, std::uintptr_t> m_zonepointers;

	public:
		ZoneBuffer();
		~ZoneBuffer();

		ZoneBuffer(std::vector<std::uint8_t> data);
		ZoneBuffer(std::size_t size);

		std::uintptr_t get_stream_pos()
		{
			return this->m_zonestreams[this->m_stream];
		}

		template <typename T>
		T* get_zone_pointer()
		{
			return reinterpret_cast<T*>(((this->m_stream & 0x0F) << this->m_shiftsize) | ((m_zonestreams[m_stream] + 1)
				& 0x0FFFFFFF));
			// auto pointer = Offset(this->m_stream, this->m_zonestreams[this->m_stream]).getPackedValue();
			// return reinterpret_cast<T*>(pointer);
		}

		Offset get_zone_offset()
		{
			return Offset(this->m_stream, this->m_zonestreams[this->m_stream]);
		}

		template <typename T = char>
		T* at()
		{
			return reinterpret_cast<T*>(m_buf.data() + m_pos);
		}

		template <typename T>
		bool has_pointer(T* pointer)
		{
			if (m_zonepointers.find(reinterpret_cast<std::uintptr_t>(pointer)) != m_zonepointers.end())
			{
				return true;
			}

			return false;
		}

		template <typename T>
		T* get_pointer(T* pointer)
		{
			if (m_zonepointers.find(reinterpret_cast<std::uintptr_t>(pointer)) != m_zonepointers.end())
			{
				return reinterpret_cast<T*>(m_zonepointers[reinterpret_cast<std::uintptr_t>(pointer)]);
			}

			return nullptr;
		}

		template <typename T>
		void create_pointer(T* pointer)
		{
			m_zonepointers[reinterpret_cast<std::uintptr_t>(pointer)] = reinterpret_cast<std::uintptr_t>(this->
				get_zone_pointer<T>());
		}

		// T is return type, should be a pointer always.
		template <typename T>
		T* write_s(uint32_t alignment, T* _data, std::size_t _count = 1, std::size_t _size = sizeof(T),
		           T** outPointer = nullptr)
		{
			if (m_stream != 2)
			{
				// if data is a nullptr, return null.
				if (!_data)
				{
					return nullptr;
				}

				// if a zonepointer is found, do not rewrite the data.
				if (m_zonepointers.find((uintptr_t)_data) != m_zonepointers.end())
				{
					return reinterpret_cast<T*>(m_zonepointers[(uintptr_t)_data]);
				}
			}

			if (alignment > 0)
			{
				if (alignof(T) - 1 > alignment)
				{
					printf(
						"[WARNING]: You might have an alignment issue somewhere fella! AlignOf(T) is higher than the specified amount!\n");

					if (IsDebuggerPresent())
					{
						DebugBreak();
					}
				}

				if (alignment % 2 == 0)
				{
					printf(
						"[WARNING]: You might have an alignment issue somewhere fella! Alignment is an even number (%i)!\n",
						alignment);
				}
			}

			// align the buffer writing.
			this->align(alignment);

			if (outPointer)
			{
				*outPointer = this->at<T>();
			}

			if (m_stream != 2)
			{
				// insert zonepointers
				for (std::size_t i = 0; i < _count; i++)
				{
					auto ptr = (uintptr_t)(this->get_zone_pointer<std::uintptr_t>());
					m_zonepointers[(uintptr_t)(void*)&_data[i]] = ptr;

					// printf("Offset 0x%08X, index %03u, size %04u -> zonepointer 0x%08X\n", &_data[i], i, _size, ptr);

					this->write<T>(&_data[i]);
				}
			}

			// return -1 (data is following)
			return reinterpret_cast<T*>(-1);
		}

		template <typename T>
		T* write_p(T* _data, std::size_t _count = 1)
		{
			if (_count > 1)
			{
				for (std::size_t i = 0; i < _count; i++)
				{
					m_zonepointers[reinterpret_cast<std::uintptr_t>(_data + sizeof(T) * i)] = reinterpret_cast<std::
						uintptr_t>(this->get_zone_pointer<std::uintptr_t>() + (sizeof(T) * i));
				}
			}
			else
			{
				m_zonepointers[reinterpret_cast<std::uintptr_t>(_data)] = reinterpret_cast<std::uintptr_t>(this->
					get_zone_pointer<std::uintptr_t>());
			}

			return write<T>(_data, _count);
		}

		void init_streams(std::size_t num_streams);

		void write_stream(const void* _data, std::size_t size, std::size_t count);
		void write_stream(const void* _data, size_t size);

		char* write_str(const std::string& _str);
		void write_str_raw(const std::string& _str);

		template <typename T>
		T* write(T* _data, std::size_t count = 1)
		{
			auto dest = this->at<T>();
			write_stream(_data, sizeof(T), count);
			return dest;
		}

		template <typename T>
		T* write_encrypted(T* _data, std::size_t count = 1)
		{
			auto dest = this->at<T>();
			write_stream(_data, sizeof(T), count);
			encrypt_data(dest, sizeof(T) * count);
			return dest;
		}

		std::uint8_t* buffer();
		std::size_t size();

		void align(std::uint32_t alignment);
		void push_stream(std::uint32_t stream);
		void pop_stream();

		std::uint8_t current_stream();
		std::uint32_t current_stream_offset();
		std::uint32_t stream_offset(std::uint8_t stream);

		std::uint16_t write_scriptstring(std::string str);
		const char* get_scriptstring(std::size_t idx);
		std::size_t scriptstring_count();

		template <typename T>
		static void ClearPointer(T* ptr)
		{
			*reinterpret_cast<std::uintptr_t*>(ptr) = -1;
		}

		void save(const std::string& filename);

		std::vector<std::uint8_t> compress_zstd();
		std::vector<std::uint8_t> compress_zlib();
		void encrypt();
	};
}
