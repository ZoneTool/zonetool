#pragma once

namespace ZoneTool
{
	namespace IW5
	{
		class ISound : public IAsset
		{
		private:
			std::string m_name;
			snd_alias_list_t* m_asset;
			bool m_parsed;

			static void write_soundfile(IZone* zone, std::shared_ptr<ZoneBuffer>& buf, SoundFile* dest);
			static void write_head(IZone* zone, std::shared_ptr<ZoneBuffer>& buf, snd_alias_t* dest);

		public:
			ISound();
			~ISound();

			snd_alias_list_t* parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem);

			void init(const std::string& name, std::shared_ptr<ZoneMemory>& mem) override;
			void prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem) override;
			void load_depending(IZone* zone) override;

			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, std::shared_ptr<ZoneBuffer>& buffer) override;

			static void dump(snd_alias_list_t* asset);
		};
	}
}
