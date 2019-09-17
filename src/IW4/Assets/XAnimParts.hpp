#pragma once

namespace ZoneTool
{
	namespace IW4
	{
		const char* SL_ConvertToString(std::uint16_t index);

		class IXAnimParts : public IAsset
		{
		private:
			std::string m_name;
			XAnimParts* m_asset;

		public:
			IXAnimParts();
			~IXAnimParts();

			XAnimParts* ParseXAE(const std::string& name, std::shared_ptr<ZoneMemory>& mem);

			void init(const std::string& name, std::shared_ptr<ZoneMemory>& mem) override;
			void prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem) override;
			void load_depending(IZone* zone) override;

			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, std::shared_ptr<ZoneBuffer>& buffer) override;

			static void dump(XAnimParts* asset,
			                 const std::function<const char*(std::uint16_t)>& convertToString = SL_ConvertToString);
		};
	}
}
