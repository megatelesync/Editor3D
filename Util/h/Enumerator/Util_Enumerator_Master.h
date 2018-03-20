#ifndef _Util_Enumerator_Master_h_
#define _Util_Enumerator_Master_h_

// INCLUDING THIS FILE 
// WILL EXPOSE ALL THE ENUMERATOR FUNCTIONALITY

//#include "Util_Enumerator.h"
#include "Util_Range.h"
#include "Util_UniquePtrIterator.h"

namespace Dv
{
	namespace Util
	{
		template<class Iterator, class ConstIterator, class ContType> 
		Range<Iterator, ConstIterator> MakeUniquePtrIteratorRange(ContType& cont);

		template<class Iterator, class ConstIterator, class ContType> 
		Range<Iterator, ConstIterator> MakeUniquePtrConstIteratorRange(const ContType& cont);




		// impl
		template<class Iterator, class ConstIterator, class ContType> 
		Range<Iterator, ConstIterator> MakeUniquePtrIteratorRange(ContType& cont)
		{			
			Iterator unique_iterator_begin = MakeUniquePtrIterator(cont.begin());
			Iterator unique_iterator_end = MakeUniquePtrIterator(cont.end());
			Range<Iterator, ConstIterator> iterator_range (unique_iterator_begin, unique_iterator_end);
			return iterator_range;
		}

		template<class Iterator, class ConstIterator, class ContType> 
		Range<Iterator, ConstIterator> MakeUniquePtrConstIteratorRange(const ContType& cont)
		{
			auto unique_iterator_begin = MakeUniquePtrIterator(cont.cbegin());
			auto unique_iterator_end = MakeUniquePtrIterator(cont.cend());
			Range<Iterator, ConstIterator> iterator_range (unique_iterator_begin, unique_iterator_end);
			return iterator_range;
		}
	} // Util
} // Dv

#endif // _Util_Enumerator_Master_h_