#include "../../h/Container/Util_AllocList.h"

namespace Dv
{
	namespace Util
	{
		namespace Container
		{
			void AllocList::clear()
			{
				_Reset(_sz);
			}

			AllocList::iterator FindAllocOrGap_ByOffset(AllocList& allocs, size_t globalOffset)
			{				
				assert(&allocs != nullptr);
				assert(globalOffset >= 0);
				assert(globalOffset < allocs.GetBufferSize());
				auto it = allocs.begin();
				auto prev_it = it;
				while(true)
				{																					
					prev_it = it;
					++it;
					if (it == allocs.end())
					{
						return prev_it;
					}
					if (it->offset > globalOffset)
					{
						return prev_it;
					}
				}
				assert(!"FindAllocOrGap_ByOffset: should not get here");
				return allocs.begin(); // RETURN SOMETHING TO MAKE COMPILABLE
			}

			AllocList::const_iterator FindAllocOrGap_ByOffset(const AllocList& allocs, size_t globalOffset)
			{
				assert(&allocs != nullptr);
				assert(globalOffset >= 0);
				assert(globalOffset < allocs.GetBufferSize());

				auto non_const_allocs = const_cast<AllocList&>(allocs);
				auto it_result_alloc_or_gap = FindAllocOrGap_ByOffset(non_const_allocs, globalOffset);				
				return it_result_alloc_or_gap;
			}

			AllocList::alloc_iterator AllocateInGap
			(
				AllocList *pAllocs,
				AllocList::gap_iterator gapIterator,
				size_t globalOffset, size_t allocSize
			)
			{
				assert(pAllocs != nullptr);
				assert(gapIterator != pAllocs->end());
				assert(gapIterator->offset <= globalOffset);
				assert(gapIterator->bGap);					
				auto it_alloc = pAllocs->allocate_in_gap_from_its_end(gapIterator, globalOffset);
				auto globalOffset_of_allocEnd = globalOffset + allocSize;
				auto it_new_free = pAllocs->free_in_alloc_from_its_end(it_alloc, globalOffset_of_allocEnd);
				return it_alloc;
			}

			AllocList::gap_iterator FreeFromAlloc
			(
				AllocList *pAllocs,
				AllocList::alloc_iterator allocIterator,
				size_t globalOffset, size_t allocSize
			)
			{
				assert(pAllocs != nullptr);
				assert(allocIterator != pAllocs->end());
				assert(allocIterator->offset <= globalOffset);
				assert(!allocIterator->bGap);				
				auto it_gap = pAllocs->free_in_alloc_from_its_end(allocIterator, globalOffset);
				auto globalOffset_of_gapEnd = globalOffset + allocSize;
				auto it_new_free = pAllocs->allocate_in_gap_from_its_end(it_gap, globalOffset_of_gapEnd);
				return it_gap;
			}

			/************************************************************************************************************/
			AllocList::alloc_iterator AllocList::allocate_in_gap_from_its_start(gap_iterator gap_iterator, unsigned int InNewGapOffset)
			{				
				auto invert_res = allocate_in_gap_from_its_start_big_result(gap_iterator, InNewGapOffset);
				return invert_res.iterator;
			}

			AllocList::AllocListResult	AllocList::allocate_in_gap_from_its_start_big_result(gap_iterator gap_iterator, unsigned int InNewGapOffset)
			{
				assert(gap_iterator->bGap);
				auto invert_res = invert_in_elem_from_its_start(gap_iterator, InNewGapOffset);
				return invert_res;
			}

			// function:
			//		allocation in gap from its end
			// description:
			//		Similar to other in-gap allocation operations.
			AllocList::alloc_iterator AllocList::allocate_in_gap_from_its_end(gap_iterator gap_iterator, unsigned int InAllocOffset)
			{				
				auto invert_res = allocate_in_gap_from_its_end_big_result(gap_iterator, InAllocOffset);				
				return invert_res.iterator;
			}

			AllocList::AllocListResult	AllocList::allocate_in_gap_from_its_end_big_result(gap_iterator gap_iterator, unsigned int InAllocOffset)
			{
				assert(gap_iterator->bGap);
				auto invert_res = invert_in_elem_from_its_end(gap_iterator, InAllocOffset);
				return invert_res;
			}


			// function:
			//		free from gap from its start
			// description:
			//		Similar to other in-alloc free operations.
			AllocList::gap_iterator AllocList::free_from_alloc_from_its_start(alloc_iterator alloc_iterator, unsigned int InNewAllocOffset)
			{				
				auto invert_res = free_from_alloc_from_its_start_big_result(alloc_iterator, InNewAllocOffset);
				return invert_res.iterator;
			}

			AllocList::AllocListResult AllocList::free_from_alloc_from_its_start_big_result(alloc_iterator alloc_iterator, unsigned int InNewAllocOffset)
			{
				assert(!alloc_iterator->bGap);
				auto invert_res = invert_in_elem_from_its_start(alloc_iterator, InNewAllocOffset);
				return invert_res;
			}

			// function:
			//		free from gap from its end
			// description:
			//		Similar to other in-alloc free operations.
			AllocList::gap_iterator AllocList::free_in_alloc_from_its_end(alloc_iterator alloc_iterator, unsigned int InNewGapOffset)
			{				
				auto invert_res = free_in_alloc_from_its_end_big_result(alloc_iterator, InNewGapOffset);
				return invert_res.iterator;
			}

			AllocList::AllocListResult AllocList::free_in_alloc_from_its_end_big_result(alloc_iterator alloc_iterator, unsigned int InNewGapOffset)
			{
				assert(!alloc_iterator->bGap);
				auto invert_res = invert_in_elem_from_its_end(alloc_iterator, InNewGapOffset);
				return invert_res;
			}


			AllocList::AllocListResult AllocList::invert_in_elem_from_its_start
			(
				alloc_elem_iterator			elem_iterator,
				unsigned int				InNewElemEndOffset
			)
			{
				assert(elem_iterator != _lst.end());
				AllocListResult res (elem_iterator);
				// Previous alloc elem is never removed;
				// Next elem is to be removed only if new elem end offset is equal to the start of next element
				// (if the next element exists, of course);
				// Current elem is to be removed only if new elem end offset is equal to the start of next element

				bool bCoalesceWithNext = false;

				auto it_next = std::next(elem_iterator);
				if (it_next != _lst.end())
				{
					if (it_next->offset == InNewElemEndOffset)
					{
						bCoalesceWithNext = true;
						res.next_element = EAllocListElemUpdate::Remove;
						_lst.erase(it_next);						
					}
				}				
				else
				{
					assert(it_next == _lst.end());
					if (_sz == InNewElemEndOffset)
					{
						bCoalesceWithNext = true;
					}
				}
				if (elem_iterator != _lst.begin())
				{
					auto it_prev = std::prev(elem_iterator);					
					if(bCoalesceWithNext)
					{
						res.iterator = it_prev;
						res.bCurrentRemoved = true;						
						_lst.erase(elem_iterator);
					}
					else
					{
						res.iterator = elem_iterator;						
						elem_iterator->offset = InNewElemEndOffset;
					}
				}			
				else
				{
					res.iterator = elem_iterator;
					if (bCoalesceWithNext)
					{						
						elem_iterator->bGap = !elem_iterator->bGap;
					}
					else
					{
						_lst.insert(std::next(elem_iterator), AllocListElem(InNewElemEndOffset, elem_iterator->bGap));
						elem_iterator->bGap = !elem_iterator->bGap;						
					}
				}
				return res;
			}

			AllocList::AllocListResult AllocList::invert_in_elem_from_its_end
			(
				alloc_elem_iterator elem_iterator,
				unsigned int InNewElemStartOffset
			)
			{
				// Default iterator to return is elem_iterator, so it's provided as conructor's argument
				AllocListResult res(elem_iterator);
				// Previous element is never removed;				
				// IF there's next element THEN:
				//		set start offset of the new element to the new elem start offset
				// IF new elem start ofs == elem start ofs THEN
				// BEGIN												
				//		IF there's previous element THEN
				//		BEGIN
				//			IF there's next element THEN erase next element 
				//			erase elem to split
				//		END
				//		ELSE
				//		BEGIN
				//			IF there's next element THEN erase elem to split
				//			ELSE invert current element's bGap
				//		END
				// END		
				// ELSE
				// BEGIN
				//		// COMMENT: new element start ofs != element start ofs
				//		IF there's next element THEN 
				//		BEGIN
				//			nextElement.Offset = newSplitOffset				
				//		END				
				//		ELSE
				//		BEGIN
				//			// COMMENT: no NEXT element
				//			insert new elem after the elem to split with inverted bGap	
				//		END
				// END					
				auto it_next = std::next(elem_iterator);
				if (InNewElemStartOffset == elem_iterator->offset)
				{
					if (elem_iterator != _lst.begin())
					{
						if (it_next != _lst.end())
						{
							_lst.erase(it_next);
							res.next_element = EAllocListElemUpdate::Remove;
						}
						res.iterator = std::prev(elem_iterator);
						_lst.erase(elem_iterator);
						res.bCurrentRemoved = true;						
					}
					else
					{
						assert(elem_iterator == _lst.begin());
						if (it_next != _lst.end())
						{
							_lst.erase(elem_iterator);
							res.bCurrentRemoved = true;
							res.iterator = it_next;
						}
						else
						{
							elem_iterator->bGap = !elem_iterator->bGap;
						}
					}
				}
				else
				{
					assert(elem_iterator->offset < InNewElemStartOffset);
					if (it_next != _lst.end())
					{
						it_next->offset = InNewElemStartOffset;
						res.iterator = it_next;
					}
					else
					{
						auto it_inserted = _lst.insert(std::next(elem_iterator), AllocListElem(InNewElemStartOffset, !elem_iterator->bGap));
						res.iterator = it_inserted;
					}
				}				
				return res;
			}
		} // Container
	} // Util
} // Dv