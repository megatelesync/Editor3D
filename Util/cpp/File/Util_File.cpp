#include "../../h/File/Util_File.h"


namespace Dv
{
	namespace Util
	{
		bool TryLoadString_FirstByteIsLength(std::ifstream *pOutStrm, std::string *pOutString)
		{
			return TryLoadString_FirstNumberIsLength<uint8_t>(pOutStrm, pOutString);
		}
		bool TryLoadString_ZeroTerminated(std::ifstream *pOutStrm, std::string *pOutString)
		{
			assert(pOutStrm);
			assert(pOutString);

			pOutString->clear();
			while(true)
			{
				char c;
				pOutStrm->read(&c, 1);
				if (pOutStrm->fail())
				{
					return false;
				}
				if (c == '\0')
				{
					return true;
				}
				pOutString->push_back(c);
			}
			return true;
		}
		bool TryLoadString_FixedSize(std::ifstream *pOutStrm, std::string *pOutString, size_t InLength)
		{
			assert(pOutStrm);
			assert(pOutString);
			pOutString->resize(InLength);
			for (int i = 0; i < InLength; i++)
			{
				char c;
				pOutStrm->read(&c, 1);
				if (pOutStrm->fail())
				{
					return false;
				}
				pOutString->at(i) = c;
			}
			return true;
		}

		bool TryLoadFloat32(std::ifstream *pOutStrm, float *pOutData)
		{
			assert(sizeof(float) == 4);
			return TryLoadData<float>(pOutStrm, pOutData);
		}

		bool TryLoadUInt64(std::ifstream *pOutStrm, uint64_t *pOutData)
		{
			return TryLoadData<uint64_t>(pOutStrm, pOutData);
		}
		bool TryLoadUInt32(std::ifstream *pOutStrm, uint32_t *pOutData)
		{
			return TryLoadData<uint32_t>(pOutStrm, pOutData);
		}
		bool TryLoadUInt16(std::ifstream *pOutStrm, uint16_t *pOutData)
		{
			return TryLoadData<uint16_t>(pOutStrm, pOutData);
		}
		bool TryLoadUInt8(std::ifstream *pOutStrm, uint8_t *pOutData)
		{
			return TryLoadData<uint8_t>(pOutStrm, pOutData);
		}
		bool TryLoadByteArray(std::ifstream *pOutStrm, uint8_t *pOutBytes, size_t InNumBytes)
		{
			assert(pOutStrm);
			assert(pOutBytes);

			void *pOutBytesVoid = pOutBytes;
			pOutStrm->read(static_cast<char *>(pOutBytesVoid), InNumBytes);
			if (pOutStrm->fail())
			{
				return false;
			}
			return true;
		}
	}
}