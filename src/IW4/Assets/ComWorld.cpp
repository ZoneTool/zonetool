// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "../iw5/Assets/ComWorld.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		ComWorld* IComWorld::parse(const std::string& name, ZoneMemory* mem)
		{
			auto* iw5_comworld = IW5::IComWorld::parse(name, mem);

			if (!iw5_comworld)
			{
				return nullptr;
			}

			// fixup data
			auto* light_array = mem->Alloc<ComPrimaryLight>(iw5_comworld->primaryLightCount);

			// convert structure
			for (auto i = 0u; i < iw5_comworld->primaryLightCount; i++)
			{
				memcpy(light_array[i]._portpad0, iw5_comworld->primaryLights[i]._portpad0, 28);
				memcpy(light_array[i]._portpad1, iw5_comworld->primaryLights[i]._portpad1, 40);
			}

			// set pointer
			iw5_comworld->primaryLights = (IW5::ComPrimaryLight*)light_array;

			// asset is the exact same so just cast to the correct type here
			return (ComWorld*)iw5_comworld;
		}

		void IComWorld::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = "maps/"s + (currentzone.substr(0, 3) == "mp_" ? "mp/" : "") + currentzone + ".d3dbsp"; // name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), name.data()).comworld;
			}
		}

		void IComWorld::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IComWorld::load_depending(IZone* zone)
		{
			auto* asset = this->asset_;

			for (auto i = 0u; i < asset->primaryLightCount; i++)
			{
				if (asset->primaryLights[i].defName)
				{
					zone->add_asset_of_type(lightdef, asset->primaryLights[i].defName);
				}
			}
		}

		std::string IComWorld::name()
		{
			return this->name_;
		}

		std::int32_t IComWorld::type()
		{
			return com_map;
		}

		void IComWorld::write(IZone* zone, ZoneBuffer* buf)
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(3);
			
			dest->name = buf->write_str(this->name());

			if (data->primaryLights)
			{
				buf->align(3);
				auto* primary_light = buf->write(data->primaryLights, data->primaryLightCount);
				
				for (auto i = 0u; i < data->primaryLightCount; i++)
				{
					if (data->primaryLights[i].defName)
					{
						primary_light[i].defName = buf->write_str(data->primaryLights[i].defName);
					}
				}
			}

			buf->pop_stream();
		}

		void IComWorld::dump(ComWorld* asset)
		{
			// alloc comworld
			auto* iw5_comworld = new IW5::ComWorld;
			memcpy(iw5_comworld, asset, sizeof ComWorld);

			// alloc lights
			iw5_comworld->primaryLights = new IW5::ComPrimaryLight[iw5_comworld->primaryLightCount];
			memset(iw5_comworld->primaryLights, 0, sizeof(IW5::ComPrimaryLight) * iw5_comworld->primaryLightCount);

			// copy data
			for (auto i = 0u; i < iw5_comworld->primaryLightCount; i++)
			{
				memcpy(iw5_comworld->primaryLights[i]._portpad0, asset->primaryLights[i]._portpad0, 28);
				memcpy(iw5_comworld->primaryLights[i]._portpad1, asset->primaryLights[i]._portpad1, 40);

				iw5_comworld->primaryLights[i].up[0] = 0.0f;
				iw5_comworld->primaryLights[i].up[1] = 0.0f;
				iw5_comworld->primaryLights[i].up[2] = 0.0f;
			}

			// dump comworld
			IW5::IComWorld::dump(iw5_comworld);

			// free memory_
			delete[] iw5_comworld->primaryLights;
			delete iw5_comworld;
		}
	}
}
