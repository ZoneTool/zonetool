#pragma once

#include <initializer_list>
#include <functional>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <memory>
#include <vector>
#include <mutex>
#include <map>

#include "Json.hpp"
using Json = nlohmann::json;

#undef xor
#undef and

#define MAX_ZONE_SIZE 1024 * 1024 * 200

namespace ZoneTool
{
#pragma push(pack, 1)
	struct XFileHeader
	{
		char header[8];
		std::int32_t version;
		std::uint8_t allowOnlineUpdate;
		std::uint32_t dwHighDateTime;
		std::uint32_t dwLowDateTime;
	};

	template <std::size_t num_streams>
	struct XZoneMemory
	{
		std::uint32_t size;
		std::uint32_t externalsize;
		std::uint32_t streams[num_streams];
	};

	struct XZoneInfo
	{
		const char* zone;
		std::int32_t loadFlags;
		std::int32_t unloadFlags;
	};
#pragma push(pop)
}

#include "IPatch.hpp"
#include "CSV.hpp"
#include "Zone/ZoneMemory.hpp"
#include "Zone/ZoneBuffer.hpp"
#include "Zone/Zone.hpp"
#include "IAsset.hpp"
#include "Utils/FileReader.hpp"
#include "Utils/FileSystem.hpp"
#include "Utils/Function.hpp"
#include "Utils/Memory.hpp"
#include "Utils/BinaryDumper.hpp"
#include "Utils/Expressions.hpp"
#include "Linker.hpp"

#define MAKE_STRING(__data__) #__data__

#define ASSET_SIZE(__size__) \
	void _assert_size() \
	{ \
		static_assert(sizeof(*this) == __size__, __FUNCTION__": Invalid struct size.\n"); \
	}

#define ZONETOOL_INFO(__FMT__,...) \
	printf("[ INFO ][ " __FUNCTION__ " ]: " __FMT__ "\n", __VA_ARGS__)

#define ZONETOOL_ERROR(__FMT__,...) \
	printf("[ ERROR ][ " __FUNCTION__ " ]: " __FMT__ "\n", __VA_ARGS__)

#define ZONETOOL_FATAL(__FMT__,...) \
	printf("[ FATAL ][ " __FUNCTION__ " ]: " __FMT__ "\n", __VA_ARGS__); \
	MessageBoxA(nullptr, &va("Oops! An unexpected error occured. Error was: " __FMT__ "\n\nZoneTool must be restarted to resolve the error. Last error code reported by windows: 0x%08X (%u)", __VA_ARGS__, GetLastError(), GetLastError())[0], nullptr, 0); \
	std::exit(0)

#define ZONETOOL_WARNING(__FMT__,...) \
	printf("[ WARNING ][ " __FUNCTION__ " ]: " __FMT__ "\n", __VA_ARGS__)

/*
 *	Debugging purposes
 */

// #define FILEPOINTERS_DEBUG
#ifdef FILEPOINTERS_DEBUG
#define START_LOG_STREAM \
		auto streamStartPos = buf->get_stream_pos();

#define END_LOG_STREAM \
		streamStartPos = buf->get_stream_pos() - streamStartPos; \
		ZONETOOL_INFO("Streamsize consumed for asset is %u", streamStartPos);
#else
#define START_LOG_STREAM
#define END_LOG_STREAM
#endif

template <typename T1, typename T2>
std::size_t Difference(const T1& t1, const T2& t2)
{
	return std::uintptr_t(t1) - std::uintptr_t(t2);
}

template <typename ... Args>
std::string va(const std::string& format, Args ... args)
{
	size_t size = _snprintf(nullptr, 0, format.c_str(), args ...) + 1;
	std::vector<char> buf;
	buf.resize(size);
	_snprintf(buf.data(), size, format.c_str(), args ...);
	return std::string(buf.data(), buf.data() + size - 1);
}

static std::vector<std::string> split(const std::string& rawInput, const std::vector<char>& delims)
{
	std::vector<std::string> strings;

	auto findFirstDelim = [](const std::string& input, const std::vector<char>& delims) -> std::pair<char, std::size_t>
	{
		auto firstDelim = 0;
		auto firstDelimIndex = static_cast<std::size_t>(-1);
		auto index = 0u;

		for (auto& delim : delims)
		{
			if ((index = input.find(delim)) != std::string::npos)
			{
				if (firstDelimIndex == -1 || index < firstDelimIndex)
				{
					firstDelim = delim;
					firstDelimIndex = index;
				}
			}
		}

		return {firstDelim, firstDelimIndex};
	};

	std::string input = rawInput;

	while (!input.empty())
	{
		auto splitDelim = findFirstDelim(input, delims);
		if (splitDelim.first != 0)
		{
			strings.push_back(input.substr(0, splitDelim.second));
			input = input.substr(splitDelim.second + 1);
		}
		else
		{
			break;
		}
	}

	strings.push_back(input);
	return strings;
}

static std::vector<std::string> split(std::string& str, char delimiter)
{
	return split(str, std::vector<char>({delimiter}));
}

template <typename T>
static std::shared_ptr<T> RegisterPatch()
{
	return std::make_shared<T>();
}

class CSV
{
public:
	CSV(const CSV& csv) : columns(csv.columns)
	{
	}

	CSV(const std::string& table)
	{
		std::ifstream iFile(table);

		if (iFile.is_open())
		{
			while (!iFile.eof())
			{
				std::string row;
				iFile >> row;

				auto cols = split(row, ',');
				columns.push_back(cols);
			}

			iFile.close();
		}
	}

	std::string Get(const std::uint32_t& row, const std::uint32_t& column)
	{
		// if (columns.empty()) return "";
		// if (columns.size() <= row) return "";
		// if (columns[column].size() <= column) return "";

		return columns[row][column];
	}

	std::uint32_t Rows()
	{
		return columns.size();
	}

	std::uint32_t Columns(std::uint32_t row = 0)
	{
		if (columns.empty()) return 0;
		return columns[row].size();
	}

private:
	std::vector<std::vector<std::string>> columns;
};
