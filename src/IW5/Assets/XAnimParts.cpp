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
		char* freadstr(FILE* file);
		int freadint(FILE* file);

		void fwritestr(FILE* file, const char* str);
		void fwriteint(FILE* file, int value);

		IXAnimParts::IXAnimParts()
		{
		}

		IXAnimParts::~IXAnimParts()
		{
		}

		XAnimParts* IXAnimParts::ParseXAE(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			auto path = "XAnim\\"s + name + ".xae"s;

			auto file = FileSystem::FileOpen(path, "rb"); // fopen(name.c_str(), "rb");
			if (!file) return nullptr;

			ZONETOOL_INFO("Parsing XAE file \"%s\"...", name.c_str());

			XAnimParts* anim = mem->Alloc<XAnimParts>(); // new XAnimParts;
			fread(anim, sizeof(XAnimParts), 1, file);

			anim->name = freadstr(file);
			anim->tagnames = mem->Alloc<unsigned short>(anim->boneCount[9]); // new unsigned short[anim->boneCount[9]];

			for (int i = 0; i < anim->boneCount[9]; i++)
				anim->tagnames[i] = SL_AllocString(freadstr(file));

			if (freadint(file))
			{
				anim->notetracks = mem->Alloc<XAnimNotifyInfo>(anim->notetrackCount);
				//new XAnimNotifyInfo[anim->notetrackCount];

				for (int i = 0; i < (anim->notetrackCount); i++)
				{
					fread(&anim->notetracks[i], sizeof(XAnimNotifyInfo), 1, file);

					const char* str = freadstr(file);
					anim->notetracks[i].name = SL_AllocString(str);
				}
			}

			if (anim->dataByte)
			{
				anim->dataByte = mem->Alloc<char>(anim->dataByteCount); // new char[anim->dataByteCount];
				fread(anim->dataByte, 1, anim->dataByteCount, file);
			}
			if (anim->dataShort)
			{
				anim->dataShort = mem->Alloc<short>(anim->dataShortCount);
				fread(anim->dataShort, 2, anim->dataShortCount, file);
			}
			if (anim->dataInt)
			{
				anim->dataInt = mem->Alloc<int>(anim->dataIntCount);
				fread(anim->dataInt, 4, anim->dataIntCount, file);
			}
			if (anim->randomDataShort)
			{
				anim->randomDataShort = mem->Alloc<short>(anim->randomDataShortCount);
				fread(anim->randomDataShort, 2, anim->randomDataShortCount, file);
			}
			if (anim->randomDataByte)
			{
				anim->randomDataByte = mem->Alloc<char>(anim->randomDataByteCount);
				fread(anim->randomDataByte, 1, anim->randomDataByteCount, file);
			}
			if (anim->randomDataInt)
			{
				anim->randomDataInt = mem->Alloc<int>(anim->randomDataIntCount);
				fread(anim->randomDataInt, 4, anim->randomDataIntCount, file);
			}

			if (anim->delta)
			{
				anim->delta = nullptr;
			}
			if (anim->indices.data)
			{
				anim->indices.data = nullptr;
			}

			FileSystem::FileClose(file);

			return anim;
		}

		void IXAnimParts::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			this->m_name = name;
			this->m_asset = this->ParseXAE(name, mem); // 

			if (!this->m_asset)
			{
				this->m_asset = DB_FindXAssetHeader(this->type(), this->name().data(), 1).xanimparts;
			}
		}

		void IXAnimParts::prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
		{
			// fixup scriptstrings
			auto xanim = mem->Alloc<XAnimParts>();
			memcpy(xanim, this->m_asset, sizeof XAnimParts);

			// allocate tagnames
			if (this->m_asset->tagnames)
			{
				xanim->tagnames = mem->Alloc<unsigned short>(xanim->boneCount[9]);
				memcpy(xanim->tagnames, this->m_asset->tagnames, sizeof(unsigned short) * xanim->boneCount[9]);

				for (int i = 0; i < xanim->boneCount[9]; i++)
				{
					if (xanim->tagnames[i])
					{
						std::string bone = SL_ConvertToString(this->m_asset->tagnames[i]);
						xanim->tagnames[i] = buf->write_scriptstring(bone);
					}
				}
			}

			// allocate notetracks
			xanim->notetracks = mem->Alloc<XAnimNotifyInfo>(xanim->notetrackCount);
			memcpy(xanim->notetracks, this->m_asset->notetracks, sizeof XAnimNotifyInfo * xanim->notetrackCount);

			// touch XAnimNotifyInfo, realloc tagnames
			for (int i = 0; i < xanim->notetrackCount; i++)
			{
				std::string bone = SL_ConvertToString(this->m_asset->notetracks[i].name);
				xanim->notetracks[i].name = buf->write_scriptstring(bone);
			}

			this->m_asset = xanim;
		}

		void IXAnimParts::load_depending(IZone* zone)
		{
		}

		std::string IXAnimParts::name()
		{
			return this->m_name;
		}

		std::int32_t IXAnimParts::type()
		{
			return xanim;
		}

		void IXAnimParts::write(IZone* zone, std::shared_ptr<ZoneBuffer>& buf)
		{
			auto data = this->m_asset;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->tagnames) // tagnames
			{
				buf->align(1);
				buf->write_stream(data->tagnames, sizeof(short), data->boneCount[9]);
				dest->tagnames = (unsigned short*)-1;
			}
			if (data->notetracks) // notetracks
			{
				buf->align(3);
				buf->write_stream(data->notetracks, sizeof(XAnimNotifyInfo), data->notetrackCount);
				dest->tagnames = (unsigned short*)-1;
			}

			if (data->delta) // XAnimDeltaParts
			{
				buf->align(3);

				XAnimDeltaPart* partdata = data->delta;
				XAnimDeltaPart* partdest = (XAnimDeltaPart*)buf->at();
				buf->write_stream(data, sizeof(XAnimDeltaPart), 1);

				if (partdata->trans)
				{
					buf->align(3);
					buf->write_stream(partdata->trans, 4, 1); // not full struct
					if (partdata->trans->size)
					{
						buf->write_stream(&partdata->trans->u, 0x1C, 1); // not full struct
						if (data->framecount > 0x100)
							buf->write_stream(&partdata->trans->u.frames.indices,
							                  sizeof(short), partdata->trans->size + 1);
						else
							buf->write_stream(&partdata->trans->u.frames.indices, sizeof(char),
							                  partdata->trans->size + 1);

						if (partdata->trans->u.frames.frames._1)
						{
							if (partdata->trans->smallTrans)
							{
								buf->align(0);
								buf->write_stream(partdata->trans->u.frames.frames._1, sizeof(char) * 3,
								                  partdata->trans->size + 1);
							}
							else
							{
								buf->align(3);
								buf->write_stream(partdata->trans->u.frames.frames._2, sizeof(short) * 3,
								                  partdata->trans->size + 1);
							}

							//dest->trans->u.frames.frames = (char*)-1;
						}
					}
					else buf->write_stream(partdata->trans->u.frame0, sizeof(float), 3);
					partdest->trans = (XAnimPartTrans*)-1;
				}

				if (partdata->quat2)
				{
					buf->align(3);
					buf->write_stream(partdata->quat2, 4, 1); // not full struct
					if (partdata->quat2->size)
					{
						buf->write_stream(&partdata->quat2->u, 0x4, 1); // not full struct
						if (data->framecount > 0x100)
							buf->write_stream(&partdata->quat2->u.frames.indices,
							                  sizeof(short), partdata->quat2->size + 1);
						else
							buf->write_stream(&partdata->quat2->u.frames.indices, sizeof(char),
							                  partdata->quat2->size + 1);

						if (partdata->quat2->u.frames.frames)
						{
							buf->align(3);
							buf->write_stream(partdata->quat2->u.frames.frames, sizeof(short) * 2,
							                  partdata->quat2->size + 1);
							//dest->quat2->u.frames.frames = (short*)-1;
						}
					}
					else buf->write_stream(partdata->quat2->u.frame0, sizeof(short) * 2, 1);
					partdest->quat2 = (XAnimDeltaPartQuat2*)-1;
				}

				if (partdata->quat)
				{
					buf->align(3);
					buf->write_stream(partdata->quat, 4, 1);
					if (partdata->quat->size)
					{
						buf->write_stream(&partdata->quat->u, 4, 1); // not full struct

						if (data->framecount > 0x100)
							buf->write_stream(&partdata->quat->u.frames.indices,
							                  sizeof(short), partdata->quat->size + 1);
						else
							buf->write_stream(&partdata->quat->u.frames.indices, sizeof(char),
							                  partdata->quat->size + 1);

						if (partdata->quat->u.frames.frames)
						{
							buf->align(3);
							buf->write_stream(partdata->quat->u.frames.frames, sizeof(short) * 4,
							                  partdata->quat->size + 1);
							//dest->quat->u.frames.frames = (short*)-1;
						}
					}
					else
						buf->write_stream(partdata->quat->u.frame0, sizeof(short) * 4, 1);

					partdest->quat = (XAnimDeltaPartQuat*)-1;
				}

				dest->delta = (XAnimDeltaPart*)-1;
			}

			if (data->dataByte) // dataByte
			{
				buf->align(0);
				buf->write_stream(data->dataByte, sizeof(char), data->dataByteCount);
				dest->dataByte = (char*)-1;
			}
			if (data->dataShort) // dataShort
			{
				buf->align(1);
				buf->write_stream(data->dataShort, sizeof(short), data->dataShortCount);
				dest->dataShort = (short*)-1;
			}
			if (data->dataInt) // dataInt
			{
				buf->align(3);
				buf->write_stream(data->dataInt, sizeof(int), data->dataIntCount);
				dest->dataInt = (int*)-1;
			}
			if (data->randomDataShort) // randomDataShort
			{
				buf->align(1);
				buf->write_stream(data->randomDataShort, sizeof(short), data->randomDataShortCount);
				dest->randomDataShort = (short*)-1;
			}
			if (data->randomDataByte) // randomDataByte
			{
				buf->align(0);
				buf->write_stream(data->randomDataByte, sizeof(char), data->randomDataByteCount);
				dest->randomDataByte = (char*)-1;
			}
			if (data->randomDataInt) // randomDataInt
			{
				buf->align(3);
				buf->write_stream(data->randomDataInt, sizeof(int), data->randomDataIntCount);
				dest->randomDataInt = (int*)-1;
			}

			// XAnim indice data
			if (data->indices.data)
			{
				if (data->framecount > 255)
				{
					buf->align(1);
					buf->write_stream(data->indices.data, data->indexcount * 2, 1);
				}
				else
				{
					buf->align(0);
					buf->write_stream(data->indices.data, data->indexcount, 1);
				}

				dest->indices.data = (void*)-1;
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IXAnimParts::dump(XAnimParts* asset, const std::function<const char*(std::uint16_t)>& convertToString)
		{
			//auto path = "XAnim\\"s + asset->name + ".xae2";

			//AssetDumper dump;
			//if (!dump.Open(path))
			//{
			//	return;
			//}

			//dump.Array(asset, 1);
			//dump.String(asset->name);

			//for (int i = 0; i < asset->boneCount[9]; i++)
			//{
			//	dump.String(convertToString(asset->tagnames[i]));
			//}

			//if (asset->notetracks)
			//{
			//	dump.Array(asset->notetracks, asset->notetrackCount);
			//	
			//	for (int i = 0; i < asset->notetrackCount; i++)
			//	{
			//		dump.String(convertToString(asset->notetracks[i].name));
			//	}
			//}

			///*if (asset->delta)
			//{
			//	dump.Array(asset->delta, 1);

			//	if (asset->delta->quat)
			//	{

			//	}
			//}*/

			//if (asset->dataByte)
			//{
			//	dump.Array(asset->dataByte, asset->dataByteCount);
			//}
			//if (asset->dataShort)
			//{
			//	dump.Array(asset->dataShort, asset->dataShortCount);
			//}
			//if (asset->dataInt)
			//{
			//	dump.Array(asset->dataInt, asset->dataIntCount);
			//}
			//if (asset->randomDataShort)
			//{
			//	dump.Array(asset->randomDataShort, asset->randomDataShortCount);
			//}
			//if (asset->randomDataByte)
			//{
			//	dump.Array(asset->randomDataByte, asset->randomDataByteCount);
			//}
			//if (asset->randomDataInt)
			//{
			//	dump.Array(asset->randomDataInt, asset->randomDataIntCount);
			//}

			//if (asset->indices.data)
			//{
			//	if (asset->framecount > 255)
			//	{
			//		dump.Array(asset->indices._2, asset->framecount);
			//	}
			//	else
			//	{
			//		dump.Array(asset->indices._1, asset->framecount);
			//	}
			//}
			//
			//dump.Close();

			auto path = "XAnim\\"s + asset->name + ".xae";

			if (FileSystem::FileExists(path))
			{
				return;
			}

			auto file = FileSystem::FileOpen(path, "wb");

			// Write XAE file
			fwrite(asset, sizeof XAnimParts, 1, file);
			fwritestr(file, asset->name);

			// Write tag names
			for (int i = 0; i < asset->boneCount[9]; i++)
				fwritestr(file, convertToString(asset->tagnames[i]));

			// Write notetracks
			if (asset->notetracks)
			{
				fwriteint(file, 1);

				for (int i = 0; i < asset->notetrackCount; i++)
				{
					fwrite(&asset->notetracks[i], sizeof(XAnimNotifyInfo), 1, file);
					fwritestr(file, convertToString(asset->notetracks[i].name));
				}
			}
			else fwriteint(file, 0);

			// Bytes/Shorts/Ints
			if (asset->dataByte)
			{
				fwrite(asset->dataByte, 1, asset->dataByteCount, file);
			}
			if (asset->dataShort)
			{
				fwrite(asset->dataShort, 2, asset->dataShortCount, file);
			}
			if (asset->dataInt)
			{
				fwrite(asset->dataInt, 4, asset->dataIntCount, file);
			}
			if (asset->randomDataShort)
			{
				fwrite(asset->randomDataShort, 2, asset->randomDataShortCount, file);
			}
			if (asset->randomDataByte)
			{
				fwrite(asset->randomDataByte, 1, asset->randomDataByteCount, file);
			}
			if (asset->randomDataInt)
			{
				fwrite(asset->randomDataInt, 4, asset->randomDataIntCount, file);
			}

			FileSystem::FileClose(file);
		}
	}
}
