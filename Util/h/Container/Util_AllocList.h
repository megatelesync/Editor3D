#ifndef _Util_AllocList_h_
#define _Util_AllocList_h_

#include <list>
#include <cassert>

namespace Dv
{
	namespace Util
	{
		namespace Container
		{
		
			/*****************************************************************************************
			* AllocList TODO
			*
			* Non-member functions
			* 
			* 1. Search
			* 1.1. Find alloc or gap that contains given offset
			* (+ CODED: 21:23/06.12.2016)
			* 2. Allocate/Free with offset and size
			* 2.1. Alloc/Free by iterator to the gap/alloc
			* (+ CODED: 21:23/06.12.2016)
			* 2.2. Alloc/Free by given offset and size only		
			*
			* Search by size:
			* 1. Search gap capable of at least given size
			*
			* Member functions
			*
			* 1. Allocate/Free from start/end:
			* 1.1. allocate_in_gap_from_its_start (+TESTED: 08.12.2016/1:04)
			* (+CODED: 06.12.2016/21:36)
			* 1.2. allocate_in_gap_from_its_end (+TESTED: 08.12.2016/1:04)
			* 1.2.1. See invert_in_elem_from_its_end
			* 1.3. free_from_alloc_from_its_start
			* (+CODED: 06.12.2016/21:36)
			* 
			* 1.4. free_from_alloc_from_its_end
			* (+CODED: 06.12.2016/21:36)
			*
			* 2. Inversion functions
			* 2.1. invert_in_elem_from_its_start (+TESTED: 08.12.2016/1:04; +CODED: 06.12.2016/21:35)
			* 2.2. invert_in_elem_from_its_end (+TESTED: 08.12.2016/1:04; CODED: 08.12.2016/1:04)
			* 2.2.1. Why the result is iterator and not AllocListResult?
			*****************************************************************************************/

			struct AllocListElem
			{
				// offset of the gap or allocation in abstract units (bytes, elements - whatever)
				unsigned int				offset;
				// gap or allocation
				// (true if this element is gap, false if this element is allocation)
				bool						bGap; 

				// Create the allocation or gap
				inline AllocListElem(unsigned int InAllocAddr = 0, bool bInGap = false);
			};

			typedef std::list<AllocListElem>		AllocListCont;			

			// Allocation list.
			// The main property of this list:
			// "There're no two consecutive gaps or two consecutive allocs in the alloc list"!
			// Other properties:
			// 1. Empty list has only one gap
			// Design notes:
			// Elements of the allocation list class instances should represent the existing elements
			// (no handle pattern/concept should be used). For that kind of functionality - use another
			// layor on top of this allocation list class.
			class AllocList
			{
			public:
				struct AllocListResult;

				// Alloc or gap iterator
				typedef AllocListCont::iterator                 iterator;
				typedef AllocListCont::const_iterator			const_iterator;

				// typedef of the iterator to the gap (warning! may be wrong - use non-const)
				typedef AllocListCont::iterator					gap_iterator;
				// typedef of the iterator to the alloc (warning! may be wrong - use non-const)
				typedef AllocListCont::iterator					alloc_iterator;
				// typedef of the iterator to the gap or alloc (warning! may be wrong - use non-const)
				typedef AllocListCont::iterator					alloc_elem_iterator;
				typedef AllocListCont::const_iterator			alloc_elem_const_iterator;

				// default ctor
				// Init the empty list of zero size
				AllocList() { _Reset(0); }
				explicit AllocList(size_t InSize) { _Reset(InSize); }

				// reset the size of the list with emptying it out
				void								reset			(size_t InSize)			{ _Reset(InSize); }

				// sizeof of the list in abstract units (bytes or elems, etc.)
				size_t								GetBufferSize	()			const		{ return _sz; }

				// count of list elems (including both allocs and gaps)
				size_t								size			()			const		{ return _lst.size(); }

				// range of iterators of all allocs and gaps
				alloc_elem_const_iterator			cbegin			()			const		{ return _lst.cbegin();  }
				alloc_elem_const_iterator			cend			()			const		{ return _lst.cend();  }
				alloc_elem_iterator					begin			()						{ return _lst.begin();  }
				alloc_elem_iterator					end				()						{ return _lst.end();  }
				alloc_elem_const_iterator			begin           ()          const       { return _lst.begin(); }
				alloc_elem_const_iterator			end             ()          const       { return _lst.end(); }

				// ----------------------------------------------------------------------------
				// operations on the list
				// ---------------------------------------------------------------------------
				void clear();

				// function:
				//		allocation in gap from its start
				// parameters;
				//		InOffset - offset of the verge between the gap and the allocation
				// description:
				//		make the allocation from the start of the given gap				
				// returns:
				//		iterator to the result allocation
				alloc_iterator						allocate_in_gap_from_its_start		(gap_iterator gap_iterator, unsigned int InNewGapOffset);
				AllocListResult						allocate_in_gap_from_its_start_big_result		(gap_iterator gap_iterator, unsigned int InNewGapOffset);

				// function:
				//		allocation in gap from its end
				// description:
				//		Similar to other in-gap allocation operations.
				alloc_iterator						allocate_in_gap_from_its_end		(gap_iterator gap_iterator, unsigned int InAllocOffset);
				AllocListResult						allocate_in_gap_from_its_end_big_result(gap_iterator gap_iterator, unsigned int InAllocOffset);

				
				// function:
				//		free from gap from its start
				// description:
				//		Similar to other in-alloc free operations.
				gap_iterator						free_from_alloc_from_its_start(alloc_iterator alloc_iterator, unsigned int InNewAllocOffset);
				AllocListResult						free_from_alloc_from_its_start_big_result(alloc_iterator alloc_iterator, unsigned int InNewAllocOffset);

				
				// function:
				//		free from gap from its end
				// description:
				//		Similar to other in-alloc free operations.
				gap_iterator						free_in_alloc_from_its_end			(alloc_iterator alloc_iterator, unsigned int InNewGapOffset);
				AllocListResult						free_in_alloc_from_its_end_big_result (alloc_iterator alloc_iterator, unsigned int InNewGapOffset);

				enum class EAllocListElemUpdate
				{
					Unchanged,
					Insert,
					Remove
				};

				// result of allocating/deallocating in the alloc list
				struct AllocListResult
				{					
					EAllocListElemUpdate			previous_element;
					EAllocListElemUpdate			next_element;
					bool							bCurrentRemoved;					

					// iterator to the new allocation or gap
					alloc_elem_iterator				iterator;					

					AllocListResult					(alloc_elem_iterator InIterator);
					AllocListResult					(alloc_elem_iterator InIterator, EAllocListElemUpdate InPrevious, EAllocListElemUpdate InNext, bool bInCurrent);
				};

				AllocListResult						invert_in_elem_from_its_start		(alloc_elem_iterator alloc_elem_iterator, unsigned int InNewElemEndOffset);
				AllocListResult						invert_in_elem_from_its_end			(alloc_elem_iterator alloc_elem_iterator, unsigned int InNewElemStartOffset);


			private:
				AllocListCont		_lst;
				size_t				_sz;

				// Remove all elements of the list and add only one gap of the given size
				inline void						_Reset						(size_t InSize);				
			};			

			size_t GetNextOffset(const AllocList& allocs, AllocList::const_iterator it);

			// Find iterator to the alloc or gap in the alloc list 
			// (if there's a verge at the given offset then the farthest of two alloc elems to be returned)
			AllocList::iterator         FindAllocOrGap_ByOffset(AllocList& allocs, size_t globalOffset);
			AllocList::const_iterator   FindAllocOrGap_ByOffset(const AllocList& allocs, size_t globalOffset);

			// Make allocation in the given gap
			AllocList::alloc_iterator AllocateInGap
			(
				AllocList *pAllocs, 
				AllocList::gap_iterator gapIterator, 
				size_t globalOffset, size_t allocSize
			);
			// Free from the given alloc
			AllocList::gap_iterator FreeFromAlloc
			(
				AllocList *pAllocs, 
				AllocList::alloc_iterator allocIterator, 
				size_t globalOffset, size_t allocSize
			);

			// Make the inversion of the allocation in range [InElemStart;InElemEnd]
			// in the given list
			AllocList::alloc_elem_iterator invert_alloc
			(
				AllocList									*pList,
				AllocList::alloc_elem_iterator				iterator, 
				unsigned int InElemStart, unsigned int InElemEnd
			);

			// Impl
			inline void AllocList::_Reset(size_t InSize)
			{
				_sz = InSize;
				_lst.clear();
				_lst.push_back(AllocListElem(0, true));
			} 

			// AllocListElem impl
			inline AllocListElem::AllocListElem(unsigned int InAllocAddr, bool bInGap) :
				offset(InAllocAddr), bGap(bInGap)
			{
			}

			// AllocListResult impl
			inline AllocList::AllocListResult::AllocListResult(AllocList::alloc_elem_iterator InIterator) :
				iterator(InIterator),
				previous_element(EAllocListElemUpdate::Unchanged),
				next_element(EAllocListElemUpdate::Unchanged),
				bCurrentRemoved(false)
			{
			}
			inline AllocList::AllocListResult::AllocListResult::AllocListResult
			(
				AllocList::alloc_elem_iterator InIterator, 
				EAllocListElemUpdate InPrevious, 
				EAllocListElemUpdate InNext, 
				bool bInCurrentRemoved
			) :
				iterator(InIterator),
				previous_element(InPrevious),
				next_element(InNext),
				bCurrentRemoved(bInCurrentRemoved)
			{
			}
		} // Container
	} // Util
} // Dv

#endif // _Util_AllocList_h_