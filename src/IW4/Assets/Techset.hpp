#pragma once

namespace ZoneTool
{
	namespace IW4
	{
		class ITechset : public IAsset
		{
		private:
			std::string m_name;
			MaterialTechniqueSet* m_asset;
			bool m_parsed;

			bool isExternalTechnique[48];
			MaterialTechnique* parseTechniquePass(const std::string& name, std::shared_ptr<ZoneMemory>& mem,
			                                      std::uint32_t index);
			MaterialTechniqueSet* parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem);

		public:
			ITechset();
			~ITechset();

			void init(const std::string& name, std::shared_ptr<ZoneMemory>& mem) override;
			void prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem) override;
			void load_depending(IZone* zone) override;
			void* pointer() override { return m_asset; }

			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, std::shared_ptr<ZoneBuffer>& buffer) override;

			static std::string RemoveSatFromName(const std::string& name);
			static void dump(MaterialTechniqueSet* asset);
			static void dumpTechniquePass(MaterialTechnique* asset);
			static void dumpToJson(MaterialTechniqueSet* asset);

			static bool IsMappedTechset(const std::string& name);
			static std::string GetMappedTechset(const std::string& name);
		};
	}
}
#pragma once
