#pragma once

#include <string>
#include <string_view>
#include "CoreUtil_StringCoreUtils.h"
#include "CoreUtil_CStrView.h"

namespace Dv
{
	namespace Util
	{
		/*************************************************************************************************************
		* Fmt
		**************************************************************************************************************/		
		// Type-safe implementation of snprintf
		// WARNING!!! Out version of snprintf returns count of WRITTEN chars (excluding the end character!!!), NOT count of chars that would be written;
		// Always checks with assert, that buffer is enough!!!
		template<class FirstArgT, class... Args>
		[[nodiscard]]
		int snprintf_return_numWritten(char *pBuffer, size_t buf_size, const char* fmt, FirstArgT arg0, Args&&... args);

		[[nodiscard]]
		int snprintf_return_numWritten(char *pBuffer, size_t buf_size, const char* fmt);

		enum class EFmtSpecLength
		{
			Default = 0,
			Short,
			ShortShort,
			Long,
			LongLong,
			SizeT,
			PtrDiffT
		};

		// Finds first format spec or end of the string (meaning pointer to the '\0' character),
		// and copies all other chars before the given.
		[[nodiscard]]
		const char* FindFmtSpec(const char* fmt);

		// Find the format spec code substring (f, d,  or other alpha-numering printf-type code);
		// WARNING!!! Checked with asssert that the format code exists in the string;
		[[nodiscard]]
		std::pair<const char*, EFmtSpecLength> FindFmtSpecCode(const char* fmt, char* pBuffer, size_t buf_size);

		// Checks that the given argument conforms to the fmt spec code (that is pointed to the fmt),
		// outputs formatted arg into the given buffer;
		// and returns the unprocessed suffix of the fmt string (that is started right after the given specified)
		template<class T>
		[[nodiscard]]
		std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars(const char* fmt, char* pBuffer, size_t buf_size, T arg);

		template<>
		[[nodiscard]]
		std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<long long>(const char* fmt, char* pBuffer, size_t buf_size, long long arg);

		template<>
		[[nodiscard]]
		std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<long>(const char* fmt, char* pBuffer, size_t buf_size, long arg);

		template<>
		[[nodiscard]]
		std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<short>(const char* fmt, char* pBuffer, size_t buf_size, short arg);


		template<>
		[[nodiscard]]
		std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<int>(const char* fmt, char* pBuffer, size_t buf_size, int arg);

		template<>
		[[nodiscard]]
		std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<unsigned int>(const char* fmt, char* pBuffer, size_t buf_size, unsigned int arg);

		template<>
		[[nodiscard]]
		std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<unsigned long>(const char* fmt, char* pBuffer, size_t buf_size, unsigned long arg);		

		template<>
		[[nodiscard]]
		std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<float>(const char* fmt, char* pBuffer, size_t buf_size, float arg);

		template<>
		[[nodiscard]]
		std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<double>(const char* fmt, char* pBuffer, size_t buf_size, double arg);

		template<>
		[[nodiscard]]
		std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<const char*>(const char* fmt, char* pBuffer, size_t buf_size, const char* arg);

		template<>
		[[nodiscard]]
		std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<std::string_view>(const char* fmt, char* pBuffer, size_t buf_size, std::string_view arg);

		template<>
		[[nodiscard]]
		std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<std::string>(const char* fmt, char* pBuffer, size_t buf_size, std::string arg);

		template<>
		[[nodiscard]]
		std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<char>(const char* fmt, char* pBuffer, size_t buf_size, char arg);

		template<>
		[[nodiscard]]
		std::pair<const char*, int> WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars<wint_t>(const char* fmt, char* pBuffer, size_t buf_size, wint_t arg);

		/**************************************************************************************************
		* Implementation		
		**************************************************************************************************/
		inline std::pair<const char *, int> copy_until_non_quote_spec_found_return_pair_unprocessed_suffix_numWritten(char *pBuffer, size_t buf_size, const char* fmt)
		{
			size_t numWrittenChars = 0;
			auto curr_buf_size = buf_size;

			// start of the fmt string, that is NOT yet handled
			auto pUnprocessedSuffix = fmt;

			// Until found non-quote specifier, copy strings, and handle quote specifiers
			const char *pFmtSpec = nullptr;
			while (true)
			{
				pFmtSpec = FindFmtSpec(pUnprocessedSuffix);				

				// output part of the string from the last quote
				auto numCharsToWrite = (pFmtSpec - pUnprocessedSuffix);
				std::memcpy(pBuffer + numWrittenChars, pUnprocessedSuffix, numCharsToWrite);
				numWrittenChars += numCharsToWrite;
				curr_buf_size -= numCharsToWrite;

				if (*pFmtSpec == '\0')
				{
					pUnprocessedSuffix = pFmtSpec;
					pBuffer[numWrittenChars] = '\0';
					curr_buf_size--;
					break;
				}

				// If our specified is normal specifier (non-quote) specifier, then exit
				if (pFmtSpec[1] != '%')
				{
					pUnprocessedSuffix = pFmtSpec;
					break;
				}

				// At this point, the only possible case is the quote specifier %%				
				pBuffer[numWrittenChars] = '%';
				curr_buf_size--;
				numWrittenChars++;
				pUnprocessedSuffix = pFmtSpec + 2;
			}
			return std::make_pair(pUnprocessedSuffix, numWrittenChars);
		}

		inline int snprintf_return_numWritten(char *pBuffer, size_t buf_size, const char* fmt)
		{
			auto [pUnprocessedSuffix, numWritten] = copy_until_non_quote_spec_found_return_pair_unprocessed_suffix_numWritten(pBuffer, buf_size, fmt);
			BOOST_ASSERT_MSG(*pUnprocessedSuffix == '\0', "snprintf_return_numWritten: count of arguments is less than specifiers");
			return numWritten;
		}		

		template<class FirstArgT, class... Args>
		int snprintf_return_numWritten(char *pBuffer, size_t buf_size, const char* fmt, FirstArgT arg0, Args&&... args)
		{
			// Let's copy all chars and process all quote specifiers until the first non-quote specifier is found
			auto [pUnprocessedSuffix, numWrittenChars] = copy_until_non_quote_spec_found_return_pair_unprocessed_suffix_numWritten(pBuffer, buf_size, fmt);
			auto curr_buf_size = buf_size - numWrittenChars;
			
			// Process current non-quote specifier
			auto res = WriteFormattedWidthSpecCode_AndReturnPairOf_UnprocessedSuffixPtr_And_NumWrittenChars
			(
				pUnprocessedSuffix, pBuffer + numWrittenChars, curr_buf_size, arg0
			);
			pUnprocessedSuffix = res.first;
			auto curr_numWrittenChars = res.second;

			curr_buf_size -= curr_numWrittenChars;
			numWrittenChars += curr_numWrittenChars;

			// Process the rest of the arugments, if not end of the string
			if (*pUnprocessedSuffix == '\0')
			{
				// We found end of the string, so all our arguments must be processed
				BOOST_ASSERT_MSG((sizeof...(Args)) == 0, "snprintf_return_numWritten: excess of arguments");
				return numWrittenChars;
			}

			// WARNING!!! += must be used to calculate total number of written chars
			numWrittenChars += snprintf_return_numWritten(pBuffer + numWrittenChars, curr_buf_size, pUnprocessedSuffix, std::forward<Args>(args)...);
			return numWrittenChars;
		}

		/**************************************************************************************************
		* Null-terminated C-like string view
		* - With NO extra functionality (like length computation or hasing);
		* - WARNING!!! strlen() will use linear search;
		* - operator=() is NOT overloaded for sources of non-view types, 
		*   because of inconsistency (by value vs. by reference assignment);
		* - Can be easily used with C++ containers as a key;
		* - Should be passed by value (as it's only one pointer);
		* - Checks that index is NOT out of range with assert;
		* - NOT nullable;
		**************************************************************************************************/
		/*
		template<class C>
		class BasicCStrView
		{
		public:
			// type aliases
			using ThisType = typename BasicCStrView<typename C>;
			using value_type = typename C;
			using traits_type = typename std::char_traits<C>;
			using reference = typename C&;
			using const_reference = typename const C&;
			using pointer = typename C *;
			using const_pointer = typename const C *;
			using iterator = pointer;
			using const_iterator = const_pointer;
			using reverse_iterator = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;
			using difference_type = ptrdiff_t;
			using size_type = size_t;

			// Default ctor: creates uninitialized string (with nullptr)
			constexpr BasicCStrView() = default;

			// It should be implicitly createable from a C-str
			constexpr BasicCStrView(C* pInPtr) : _ptr(pInPtr)  { BOOST_ASSERT_MSG(pInPtr, "Dv::Util::BasicCStrView::ctor: pointer may NOT be nullptr"); }

			// Never should be created from rvalue std::string&&

			// Copying is allowed
			constexpr BasicCStrView(const ThisType&) = default;
			constexpr ThisType& operator(const ThisType&) = default;

			// Moving is allowed, of course
			constexpr BasicCStrView(ThisType&&) = default;
			constexpr ThisType& operator(ThisType&&) = default;

			// Special copy operations
			template
			<
				class TraitsT, class AllocatorT,
				class = typename std::enable_if<false == std::is_const<C>::value>::type
			>
			ThisType& operator=(const std::basic_string_view<C, TraitsT>& str);

			// assign (only for non-const character)
			template<class = typename std::enable_if<false == std::is_const<C>::value>::type>
			ThisType& assign(size_type count, C ch);

			template
			<
				class TraitsT, class AllocatorT,
				class = typename std::enable_if<false == std::is_const<C>::value>::type
			>
			ThisType& assign(const std::basic_string<C, TraitsT, AllocatorT>&);

			template
			<
				class TraitsT, class AllocatorT,
				class = typename std::enable_if<false == std::is_const<C>::value>::type
			>
			ThisType& assign
			(
				const std::basic_string<C, TraitsT, AllocatorT>& InString, size_type pos, size_type count
			);

			template<class = typename std::enable_if<false == std::is_const<C>::value>::type>
			ThisType& assign(const C* s, size_type count);

			template<class = typename std::enable_if<false == std::is_const<C>::value>::type>
			ThisType& assign(const C* s);

			template
			<
				class InputIt,
				class = typename std::enable_if<false == std::is_const<C>::value>::type
			>
			ThisType& assign(InputIt first, InputIt);

			template<class = typename std::enable_if<false == std::is_const<C>::value>::type>
			ThisType& assign(std::initializer_list<InputIt> iList);

			// iterator getters
			constexpr const_iterator cbegin() const { return _ptr; }
			const_iterator cend() const { return _ptr + size(); } // not constexpr: size()
			constexpr const_iterator begin() const { return cbegin(); }
			const_iterator end() const { return cend(); }
			iterator begin() { return _ptr; }
			iterator end() { return _ptr + size(); }

			// Getting data: implicitly convertible to C * using c_str()
			constexpr C* operator C* () const { return c_str(); }
			// c_str(): checks that string is initialized
			constexpr C* c_str() const { BOOST_ASSERT_MSG(_ptr, "Dv::Util::BasicCStrView::c_str(): string must be initialized"); return _ptr; }
			// get_ptr does NOT check that string is initialized (i.e. pointer is NOT nullptr)
			constexpr C* get_ptr() const { return _ptr; }

			bool empty() const { return (0 == size()); }
			
			const C& back() const { return _ptr[size()-1]; }
			C& back() { return _ptr[size() - 1]; }

			const C& front() const { return *_ptr; }
			C& front() { return *_ptr; }

			// size (not constexpr, because of loop)
			size_t size() const;
			size_t length() const { return size(); }

			// access operators
			constexpr C at(size_t InIndex) const { BOOST_ASSERT_MSG(_ptr, "Dv::Util::BasicCStrView:at pointer may NOT be nullptr"); return _ptr[InIndex]; }
			constexpr C operator[] (size_t InIndex) const { BOOST_ASSERT_MSG(_ptr, "Dv::Util::BasicCStrView:operator[] pointer may NOT be nullptr"); return _ptr[InIndex]; }

			// pointer getting operations
			constexpr C* get_ptr_at(size_t InIndex) const { BOOST_ASSERT_MSG(_ptr, "Dv::Util::BasicCStrView:get_ptr_at pointer may NOT be nullptr"); return _ptr + InIndex; }

			// substr: allows only to remove prefix
			constexpr ThisType substr(size_type pos = 0) const;

			// search (never constexpr, because of loop)
			size_t find(BasicCStrView InSubstr, size_type InPos = 0) const;
			size_t find(const std::basic_string<C>& InSubstr, size_type InPos = 0) const;
			size_t find(const std::basic_string_view<C>& InSubstr, size_type InPos = 0) const;
			size_t find(C InCh, size_type InPos = 0) const;
			size_t find(C InCh, size_type InPos, size_type InCount) const;
			size_t find(C InCh, size_type InPos = 0) const;

			size_t rfind(BasicCStrView InSubstr, size_type InPos = 0) const;
			size_t rfind(const std::basic_string<C>& InSubstr, size_type InPos = 0) const;
			size_t rfind(const std::basic_string_view<C>& InSubstr, size_type InPos = 0) const;
			size_t rfind(C InCh, size_type InPos = 0) const;
			size_t rfind(C InCh, size_type InPos, size_type InCount) const;
			size_t rfind(C InCh, size_type InPos = 0) const;

			//
			size_t find_first_of() const;
			size_t find_last_of() const;
			size_t find_first_not_of() const;
			size_t find_last_not_of() const;

			//
			constexpr int compare();

			// Swapping: for consistency with std::string_view and std::string
			constexpr void swap(BasicCStrView<C>& OutView);

			// copying (never constexpr, because of loop),
			// But does NOT append null char (to be consistent with std::string behaviour)
			size_type copy(C* pOutDest, size_type InCount, size_type InPos) const;
			// The same, but appends null character;
			size_type copy_null(C* pOutDest, size_type InCount, size_type InPos) const;

			// Removing prefix:
			// - Unlike std::string_view, unable to add prefix so that it passes end;
			void remove_prefix(size_type n);
			// Removing suffix (ONLY FOR non-const pointer):
			// (null character is set)
			void remove_suffix(size_type n);

			// static const
			static constexpr size_type npos = size_type(-1);

		private:
			CharT* _ptr = nullptr;
		};

		// Assignments operators
		template<class C>
		bool operator==(const BasicCStrView<C>&, const BasicCStrView<C>&);

		template<class C>
		bool operator==(const std::string&, const BasicCStrView<C>&);

		template<class C>
		bool operator==(const BasicCStrView<C>&, const std::basic_string<C>&);

		template<class C>
		bool operator==(const BasicCStrView<C>&, const std::basic_string_view<C>&);

		template<class C>
		bool operator==(const std::basic_string_view<C>&, const BasicCStrView<C>&);

		template<class C>
		bool operator!=(const BasicCStrView<C>&, const BasicCStrView<C>&);

		// Assignments operators
		template<class C>
		bool operator<(const BasicCStrView<C>&, const BasicCStrView<C>&);

		template<class C>
		bool operator>(const BasicCStrView<C>&, const BasicCStrView<C>&);

		template<class C>
		bool operator<=(const BasicCStrView<C>&, const BasicCStrView<C>&);

		template<class C>
		bool operator>=(const BasicCStrView<C>&, const BasicCStrView<C>&);

		// Concatenation operators
		template<class C>
		std::string operator+(const BasicCStrView<C>&, const BasicCStrView<C>&);

		template<class C>
		std::string operator+(const BasicCStrView<C>&, const std::string&);

		template<class C>
		std::string operator+(const std::string&, const BasicCStrView<C>&);

		// Hash 

		// Output and input extractors
		*/
	} // Util
} // Dv