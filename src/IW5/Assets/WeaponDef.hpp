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
	namespace IW5
	{
		extern const char* SL_ConvertToString(std::uint16_t index);

		class IWeaponDef : public IAsset
		{
		private:
			std::string m_name;
			WeaponCompleteDef* m_asset;

		public:
			IWeaponDef();
			~IWeaponDef();

			WeaponDef* parse_weapondef(Json& data, WeaponCompleteDef* baseAsset, ZoneMemory* mem);

			WeaponCompleteDef* parse(const std::string& name, ZoneMemory* mem);

			void init(const std::string& name, ZoneMemory* mem) override;
			void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;

			void load_depending_WeaponDef(IZone* zone, WeaponDef* data);
			void load_depending(IZone* zone) override;

			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, ZoneBuffer* buffer) override;
			void write_WeaponDef(IZone* zone, ZoneBuffer* buf, WeaponCompleteDef* complete,
			                     WeaponDef* data);

			static Json dump_weapondef(WeaponDef* asset, const std::function<const char*(uint16_t)>& convertToString);
			static Json dump_complete(WeaponCompleteDef* asset,
			                          const std::function<const char*(uint16_t)>& convertToString);
			static void dump(WeaponCompleteDef* asset,
			                 const std::function<const char*(uint16_t)>& convertToString = SL_ConvertToString);
		};
	}
}
