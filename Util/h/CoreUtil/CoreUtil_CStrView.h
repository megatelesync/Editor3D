#pragma once

#include <string>
#include <string_view>
#include <ostream>
#include "CoreUtil_Assert.h"
#include "CoreUtil_StringCoreUtils.h"

namespace Dv
{
	namespace Util
	{
		/***********************************************************************************
		* C-str iterator
		*
		* nullptr iterator is always end iterator
		***********************************************************************************/
		template<class CharT>
		class CStrIterator
		{
		public:
			using ThisType                            = typename CStrIterator<typename CharT>;
			using value_type                          = typename CharT;
			using difference_type                     = std::ptrdiff_t;
			using pointer                             = typename CharT *;
			using reference                           = typename CharT&;
			using const_reference                     = typename const CharT&;
			using iterator_category                   = std::random_access_iterator_tag;

			// default ctor: creates end() iterator
			constexpr CStrIterator() {}

			// We want to IMPLICITLY create iterator from char
			constexpr CStrIterator(CharT* pInPtr) : _ptr(_GetCorrectedPointer(pInPtr)) {}

			// copying (allowed)
			constexpr ThisType(const ThisType& right) = default;
			constexpr ThisType& operator=(const ThisType& right) = default;

			// moving
			constexpr ThisType(ThisType&& right) = default;
			constexpr ThisType& operator=(ThisType&& right) = default;

			// pointer (may return nullptr)
			constexpr pointer get_ptr() const { return _ptr; }

			// indexing (does NOT check that index is out of range)
			reference operator[](size_t index)
			{
				BOOST_ASSERT_MSG(_ptr, "CStrIterator<CharT>::operator[]: end() operator may NOT be dereferenced");
				return _ptr[index];
			}

			constexpr const_reference operator[](size_t index) const
			{
				BOOST_ASSERT_MSG(_ptr, "CStrIterator<CharT>::operator[]: end() operator may NOT be dereferenced");
				return _ptr[index];
			}

			// dereferencing
			reference operator*()
			{
				BOOST_ASSERT_MSG(_ptr, "CStrIterator<CharT>::operator*: end() operator may NOT be dereferenced");
				return *_ptr;
			}

			const_reference operator*() const 
			{
				BOOST_ASSERT_MSG(_ptr, "CStrIterator<CharT>::operator*: end() operator may NOT be dereferenced");
				return *_ptr; 
			}

			// Increment
			ThisType& operator+=(size_t n)
			{
				_ptr += n;
				_InvalidatePointerIfEndReached();
				return *this;
			}

			ThisType& operator++()
			{
				++_ptr; 
				_InvalidatePointerIfEndReached();
				return *this; 
			}

			ThisType operator++(int)
			{
				auto oldValue = *this;
				++_ptr;
				_InvalidatePointerIfEndReached();
				return oldValue;
			}

			// Decrement
			ThisType& operator-=(size_t n)
			{
				_ptr -= n;
				_InvalidatePointerIfEndReached();
				return *this;
			}

			ThisType& operator--()
			{
				--_ptr;
				_InvalidatePointerIfEndReached();
				return *this;
			}

			ThisType operator--(int)
			{
				auto oldValue = *this;
				--_ptr;
				_InvalidatePointerIfEndReached();
				return oldValue;
			}

			// Random access operations (WARNING no checking of passed-end situation!!!)
			constexpr ThisType operator+(size_t n) const
			{
				return ThisType(left._ptr + n);
			}

			constexpr ThisType operator-(size_t n) const
			{
				return ThisType(left._ptr - n);
			}

		private:
			constexpr CharT* _GetCorrectedPointer(CharT* pInPtr) const
			{
				if (pInPtr && *pInPtr == '\0')
				{
					return nullptr;
				}
				else
				{
					return pInPtr;
				}
			}

			void _InvalidatePointerIfEndReached()
			{
				_ptr = _GetCorrectedPointer(_ptr);
			}

			CharT* _ptr = nullptr;
		};

		// equality
		template<class CharT>
		constexpr bool operator==(CStrIterator<CharT> left, CStrIterator<CharT> right);

		template<class CharT>
		constexpr bool operator!=(CStrIterator<CharT> left, CStrIterator<CharT> right);

		template<class CharT>
		constexpr bool operator<(CStrIterator<CharT> left, CStrIterator<CharT> right);

		template<class CharT>
		constexpr bool operator>(CStrIterator<CharT> left, CStrIterator<CharT> right);

		template<class CharT>
		constexpr bool operator<=(CStrIterator<CharT> left, CStrIterator<CharT> right);

		template<class CharT>
		constexpr bool operator>=(CStrIterator<CharT> left, CStrIterator<CharT> right);

		template<class CharT>
		constexpr CStrIterator<CharT> operator+(size_t n, CStrIterator<CharT> it);

		template<class CharT>
		constexpr typename CStrIterator<CharT>::difference_type operator-(CStrIterator<CharT> left, CStrIterator<CharT> right);

		template<class CharT>
		constexpr CStrIterator<CharT> operator+(size_t n, CStrIterator<CharT> it)
		{
			return operator+(it, n);
		}

		template<class CharT>
		constexpr typename CStrIterator<CharT>::difference_type operator-(CStrIterator<CharT> left, CStrIterator<CharT> right)
		{
			return CStrIterator<CharT>(left.get_ptr() - right.get_ptr());
		}

		/***********************************************************************************
		* Equality operations implementation
		***********************************************************************************/
		template<class CharT>
		constexpr bool operator==(CStrIterator<CharT> left, CStrIterator<CharT> right)
		{
			return left.get_ptr() == right.get_ptr();
		}

		template<class CharT>
		constexpr bool operator!=(CStrIterator<CharT> left, CStrIterator<CharT> right)
		{
			return (false == operator==(left, right));
		}

		template<class CharT>
		constexpr bool operator<(CStrIterator<CharT> left, CStrIterator<CharT> right)
		{
			return left.get_ptr() < right.get_ptr();
		}

		template<class CharT>
		constexpr bool operator>(CStrIterator<CharT> left, CStrIterator<CharT> right)
		{
			return left.get_ptr() > right.get_ptr();
		}

		template<class CharT>
		constexpr bool operator<=(CStrIterator<CharT> left, CStrIterator<CharT> right)
		{
			return left.get_ptr() <= right.get_ptr();
		}

		template<class CharT>
		constexpr bool operator>=(CStrIterator<CharT> left, CStrIterator<CharT> right)
		{
			return left.get_ptr() >= right.get_ptr();
		}

		/***********************************************************************************
		* Basic C-str view
		***********************************************************************************/
		template<class CharT>
		class BasicCStrView
		{
		public:
			// type aliases
			using ThisType                          = typename BasicCStrView<typename CharT>;
			using value_type                        = typename CharT;
			using traits_type                       = typename std::char_traits<CharT>;
			using reference                         = typename CharT&;
			using const_reference                   = typename const CharT&;
			using pointer                           = typename CharT *;
			using const_pointer                     = typename CStrIterator<const CharT>;
			using iterator                          = typename CStrIterator<CharT>;
			using const_iterator                    = const_pointer;
			using reverse_iterator                  = std::reverse_iterator<iterator>;
			using const_reverse_iterator            = std::reverse_iterator<const_iterator>;
			using difference_type                   = ptrdiff_t;
			using size_type                         = size_t;
			
			// condition
			using define_const_function = std::enable_if<false == std::is_const<typename CharT>::value>;

			// default ctor: creates uninitialized view
			constexpr BasicCStrView() {};

			// copy-assignment: allowed
			constexpr ThisType(const ThisType&) = default;
			constexpr ThisType& operator=(const ThisType&) = default;

			// moving: allowed
			constexpr ThisType(ThisType&&) = default;
			constexpr ThisType& operator=(ThisType&&) = default;

			// copy-assignment from views
			template<class SourceCharT, template<class> class CharTraitsT>
			ThisType& operator=(std::basic_string_view<SourceCharT, CharTraitsT<SourceCharT>> view);

			// construction by pointer - performed implicitly
			constexpr BasicCStrView(CharT *pInCStr) : _ptr(pInCStr) {}

			// construction by view - only explicitly
			template<class SourceCharT, template<class> class CharTraitsT>
			explicit constexpr ThisType(std::basic_string_view<SourceCharT, CharTraitsT<SourceCharT>> view);

			// C-str must be implicitly-convertible to c_str()
			constexpr operator CharT*() { return c_str(); }

			// C-str must be implicitly convertible to string-views:
			// WARNING!!! String view wil be created automatically from the C-str, because it created implicitly from CharT*.

			// iteration: warning, custom iterator is to be coded,
			// because end() iterator is TOO expensive - iterates ALL characters before reaching end
			// (so the for-each loop will be executed TWICE in this case!!!)
			constexpr iterator end() { return iterator(); }
			constexpr iterator begin() { return _ptr; }

			constexpr const_iterator end() const { return iterator(); }
			constexpr const_iterator begin() const { return _ptr; }

			constexpr const_iterator cend() const { return iterator(); }
			constexpr const_iterator cbegin() const { return _ptr; }

			// c_str(): checks that string is initialized
			[[nodiscard]]
			constexpr CharT* c_str() const;

			// data: does NOT check that string is initialized (i.e. pointer is NOT nullptr)
			[[nodiscard]]
			constexpr CharT* data() const { return _ptr; }

			// length: warning: O(N);
			// works well when string is nullptr;
			[[nodiscard]]
			constexpr size_type size() const;
			[[nodiscard]]
			constexpr size_type length() const { return size(); }
			[[nodiscard]]
			constexpr bool empty() const { return size() == 0; }

			// indexing
			[[nodiscard]]
			constexpr CharT at(size_type index) const;
			[[nodiscard]]
			constexpr CharT operator[] (size_type index) const;

			[[nodiscard]]
			reference at(size_type index);
			[[nodiscard]]
			reference operator[] (size_type index);

			// getting first and last
			[[nodiscard]]
			constexpr reference front() const;
			[[nodiscard]]
			constexpr reference back() const;

			// helper functions:
			// is index valid: checks well when string is nullptr;
			[[nodiscard]]
			constexpr bool is_index_valid(size_type index) const { return index >= 0 && index < length(); }

			// getting pointers
			[[nodiscard]]
			constexpr pointer get_char_ptr(size_type index) const;

			// compare
			template<template<class> class CharTraitsT>
			constexpr int compare(std::basic_string_view<CharT, CharTraitsT<CharT>>) const;

			// substr
			constexpr ThisType substr(size_t pos) const { return ThisType(get_char_ptr(pos)); }

			// copying to external buffer
			size_type copy(CharT* dest, size_type count, size_type pos = 0) const;

			// Assigment (automatically truncates string, if necessary with '\0');
			// Copies string of n characters and stores by the view pointer
			template<typename = typename define_const_function>
			ThisType& assign(CharT c, size_t n);

			// constants
			const static size_type npos = -1;

		private:
			CharT* _ptr = nullptr;
		};
		using CStrView = BasicCStrView<const char>;
		using WCStrView = BasicCStrView<const wchar_t>;
		using CStrViewMutable = BasicCStrView<char>;
		using WCStrViewMutable = BasicCStrView<wchar_t>;

		// Output operator
		template<class StrmType, class CharT>
		StrmType& operator<<(StrmType& Strm, const BasicCStrView<CharT>& InView);

		// equality operators: perform char-by-char comparison
		template<class CharT>
		constexpr bool operator==(BasicCStrView<CharT> left, BasicCStrView<CharT> right);

		template<class CharT>
		constexpr bool operator!=(BasicCStrView<CharT> left, BasicCStrView<CharT> right);

		template<class CharT>
		constexpr bool operator<(BasicCStrView<CharT> left, BasicCStrView<CharT> right);

		template<class CharT>
		constexpr bool operator>(BasicCStrView<CharT> left, BasicCStrView<CharT> right);

		template<class CharT>
		constexpr bool operator==(const char *left, BasicCStrView<CharT> right);

		template<class CharT>
		constexpr bool operator!=(const char *left, BasicCStrView<CharT> right);

		template<class CharT>
		constexpr bool operator<(const char *left, BasicCStrView<CharT> right);

		template<class CharT>
		constexpr bool operator>(const char *left, BasicCStrView<CharT> right);

		template<class CharT>
		constexpr bool operator==(BasicCStrView<CharT> left, const char *right);

		template<class CharT>
		constexpr bool operator!=(BasicCStrView<CharT> left, const char *right);

		template<class CharT>
		constexpr bool operator<(BasicCStrView<CharT> left, const char *right);

		template<class CharT>
		constexpr bool operator>(BasicCStrView<CharT> left, const char *right);

		/**************************************************************************************
		* Global functions implementation
		***************************************************************************************/
		template<class CharT>
		constexpr bool operator==(BasicCStrView<CharT> left, BasicCStrView<CharT> right)
		{
			return constexpr_strequal(left.data(), right.data());
		}

		template<class CharT>
		constexpr bool operator!=(BasicCStrView<CharT> left, BasicCStrView<CharT> right)
		{
			return (false == (operator==(left, right)));
		}

		template<class CharT>
		constexpr bool operator<(BasicCStrView<CharT> left, BasicCStrView<CharT> right)
		{
			return constexpr_strcmp(left.data(), right.data()) < 0;
		}

		template<class CharT>
		constexpr bool operator>(BasicCStrView<CharT> left, BasicCStrView<CharT> right)
		{
			return constexpr_strcmp(left.data(), right.data()) > 0;
		}

		template<class CharT>
		constexpr bool operator==(BasicCStrView<CharT> left, const char *right) 
		{
			return operator==(left, BasicCStrView<CharT>(right));
		}

		template<class CharT>
		constexpr bool operator!=(BasicCStrView<CharT> left, const char *right)
		{
			return operator!=(left, BasicCStrView<CharT>(right));
		}

		template<class CharT>
		constexpr bool operator<(BasicCStrView<CharT> left, const char *right)
		{
			return operator<(left, BasicCStrView<CharT>(right));
		}

		template<class CharT>
		constexpr bool operator>(BasicCStrView<CharT> left, const char *right)
		{
			return operator>(left, BasicCStrView<CharT>(right));
		}

		template<class CharT>
		constexpr bool operator==(const char *left, BasicCStrView<CharT> right)
		{
			return operator==(BasicCStrView<CharT>(left), right);
		}

		template<class CharT>
		constexpr bool operator!=(const char *left, BasicCStrView<CharT> right)
		{
			return operator!=(BasicCStrView<CharT>(left), right);
		}

		template<class CharT>
		constexpr bool operator<(const char *left, BasicCStrView<CharT> right)
		{
			return operator<(BasicCStrView<CharT>(left), right);
		}

		template<class CharT>
		constexpr bool operator>(const char *left, BasicCStrView<CharT> right)
		{
			return operator>(BasicCStrView<CharT>(left), right);
		}

		/**************************************************************************************
		* implementation
		***************************************************************************************/
		template<class CharT>
		template<class SourceCharT, template<class> class CharTraitsT>
		constexpr BasicCStrView<CharT>::BasicCStrView(std::basic_string_view<SourceCharT, CharTraitsT<SourceCharT>> view) :
			_ptr(view.data())
		{
			BOOST_ASSERT_MSG(view.data()[view.length()] == '\0', "BasicCStrView: source view must contain '\0' ending string");
		}

		template<class CharT>
		template<class SourceCharT, template<class> class CharTraitsT>
		typename BasicCStrView<CharT>::ThisType& BasicCStrView<CharT>::operator=(std::basic_string_view<SourceCharT, CharTraitsT<SourceCharT>> view)
		{
			BOOST_ASSERT_MSG(view.data()[view.length()] == '\0', "BasicCStrView: source view must contain '\0' ending string");
			_ptr = view.data();
			return *this;
		}

		template<class CharT>		
		constexpr CharT* BasicCStrView<CharT>::c_str() const
		{
			BOOST_ASSERT_MSG(_ptr, "Dv::Util::BasicCStrView::c_str(): string must be initialized"); 
			return _ptr; 
		}

		template<class StrmType, class CharT>
		StrmType& operator<<(StrmType& Strm, const BasicCStrView<CharT>& InView)
		{
			auto pNullOrString = InView.data();
			if (pNullOrString == nullptr)
			{
				Strm << "{nullptr}";
			}
			else
			{
				Strm << pNullOrString;
			}
			return Strm;
		}

		template<class CharT>
		constexpr typename BasicCStrView<CharT>::size_type BasicCStrView<CharT>::size() const
		{
			if (nullptr == _ptr)
			{
				return 0;
			}
			size_type result_length = 0;
			for (auto pCurr = _ptr; *pCurr != '\0'; pCurr++) { result_length++; }
			return result_length;
		}

		template<class CharT>
		constexpr CharT BasicCStrView<CharT>::at(typename BasicCStrView<CharT>::size_type index) const
		{
			BOOST_ASSERT_MSG(is_index_valid(index), "BasicCStrView<CharT>::at: invalid index");
			return _ptr[index];
		}

		template<class CharT>
		constexpr CharT BasicCStrView<CharT>::operator[] (typename BasicCStrView<CharT>::size_type index) const
		{
			BOOST_ASSERT_MSG(is_index_valid(index), "BasicCStrView<CharT>::operator[]: invalid index");
			return _ptr[index];
		}

		template<class CharT>
		typename BasicCStrView<CharT>::reference BasicCStrView<CharT>::at(typename BasicCStrView<CharT>::size_type index)
		{
			BOOST_ASSERT_MSG(is_index_valid(index), "BasicCStrView<CharT>::at: invalid index");
			return _ptr[index];
		}

		template<class CharT>
		typename BasicCStrView<CharT>::reference BasicCStrView<CharT>::operator[] (typename BasicCStrView<CharT>::size_type index)
		{
			BOOST_ASSERT_MSG(is_index_valid(index), "BasicCStrView<CharT>::operator[]: invalid index");
			return _ptr[index];
		}

		template<class CharT>
		constexpr typename BasicCStrView<CharT>::pointer BasicCStrView<CharT>::get_char_ptr(size_type index) const
		{
			BOOST_ASSERT_MSG(is_index_valid(index), "BasicCStrView<CharT>::get_char_ptr: invalid index");
			return _ptr + index;
		}

		template<class CharT>
		constexpr typename BasicCStrView<CharT>::reference BasicCStrView<CharT>::front() const
		{
			return *c_str();
		}

		template<class CharT>
		constexpr typename BasicCStrView<CharT>::reference BasicCStrView<CharT>::back() const
		{
			BOOST_ASSERT_MSG(_ptr, "BasicCStrView<CharT>::back: string may NOT be nullptr");
			BOOST_ASSERT_MSG(false == empty(), "BasicCStrView<CharT>::back: string may NOT be empty");
			return *get_char_ptr(size() - 1);
		}
		template<class CharT>
		template<typename = typename define_const_function>
		typename BasicCStrView<CharT>::ThisType& BasicCStrView<CharT>::assign(CharT c, size_t n)
		{
			BOOST_ASSERT_MSG(n <= length(), "BasicCStrView<CharT>::assign: string buffer is too small or null");
			std::fill(_ptr, _ptr + n, c);
			return *this;
		}

		template<class CharT>
		typename BasicCStrView<CharT>::size_type BasicCStrView<CharT>::copy(CharT* dest, size_type count, size_type pos) const
		{
			BOOST_ASSERT_MSG(dest, "BasicCStrView<CharT>::copy: output buffer may NOT be nullptr");
			BOOST_ASSERT_MSG((false == std::find(_ptr, _ptr + pos, '\0')), "BasicCStrView<CharT>::copy: source string index is out of range");
			return copy_c_subtr(dest, _ptr, pos, count);
		}

		template<class CharT>
		template<template<class> class CharTraitsT>
		constexpr int BasicCStrView<CharT>::compare(std::basic_string_view<CharT, CharTraitsT<CharT>> right) const
		{
			return constexpr_strcmp(left, right.data());
		}
	} // Util
}// DV