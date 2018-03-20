#ifndef _Util_UniquePtrIterator_h_
#define _Util_UniquePtrIterator_h_

#include <iterator>

namespace Dv
{
	namespace Util
	{
		// TODO:
		// 1. Type of the iterator tag should correspond the type of the implementing iterator +
		// 2. A freedom of choosing the iterator element type should be provided +
		// 3. Implement operations of the random access iterators +
		// 4. implement make operation
		//		4. Non-const +
		//		5. Const

		// Iterator to a container of std::unique_ptr<T>,
		// where T is pointer corresponding the type of the iterator
		// dereferencing this iterator gives a pointer of type  "ElementTypeArg *"
		// 

		// Template arguments:
		// 1. Iterator - type of the implementing iterator 
		// 2. ValueTypeArg - type of the value, to which the iterator will point to
		// Examples:
		//		(ISubsystem is derived from IObject)
		//		typedef std::list<std::unique_ptr<ISubsystem>> ContType;
		//		ContType subsystems;
		//		// a
		//		UniquePtrIterator<ContType::iterator,const ISubsystem> it = subsystems.cbegin();
		//		// b
		//		UniquePtrIterator<ContType::iterator,const IObject> it = subsystems.cbegin();
		

		template<class Iterator, class ValueTypeArg> class UniquePtrIterator : 
			public std::iterator<typename Iterator::iterator_category, typename Iterator::value_type::pointer>
		{
		public:				
			typedef						typename Iterator::value_type										UniquePtrType;
			typedef						UniquePtrIterator<Iterator,ValueTypeArg>							ThisType;			

			UniquePtrIterator								() {}
			UniquePtrIterator								(const Iterator& iteratorIn);

			value_type				operator*				()									const      { return _iterator->get(); }			
			// May be it's std violation, but it's impractical to return the pointer to pointer with operator->;
			// It does make sense to return a pointer itself			
			value_type				operator->				()									const	   { return _iterator->get(); }

			ThisType&				operator++				();
			ThisType&				operator--				();
			ThisType				operator++				(int);
			ThisType				operator--				(int);			

			Iterator				GetIterator				()									const		{ return _iterator; }

		private:
			Iterator				_iterator;
		};

		// Wrap the iterator to element of type std::unique_ptr<T> with a UniquePtrIterator class
		template<class Iterator>
		UniquePtrIterator<Iterator,typename Iterator::value_type::element_type> MakeUniquePtrIterator(const Iterator& it);

		template<class Iterator>
		UniquePtrIterator<Iterator,typename Iterator::value_type::element_type> MakeUniquePtrConstIterator(const Iterator& it);

		template<class Iterator, class ValueTypeArg>
		bool operator==(const UniquePtrIterator<Iterator,ValueTypeArg>& a, const UniquePtrIterator<Iterator,ValueTypeArg>& b);

		template<class Iterator, class ValueTypeArg>
		bool operator!=(const UniquePtrIterator<Iterator,ValueTypeArg>& a, const UniquePtrIterator<Iterator,ValueTypeArg>& b);

		template<class Iterator, class ValueTypeArg>
		bool operator<(const UniquePtrIterator<Iterator,ValueTypeArg>& a, const UniquePtrIterator<Iterator,ValueTypeArg>& b);		

		template<class Iterator, class ValueTypeArg>
		bool operator>(const UniquePtrIterator<Iterator,ValueTypeArg>& a, const UniquePtrIterator<Iterator,ValueTypeArg>& b);		

		template<class Iterator, class ValueTypeArg>
		bool operator<=(const UniquePtrIterator<Iterator,ValueTypeArg>& a, const UniquePtrIterator<Iterator,ValueTypeArg>& b);		

		template<class Iterator, class ValueTypeArg>
		bool operator>=(const UniquePtrIterator<Iterator,ValueTypeArg>& a, const UniquePtrIterator<Iterator,ValueTypeArg>& b);	

		template<class Iterator, class ValueTypeArg>
		typename UniquePtrIterator<Iterator,ValueTypeArg>::ThisType operator+(const UniquePtrIterator<Iterator,ValueTypeArg>& it, int shift);	

		template<class Iterator, class ValueTypeArg>
		typename UniquePtrIterator<Iterator,ValueTypeArg>::ThisType operator-(const UniquePtrIterator<Iterator,ValueTypeArg>& it, int shift);		
		

		// UniquePtrIterator function impl
		template<class Iterator>
		UniquePtrIterator<Iterator,typename Iterator::value_type::element_type> MakeUniquePtrIterator(const Iterator& it)
		{
			return UniquePtrIterator<Iterator, typename Iterator::value_type::element_type>(it);
		}

		template<class Iterator, class ValueTypeArg>
		bool operator==(const UniquePtrIterator<Iterator,ValueTypeArg>& a, const UniquePtrIterator<Iterator,ValueTypeArg>& b)
		{
			return a.GetIterator() == b.GetIterator();
		}

		template<class Iterator, class ValueTypeArg>
		bool operator!=(const UniquePtrIterator<Iterator,ValueTypeArg>& a, const UniquePtrIterator<Iterator,ValueTypeArg>& b)
		{
			return !(a == b);
		}

		template<class Iterator, class ValueTypeArg>
		typename UniquePtrIterator<Iterator,ValueTypeArg>::ThisType 
		operator+(const UniquePtrIterator<Iterator,ValueTypeArg>& it, int shift)
		{
			return typename UniquePtrIterator<Iterator,ValueTypeArg>::ThisType(it.GetIterator() + shift);
		}

		template<class Iterator, class ValueTypeArg>
		bool operator<(const UniquePtrIterator<Iterator,ValueTypeArg>& a, const UniquePtrIterator<Iterator,ValueTypeArg>& b)
		{
			return a.GetIterator() < b.GetIterator();
		}

		template<class Iterator, class ValueTypeArg>
		bool operator>(const UniquePtrIterator<Iterator,ValueTypeArg>& a, const UniquePtrIterator<Iterator,ValueTypeArg>& b)
		{
			return a.GetIterator() > b.GetIterator();
		}

		template<class Iterator, class ValueTypeArg>
		bool operator<=(const UniquePtrIterator<Iterator,ValueTypeArg>& a, const UniquePtrIterator<Iterator,ValueTypeArg>& b)
		{
			return a.GetIterator() <= b.GetIterator();
		}

		template<class Iterator, class ValueTypeArg>
		bool operator>=(const UniquePtrIterator<Iterator,ValueTypeArg>& a, const UniquePtrIterator<Iterator,ValueTypeArg>& b)
		{
			return a.GetIterator() >= b.GetIterator();
		}

		template<class Iterator, class ValueTypeArg>
		typename UniquePtrIterator<Iterator,ValueTypeArg>::ThisType 
		operator-(const UniquePtrIterator<Iterator,ValueTypeArg>& it, int shift)
		{
			return typename UniquePtrIterator<Iterator,ValueTypeArg>::ThisType(it.GetIterator() - shift);
		}

		// UniquePtrIterator impl
		template<class Iterator, class ValueTypeArg>
		UniquePtrIterator<Iterator,ValueTypeArg>::UniquePtrIterator(const Iterator& iteratorIn) : _iterator(iteratorIn) {}

		template<class Iterator, class ValueTypeArg>
		typename UniquePtrIterator<Iterator,ValueTypeArg>::ThisType& UniquePtrIterator<Iterator,ValueTypeArg>::operator--()
		{
			--_iterator;
			return *this;
		}

		template<class Iterator, class ValueTypeArg>
		typename UniquePtrIterator<Iterator, ValueTypeArg>::ThisType& UniquePtrIterator<Iterator, ValueTypeArg>::operator++()
		{
			++_iterator;
			return *this;
		}

		template<class Iterator, class ValueTypeArg>
		typename UniquePtrIterator<Iterator,ValueTypeArg>::ThisType	UniquePtrIterator<Iterator,ValueTypeArg>::operator++	(int)
		{
			return ThisType(std::next(_iterator));
		}

		template<class Iterator, class ValueTypeArg>
		typename UniquePtrIterator<Iterator,ValueTypeArg>::ThisType	UniquePtrIterator<Iterator,ValueTypeArg>::operator--	(int)
		{
			return ThisType(std::prev(_iterator));
		}
	} // Util
} // Dv

#endif // _Util_UniquePtrIterator_h_