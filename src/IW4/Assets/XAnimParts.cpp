// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "../IW5/Assets/XAnimParts.hpp"

namespace ZoneTool::IW4
{
	void IXAnimParts::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
	{
		this->m_name = name;
		this->m_asset = reinterpret_cast<IW4::XAnimParts*>(IW5::IXAnimParts::parse(name, mem, SL_AllocString)); // 

		if (!this->m_asset)
		{
			this->m_asset = DB_FindXAssetHeader(this->type(), this->name().data()).xanimparts;
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

			for (auto i = 0; i < xanim->boneCount[9]; i++)
			{
				if (xanim->tagnames[i])
				{
					const std::string bone = SL_ConvertToString(this->m_asset->tagnames[i]);
					xanim->tagnames[i] = buf->write_scriptstring(bone);
				}
			}
		}

		// allocate notetracks
		xanim->notetracks = mem->Alloc<XAnimNotifyInfo>(xanim->notetrackCount);
		memcpy(xanim->notetracks, this->m_asset->notetracks, sizeof XAnimNotifyInfo * xanim->notetrackCount);

		// touch XAnimNotifyInfo, realloc tagnames
		for (auto i = 0; i < xanim->notetrackCount; i++)
		{
			const std::string bone = SL_ConvertToString(this->m_asset->notetracks[i].name);
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
			dest->tagnames = reinterpret_cast<unsigned short*>(-1);
		}
		if (data->notetracks) // notetracks
		{
			buf->align(3);
			buf->write_stream(data->notetracks, sizeof(XAnimNotifyInfo), data->notetrackCount);
			dest->tagnames = reinterpret_cast<unsigned short*>(-1);
		}

		if (data->delta) // XAnimDeltaParts
		{
			buf->align(3);

			auto partdata = data->delta;
			auto partdest = reinterpret_cast<XAnimDeltaPart*>(buf->at());
			buf->write_stream(partdata, sizeof(XAnimDeltaPart), 1);

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
				partdest->trans = reinterpret_cast<XAnimPartTrans*>(-1);
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
				partdest->quat2 = reinterpret_cast<XAnimDeltaPartQuat2*>(-1);
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
					}
				}
				else
					buf->write_stream(partdata->quat->u.frame0, sizeof(short) * 4, 1);

				partdest->quat = reinterpret_cast<XAnimDeltaPartQuat*>(-1);
			}

			dest->delta = reinterpret_cast<XAnimDeltaPart*>(-1);
		}

		if (data->dataByte) // dataByte
		{
			buf->align(0);
			buf->write_stream(data->dataByte, sizeof(char), data->dataByteCount);
			dest->dataByte = reinterpret_cast<char*>(-1);
		}
		if (data->dataShort) // dataShort
		{
			buf->align(1);
			buf->write_stream(data->dataShort, sizeof(short), data->dataShortCount);
			dest->dataShort = reinterpret_cast<short*>(-1);
		}
		if (data->dataInt) // dataInt
		{
			buf->align(3);
			buf->write_stream(data->dataInt, sizeof(int), data->dataIntCount);
			dest->dataInt = reinterpret_cast<int*>(-1);
		}
		if (data->randomDataShort) // randomDataShort
		{
			buf->align(1);
			buf->write_stream(data->randomDataShort, sizeof(short), data->randomDataShortCount);
			dest->randomDataShort = reinterpret_cast<short*>(-1);
		}
		if (data->randomDataByte) // randomDataByte
		{
			buf->align(0);
			buf->write_stream(data->randomDataByte, sizeof(char), data->randomDataByteCount);
			dest->randomDataByte = reinterpret_cast<char*>(-1);
		}
		if (data->randomDataInt) // randomDataInt
		{
			buf->align(3);
			buf->write_stream(data->randomDataInt, sizeof(int), data->randomDataIntCount);
			dest->randomDataInt = reinterpret_cast<int*>(-1);
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

			dest->indices.data = reinterpret_cast<void*>(-1);
		}

		END_LOG_STREAM;
		buf->pop_stream();
	}

	void IXAnimParts::dump(XAnimParts* asset, const std::function<const char*(std::uint16_t)>& convertToString)
	{
		IW5::IXAnimParts::dump(reinterpret_cast<IW5::XAnimParts*>(asset), convertToString);
	}
}
