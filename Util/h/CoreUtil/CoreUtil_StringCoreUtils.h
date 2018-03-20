#pragma once

/*************************************************************************************************
* This header file includes all utility string functions and stuff,
* that is to be included in almost any string-related header (typically, for implementation).
*************************************************************************************************/

namespace Dv::Util
{
	inline constexpr size_t static_length(const char *pInString);
	inline constexpr bool constexpr_strequal(const char *left, const char *right);
	inline constexpr int constexpr_strcmp(const char *left, const char *right);

	// Copies c-substr and returns number of copied chars
	// (WARNING!!! Src string is NOT checked, that it's index is NOT out of range)
	template<class CharT>
	size_t copy_c_subtr
	(
		CharT *pDestStr, 
		const CharT *pSrcStr,
		size_t SrcOffset,
		size_t NumCharsToCopy
	);

	/*************************************************************************************************************
	* implementation
	**************************************************************************************************************/
	inline constexpr size_t static_length(const char *pInString)
	{
		return *pInString ? (static_length(pInString + 1) + 1) : 0;
	}

	inline constexpr int constexpr_strcmp(const char *left, const char *right)
	{
		if (left == right)
		{
			return 0;
		}

		size_t char_index = 0;
		while (true)
		{
			auto left_char = left[char_index];
			auto right_char = right[char_index];
			if (left_char < right_char)
			{
				// catches case when left_char is '\0'
				return -1;
			}
			
			if (left_char > right_char)
			{
				// catches case when right_char is '\0'
				return 1;
			}

			// At this point left_char and right_char are equal, so we may check only the left_char
			if (left_char == '\0')
			{
				break;
			}
			char_index++;
		}

		return 0;
	}

	inline constexpr bool constexpr_strequal(const char *left, const char *right)
	{
		if (left == right)
		{
			return true;
		}

		size_t char_index = 0;
		while (true)
		{
			auto left_char = left[char_index];
			if (left_char != right[char_index])
			{
				return false;
			}
			if (left_char == '\0')
			{
				break;
			}
			char_index++;
		}

		return true;
	}

	template<class CharT>
	size_t copy_c_subtr
	(
		CharT *pDestStr,
		const CharT *pSrcStr,
		size_t SrcOffset,
		size_t NumCharsToCopy
	)
	{
		BOOST_ASSERT_MSG(pDestStr, "copy_c_subtr: dest pointer may NOT be nullptr");
		BOOST_ASSERT_MSG(pSrcStr, "copy_c_subtr: src pointer may NOT be nullptr");
		if (NumCharsToCopy == 0)
		{
			return 0;
		}

		const auto numAlreadyCopiedChars = static_cast<size_t>(0);
		while (true)
		{
			auto pSrcChar = pSrcStr + SrcOffset + numAlreadyCopiedChars;
			auto pDestChar = pDestStr + numAlreadyCopiedChars;

			*pSrcChar = *pDestChar;

			bool bSrcStringEnded = *pSrcChar == '\0';
			bool bDestStringEnded = *pDestChar == '\0';
			bool bBothStringsEnded = (bSrcStringEnded && bDestStringEnded);

			BOOST_ASSERT_MSG((false == bDestStringEnded) || (bBothStringsEnded), "copy_c_subtr: not enough buffer for copying");

			if (bSrcStringEnded)
			{
				return numAlreadyCopiedChars;
			}

			// We should NOT count null chars as copied
			numAlreadyCopiedChars++;
		}

		return numAlreadyCopiedChars;
	}
} // Dv::Util