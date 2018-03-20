#pragma once

#include <fstream>
#include <cassert>
#include <string>

namespace Dv
{
	namespace Util
	{
		/*****************************************************************************************************************************
		* Loading types from binary input fstreams
		*
		* Returns false if an error occured while loading.
		*****************************************************************************************************************************/
		template<class T> 
		bool                              TryLoadData                                (std::ifstream *pOutStrm, T *pOutData);

		bool                              TryLoadFloat32                             (std::ifstream *pOutStrm, float *pOutData);

		bool                              TryLoadUInt64                              (std::ifstream *pOutStrm, uint64_t *pOutData);
		bool                              TryLoadUInt32                              (std::ifstream *pOutStrm, uint32_t *pOutData);
		bool                              TryLoadUInt16                              (std::ifstream *pOutStrm, uint16_t *pOutData);
		bool                              TryLoadUInt8                               (std::ifstream *pOutStrm, uint8_t *pOutData);

		bool                              TryLoadByteArray                           (std::ifstream *pOutStrm, uint8_t *pOutBytes, size_t InNumBytes);

		// Load string where number of chars specified as first byte
		bool                              TryLoadString_FirstByteIsLength            (std::ifstream *pOutStrm, std::string *pOutString);
		bool                              TryLoadString_ZeroTerminated               (std::ifstream *pOutStrm, std::string *pOutString);
		bool                              TryLoadString_FixedSize                    (std::ifstream *pOutStrm, std::string *pOutString, size_t InLength);

		// Load string prefixed with its length of type (and size) SizeType
		template<class SizeType>
		bool                              TryLoadString_FirstNumberIsLength          (std::ifstream *pOutStrm, std::string *pOutStream);

		/******************************************************************************************************************************
		* implementation
		******************************************************************************************************************************/

		template<class T> bool TryLoadData(std::ifstream *pOutStrm, T *pOutData)
		{
			assert(pOutStrm);
			assert(pOutData);

			auto pOutDataVoid = static_cast<void *>(pOutData);
			pOutStrm->read(static_cast<char *>(pOutDataVoid), sizeof(T));

			if (pOutStrm->fail())
			{
				return false;
			}

			return true;
		}

		template<class SizeType>
		bool TryLoadString_FirstNumberIsLength(std::ifstream *pOutStrm, std::string *pOutString)
		{
			assert(pOutStrm);
			assert(pOutString);

			SizeType length;
			bool bLengthLoaded = TryLoadData(pOutStrm, &length);
			if (false == bLengthLoaded)
			{
				return false;
			}

			pOutString->resize(length);
	
			for(int i = 0; i < length; i++)
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
	}
}