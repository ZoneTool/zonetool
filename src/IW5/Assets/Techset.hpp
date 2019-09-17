#pragma once

namespace ZoneTool
{
	namespace IW5
	{
		class ITechset : public IAsset
		{
		private:
			std::string m_name;
			MaterialTechniqueSet* m_asset;
			bool m_parsed;

			MaterialTechnique* parseTechniquePass(const std::string& name, std::shared_ptr<ZoneMemory>& mem,
			                                      std::uint32_t index);
			MaterialTechniqueSet* parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem);

		public:
			ITechset();
			~ITechset();

			void init(const std::string& name, std::shared_ptr<ZoneMemory>& mem) override;
			void prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem) override;
			void load_depending(IZone* zone) override;

			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, std::shared_ptr<ZoneBuffer>& buffer) override;

			static void dump(MaterialTechniqueSet* asset);
			static void dumpTechniquePass(MaterialTechnique* asset);
			static void dumpToJson(MaterialTechniqueSet* asset);
		};
	}
}
