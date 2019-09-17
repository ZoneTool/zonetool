#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW5
	{
		IFxEffectDef::IFxEffectDef()
		{
		}

		IFxEffectDef::~IFxEffectDef()
		{
		}

		void IFxEffectDef::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			this->m_name = name;
			this->m_asset = DB_FindXAssetHeader(this->type(), this->name().data(), 1).fx;
		}

		void IFxEffectDef::prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
		{
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
								zone->AddAssetOfType(material, vis->markArray[i][0]->name);
							if (vis->markArray[i][1])
								zone->AddAssetOfType(material, vis->markArray[i][1]->name);
						}
					}
				}
				else if (def->visualCount > 1)
				{
					for (int i = 0; i < def->visualCount; i++)
					{
						if (def->elemType == 12)
							zone->AddAssetOfType(fx, vis->array[i].effectDef->name);
						else if (def->elemType == 10)
							zone->AddAssetOfType(sound, vis->array[i].soundName);
						else if (def->elemType == 7)
							zone->AddAssetOfType(xmodel, vis->array[i].xmodel->name);
						else
						{
							if (def->elemType != 8)
								zone->AddAssetOfType(material, vis->array[i].material->name);
						}
					}
				}
				else
				{
					if (def->elemType == 12)
						zone->AddAssetOfType(fx, vis->instance.effectDef->name);
					else if (def->elemType == 10)
						zone->AddAssetOfType(sound, vis->instance.soundName);
					else if (def->elemType == 7)
						zone->AddAssetOfType(xmodel, vis->instance.xmodel->name);
					else
					{
						if (def->elemType != 8)
							zone->AddAssetOfType(material, vis->instance.material->name);
					}
				}
			};

			// Loop through frames
			for (int i = 0; i < data->elemDefCountEmission + data->elemDefCountLooping + data->elemDefCountOneShot; i++)
			{
				FxElemDef& def = data->elemDefs[i];

				// Sub-FX effects
				if (def.effectEmitted)
					zone->AddAssetOfType(fx, def.effectEmitted->name);
				if (def.effectOnDeath)
					zone->AddAssetOfType(fx, def.effectOnDeath->name);
				if (def.effectOnImpact)
					zone->AddAssetOfType(fx, def.effectOnImpact->name);

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

		void IFxEffectDef::write_FxElemVisuals(IZone* zone, std::shared_ptr<ZoneBuffer>& buf, FxElemDef* def,
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
						                  ? zone->GetAssetPointer(lightdef, ((GfxLightDef*)data->anonymous)->name)
						                  : nullptr;
					break;
				}
			case FX_ELEM_TYPE_MODEL:
				{
					dest->xmodel = (data->xmodel)
						               ? reinterpret_cast<XModel*>(zone->GetAssetPointer(xmodel, data->xmodel->name))
						               : nullptr;
					break;
				}
			default:
				{
					if (def->elemType != FX_ELEM_TYPE_OMNI_LIGHT)
					{
						dest->material = (data->material)
							                 ? reinterpret_cast<Material*>(zone->GetAssetPointer(
								                 material, data->material->name))
							                 : nullptr;
					}
				}
			}
		}

		void IFxEffectDef::write_FxElemDefVisuals(IZone* zone, std::shared_ptr<ZoneBuffer>& buf, FxElemDef* def,
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
							                    ? reinterpret_cast<Material*>(zone->GetAssetPointer(
								                    material, data->markArray[i][0]->name))
							                    : nullptr;
						destvisuals[i][1] = (data->markArray[i][1])
							                    ? reinterpret_cast<Material*>(zone->GetAssetPointer(
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

		void IFxEffectDef::write_FxElemDef(IZone* zone, std::shared_ptr<ZoneBuffer>& buf, FxElemDef* dest)
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

		void IFxEffectDef::write(IZone* zone, std::shared_ptr<ZoneBuffer>& buf)
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

		std::vector<std::pair<FxElemDefFlags, std::string>> flagMap =
		{
			//{ FX_ED_FLAG_LOOPING, "looping" },
			//{ FX_ED_FLAG_USE_RANDOM_COLOR, "useRandColor" },
			//{ FX_ED_FLAG_USE_RANDOM_ALPHA, "useRandAlpha" },
			//{ FX_ED_FLAG_USE_RANDOM_SIZE_0, "useRandSize0" },
			//{ FX_ED_FLAG_USE_RANDOM_SIZE_1, "useRandSize1" },
			//{ FX_ED_FLAG_USE_RANDOM_SCALE, "useRandScale" },
			//{ FX_ED_FLAG_USE_RANDOM_ROTATION_DELTA, "useRandRotDelta" },
			//{ FX_ED_FLAG_MODULATE_COLOR_BY_ALPHA, "modColorByAlpha" },
			//{ FX_ED_FLAG_USE_RANDOM_VELOCITY_0, "useRandVel0" },
			//{ FX_ED_FLAG_USE_RANDOM_VELOCITY_1, "useRandVel1" },
			//{ FX_ED_FLAG_BACKCOMPAT_VELOCITY, "useBackCompatVel" },
			//{ FX_ED_FLAG_ABSOLUTE_VELOCITY_0, "absVel0" },
			//{ FX_ED_FLAG_ABSOLUTE_VELOCITY_1, "absVel1" },

			// { FX_ED_FLAG_PLAY_ON_TOUCH, "useBackCompatVel" },
			// { FX_ED_FLAG_BACKCOMPAT_VELOCITY, "useBackCompatVel" },
		};

		std::string GenerateFlagsString(int flags)
		{
			std::string flagString = "";

			for (auto i = 0u; i < flagMap.size(); i++)
			{
				if ((flags & flagMap[i].first) == flagMap[i].first)
				{
					flagString += flagMap[i].second + " "s;
				}
			}

			if (!flagString.empty())
			{
				flagString = flagString.substr(0, flagString.length() - 1);
			}

			return flagString;
		}

		void IFxEffectDef::dumpToLegacyFormat(FxEffectDef* asset)
		{
			auto fp = FileSystem::FileOpen("fx\\"s + asset->name + ".fx_raw"s, "wb");
			if (!fp)
			{
				return;
			}

#define PRINTFIELDINTERNAL(__IDENT__, __FIELD__, __DATA__) \
	for (int i = 0; i < __IDENT__; i++) { fprintf(fp, "\t"); } \
	fprintf(fp, __FIELD__ " "); \
	fprintf(fp, __DATA__); \
	fprintf(fp, ";\n")

#define PRINTSTRING(__IDENT__, __FIELD__, __VALUE__) \
	PRINTFIELDINTERNAL(__IDENT__, __FIELD__, &va("\"%s\"", __VALUE__)[0])

#define PRINTFLOATRANGE(__IDENT__, __FIELD__, __VALUE__) \
	PRINTFIELDINTERNAL(__IDENT__, __FIELD__, &va("%g %g", __VALUE__.base, __VALUE__.amplitude)[0])

#define PRINTFLOAT(__IDENT__, __FIELD__, __VALUE__) \
	PRINTFIELDINTERNAL(__IDENT__, __FIELD__, &va("%g", __VALUE__)[0])

#define PRINTINTRANGE(__IDENT__, __FIELD__, __VALUE__) \
	PRINTFIELDINTERNAL(__IDENT__, __FIELD__, &va("%i %i", __VALUE__.base, __VALUE__.amplitude)[0])

#define PRINTINT(__IDENT__, __FIELD__, __VALUE__) \
	PRINTFIELDINTERNAL(__IDENT__, __FIELD__, &va("%i", __VALUE__)[0])

			fprintf(fp, "iwfx 2\n\n");
			for (int i = 0; i < asset->elemDefCountEmission + asset->elemDefCountLooping + asset->elemDefCountOneShot; i
			     ++)
			{
				auto elem = &asset->elemDefs[i];

				fprintf(fp, "{\n");

				PRINTSTRING(1, "name", &va("elem%i", i)[0]);
				/*if (i > asset->elemDefCountEmission && i < asset->elemDefCountEmission + asset->elemDefCountLooping)
				{
					fprintf(fp, "\teditorFlags looping;\n");
				}*/
				PRINTSTRING(1, "flags", &GenerateFlagsString(elem->flags)[0]);
				PRINTFLOATRANGE(1, "spawnRange", elem->spawnRange);
				PRINTFLOATRANGE(1, "fadeInRange", elem->fadeInRange);
				PRINTFLOATRANGE(1, "fadeOutRange", elem->fadeOutRange);
				PRINTFLOAT(1, "spawnFrustumCullRadius", elem->spawnFrustumCullRadius);
				PRINTFIELDINTERNAL(1, "spawnLooping", &va("%i %i", elem->spawn.looping.intervalMsec, elem->spawn.looping
.count)[0]);
				PRINTFIELDINTERNAL(1, "spawnOneShot", &va("%i %i", elem->spawn.oneShot.count.base, elem->spawn.oneShot.
count.amplitude)[0]);
				PRINTINTRANGE(1, "spawnDelayMsec", elem->spawnDelayMsec);
				PRINTINTRANGE(1, "lifeSpanMsec", elem->lifeSpanMsec);
				PRINTFLOATRANGE(1, "spawnOrgX", elem->spawnOrigin[0]);
				PRINTFLOATRANGE(1, "spawnOrgY", elem->spawnOrigin[1]);
				PRINTFLOATRANGE(1, "spawnOrgZ", elem->spawnOrigin[2]);
				PRINTFLOATRANGE(1, "spawnOffsetRadius", elem->spawnOffsetRadius);
				PRINTFLOATRANGE(1, "spawnOffsetHeight", elem->spawnOffsetHeight);
				PRINTFLOATRANGE(1, "spawnAnglePitch", elem->spawnAngles[0]);
				PRINTFLOATRANGE(1, "spawnAngleYaw", elem->spawnAngles[1]);
				PRINTFLOATRANGE(1, "spawnAngleRoll", elem->spawnAngles[2]);
				PRINTFLOATRANGE(1, "angleVelPitch", elem->angularVelocity[0]);
				PRINTFLOATRANGE(1, "angleVelYaw", elem->angularVelocity[1]);
				PRINTFLOATRANGE(1, "angleVelRoll", elem->angularVelocity[2]);
				PRINTFLOATRANGE(1, "initialRot", elem->initialRotation);
				PRINTFLOATRANGE(1, "gravity", elem->gravity);
				PRINTFLOATRANGE(1, "elasticity", elem->reflectionFactor); // NOT SURE

				for (int g = 0; g < std::min(elem->velIntervalCount + 1, 2); g++)
				{
#define DUMPVELGRAPH(__INDEX__, __CHARACTER__) \
					fprintf(fp, "\tvelGraph%i" __CHARACTER__ " 0\n", g); \
					fprintf(fp, "\t{\n"); \
					fprintf(fp, "\t\t{\n"); \
					fprintf(fp, "\t\t\t0 %g\n", elem->velSamples[g].local.velocity.base[__INDEX__]); \
					fprintf(fp, "\t\t\t1 %g\n", elem->velSamples[g].local.velocity.amplitude[__INDEX__]); \
					fprintf(fp, "\t\t}\n"); \
					fprintf(fp, "\t\t{\n"); \
					fprintf(fp, "\t\t\t0 %g\n", elem->velSamples[g].local.totalDelta.base[__INDEX__]); \
					fprintf(fp, "\t\t\t1 %g\n", elem->velSamples[g].local.totalDelta.amplitude[__INDEX__]); \
					fprintf(fp, "\t\t}\n"); \
					fprintf(fp, "\t};\n")

					DUMPVELGRAPH(0, "X");
					DUMPVELGRAPH(1, "Y");
					DUMPVELGRAPH(2, "Z");
				}

				// TODO dump atlas data
				PRINTINT(1, "atlastBehavior", elem->atlas.behavior);
				PRINTINT(1, "atlasIndex", elem->atlas.index);
				PRINTINT(1, "atlasFps", elem->atlas.fps);
				PRINTINT(1, "atlasLoopCount", elem->atlas.loopCount);
				PRINTINT(1, "atlasColIndexBits", elem->atlas.colIndexBits);
				PRINTINT(1, "atlasRowIndexBits", elem->atlas.rowIndexBits);
				PRINTINT(1, "atlasEntryCount", elem->atlas.entryCount);

				// TODO dump graphs
				PRINTINT(1, "lightingFrac", elem->lightingFrac);
				// TODO convert bounds to origin + radius
				PRINTSTRING(1, "fxOnImpact", (elem->effectOnImpact) ? elem->effectOnImpact->name : "");
				PRINTSTRING(1, "fxOnDeath", (elem->effectOnDeath) ? elem->effectOnDeath->name : "");
				PRINTINT(1, "sortOrder", elem->sortOrder);
				PRINTSTRING(1, "emission", (elem->effectEmitted) ? elem->effectEmitted->name : "");
				// TODO add trail data
				PRINTFLOATRANGE(1, "emitDist", elem->emitDist);
				PRINTFLOATRANGE(1, "emitDistVariance", elem->emitDistVariance);

				fprintf(fp, "}\n");
			}


			FileSystem::FileClose(fp);
		}

		void IFxEffectDef::dump(FxEffectDef* asset)
		{
			dumpToLegacyFormat(asset);

			AssetDumper dump;

			if (!dump.Open("fx\\"s + asset->name + ".fxe"))
			{
				return;
			}

			dump.Array(asset, 1);
			dump.String(asset->name);
			dump.Array(asset->elemDefs,
			           asset->elemDefCountEmission + asset->elemDefCountLooping + asset->elemDefCountOneShot);

			// dump elemDefs
			for (int i = 0; i < asset->elemDefCountEmission + asset->elemDefCountLooping + asset->elemDefCountOneShot; i
			     ++)
			{
				auto def = &asset->elemDefs[i];

				// dump elem samples
				dump.Array(def->velSamples, def->velIntervalCount + 1);
				dump.Array(def->visSamples, def->visStateIntervalCount + 1);

				// todo: dump visuals!

				// dump reference FX defs
				dump.Asset(def->effectOnImpact);
				dump.Asset(def->effectOnDeath);
				dump.Asset(def->effectEmitted);

				// dump extended FX data
				if (def->extended.trailDef)
				{
					if (def->elemType == FX_ELEM_TYPE_TRAIL)
					{
						dump.Array(def->extended.trailDef, 1);

						if (def->extended.trailDef->verts)
						{
							dump.Array(def->extended.trailDef->verts, def->extended.trailDef->vertCount);
						}

						if (def->extended.trailDef->inds)
						{
							dump.Array(def->extended.trailDef->inds, def->extended.trailDef->indCount);
						}
					}
					else if (def->elemType == FX_ELEM_TYPE_SPARKFOUNTAIN)
					{
						dump.Array(def->extended.sparkFountain, 1);
					}
					else if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
					{
						dump.Array(def->extended.unknownDef, 24);
					}
					else
					{
						dump.Array(def->extended.unknownDef, 1);
					}
				}
			}

			dump.Close();
		}
	}
}
