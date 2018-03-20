#ifndef _Util_Enumerator_h_
#define _Util_Enumerator_h_

#include <iterator>

namespace Dv
{
	namespace Util
	{
		// Enumerator is a forward iterator that stores information about 
		// both the ends of the range of the elements to iterate over.				
		//
		// The reason for inventing the enumerator concept is 
		// to have a remedy for enumeration of the collection-like properties of the
		// interfaces (e.g. enumerating childs of the parent objects) with diminishing
		// number of the interface methods to be provided to fullfill this goal.		
		//
		// REQUIREMENTS
		//		1. The enumerator should be STL-compliant (fully compatible with the STL iterator concept).
		//
		// IMPLEMENTATION NOTES
		//		Enumerator is implemented as bidirectional iterator		
		// - it would be impractical to implement it as a forward iterator
		// (because almost all the STL iterators are bidirectional)
		//
		// BEHAVIOUR ON THE CONTAINER CHANGE
		//		When the element is removed from the container during the iteration,
		// the bahaviour depends on whether the removed element is the element the enumerator
		// currently points to. If the removed element is element the enumerator currently points to,
		// the enumerator is invalidated. Otherwise, the enumerator behaviour is not influenced.
		//		When the elements are inserted into the collection,	the enumerator behaviour 
		// is not influenced. In the case of inserting elements positioned after
		// the element that enumerator currently points to, the iterator
		// will enumerate over the original range and will not touch the inserted
		// elements.

		template<class IteratorTypeArg>
		class Enumerator : public std::iterator<std::bidirectional_iterator_tag>
		{
		public:
			typedef						IteratorTypeArg					IteratorType;
			typedef						Enumerator<IteratorType>		ThisType;

			Enumerator();
			Enumerator
			(
				const IteratorTypeArg& rangeFirstIn, const IteratorTypeArg& rangeLastIn, 
				const IteratorTypeArg& currElementIn
			);						

			IteratorTypeArg				GetIterator					()				const				{ return _currElement; }

			IteratorTypeArg				GetFirstIterator			()				const				{ return _rangeFirst; }
			IteratorTypeArg				GetLastIterator				()				const				{ return _rangeLast; }

			// get enumerator to the first element of the range
			ThisType					GetFirstEnumerator			()				const;
			// get enumerator of the first element after the last element of the range
			ThisType					GetLastEnumerator			()				const;

			bool						IsFirst						()				const;
			bool						IsLast						()				const;
			

		private:
			IteratorTypeArg							_rangeFirst;
			IteratorTypeArg							_rangeLast;
			IteratorTypeArg							_currElement;
		};
	} // Util
} // Dv

#endif // _Util_Enumerator_h_