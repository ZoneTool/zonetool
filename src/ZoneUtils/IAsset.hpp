#pragma once

namespace ZoneTool
{
	class IAsset
	{
	public:
		virtual void init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
		}

		virtual void init(void* asset, std::shared_ptr<ZoneMemory>& mem)
		{
		}

		virtual void prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
		{
		}

		virtual std::string name() { return ""; }
		virtual std::int32_t type() { return -1; }
		virtual void* pointer() { return nullptr; }

		virtual void write(IZone* zone, std::shared_ptr<ZoneBuffer>& buffer)
		{
		}

		virtual void load_depending(IZone* zone)
		{
		}
	};
}
