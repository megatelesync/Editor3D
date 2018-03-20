
#include "../../H/CoreUtil/CoreUtil_String.h"

namespace Dv::Util
{
	/***********************************************************************************
	* Fmt helper functions implementation
	***********************************************************************************/
	// Finds first format spec or end of the string (meaning pointer to the '\0' character),
	// and copies all other chars before the given.
	const char* FindFmtSpec(const char* fmt)
	{
		while ((*fmt != '\0') && (*fmt != '%')) { fmt++; }
		return fmt;
	}

	// Find the format spec code substring (f, d,  or other alpha-numering printf-type code);
	// WARNING!!! Checked with asssert that the format code exists in the string;
	std::pair<const char*, EFmtSpecLength> FindFmtSpecCode(const char* fmt, char* pBuffer, size_t buf_size)
	{
		EFmtSpecLength lengthSpec = EFmtSpecLength::Default;
		BOOST_ASSERT_MSG(*fmt == '%', "FindFmtSpecCode: the fmt string must start with '%'");
		const char* pChar = fmt;
		pChar++; // Skipping the '%' character
		while (*pChar != '\0')
		{
			switch (*pChar)
			{
			case 'l':
				switch (lengthSpec)
				{
				case EFmtSpecLength::Default:
					lengthSpec = EFmtSpecLength::Long;
					break;
				case EFmtSpecLength::Long:
					lengthSpec = EFmtSpecLength::LongLong;
					break;
				default:
					BOOST_ASSERT_MSG(false, "FindFmtSpecCode: wrong length specifier");
					break;
				}
				break;

			case 'h':
				switch (lengthSpec)
				{
				case EFmtSpecLength::Default:
					lengthSpec = EFmtSpecLength::Short;
					break;
				case EFmtSpecLength::Short:
					lengthSpec = EFmtSpecLength::ShortShort;
					break;
				default:
					BOOST_ASSERT_MSG(false, "FindFmtSpecCode: wrong length specifier");
					break;
				}
				break;

			case 'z':
				switch (lengthSpec)
				{
				case EFmtSpecLength::Default:
					lengthSpec = EFmtSpecLength::SizeT;
					break;

				default:
					BOOST_ASSERT_MSG(false, "FindFmtSpecCode: wrong length specifier");
					break;
				}
				break;

			case 't':
				switch (lengthSpec)
				{
				case EFmtSpecLength::Default:
					lengthSpec = EFmtSpecLength::PtrDiffT;
					break;

				default:
					BOOST_ASSERT_MSG(false, "FindFmtSpecCode: wrong length specifier");
					break;
				}
				break;


			case 'd': //fallthrough
			case 'i': //fallthrough
			case 'o': //fallthrough
			case 'x': //fallthrough
			case 'X': //fallthrough
			case 'f': //fallthrough
			case 'F': //fallthrough
			case 'e': //fallthrough
			case 'E': //fallthrough
			case 'g': //fallthrough
			case 'G': //fallthrough
			case 'a': //fallthrough
			case 'A': //fallthrough
			case 'c': //fallthrough
			case 's': //fallthrough
			case 'n': //fallthrough
			case 'p': //fallthrough
			case '%': //fallthrough
			case 'u':
				return std::make_pair(pChar, lengthSpec);
			}
			pChar++;
		}
		BOOST_ASSERT_MSG(false, "FindFmtSpecCode: wrong specifier format: no spec code found");
		return std::pair<const char*, EFmtSpecLength>();
	}

	template<>
	std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<std::string_view>(const char* fmt, char* pBuffer, size_t buf_size, std::string_view arg)
	{
		BOOST_ASSERT_MSG(*fmt == '%', "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: the fmt string must start with '%'");
		BOOST_ASSERT_MSG(pBuffer, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer pointer may NOT be nullptr");

		// WARNING!!! Checking that buffer is capable to store more than once char is needed for %% fmt spec processing
		BOOST_ASSERT_MSG(buf_size > 0 && buf_size > arg.length(), "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer is not enough");

		auto[pSpecCode, FmtSpecLength] = FindFmtSpecCode(fmt, pBuffer, buf_size);
		auto pUnprocessedSuffix = pSpecCode;
		int numCharsWantedToBeWritten = 0;
		switch (*pSpecCode)
		{
		case 's':
			switch (FmtSpecLength)
			{
			case EFmtSpecLength::Default:
				arg.copy(pBuffer, arg.length());
				pBuffer[arg.length()] = '\0';
				numCharsWantedToBeWritten = arg.length();
				break;

			default:
				BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars: wrong length specifier");
			}
			break;

		default:
			BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: unknown specifier code");
			break;
		}
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten >= 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: wrong code returned from snprintf");
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten < buf_size, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer is not enough");
		// Increment unprocessed suffix poitner once (for codes that include more than one chars additional increments are to be used)
		pUnprocessedSuffix++;
		return std::pair<const char*, int>(pUnprocessedSuffix, numCharsWantedToBeWritten);
	}

	template<>
	[[nodiscard]]
	std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<wint_t>(const char* fmt, char* pBuffer, size_t buf_size, wint_t arg)
	{
		BOOST_ASSERT_MSG(*fmt == '%', "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: the fmt string must start with '%'");
		BOOST_ASSERT_MSG(pBuffer, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer pointer may NOT be nullptr");

		// WARNING!!! Checking that buffer is capable to store more than once char is needed for %% fmt spec processing
		BOOST_ASSERT_MSG(buf_size > 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer must be enough to store at least one char");

		auto[pSpecCode, FmtSpecLength] = FindFmtSpecCode(fmt, pBuffer, buf_size);
		char spec_buf[64];
		auto spec_length = (pSpecCode - fmt) + 1;
		spec_buf[spec_length] = '\0';
		strncpy(spec_buf, fmt, spec_length);

		auto pUnprocessedSuffix = pSpecCode;
		int numCharsWantedToBeWritten = 0;
		switch (*pSpecCode)
		{
		case 'c':
			switch (FmtSpecLength)
			{
			case EFmtSpecLength::Long:
			{
				numCharsWantedToBeWritten = snprintf(pBuffer, buf_size, spec_buf, arg);
			}
			break;

			default:
				BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars: %lc must be specified for wint_t");
			}
			break;

		default:
			BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: wrong specifier code");
			break;
		}
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten >= 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: wrong code returned from snprintf");
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten < buf_size, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer is not enough");
		// Increment unprocessed suffix poitner once (for codes that include more than one chars additional increments are to be used)
		pUnprocessedSuffix++;
		return std::pair<const char*, int>(pUnprocessedSuffix, numCharsWantedToBeWritten);
	}

	

	template<>
	std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<std::string>(const char* fmt, char* pBuffer, size_t buf_size, std::string arg)
	{
		return WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars(fmt, pBuffer, buf_size, std::string_view(arg));
	}

	template<>
	std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<const char*>(const char* fmt, char* pBuffer, size_t buf_size, const char* arg)
	{
		BOOST_ASSERT_MSG(*fmt == '%', "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: the fmt string must start with '%'");
		BOOST_ASSERT_MSG(pBuffer, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer pointer may NOT be nullptr");

		// WARNING!!! Checking that buffer is capable to store more than once char is needed for %% fmt spec processing
		BOOST_ASSERT_MSG(buf_size > 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer must be enough to store at least one char");

		auto[pSpecCode, FmtSpecLength] = FindFmtSpecCode(fmt, pBuffer, buf_size);
		char spec_buf[64];
		auto spec_length = (pSpecCode - fmt) + 1;
		spec_buf[spec_length] = '\0';
		strncpy(spec_buf, fmt, spec_length);

		auto pUnprocessedSuffix = pSpecCode;
		int numCharsWantedToBeWritten = 0;
		switch (*pSpecCode)
		{
		case 's':
			switch (FmtSpecLength)
			{
			case EFmtSpecLength::Default:
				{
					//BOOST_ASSERT_MSG(std::is_convertible<T, const char *>::value, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: %s specifier require that argument is convertible to (const char *)");
					auto string_arg = static_cast<const char *>(arg);
					numCharsWantedToBeWritten = snprintf(pBuffer, buf_size, spec_buf, string_arg);
				}
				break;

			default:
				BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars: wrong length specifier");
			}
			break;

		default:
			BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: unknown specifier code");
			break;
		}
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten >= 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: wrong code returned from snprintf");
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten < buf_size, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer is not enough");
		// Increment unprocessed suffix poitner once (for codes that include more than one chars additional increments are to be used)
		pUnprocessedSuffix++;
		return std::pair<const char*, int>(pUnprocessedSuffix, numCharsWantedToBeWritten);
	}

	template<>
	[[nodiscard]]
	std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<double>(const char* fmt, char* pBuffer, size_t buf_size, double arg)
	{
		BOOST_ASSERT_MSG(*fmt == '%', "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: the fmt string must start with '%'");
		BOOST_ASSERT_MSG(pBuffer, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer pointer may NOT be nullptr");

		// WARNING!!! Checking that buffer is capable to store more than once char is needed for %% fmt spec processing
		BOOST_ASSERT_MSG(buf_size > 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer must be enough to store at least one char");


		auto[pSpecCode, FmtSpecLength] = FindFmtSpecCode(fmt, pBuffer, buf_size);
		char spec_buf[64];
		auto spec_length = (pSpecCode - fmt) + 1;
		spec_buf[spec_length] = '\0';
		strncpy(spec_buf, fmt, spec_length);

		auto pUnprocessedSuffix = pSpecCode;
		int numCharsWantedToBeWritten = 0;
		switch (*pSpecCode)
		{
		case 'f': // fallthrough
		case 'e': // fallthrough
		case 'g': // fallthrough
		case 'G': // fallthrough
		case 'E':
		case 'a':
		case 'A':
			switch(FmtSpecLength)
			{
			case EFmtSpecLength::Long:
				{
					auto double_arg = static_cast<double>(arg);
					numCharsWantedToBeWritten = snprintf(pBuffer, buf_size, spec_buf, double_arg);
				}	
				break;

			default:
				BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars: wrong length specifier");
			}
			break;

		default:
			BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: unknown specifier code");
			break;
		}
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten >= 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: wrong code returned from snprintf");
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten < buf_size, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer is not enough");
		// Increment unprocessed suffix poitner once (for codes that include more than one chars additional increments are to be used)
		pUnprocessedSuffix++;
		return std::pair<const char*, int>(pUnprocessedSuffix, numCharsWantedToBeWritten);
	}

	template<>
	std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<float>(const char* fmt, char* pBuffer, size_t buf_size, float arg)
	{
		BOOST_ASSERT_MSG(*fmt == '%', "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: the fmt string must start with '%'");
		BOOST_ASSERT_MSG(pBuffer, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer pointer may NOT be nullptr");

		// WARNING!!! Checking that buffer is capable to store more than once char is needed for %% fmt spec processing
		BOOST_ASSERT_MSG(buf_size > 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer must be enough to store at least one char");

		auto[pSpecCode, FmtSpecLength] = FindFmtSpecCode(fmt, pBuffer, buf_size);
		char spec_buf[64];
		auto spec_length = (pSpecCode - fmt) + 1;
		spec_buf[spec_length] = '\0';
		strncpy(spec_buf, fmt, spec_length);

		auto pUnprocessedSuffix = pSpecCode;
		int numCharsWantedToBeWritten = 0;
		switch (*pSpecCode)
		{
		case 'f': // fallthrough
		case 'e': // fallthrough
		case 'g': // fallthrough
		case 'G': // fallthrough
		case 'E':
		case 'a':
		case 'A':
			switch (FmtSpecLength)
			{
			case EFmtSpecLength::Default:
			{
				auto float_arg = static_cast<float>(arg);
				numCharsWantedToBeWritten = snprintf(pBuffer, buf_size, spec_buf, float_arg);
			}
			break;

			default:
				BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars: wrong length specifier");
			}
			break;

		default:
			BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: unknown specifier code");
			break;
		}
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten >= 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: wrong code returned from snprintf");
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten < buf_size, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer is not enough");
		// Increment unprocessed suffix poitner once (for codes that include more than one chars additional increments are to be used)
		pUnprocessedSuffix++;
		return std::pair<const char*, int>(pUnprocessedSuffix, numCharsWantedToBeWritten);
	}

	template<>
	[[nodiscard]]
	std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<long long>(const char* fmt, char* pBuffer, size_t buf_size, long long arg)
	{
		BOOST_ASSERT_MSG(*fmt == '%', "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: the fmt string must start with '%'");
		BOOST_ASSERT_MSG(pBuffer, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer pointer may NOT be nullptr");

		// WARNING!!! Checking that buffer is capable to store more than once char is needed for %% fmt spec processing
		BOOST_ASSERT_MSG(buf_size > 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer must be enough to store at least one char");

		auto[pSpecCode, FmtSpecLength] = FindFmtSpecCode(fmt, pBuffer, buf_size);
		char spec_buf[64];
		auto spec_length = (pSpecCode - fmt) + 1;
		spec_buf[spec_length] = '\0';
		strncpy(spec_buf, fmt, spec_length);

		auto pUnprocessedSuffix = pSpecCode;
		int numCharsWantedToBeWritten = 0;
		switch (*pSpecCode)
		{
		case 'd': // fallthrough
		case 'i': // fallthrough
		case 'o': // fallthrough
		case 'x': // fallthrough
		case 'X':
			switch (FmtSpecLength)
			{
			case EFmtSpecLength::Default:
			case EFmtSpecLength::Long:
			case EFmtSpecLength::Short:
			case EFmtSpecLength::ShortShort:
			case EFmtSpecLength::SizeT:
			{
				BOOST_ASSERT_MSG(false, "snprintf_return_numWritten: %ll length spec must be specified when passing long long");
			}
			break;

			case EFmtSpecLength::LongLong:
			{
				numCharsWantedToBeWritten = snprintf(pBuffer, buf_size, fmt, arg);
			}
			break;

			default:
				BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars: wrong length specifier");
			}
			break;

		default:
			BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: unknown specifier code");
			break;
		}
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten >= 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: wrong code returned from snprintf");
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten < buf_size, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer is not enough");
		// Increment unprocessed suffix poitner once (for codes that include more than one chars additional increments are to be used)
		pUnprocessedSuffix++;
		return std::pair<const char*, int>(pUnprocessedSuffix, numCharsWantedToBeWritten);
	}

	template<>
	[[nodiscard]]
	std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<long>(const char* fmt, char* pBuffer, size_t buf_size, long arg)
	{
		BOOST_ASSERT_MSG(*fmt == '%', "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: the fmt string must start with '%'");
		BOOST_ASSERT_MSG(pBuffer, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer pointer may NOT be nullptr");

		// WARNING!!! Checking that buffer is capable to store more than once char is needed for %% fmt spec processing
		BOOST_ASSERT_MSG(buf_size > 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer must be enough to store at least one char");

		auto[pSpecCode, FmtSpecLength] = FindFmtSpecCode(fmt, pBuffer, buf_size);
		char spec_buf[64];
		auto spec_length = (pSpecCode - fmt) + 1;
		spec_buf[spec_length] = '\0';
		strncpy(spec_buf, fmt, spec_length);

		auto pUnprocessedSuffix = pSpecCode;
		int numCharsWantedToBeWritten = 0;
		switch (*pSpecCode)
		{
		case 'd': // fallthrough
		case 'i': // fallthrough
		case 'o': // fallthrough
		case 'x': // fallthrough
		case 'X':
			switch (FmtSpecLength)
			{
			case EFmtSpecLength::Default:
			case EFmtSpecLength::LongLong:
			case EFmtSpecLength::Short:
			case EFmtSpecLength::ShortShort:
			case EFmtSpecLength::SizeT:
			{
				BOOST_ASSERT_MSG(false, "snprintf_return_numWritten: %l length spec must be specified when passing long");
			}
			break;

			case EFmtSpecLength::Long:
			{
				numCharsWantedToBeWritten = snprintf(pBuffer, buf_size, fmt, arg);
			}
			break;

			default:
				BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars: wrong length specifier");
			}
			break;

		default:
			BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: unknown specifier code");
			break;
		}
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten >= 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: wrong code returned from snprintf");
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten < buf_size, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer is not enough");
		// Increment unprocessed suffix poitner once (for codes that include more than one chars additional increments are to be used)
		pUnprocessedSuffix++;
		return std::pair<const char*, int>(pUnprocessedSuffix, numCharsWantedToBeWritten);
	}

	template<>
	[[nodiscard]]
	std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<short>(const char* fmt, char* pBuffer, size_t buf_size, short arg)
	{
		BOOST_ASSERT_MSG(*fmt == '%', "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: the fmt string must start with '%'");
		BOOST_ASSERT_MSG(pBuffer, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer pointer may NOT be nullptr");

		// WARNING!!! Checking that buffer is capable to store more than once char is needed for %% fmt spec processing
		BOOST_ASSERT_MSG(buf_size > 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer must be enough to store at least one char");

		auto[pSpecCode, FmtSpecLength] = FindFmtSpecCode(fmt, pBuffer, buf_size);
		char spec_buf[64];
		auto spec_length = (pSpecCode - fmt) + 1;
		spec_buf[spec_length] = '\0';
		strncpy(spec_buf, fmt, spec_length);

		auto pUnprocessedSuffix = pSpecCode;
		int numCharsWantedToBeWritten = 0;
		switch (*pSpecCode)
		{
		case 'd': // fallthrough
		case 'i': // fallthrough
		case 'o': // fallthrough
		case 'x': // fallthrough
		case 'X':
			switch (FmtSpecLength)
			{
			case EFmtSpecLength::Default:
			case EFmtSpecLength::Long:
			case EFmtSpecLength::LongLong:
			case EFmtSpecLength::ShortShort:
			case EFmtSpecLength::SizeT:
				{
					BOOST_ASSERT_MSG(false, "snprintf_return_numWritten: %h length spec must be specified when passing short");
				}
				break;

			case EFmtSpecLength::Short:
			{
				numCharsWantedToBeWritten = snprintf(pBuffer, buf_size, fmt, arg);
			}
			break;

			default:
				BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars: wrong length specifier");
			}
			break;

		default:
			BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: unknown specifier code");
			break;
		}
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten >= 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: wrong code returned from snprintf");
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten < buf_size, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer is not enough");
		// Increment unprocessed suffix poitner once (for codes that include more than one chars additional increments are to be used)
		pUnprocessedSuffix++;
		return std::pair<const char*, int>(pUnprocessedSuffix, numCharsWantedToBeWritten);
	}

	template<>
	[[nodiscard]]
	std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<char>(const char* fmt, char* pBuffer, size_t buf_size, char arg)
	{
		BOOST_ASSERT_MSG(*fmt == '%', "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: the fmt string must start with '%'");
		BOOST_ASSERT_MSG(pBuffer, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer pointer may NOT be nullptr");

		// WARNING!!! Checking that buffer is capable to store more than once char is needed for %% fmt spec processing
		BOOST_ASSERT_MSG(buf_size > 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer must be enough to store at least one char");

		auto[pSpecCode, FmtSpecLength] = FindFmtSpecCode(fmt, pBuffer, buf_size);
		char spec_buf[64];
		auto spec_length = (pSpecCode - fmt) + 1;
		spec_buf[spec_length] = '\0';
		strncpy(spec_buf, fmt, spec_length);

		auto pUnprocessedSuffix = pSpecCode;
		int numCharsWantedToBeWritten = 0;
		switch (*pSpecCode)
		{
		case 'c':
			switch (FmtSpecLength)
			{
			case EFmtSpecLength::Default:
			{
				numCharsWantedToBeWritten = snprintf(pBuffer, buf_size, spec_buf, arg);
			}
			break;

			default:
				BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars: wrong length specifier for c");
			}
			break;

		default:
			BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: wrong specifier code");
			break;
		}
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten >= 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: wrong code returned from snprintf");
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten < buf_size, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer is not enough");
		// Increment unprocessed suffix poitner once (for codes that include more than one chars additional increments are to be used)
		pUnprocessedSuffix++;
		return std::pair<const char*, int>(pUnprocessedSuffix, numCharsWantedToBeWritten);
	}

	template<>
	[[nodiscard]]
	std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<unsigned long>(const char* fmt, char* pBuffer, size_t buf_size, unsigned long arg)
	{
		BOOST_ASSERT_MSG(*fmt == '%', "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: the fmt string must start with '%'");
		BOOST_ASSERT_MSG(pBuffer, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer pointer may NOT be nullptr");

		// WARNING!!! Checking that buffer is capable to store more than once char is needed for %% fmt spec processing
		BOOST_ASSERT_MSG(buf_size > 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer must be enough to store at least one char");

		auto[pSpecCode, FmtSpecLength] = FindFmtSpecCode(fmt, pBuffer, buf_size);
		char spec_buf[64];
		auto spec_length = (pSpecCode - fmt) + 1;
		spec_buf[spec_length] = '\0';
		strncpy(spec_buf, fmt, spec_length);
		auto pUnprocessedSuffix = pSpecCode;
		int numCharsWantedToBeWritten = 0;
		switch (*pSpecCode)
		{
		case 'u': // fallthrough
		case 'o': // fallthrough
		case 'x': // fallthrough
		case 'X':
			switch (FmtSpecLength)
			{
			case EFmtSpecLength::Short: // fallthrough
			case EFmtSpecLength::LongLong: // fallthrough			
			case EFmtSpecLength::Default: // fallthrough
			case EFmtSpecLength::SizeT: // fallthrough	 
			case EFmtSpecLength::ShortShort:
				BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars: wrong length spec for unsigned long int");
				break;

			case EFmtSpecLength::Long: 
				{
					numCharsWantedToBeWritten = snprintf(pBuffer, buf_size, fmt, arg);
				}
				break;
			}
			break;

		default:
			BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: unknown specifier code");
			break;
		}
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten >= 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: wrong code returned from snprintf");
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten < buf_size, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer is not enough");
		// Increment unprocessed suffix poitner once (for codes that include more than one chars additional increments are to be used)
		pUnprocessedSuffix++;
		return std::pair<const char*, int>(pUnprocessedSuffix, numCharsWantedToBeWritten);
	}	

	template<>
	[[nodiscard]]
	std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<unsigned int>(const char* fmt, char* pBuffer, size_t buf_size, unsigned int arg)
	{
		BOOST_ASSERT_MSG(*fmt == '%', "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: the fmt string must start with '%'");
		BOOST_ASSERT_MSG(pBuffer, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer pointer may NOT be nullptr");

		// WARNING!!! Checking that buffer is capable to store more than once char is needed for %% fmt spec processing
		BOOST_ASSERT_MSG(buf_size > 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer must be enough to store at least one char");

		auto[pSpecCode, FmtSpecLength] = FindFmtSpecCode(fmt, pBuffer, buf_size);
		char spec_buf[64];
		auto spec_length = (pSpecCode - fmt) + 1;
		spec_buf[spec_length] = '\0';
		strncpy(spec_buf, fmt, spec_length);
		auto pUnprocessedSuffix = pSpecCode;
		int numCharsWantedToBeWritten = 0;
		switch (*pSpecCode)
		{
		case 'u': // fallthrough
		case 'o': // fallthrough
		case 'x': // fallthrough
		case 'X':
			switch (FmtSpecLength)
			{			
			case EFmtSpecLength::Long: // fallthrough
			case EFmtSpecLength::LongLong: // fallthrough
			case EFmtSpecLength::Short: // fallthrough
			case EFmtSpecLength::ShortShort:	// fallthrough		
				BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars: wrong length spec for unsigned int");
				break;

			case EFmtSpecLength::Default: // fallthrough
			case EFmtSpecLength::SizeT:
				{				
					numCharsWantedToBeWritten = snprintf(pBuffer, buf_size, fmt, arg);
				}
				break;
			}
			break;

		default:
			BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: unknown specifier code");
			break;
		}
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten >= 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: wrong code returned from snprintf");
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten < buf_size, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer is not enough");
		// Increment unprocessed suffix poitner once (for codes that include more than one chars additional increments are to be used)
		pUnprocessedSuffix++;
		return std::pair<const char*, int>(pUnprocessedSuffix, numCharsWantedToBeWritten);
	}
	template<>
	std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<int>(const char* fmt, char* pBuffer, size_t buf_size, int arg)
	{
		BOOST_ASSERT_MSG(*fmt == '%', "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: the fmt string must start with '%'");
		BOOST_ASSERT_MSG(pBuffer, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer pointer may NOT be nullptr");

		// WARNING!!! Checking that buffer is capable to store more than once char is needed for %% fmt spec processing
		BOOST_ASSERT_MSG(buf_size > 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer must be enough to store at least one char");

		auto[pSpecCode, FmtSpecLength] = FindFmtSpecCode(fmt, pBuffer, buf_size);
		char spec_buf[64];
		auto spec_length = (pSpecCode - fmt) + 1;
		spec_buf[spec_length] = '\0';
		strncpy(spec_buf, fmt, spec_length);

		auto pUnprocessedSuffix = pSpecCode;
		int numCharsWantedToBeWritten = 0;
		switch (*pSpecCode)
		{
		case 'd': // fallthrough
		case 'i': // fallthrough
		case 'o': // fallthrough
		case 'x': // fallthrough
		case 'X':
			switch (FmtSpecLength)
			{
			case EFmtSpecLength::Default:
			{
				//BOOST_ASSERT_MSG(std::is_convertible<typename T, int>::value, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: %d and %i specifiers require that argument is convertible to int");
				auto int_arg = static_cast<int>(arg);
				numCharsWantedToBeWritten = snprintf(pBuffer, buf_size, spec_buf, int_arg);
			}
			break;

			case EFmtSpecLength::Long:
			{
				//BOOST_ASSERT_MSG(std::is_convertible<T, long int>::value, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: %ld and %li specifiers require that argument is convertible to long int");
				auto long_int_arg = static_cast<long int>(arg);
				numCharsWantedToBeWritten = snprintf(pBuffer, buf_size, spec_buf, long_int_arg);
			}
			break;

			case EFmtSpecLength::LongLong:
			{
				//BOOST_ASSERT_MSG(std::is_convertible<T, long long int>::value, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: %lld and %lli specifiers require that argument is convertible to long long int");
				auto long_long_int_arg = static_cast<long long int>(arg);
				numCharsWantedToBeWritten = snprintf(pBuffer, buf_size, spec_buf, long_long_int_arg);
			}
			break;


			case EFmtSpecLength::Short:
			{
				//BOOST_ASSERT_MSG(std::is_convertible<T, short int>::value, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: %hd and %hi specifiers require that argument is convertible to short int");
				auto short_int_arg = static_cast<short int>(arg);
				numCharsWantedToBeWritten = snprintf(pBuffer, buf_size, fmt, short_int_arg);
			}
			break;

			case EFmtSpecLength::ShortShort:
			{
				//BOOST_ASSERT_MSG(std::is_convertible<T, signed char>::value, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: %hhd and %hhi specifiers require that argument is convertible to signed char");
				auto signed_char_arg = static_cast<signed char>(arg);
				numCharsWantedToBeWritten = snprintf(pBuffer, buf_size, fmt, signed_char_arg);
			}
			break;

			case EFmtSpecLength::SizeT:
			{	
				BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: %z length specifier requires that size_t(or unsigned int) is passed");
			}
			break;

			case EFmtSpecLength::PtrDiffT:
			{
				numCharsWantedToBeWritten = snprintf(pBuffer, buf_size, fmt, arg);
			}
			break;
	
			default:
				BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars: wrong length specifier");
			}
			break;

		default:
			BOOST_ASSERT_MSG(false, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: unknown specifier code");
			break;
		}
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten >= 0, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: wrong code returned from snprintf");
		BOOST_ASSERT_MSG(numCharsWantedToBeWritten < buf_size, "WriteFormattedWidthSpecCode_AndReturnUnprocessedSuffix: buffer is not enough");
		// Increment unprocessed suffix poitner once (for codes that include more than one chars additional increments are to be used)
		pUnprocessedSuffix++;
		return std::pair<const char*, int>(pUnprocessedSuffix, numCharsWantedToBeWritten);
	}
}