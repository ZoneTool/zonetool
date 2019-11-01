// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		IFxEffectDef::IFxEffectDef()
		{
		}

		IFxEffectDef::~IFxEffectDef()
		{
		}

		void IFxEffectDef::init(const std::string& name, ZoneMemory* mem)
		{
			this->m_name = name;
			this->m_asset = DB_FindXAssetHeader(this->type(), this->name().data()).fx;
		}

		void IFxEffectDef::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		FxEffectDef* IFxEffectDef::parse(const std::string& name, ZoneMemory* mem)
		{
			auto path = "fx\\" + name;
			if (!FileSystem::FileExists(path))
			{
				path = "fx\\" + name + ".fxe";
				FileSystem::PreferLocalOverExternal(true);

				if (!FileSystem::FileExists(name))
				{
					FileSystem::PreferLocalOverExternal(false);
					return nullptr;
				}

				AssetReader read(mem);
				if (!read.open(path))
				{
					FileSystem::PreferLocalOverExternal(false);
					return nullptr;
				}

				ZONETOOL_INFO("Parsing \"%s\"", name.data());

				auto fx = read.read_array<FxEffectDef>();
				fx->elemDefCountEmission = read.read_int();
				fx->elemDefCountLooping = read.read_int();
				fx->elemDefCountOneShot = read.read_int();
				fx->elemDefs = read.read_array<FxElemDef>();
				fx->flags = read.read_int();
				fx->msecLoopingLife = read.read_int();
				fx->name = read.read_string();
				fx->totalSize = read.read_int();
				read.close();

				FileSystem::PreferLocalOverExternal(false);

				return fx;
			}
		}

		void IFxEffectDef::load_depending(IZone* zone)
		{
			auto data = this->m_asset;

			auto load_FxElemVisuals = [zone](FxElemDef* def, FxElemDefVisuals* vis)
			{
				if (def->elemType == 11)
				{
					for (int i = 0; i < def->visualCount; i++)
					{
						if (vis->markArray[i])
						{
							if (vis->markArray[i][0])
								zone->add_asset_of_type(material, vis->markArray[i][0]->name);
							if (vis->markArray[i][1])
								zone->add_asset_of_type(material, vis->markArray[i][1]->name);
						}
					}
				}
				else if (def->visualCount > 1)
				{
					for (int i = 0; i < def->visualCount; i++)
					{
						if (def->elemType == 12)
							zone->add_asset_of_type(fx, vis->array[i].effectDef->name);
						else if (def->elemType == 10)
							zone->add_asset_of_type(sound, vis->array[i].soundName);
						else if (def->elemType == 7)
							zone->add_asset_of_type(xmodel, vis->array[i].xmodel->name);
						else
						{
							if (def->elemType != 8)
								zone->add_asset_of_type(material, vis->array[i].material->name);
						}
					}
				}
				else
				{
					if (def->elemType == 12)
						zone->add_asset_of_type(fx, vis->instance.effectDef->name);
					else if (def->elemType == 10)
						zone->add_asset_of_type(sound, vis->instance.soundName);
					else if (def->elemType == 7)
						zone->add_asset_of_type(xmodel, vis->instance.xmodel->name);
					else
					{
						if (def->elemType != 8)
							zone->add_asset_of_type(material, vis->instance.material->name);
					}
				}
			};

			// Loop through frames
			for (int i = 0; i < data->elemDefCountEmission + data->elemDefCountLooping + data->elemDefCountOneShot; i++)
			{
				FxElemDef& def = data->elemDefs[i];

				// Sub-FX effects
				if (def.effectEmitted)
					zone->add_asset_of_type(fx, def.effectEmitted->name);
				if (def.effectOnDeath)
					zone->add_asset_of_type(fx, def.effectOnDeath->name);
				if (def.effectOnImpact)
					zone->add_asset_of_type(fx, def.effectOnImpact->name);

				// Visuals
				load_FxElemVisuals(&def, &def.visuals);
			}
		}

		std::string IFxEffectDef::name()
		{
			return this->m_name;
		}

		std::int32_t IFxEffectDef::type()
		{
			return fx;
		}

		void IFxEffectDef::write_FxElemVisuals(IZone* zone, ZoneBuffer* buf, FxElemDef* def,
		                                       FxElemVisuals* dest)
		{
			auto data = dest;

			switch (def->elemType)
			{
			case FX_ELEM_TYPE_RUNNER:
				{
					buf->write_str(data->effectDef->name);
					ZoneBuffer::ClearPointer(&dest->effectDef);
					break;
				}
			case FX_ELEM_TYPE_SOUND:
				{
					if (data->soundName)
					{
						buf->write_str(data->soundName);
						ZoneBuffer::ClearPointer(&dest->soundName);
					}
					break;
				}
			case FX_ELEM_TYPE_SPOT_LIGHT:
				{
					dest->anonymous = (data->anonymous)
						                  ? zone->get_asset_pointer(lightdef, ((GfxLightDef*)data->anonymous)->name)
						                  : nullptr;
					break;
				}
			case FX_ELEM_TYPE_MODEL:
				{
					dest->xmodel = (data->xmodel)
						               ? reinterpret_cast<XModel*>(zone->get_asset_pointer(xmodel, data->xmodel->name))
						               : nullptr;
					break;
				}
			default:
				{
					if (def->elemType != FX_ELEM_TYPE_OMNI_LIGHT)
					{
						dest->material = (data->material)
							                 ? reinterpret_cast<Material*>(zone->get_asset_pointer(
								                 material, data->material->name))
							                 : nullptr;
					}
				}
			}
		}

		void IFxEffectDef::write_FxElemDefVisuals(IZone* zone, ZoneBuffer* buf, FxElemDef* def,
		                                          FxElemDefVisuals* dest)
		{
			auto data = dest;

			if (def->elemType == FX_ELEM_TYPE_DECAL)
			{
				if (data->markArray)
				{
					auto destvisuals = buf->write(data->markArray, def->visualCount);

					for (int i = 0; i < def->visualCount; i++)
					{
						destvisuals[i][0] = (data->markArray[i][0])
							                    ? reinterpret_cast<Material*>(zone->get_asset_pointer(
								                    material, data->markArray[i][0]->name))
							                    : nullptr;
						destvisuals[i][1] = (data->markArray[i][1])
							                    ? reinterpret_cast<Material*>(zone->get_asset_pointer(
								                    material, data->markArray[i][1]->name))
							                    : nullptr;
					}
				}
			}
			else if (def->visualCount > 1)
			{
				auto vis = buf->write(data->array, def->visualCount);

				for (int i = 0; i < def->visualCount; i++)
				{
					write_FxElemVisuals(zone, buf, def, &vis[i]);
				}
			}
			else
			{
				write_FxElemVisuals(zone, buf, def, &dest->instance);
			}
		}

		void IFxEffectDef::write_FxElemDef(IZone* zone, ZoneBuffer* buf, FxElemDef* dest)
		{
			auto data = dest;

			if (data->velSamples)
			{
				buf->align(3);
				buf->write(data->velSamples, data->velIntervalCount + 1);
				ZoneBuffer::ClearPointer(&dest->velSamples);
			}

			if (data->visSamples)
			{
				buf->align(3);
				buf->write(data->visSamples, data->visStateIntervalCount + 1);
				ZoneBuffer::ClearPointer(&dest->visSamples);
			}

			write_FxElemDefVisuals(zone, buf, data, &dest->visuals);

			if (data->effectOnImpact)
			{
				buf->write_str_raw(data->effectOnImpact->name);
				ZoneBuffer::ClearPointer(&dest->effectOnImpact);
			}

			if (data->effectOnDeath)
			{
				buf->write_str_raw(data->effectOnDeath->name);
				ZoneBuffer::ClearPointer(&dest->effectOnDeath);
			}

			if (data->effectEmitted)
			{
				buf->write_str_raw(data->effectEmitted->name);
				ZoneBuffer::ClearPointer(&dest->effectEmitted);
			}

			if (data->extended.trailDef)
			{
				if (data->elemType == FX_ELEM_TYPE_TRAIL)
				{
					if (data->extended.trailDef)
					{
						buf->align(3);
						buf->write(data->extended.trailDef, sizeof(FxTrailDef));

						if (data->extended.trailDef->verts)
						{
							buf->align(3);
							buf->write(data->extended.trailDef->verts, data->extended.trailDef->vertCount);
						}

						if (data->extended.trailDef->inds)
						{
							buf->align(1);
							buf->write(data->extended.trailDef->inds, data->extended.trailDef->indCount);
						}

						ZoneBuffer::ClearPointer(&dest->extended.trailDef);
					}
				}
				else if (data->elemType == FX_ELEM_TYPE_SPARKFOUNTAIN)
				{
					if (data->extended.sparkFountain)
					{
						buf->align(3);
						buf->write(data->extended.sparkFountain);
						ZoneBuffer::ClearPointer(&dest->extended.sparkFountain);
					}
				}
				else if (data->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
				{
					if (data->extended.unknownDef)
					{
						buf->align(3);
						buf->write_stream(data->extended.unknownDef, 24);
						ZoneBuffer::ClearPointer(&dest->extended.unknownDef);
					}
				}
				else
				{
					if (data->extended.unknownDef)
					{
						buf->align(1);
						buf->write_stream(data->extended.unknownDef, sizeof(BYTE));
						ZoneBuffer::ClearPointer(&dest->extended.unknownDef);
					}
				}
			}
		}

		void IFxEffectDef::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->m_asset;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->elemDefs)
			{
				buf->align(3);
				auto destdef = buf->write(data->elemDefs,
				                          data->elemDefCountEmission + data->elemDefCountLooping + data->
				                          elemDefCountOneShot);

				for (std::int32_t i = 0; i < (data->elemDefCountEmission + data->elemDefCountLooping + data->
					     elemDefCountOneShot); i++)
				{
					write_FxElemDef(zone, buf, &destdef[i]);
				}

				ZoneBuffer::ClearPointer(&dest->elemDefs);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IFxEffectDef::dump(FxEffectDef* asset)
		{
			AssetDumper dump;

			if (!dump.open("fx\\"s + asset->name + ".fxe"))
			{
				return;
			}

			dump.dump_array(asset, 1);
			dump.dump_string(asset->name);
			dump.dump_array(asset->elemDefs,
			           asset->elemDefCountEmission + asset->elemDefCountLooping + asset->elemDefCountOneShot);

			// dump elemDefs
			for (int i = 0; i < asset->elemDefCountEmission + asset->elemDefCountLooping + asset->elemDefCountOneShot; i
			     ++)
			{
				auto def = &asset->elemDefs[i];

				// dump elem samples
				dump.dump_array(def->velSamples, def->velIntervalCount + 1);
				dump.dump_array(def->visSamples, def->visStateIntervalCount + 1);

				// todo: dump visuals!

				// dump reference FX defs
				dump.dump_asset(def->effectOnImpact);
				dump.dump_asset(def->effectOnDeath);
				dump.dump_asset(def->effectEmitted);

				// dump extended FX data
				if (def->extended.trailDef)
				{
					if (def->elemType == FX_ELEM_TYPE_TRAIL)
					{
						dump.dump_array(def->extended.trailDef, 1);

						if (def->extended.trailDef->verts)
						{
							dump.dump_array(def->extended.trailDef->verts, def->extended.trailDef->vertCount);
						}

						if (def->extended.trailDef->inds)
						{
							dump.dump_array(def->extended.trailDef->inds, def->extended.trailDef->indCount);
						}
					}
					else if (def->elemType == FX_ELEM_TYPE_SPARKFOUNTAIN)
					{
						dump.dump_array(def->extended.sparkFountain, 1);
					}
					else if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
					{
						dump.dump_array(def->extended.unknownDef, 24);
					}
					else
					{
						dump.dump_array(def->extended.unknownDef, 1);
					}
				}
			}

			dump.close();
		}
	}
}
