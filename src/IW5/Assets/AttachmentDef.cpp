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
	namespace IW5
	{
		IAttachmentDef::IAttachmentDef()
		{
		}

		IAttachmentDef::~IAttachmentDef()
		{
		}

		AttachmentDef* IAttachmentDef::parse(const std::string& name, ZoneMemory* mem)
		{
			if (name == "cheytacscope")
			{
				auto scope = mem->Alloc<AttachmentDef>();
				auto base = DB_FindXAssetHeader(attachment, /*name.data()*/ "l96a1scope", 1).attachment;

				memcpy(scope, base, sizeof AttachmentDef);
				scope->szInternalName = mem->StrDup(name);

				for (int i = 0; i < 16; i++)
				{
					scope->worldModels[i] = nullptr;
					scope->viewModels[i] = nullptr;
				}

				scope->worldModels[0] = DB_FindXAssetHeader(xmodel, "weapon_cheytac_scope", 1).xmodel;
				scope->viewModels[0] = DB_FindXAssetHeader(xmodel, "viewmodel_cheytac_scope", 1).xmodel;

				ZONETOOL_INFO("msrscope -> cheytacscope!");

				return scope;
			}

			return nullptr;
		}

		void IAttachmentDef::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data(), 1).attachment;
			}
		}

		void IAttachmentDef::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IAttachmentDef::load_depending(IZone* zone)
		{
			auto data = this->asset_;

			// if we didn't parse a custom asset and the asset is common: there is no point in writing it.
			// make a reference to the original asset instead.
			/*if (!this->m_parsed && AssetHandler::IsCommonAsset(this->type(), this->name()))
			{
				return;
			}*/

			for (std::int32_t i = 0; i < 16; i++)
			{
				if (data->worldModels && data->worldModels[i])
				{
					zone->add_asset_of_type(xmodel, data->worldModels[i]->name);
				}

				if (data->viewModels && data->viewModels[i])
				{
					zone->add_asset_of_type(xmodel, data->viewModels[i]->name);
				}
			}

			for (std::int32_t i = 0; i < 8; i++)
			{
				if (data->reticleViewModels && data->reticleViewModels[i])
				{
					zone->add_asset_of_type(xmodel, data->reticleViewModels[i]->name);
				}
			}

			if (data->ammogeneral)
			{
				if (data->ammogeneral->tracerType)
				{
					zone->add_asset_of_type(tracer, data->ammogeneral->tracerType->name);
				}
			}

			if (data->general)
			{
				if (data->general->reticleCenter)
				{
					zone->add_asset_of_type(material, data->general->reticleCenter->name);
				}

				if (data->general->reticleSide)
				{
					zone->add_asset_of_type(material, data->general->reticleSide->name);
				}
			}

			if (data->ui)
			{
				if (data->ui->ammoCounterIcon)
				{
					zone->add_asset_of_type(material, data->ui->ammoCounterIcon->name);
				}

				if (data->ui->dpadIcon)
				{
					zone->add_asset_of_type(material, data->ui->dpadIcon->name);
				}
			}

			if (data->adsOverlay)
			{
				if (data->adsOverlay->overlay.shader)
				{
					zone->add_asset_of_type(material, data->adsOverlay->overlay.shader->name);
				}

				if (data->adsOverlay->overlay.shaderLowRes)
				{
					zone->add_asset_of_type(material, data->adsOverlay->overlay.shaderLowRes->name);
				}

				if (data->adsOverlay->overlay.shaderEMP)
				{
					zone->add_asset_of_type(material, data->adsOverlay->overlay.shaderEMP->name);
				}

				if (data->adsOverlay->overlay.shaderEMPLowRes)
				{
					zone->add_asset_of_type(material, data->adsOverlay->overlay.shaderEMPLowRes->name);
				}
			}

			/*if (data->projectile)
			{
				if (data->projectile->projectileModel)
				{
					zone->AddAssetOfType(xmodel, data->projectile->projectileModel->name);
				}

				if (data->projectile->projExplosionEffect)
				{
					zone->AddAssetOfType(fx, data->projectile->projExplosionEffect->name);
				}

				if (data->projectile->projExplosionSound)
				{
					zone->AddAssetOfType(sound, data->projectile->projExplosionSound->aliasName);
				}

				if (data->projectile->projDudEffect)
				{
					zone->AddAssetOfType(fx, data->projectile->projDudEffect->name);
				}

				if (data->projectile->projDudSound)
				{
					zone->AddAssetOfType(sound, data->projectile->projDudSound->aliasName);
				}

				if (data->projectile->projTrailEffect)
				{
					zone->AddAssetOfType(fx, data->projectile->projTrailEffect->name);
				}

				if (data->projectile->projIgnitionEffect)
				{
					zone->AddAssetOfType(fx, data->projectile->projIgnitionEffect->name);
				}

				if (data->projectile->projIgnitionSound)
				{
					zone->AddAssetOfType(sound, data->projectile->projIgnitionSound->aliasName);
				}
			}*/
		}

		std::string IAttachmentDef::name()
		{
			return this->name_;
		}

		std::int32_t IAttachmentDef::type()
		{
			return attachment;
		}

		void IAttachmentDef::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->asset_;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->szInternalName = buf->write_str(this->name());

			if (data->szDisplayName)
			{
				dest->szDisplayName = buf->write_str(data->szDisplayName);
			}

			if (data->worldModels)
			{
				buf->align(3);
				auto models = buf->write(data->worldModels, 16);

				for (std::int32_t i = 0; i < 16; i++)
				{
					if (models[i])
					{
						models[i] = reinterpret_cast<XModel*>(zone->get_asset_pointer(xmodel, models[i]->name));
					}
				}

				ZoneBuffer::ClearPointer(&dest->worldModels);
			}

			if (data->viewModels)
			{
				buf->align(3);
				auto models = buf->write(data->viewModels, 16);

				for (std::int32_t i = 0; i < 16; i++)
				{
					if (models[i])
					{
						models[i] = reinterpret_cast<XModel*>(zone->get_asset_pointer(xmodel, models[i]->name));
					}
				}

				ZoneBuffer::ClearPointer(&dest->viewModels);
			}

			if (data->reticleViewModels)
			{
				buf->align(3);
				auto models = buf->write(data->reticleViewModels, 8);

				for (std::int32_t i = 0; i < 8; i++)
				{
					if (models[i])
					{
						models[i] = reinterpret_cast<XModel*>(zone->get_asset_pointer(xmodel, models[i]->name));
					}
				}

				ZoneBuffer::ClearPointer(&dest->reticleViewModels);
			}

			if (data->ammogeneral)
			{
				buf->align(3);
				auto ammo = buf->write(data->ammogeneral);

				if (ammo->tracerType)
				{
					ammo->tracerType = reinterpret_cast<TracerDef*>(zone->get_asset_pointer(
						tracer, ammo->tracerType->name));
				}

				ZoneBuffer::ClearPointer(&dest->ammogeneral);
			}

			if (data->sight)
			{
				buf->align(3);
				buf->write(data->sight);
				ZoneBuffer::ClearPointer(&dest->sight);
			}

			if (data->reload)
			{
				buf->align(1);
				buf->write(data->reload);
				ZoneBuffer::ClearPointer(&dest->reload);
			}

			if (data->addOns)
			{
				buf->align(1);
				buf->write(data->addOns);
				ZoneBuffer::ClearPointer(&dest->addOns);
			}

			if (data->general)
			{
				buf->align(3);
				auto general = buf->write(data->general);

				if (general->reticleCenter)
				{
					general->reticleCenter = reinterpret_cast<Material*>(zone->get_asset_pointer(
						material, general->reticleCenter->name));
				}

				if (general->reticleSide)
				{
					general->reticleSide = reinterpret_cast<Material*>(zone->get_asset_pointer(
						material, general->reticleSide->name));
				}

				ZoneBuffer::ClearPointer(&dest->general);
			}

			if (data->ammunition)
			{
				buf->align(3);
				buf->write(data->ammunition);
				ZoneBuffer::ClearPointer(&dest->ammunition);
			}

			if (data->idleSettings)
			{
				buf->align(3);
				buf->write(data->idleSettings);
				ZoneBuffer::ClearPointer(&dest->idleSettings);
			}

			if (data->damage)
			{
				buf->align(3);
				buf->write(data->damage);
				ZoneBuffer::ClearPointer(&dest->damage);
			}

			if (data->locationDamage)
			{
				buf->align(3);
				buf->write(data->damage);
				ZoneBuffer::ClearPointer(&dest->damage);
			}

			if (data->scopeDriftSettings)
			{
				buf->align(3);
				buf->write(data->scopeDriftSettings);
				ZoneBuffer::ClearPointer(&dest->scopeDriftSettings);
			}

			if (data->adsSettings)
			{
				buf->align(3);
				buf->write(data->adsSettings);
				ZoneBuffer::ClearPointer(&dest->adsSettings);
			}

			if (data->adsSettingsMain)
			{
				buf->align(3);
				buf->write(data->adsSettingsMain);
				ZoneBuffer::ClearPointer(&dest->adsSettingsMain);
			}

			if (data->hipSpread)
			{
				buf->align(3);
				buf->write(data->hipSpread);
				ZoneBuffer::ClearPointer(&dest->hipSpread);
			}

			if (data->gunKick)
			{
				buf->align(3);
				buf->write(data->gunKick);
				ZoneBuffer::ClearPointer(&dest->gunKick);
			}

			if (data->viewKick)
			{
				buf->align(3);
				buf->write(data->viewKick);
				ZoneBuffer::ClearPointer(&dest->viewKick);
			}

			if (data->adsOverlay)
			{
				buf->align(3);
				auto overlay = buf->write(data->adsOverlay);

				if (overlay->overlay.shader)
				{
					overlay->overlay.shader = reinterpret_cast<Material*>(zone->get_asset_pointer(
						material, overlay->overlay.shader->name));
				}

				if (overlay->overlay.shaderEMP)
				{
					overlay->overlay.shaderEMP = reinterpret_cast<Material*>(zone->get_asset_pointer(
						material, overlay->overlay.shaderEMP->name));
				}

				if (overlay->overlay.shaderEMPLowRes)
				{
					overlay->overlay.shaderEMPLowRes = reinterpret_cast<Material*>(zone->get_asset_pointer(
						material, overlay->overlay.shaderEMPLowRes->name));
				}

				if (overlay->overlay.shaderLowRes)
				{
					overlay->overlay.shaderLowRes = reinterpret_cast<Material*>(zone->get_asset_pointer(
						material, overlay->overlay.shaderLowRes->name));
				}

				ZoneBuffer::ClearPointer(&dest->adsOverlay);
			}

			if (data->ui)
			{
				buf->align(3);
				auto ui = buf->write(data->ui);

				if (ui->ammoCounterIcon)
				{
					ui->ammoCounterIcon = reinterpret_cast<Material*>(zone->get_asset_pointer(
						material, ui->ammoCounterIcon->name));
				}

				if (ui->dpadIcon)
				{
					ui->dpadIcon = reinterpret_cast<Material*>(zone->get_asset_pointer(material, ui->dpadIcon->name));
				}

				ZoneBuffer::ClearPointer(&dest->ui);
			}

			if (data->rumbles)
			{
				buf->align(3);
				auto rumbles = buf->write(data->rumbles);

				if (rumbles->fireRumble)
				{
					rumbles->fireRumble = buf->write_str(rumbles->fireRumble);
				}

				if (rumbles->meleeImpactRumble)
				{
					rumbles->meleeImpactRumble = buf->write_str(rumbles->meleeImpactRumble);
				}

				ZoneBuffer::ClearPointer(&dest->rumbles);
			}

			/*if (data->projectile)
			{
				buf->align(3);
				auto projectile = buf->write(data->projectile);

				if (projectile->projectileModel)
				{
					projectile->projectileModel = reinterpret_cast<XModel*>(zone->GetAssetPointer(xmodel, projectile->projectileModel->name));
				}

				if (projectile->projExplosionEffect)
				{
					projectile->projExplosionEffect = reinterpret_cast<FxEffectDef*>(zone->GetAssetPointer(fx, projectile->projExplosionEffect->name));
				}

				if (projectile->projExplosionSound)
				{
					projectile->projExplosionSound = reinterpret_cast<snd_alias_list_t*>(zone->GetAssetPointer(sound, projectile->projExplosionSound->aliasName));
				}

				if (projectile->projDudEffect)
				{
					projectile->projDudEffect = reinterpret_cast<FxEffectDef*>(zone->GetAssetPointer(fx, projectile->projDudEffect->name));
				}

				if (projectile->projDudSound)
				{
					projectile->projDudSound = reinterpret_cast<snd_alias_list_t*>(zone->GetAssetPointer(sound, projectile->projDudSound->aliasName));
				}

				if (projectile->projTrailEffect)
				{
					projectile->projTrailEffect = reinterpret_cast<FxEffectDef*>(zone->GetAssetPointer(fx, projectile->projTrailEffect->name));
				}

				if (projectile->projIgnitionEffect)
				{
					projectile->projIgnitionEffect = reinterpret_cast<FxEffectDef*>(zone->GetAssetPointer(fx, projectile->projIgnitionEffect->name));
				}

				if (projectile->projIgnitionSound)
				{
					projectile->projIgnitionSound = reinterpret_cast<snd_alias_list_t*>(zone->GetAssetPointer(sound, projectile->projIgnitionSound->aliasName));
				}

				ZoneBuffer::ClearPointer(&dest->projectile);
			}*/
			dest->projectile = nullptr;

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IAttachmentDef::dump(AttachmentDef* asset)
		{
			Json& data = asset->ToJson();

			std::string path = "attachments\\mp\\"s + asset->szInternalName;
			std::string json = data.dump(4);

			auto file = FileSystem::FileOpen(path, "w"s);
			fwrite(json.data(), json.size(), 1, file);
			FileSystem::FileClose(file);
		}
	}
}
