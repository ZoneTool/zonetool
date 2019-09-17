#include "stdafx.hpp"
#include "../iw5/Assets/ComWorld.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		ComWorld* IComWorld::parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			auto iw5_comworld = IW5::IComWorld::parse(name, mem);

			if (!iw5_comworld)
			{
				return nullptr;
			}

			// fixup data
			auto lightArray = mem->Alloc<ComPrimaryLight>(iw5_comworld->primaryLightCount);

			// convert structure
			for (int i = 0; i < iw5_comworld->primaryLightCount; i++)
			{
				memcpy(lightArray[i]._portpad0, iw5_comworld->primaryLights[i]._portpad0, 28);
				memcpy(lightArray[i]._portpad1, iw5_comworld->primaryLights[i]._portpad1, 40);
			}

			// set pointer
			iw5_comworld->primaryLights = (IW5::ComPrimaryLight*)lightArray;

			// asset is the exact same so just cast to the correct type here
			return (ComWorld*)iw5_comworld;
		}

		IComWorld::IComWorld()
		{
		}

		IComWorld::~IComWorld()
		{
		}

		void IComWorld::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			this->m_name = "maps/mp/" + currentzone + ".d3dbsp"; // name;
			this->m_asset = this->parse(name, mem);

			if (!this->m_asset)
			{
				this->m_asset = DB_FindXAssetHeader(this->type(), name.data()).comworld;
			}
		}

		void IComWorld::prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
		{
		}

		void IComWorld::load_depending(IZone* zone)
		{
			auto asset = this->m_asset;

			for (int i = 0; i < asset->primaryLightCount; i++)
			{
				if (asset->primaryLights[i].defName)
				{
					zone->AddAssetOfType(lightdef, asset->primaryLights[i].defName);
				}
			}
		}

		std::string IComWorld::name()
		{
			return this->m_name;
		}

		std::int32_t IComWorld::type()
		{
			return com_map;
		}

		void IComWorld::write(IZone* zone, std::shared_ptr<ZoneBuffer>& buf)
		{
			auto data = this->m_asset;
			auto dest = buf->write(data);

			buf->push_stream(3);

			dest->name = buf->write_str(this->name());

			if (data->primaryLights)
			{
				buf->align(3);
				auto destlight = buf->write(data->primaryLights, data->primaryLightCount);

				for (std::uint32_t i = 0; i < data->primaryLightCount; i++)
				{
					if (data->primaryLights[i].defName)
					{
						destlight[i].defName = buf->write_str(data->primaryLights[i].defName);
					}
				}
			}

			buf->pop_stream();
		}

		void IComWorld::dump(ComWorld* asset)
		{
			// alloc comworld
			auto iw5_comworld = new IW5::ComWorld;
			memcpy(iw5_comworld, asset, sizeof ComWorld);

			// alloc lights
			iw5_comworld->primaryLights = new IW5::ComPrimaryLight[iw5_comworld->primaryLightCount];
			memset(iw5_comworld->primaryLights, 0, sizeof(IW5::ComPrimaryLight) * iw5_comworld->primaryLightCount);

			// copy data
			for (unsigned int i = 0; i < iw5_comworld->primaryLightCount; i++)
			{
				memcpy(iw5_comworld->primaryLights[i]._portpad0, asset->primaryLights[i]._portpad0, 28);
				memcpy(iw5_comworld->primaryLights[i]._portpad1, asset->primaryLights[i]._portpad1, 40);
			}

			// dump comworld
			IW5::IComWorld::dump(iw5_comworld);

			// free memory
			delete[] iw5_comworld->primaryLights;
			delete[] iw5_comworld;
		}
	}
}
