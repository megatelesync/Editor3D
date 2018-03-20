#include "../../h/CoreUtil/CoreUtil_Hash.h"
#include "../../h/CoreUtil/CoreUtil_ptr.h"
#include <cstddef>
#include <cstdint>

namespace Dv
{
	namespace Util
	{
		size_t Calc_OneAtATime_LookupHash(const void *pBytes, size_t InNumBytes)
		{
			size_t result = 0;
			size_t curr_byte_index = 0;
			while (curr_byte_index < InNumBytes)
			{
				auto pCurrByte = static_cast<const uint8_t *>(Util::AddPtr(pBytes, curr_byte_index));
				result += *pCurrByte;
				result += (result << 10);
				result ^= (result >> 6);
			}

			result += (result << 3);
			result ^= (result >> 11);
			result += (result << 15);

			return result;
		}

		// Calculate FNV Hash: implementation
		size_t Calc_FNVHash(const void *pBytes, size_t InNumBytes)
		{
			size_t result = FNV_HASH_BASE;
			size_t curr_byte_index = 0;
			while (curr_byte_index < InNumBytes)
			{

				auto pCurrByte = static_cast<const uint8_t *>(Util::AddPtr(pBytes, curr_byte_index));
				result = (result * FNV_HASH_COEFF) ^ (*pCurrByte);
			}
			return result;
		}

		size_t Calc_BernsteinHash(const void *pBytes, size_t InNumBytes)
		{
			size_t result = 0;
			size_t curr_byte_index = 0;
			while (curr_byte_index < InNumBytes)
			{
				auto pCurrByte = static_cast<const uint8_t *>(Util::AddPtr(pBytes, curr_byte_index));
				result = 33 * result + *pCurrByte;
			}
			return result;
		}

		size_t Calc_ElfHash(const void *pBytes, size_t InNumBytes)
		{
			size_t result = 0;
			size_t curr_byte_index = 0;
			while (curr_byte_index < InNumBytes)
			{
				auto pCurrByte = static_cast<const uint8_t *>(Util::AddPtr(pBytes, curr_byte_index));
				result = (result << 4) + *pCurrByte;
				auto higher_4_bits = result & 0xF0000000;
				if (higher_4_bits != 0)
				{
					result ^= (higher_4_bits >> 24);
				}

				result &= higher_4_bits;
			}
			return result;
		}

		void Jenkins_Mix(size_t *pOutA, size_t *pOutB, size_t *pOutC)
		{
			*pOutA -= *pOutB; *pOutA -= *pOutC; *pOutA ^= (*pOutC >> 13);
			*pOutB -= *pOutC; *pOutB -= *pOutA; *pOutB ^= (*pOutA << 8);

			*pOutC -= *pOutA; *pOutC -= *pOutB; *pOutC ^= (*pOutB >> 13);
			*pOutA -= *pOutB; *pOutA -= *pOutC; *pOutA ^= (*pOutC >> 12);

			*pOutB -= *pOutC; *pOutB -= *pOutA; *pOutB ^= (*pOutA << 16);
			*pOutC -= *pOutA; *pOutC -= *pOutB; *pOutC ^= (*pOutB >> 5);
			*pOutA -= *pOutB; *pOutA -= *pOutC; *pOutA ^= (*pOutC >> 3);

			*pOutB -= *pOutC; *pOutB -= *pOutA; *pOutB ^= (*pOutA << 10);
			*pOutC -= *pOutA; *pOutC -= *pOutB; *pOutC ^= (*pOutB >> 15);
		}

		size_t Jenkins_GetShiftedInputWord(const uint8_t *pTypedBytes, size_t InByteIndex, size_t InShift)
		{
			auto pResultByte = static_cast<const void *>(Util::AddPtr(pTypedBytes, InByteIndex));
			return *(static_cast<const size_t *>(pResultByte)) << InShift;
		}

		size_t Calc_Jenkins(const void *pBytes, size_t InNumBytes, size_t InInitValue)
		{
			auto pTypedBytes = static_cast<const uint8_t *>(pBytes);

			size_t result = 0;
			size_t len = InNumBytes;

			size_t c = InInitValue;
			size_t a, b;
			a = b = 0x9e3779b9;

			while (len >= 12)
			{
				a += (*Util::AddPtr(pTypedBytes, 0) + (Jenkins_GetShiftedInputWord(pTypedBytes, 1, 8) + Jenkins_GetShiftedInputWord(pTypedBytes, 2, 16) + Jenkins_GetShiftedInputWord(pTypedBytes, 3, 24)));
				b += (*Util::AddPtr(pTypedBytes, 4) + (Jenkins_GetShiftedInputWord(pTypedBytes, 5, 8) + Jenkins_GetShiftedInputWord(pTypedBytes, 6, 16) + Jenkins_GetShiftedInputWord(pTypedBytes, 7, 24)));
				c += (*Util::AddPtr(pTypedBytes, 8) + (Jenkins_GetShiftedInputWord(pTypedBytes, 9, 8) + Jenkins_GetShiftedInputWord(pTypedBytes, 10, 16) + Jenkins_GetShiftedInputWord(pTypedBytes, 11, 24)));

				Jenkins_Mix(&a, &b, &c);

				result += 12;
				len -= 12;
			}

			c += InNumBytes;

			switch (len)
			{
			case 11: c += (Jenkins_GetShiftedInputWord(pTypedBytes, 10, 24));
			case 10: c += (Jenkins_GetShiftedInputWord(pTypedBytes, 9, 16));
			case 9: c += (Jenkins_GetShiftedInputWord(pTypedBytes, 8, 8));

			case 8: b += (Jenkins_GetShiftedInputWord(pTypedBytes, 7, 24));
			case 7: b += (Jenkins_GetShiftedInputWord(pTypedBytes, 6, 16));
			case 6: b += (Jenkins_GetShiftedInputWord(pTypedBytes, 5, 8));
			case 5: b += *Util::AddPtr(pTypedBytes, 4);


			case 4: a += (Jenkins_GetShiftedInputWord(pTypedBytes, 3, 24));
			case 3: a += (Jenkins_GetShiftedInputWord(pTypedBytes, 2, 16));
			case 2: a += (Jenkins_GetShiftedInputWord(pTypedBytes, 1, 8));
			case 1: a += *Util::AddPtr(pTypedBytes, 0);
			}

			Jenkins_Mix(&a, &b, &c);

			return result;
		}

		size_t Calc_ModifiedBernstein(const void *pBytes, size_t InNumBytes)
		{
			size_t result = 0;
			size_t curr_byte_index = 0;
			while (curr_byte_index < InNumBytes)
			{
				auto pCurrByte = static_cast<const uint8_t *>(Util::AddPtr(pBytes, curr_byte_index));
				result = 33 * result ^ *pCurrByte;
			}
			return result;
		}

		size_t Calc_ShiftAddXor(const void *pBytes, size_t InNumBytes)
		{
			size_t result = 0;
			size_t curr_byte_index = 0;
			while (curr_byte_index < InNumBytes)
			{
				auto pCurrByte = static_cast<const uint8_t *>(Util::AddPtr(pBytes, curr_byte_index));
				result = (result << 5) + (result >> 2) + *pCurrByte;
			}
			return result;
		}

		size_t Calc_RotatingHash(const void *pBytes, size_t InNumBytes)
		{
			size_t result = 0;
			size_t curr_byte_index = 0;

			while (curr_byte_index < InNumBytes)
			{
				auto pCurrByte = static_cast<const uint8_t *>(Util::AddPtr(pBytes, curr_byte_index));
				result = (result << 4) ^ (result >> 28) + *pCurrByte;
			}
			return result;
		}

		size_t Calc_XorHash(const void *pBytes, size_t InNumBytes)
		{
			size_t result = 0;
			size_t curr_byte_index = 0;

			while (curr_byte_index < InNumBytes)
			{
				auto pCurrByte = static_cast<const uint8_t *>(Util::AddPtr(pBytes, curr_byte_index));
				result ^= *pCurrByte;
			}
			return result;
		}

		size_t Calc_AddHash(const void *pBytes, size_t InNumBytes)
		{
			size_t result = 0;
			size_t curr_byte_index = 0;
			while (curr_byte_index < InNumBytes)
			{
				auto pCurrByte = static_cast<const uint8_t *>(Util::AddPtr(pBytes, curr_byte_index));
				result += *pCurrByte;
			}
			return result;
		}
	}
}