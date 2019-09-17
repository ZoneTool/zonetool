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
	namespace IW4
	{
		class IWeaponDef : public IAsset
		{
		private:
			std::string m_name;
			WeaponCompleteDef* m_asset;

		public:
			IWeaponDef();
			~IWeaponDef();

			WeaponCompleteDef* parse_weapondef(Json& data, WeaponCompleteDef* baseAsset,
			                                   std::shared_ptr<ZoneMemory>& mem);

			WeaponCompleteDef* parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem);

			void init(const std::string& name, std::shared_ptr<ZoneMemory>& mem) override;
			void prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem) override;

			void load_depending_WeaponDef(IZone* zone, WeaponCompleteDef* data);
			void load_depending(IZone* zone) override;

			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, std::shared_ptr<ZoneBuffer>& buffer) override;
			void write_WeaponDef(IZone* zone, std::shared_ptr<ZoneBuffer>& buf, WeaponCompleteDef* complete,
			                     WeaponCompleteDef* data);

			static Json dump_weapondef(WeaponCompleteDef* asset);
			static Json dump_complete(WeaponCompleteDef* asset);
			static void dump(WeaponCompleteDef* asset);
		};
	}
}
