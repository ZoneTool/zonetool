#pragma once

namespace ZoneTool
{
	namespace IW5
	{
		class IRawFile : public IAsset
		{
		private:
			std::string m_name;
			RawFile* m_asset;

			RawFile* parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem);

		public:
			IRawFile();
			~IRawFile();

			void init(const std::string& name, std::shared_ptr<ZoneMemory>& mem) override;
			void prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem) override;
			void load_depending(IZone* zone) override;

			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, std::shared_ptr<ZoneBuffer>& buffer) override;

			static void dump(RawFile* asset);
		};
	}
}
