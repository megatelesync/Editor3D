#include "../../h/Container/Util_AllocBlockList.h"

#include <cassert>

namespace Dv
{
	namespace Util
	{
		namespace Container
		{
			/*************************************************************************************
			* AllocBlockList implementation
			*************************************************************************************/
			/*************************************************************************************
			AllocOrGapBlock::BlockList::iterator AllocBlockList::AllocFromStart(AllocOrGapBlock *pInGap, size_t InSize)
			{
				assert(InSize >= 0);
				assert(pInGap != nullptr);
				assert(pInGap->it_alloc_list_elem->bGap);
				auto it_block = _invertBlockStart(pInGap, InSize);
				return it_block;
			}

			AllocOrGapBlock::BlockList::iterator AllocBlockList::AllocFromEnd(AllocOrGapBlock *pInGap, size_t InSize)
			{
				assert(InSize >= 0);
				assert(pInGap != nullptr);
				assert(pInGap->it_alloc_list_elem->bGap);
				auto it_block = _invertBlockEnd(pInGap, InSize);
				return it_block;
			}

			AllocOrGapBlock::BlockIteratorList::iterator AllocBlockList::ResizeAlloc(AllocOrGapBlock *pInAlloc, size_t InNewSize)
			{
				assert(pInAlloc != nullptr);
				assert(!pInAlloc->it_alloc_list_elem->bGap);
				assert(InNewSize > 0);
				assert(IsActual(pInAlloc));				

				auto it_actual_block = pInAlloc->it_handle_iter;
				auto it_next_actual_block = std::next(it_actual_block);
				auto it_next_block = _blocks.end();
				if (it_next_actual_block != _orderedActualBlocks.end())
				{
					it_next_block = (*it_next_actual_block)->it_block;
				}

				if (it_next_block != _blocks.end() && it_next_block->it_alloc_list_elem->bGap)
				{
					size_t new_border_offset = pInAlloc->it_alloc_list_elem->offset + InNewSize;
					// THE IF CASE where the new gap does not need to be allocated
					_moveBlockBorderTo(it_next_block, new_border_offset);
					return it_next_actual_block;
				}
				else
				{
					// THE IF CASE where the new gap needs to be created
					auto it_new_gap = _invertBlockEnd(pInAlloc, gapAugmentSize);
					return it_new_gap->it_handle_iter;
				}
			}

			AllocOrGapBlock::BlockIteratorList::iterator AllocBlockList::Free(AllocOrGapBlock *pInAlloc)
			{
				assert(pInAlloc != nullptr);
				size_t alloc_size = GetBlockSize(pInAlloc);
				auto it_block = _invertBlockStart(pInAlloc, alloc_size);
				return it_block->it_handle_iter;
			}

			AllocOrGapBlock::BlockList::iterator AllocBlockList::_invertBlockStart(AllocOrGapBlock *pInBlock, size_t InSize)
			{
				assert(InSize >= 0);
				assert(pInBlock != nullptr);
				assert(IsActual(pInBlock));
				auto it_list_elem_to_split = pInBlock->it_alloc_list_elem;
				size_t split_offset = it_list_elem_to_split->offset + InSize;
				auto split_result = _allocs.invert_in_elem_from_its_start(it_list_elem_to_split, split_offset);
				auto it_block = _pushBackBlock(it_list_elem_to_split, split_offset);

				auto it_actual_block = _orderedActualBlocks.insert(pInBlock->it_handle_iter, it_block);
				_FixIterators(pInBlock, split_result);

				return it_block;
			}

			AllocOrGapBlock::BlockList::iterator AllocBlockList::_invertBlockEnd(AllocOrGapBlock *pInBlock, size_t InSize)
			{
				assert(InSize >= 0);
				assert(pInBlock != nullptr);
				assert(IsActual(pInBlock));
				auto it_list_elem_to_split = pInBlock->it_alloc_list_elem;
				size_t split_offset = GetNextBlockOffset(it_list_elem_to_split) - InSize + 1;
				auto split_result = _allocs.invert_in_elem_from_its_end(it_list_elem_to_split, split_offset);
				auto it_block = _pushBackBlock(it_list_elem_to_split, split_offset);
				auto it_actual_block = _orderedActualBlocks.insert(std::next(pInBlock->it_handle_iter), it_block);
				_FixIterators(pInBlock, split_result);
			}

			AllocOrGapBlock::BlockList::iterator AllocBlockList::_pushBackBlock(AllocList::iterator InAllocListElem, size_t InSplitOffset)
			{
				AllocOrGapBlock block_to_insert(InAllocListElem, InSplitOffset);
				AllocOrGapBlock::BlockList::iterator it = _blocks.insert(_blocks.end(), block_to_insert);
				it->it_block = it;
				it->handle = AllocOrGapBlockHandle(this, &(*it));
				return it;
			}

			void AllocBlockList::_FixIterators(AllocOrGapBlock *pInBlock, const AllocList::AllocListResult& InResult)
			{
				auto it_block = pInBlock->it_handle_iter;
				auto it_block_backward = it_block;				

				auto it_alloc_elem = pInBlock->it_alloc_list_elem;
				size_t next_alloc_offset = GetNextOffset(_allocs, it_alloc_elem);

				if(InResult.bCurrentRemoved)
				{
					// UPDATING ITERATORS POINTING TO OLD BLOCKS
					it_block_backward = _UpdateIteratorsBackward(it_block_backward, it_alloc_elem);
					
					it_block = _UpdateIteratorsForward(it_block, next_alloc_offset, it_alloc_elem);
				}
				else
				{
					// WE NEED TO UPDATE THE BACKWARD AND FORWARD BLOCK ITERATORS ANYHOW
					// (EVER IF ALLOC ELEM LINKED WITH THE PASSED BLOCK IS NOT CHANGED)
					while (it_block_backward != _orderedActualBlocks.begin() && ((*it_block_backward)->split_offset >= it_alloc_elem->offset))
					{
						--it_block_backward;
					}
					while (it_block != _orderedActualBlocks.end() && ((*it_block)->split_offset < next_alloc_offset))
					{
						++it_block;
					}
				}

				if ((*it_block_backward)->split_offset < it_alloc_elem->offset && InResult.previous_element != AllocList::EAllocListElemUpdate::Unchanged)
				{
					assert((*it_block_backward)->split_offset >= pInBlock->it_alloc_list_elem->offset);
					auto it_prev_alloc_elem = std::prev(pInBlock->it_alloc_list_elem);
					// UPDATE BLOCKS LINKED WITH PREVIOUS ALLOC LIST ELEM
					it_block_backward = _UpdateIteratorsBackward(it_block_backward, it_prev_alloc_elem);
				}

				if(it_block != _orderedActualBlocks.end() && InResult.next_element != AllocList::EAllocListElemUpdate::Unchanged)
				{
					auto it_next_alloc_elem = std::next(it_alloc_elem);
					size_t next_next_alloc_offset = GetNextOffset(_allocs, it_next_alloc_elem);
					// UPDATE BLOCKS LINKED WITH NEXT ALLOC LIST ELEM
					it_block = _UpdateIteratorsForward(it_block, next_next_alloc_offset, it_next_alloc_elem);
				}
			}

			AllocOrGapBlock::BlockIteratorList::iterator AllocBlockList::_UpdateIteratorsBackward
			(
				AllocOrGapBlock::BlockIteratorList::iterator it_block_backward,
				AllocList::const_iterator it_alloc_elem
			)
			{
				while (true)
				{
					// CYCLE FOR UPDATING ALLOC ELEM ITERATORS FOR BLOCKS THAT POSITIONED BEFORE THE BLOCK PASSED AS ARGUMENT
					if (it_block_backward == _orderedActualBlocks.begin()) { break; }
					--it_block_backward;
					if ((*it_block_backward)->split_offset < it_alloc_elem->offset)
					{
						break;
					}
					(*it_block_backward)->it_alloc_list_elem = it_alloc_elem;
				}
				return it_block_backward;
			}

			AllocOrGapBlock::BlockIteratorList::iterator AllocBlockList::_UpdateIteratorsForward
			(
				AllocOrGapBlock::BlockIteratorList::iterator it_block,
				size_t next_alloc_offset,
				AllocList::const_iterator it_alloc_elem
			)
			{
				while (true)
				{
					// CYCLE FOR UPDATING ALLOC ELEM ITERATORS FOR BLOCKS THAT POSITION AFTER THE BLOCK PASSED AS ARGUMENT
					++it_block;
					if (it_block == _orderedActualBlocks.end())
					{
						break;
					}
					if ((*it_block)->split_offset >= next_alloc_offset)
					{
						break;
					}
					(*it_block)->it_alloc_list_elem = it_alloc_elem;
				}
				return it_block;
			}

			bool AllocBlockList::IsActual(AllocOrGapBlock *pBlock)
			{
				assert(pBlock);
				bool bActual = pBlock->it_handle_iter != _orderedActualBlocks.end();
				return bActual;
			}

			AllocOrGapBlock::AllocOrGapBlock(AllocList::iterator InAllocListElem, size_t InSplitOffset) :
				it_alloc_list_elem(InAllocListElem),
				split_offset(InSplitOffset)
			{
			}
			******************/

			/*************************************************************************************
			* AllocOrGapBlockHandle implementation
			*************************************************************************************/
			/************************************************************************************
			AllocOrGapBlockHandle::AllocOrGapBlockHandle() noexcept :
				_pList(nullptr), _pBlock(nullptr)
			{
			}

			AllocOrGapBlockHandle::AllocOrGapBlockHandle(AllocBlockList *pInList, AllocOrGapBlock *pInBlock) noexcept :
				_pList(pInList), _pBlock(pInBlock)
			{
				assert(pInList != nullptr);
				assert(pInBlock != nullptr);
			}
			************************************************************************************/
		} // Container
	} // Util
} // Dv