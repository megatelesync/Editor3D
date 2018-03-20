#pragma once

#include <boost/assert.hpp>
#include <cassert>

namespace Dv
{
	namespace Util
	{
		// Asserts that it's invalid call and returns the given type
		// (type can be anything - including references)
		template<class ReturnType> ReturnType& InvalidCall(const char *pInMessage);
		void InvalidCallVoid(const char *pInMessage);

		template<class ReturnType> ReturnType& EmptyBlock(const char *pInMessage, ReturnType InValue);
		void EmptyBlockVoid(const char *pInMessage);

		template<class ReturnType> ReturnType& NotYetImpl(const char *pInMessage);
		void NotYetImplVoid(const char *pInMessage);

		template<class ReturnType> ReturnType& NotYetImpl_Silent(const char *pInMessage);
		void NotYetImplVoid_Silent(const char *pInMessage);

		void Note(const char *pInMessage);
		void TODO(const char *pInMessage);
		void Assert(bool bInCondition, const char *pInMessage);

		/***************************************************************************************
		* IMPLEMENTATION
		***************************************************************************************/
		inline void InvalidCallVoid(const char *pInMessage)
		{
			assert(pInMessage != nullptr);
			assert(!pInMessage);
		}
		template<class ReturnType> ReturnType& InvalidCall(const char *pInMessage)
		{
			assert(pInMessage != nullptr);
			assert(!pInMessage);

			static std::remove_reference<ReturnType>::type *val = nullptr;
			return *val;
		}

		template<class ReturnType> ReturnType& NotYetImpl(const char *pInMessage)
		{
			assert(pInMessage != nullptr);
			assert(!pInMessage);

			static std::remove_reference<ReturnType>::type *val = nullptr;
			return *val;
		}

		inline void NotYetImplVoid(const char *pInMessage)
		{
			BOOST_ASSERT(pInMessage);
		}

		inline void Assert(bool bInCondition, const char *pInMessage)
		{			
			BOOST_ASSERT_MSG(bInCondition, pInMessage);
		}

		template<class ReturnType> ReturnType& EmptyBlock(const char *pInMessage, ReturnType InValue)
		{
			return InValue;
		}

		inline void EmptyBlockVoid(const char *pInMessage)
		{
		}

		template<class ReturnType> ReturnType& NotYetImpl_Silent(const char *pInMessage)
		{
			static std::remove_reference<ReturnType>::type *val = nullptr;
			return *val;
		}
		inline void NotYetImplVoid_Silent(const char *pInMessage)
		{
			EmptyBlockVoid(pInMessage);
		}
		inline void Note(const char *pInMessage)
		{
			EmptyBlockVoid(pInMessage);
		}
		inline void TODO(const char *pInMessage)
		{
			EmptyBlockVoid(pInMessage);
		}
	} // Util
} // Dv