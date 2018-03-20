#pragma once

#include <boost/align/align.hpp>
#include <array>

namespace Dv
{
	namespace Util
	{
		template<size_t Size>
		using StaticByteArray = std::array<uint8_t, Size>;

		// Get minimal aligned
		constexpr size_t CalcMinimalAlignedSize(size_t InAlignSize, size_t InDesiredSize);

		void SetByOffset(void *pInDest, size_t InOffset, const void *pInSrc, size_t InSize);

		template<class T> 
		void SetByOffset(void *pInDest, size_t InOffset, const T& InValue);

		template<class T>
		T GetByOffset(const void *pInDest, size_t InOffset);


		/***************************************************************************************************************
		* implementation
		***************************************************************************************************************/
		inline constexpr size_t CalcMinimalAlignedSize(size_t InAlignSize, size_t InDesiredSize)
		{
			return (InDesiredSize % InAlignSize) ? (((InDesiredSize / InAlignSize) + 1) * InAlignSize) : InDesiredSize;
		}

		template<class T> void SetByOffset(void *pInDest, size_t InOffset, const T& InValue)
		{
			SetByOffset(pInDest, InOffset, &InValue, sizeof(InValue));
		}

		template<class T>
		T GetByOffset(const void *pInDest, size_t InOffset)
		{
			auto pPtr = Util::AddPtr(pInDest, InOffset);
			return *(static_cast<const T *>(pPtr));
		}
	} // Util
} // Dv