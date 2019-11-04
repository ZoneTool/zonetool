// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"

namespace ZoneTool::CODO
{
	// LEGACY ZONETOOL CODE, FIX ME!
	class CSV
	{
	protected:
		std::string _name;
		std::vector<std::vector<std::string>> _data;

	public:
		CSV(std::string name, char sep = ',')
			: _name(name)
		{
			auto fp = FileSystem::FileOpen(name, "r"s);

			if (fp)
			{
				long len = FileSystem::FileSize(fp);
				auto buf = std::make_unique<char[]>(len + 1);
				memset(buf.get(), 0, len + 1);
				fread(buf.get(), len, 1, fp);
				fclose(fp);

				std::vector<std::string> rows = split(std::string(buf.get()), '\n');

				for (auto& row : rows)
				{
					// Replace literal characters
					std::size_t pos;
					while ((pos = row.find("\\n")) != std::string::npos)
					{
						row.replace(pos, 2, "\n");
					}

					while ((pos = row.find("\\t")) != std::string::npos)
					{
						row.replace(pos, 2, "\t");
					}

					_data.push_back(split(row, sep));
				}
			}
		}

		std::string entry(std::size_t row, std::size_t column)
		{
			return _data[row][column];
		}

		std::size_t rows()
		{
			return _data.size();
		}

		std::size_t columns(std::size_t row)
		{
			return _data[row].size();
		}

		std::size_t max_columns()
		{
			std::size_t _max = 0;

			for (std::size_t row = 0; row < this->rows(); row++)
			{
				if (_max < this->columns(row))
					_max = this->columns(row);
			}

			return _max;
		}

		void clear()
		{
			for (std::size_t i = 0; i < _data.size(); i++)
			{
				for (std::size_t j = 0; j < _data[i].size(); j++)
					_data[i][j].clear();

				_data[i].clear();
			}

			_data.clear();
		}
	};

	int StringTable_Hash(const char* string)
	{
		int hash = 0;
		char* data = _strdup(string);

		while (*data != 0)
		{
			hash = tolower(*data) + (31 * hash);
			data++;
		}

		return hash;
	}

	StringTable* StringTable_Parse(std::string name, ZoneMemory* mem)
	{
		auto table = std::make_unique<CSV>(name);
		auto stringtable = mem->Alloc<StringTable>();

		stringtable->name = _strdup(name.c_str());
		stringtable->rows = table->rows();
		stringtable->columns = table->max_columns();
		stringtable->strings = mem->Alloc<StringTableCell>(stringtable->rows * stringtable->columns);

		for (int row = 0; row < table->rows(); row++)
		{
			for (int col = 0; col < table->columns(row); col++)
			{
				int entry = (row * stringtable->columns) + col;
				stringtable->strings[entry].string = strdup(table->entry(row, col).c_str());
				stringtable->strings[entry].hash = StringTable_Hash(stringtable->strings[entry].string);
			}
		}

		return stringtable;
	}

	IStringTable::IStringTable()
	{
	}

	IStringTable::~IStringTable()
	{
	}

	void IStringTable::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = name;
		this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data()).stringtable;

		if (FileSystem::FileExists(name))
		{
			ZONETOOL_INFO("Parsing stringtable %s...", name.data());
			this->asset_ = StringTable_Parse(name, mem);
		}
	}

	void IStringTable::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void IStringTable::load_depending(IZone* zone)
	{
	}

	std::string IStringTable::name()
	{
		return this->name_;
	}

	std::int32_t IStringTable::type()
	{
		return stringtable;
	}

	void IStringTable::write(IZone* zone, ZoneBuffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);
		START_LOG_STREAM;

		// TODO

		StringTable;

		dest->name = buf->write_str(this->name());

		if (data->strings)
		{
			buf->align(3);
			auto destStrings = buf->write(data->strings, data->columns * data->rows);

			if (data->columns * data->rows > 0)
			{
				for (int i = 0; i < data->columns * data->rows; i++)
				{
					destStrings[i].string = buf->write_str(data->strings[i].string);
				}
			}

			ZoneBuffer::ClearPointer(&dest->strings);
		}

		END_LOG_STREAM;
		buf->pop_stream();
	}

	void IStringTable::dump(StringTable* asset)
	{
		std::string path = asset->name;

		auto file = FileSystem::FileOpen(path, "w"s);

		for (int row = 0; row < asset->rows; row++)
		{
			for (int column = 0; column < asset->columns; column++)
			{
				fprintf(
					file,
					"%s%s",
					(asset->strings[(row * asset->columns) + column].string)
						? asset->strings[(row * asset->columns) + column].string
						: "",
					(column == asset->columns - 1) ? "\n" : ","
				);
			}
		}

		FileSystem::FileClose(file);
	}
}
