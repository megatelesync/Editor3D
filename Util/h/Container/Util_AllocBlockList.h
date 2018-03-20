#pragma once

/**************************************************************************************************************
* DEVELOPING IS FREEZED
**************************************************************************************************************/

#include "Util_AllocList.h"
#include <list>

namespace Dv
{
	namespace Util
	{
		namespace Container
		{
			/************************************************************************************************
			* Alloc block list
			*
			* AllocOrGapBlockHandle
			* - GetBufferSize() returns size of the buffer
			* - GetNext()/GetPrevious() return pointers to next or previous alloc or handle block handle;
			*   Pointers are returned to avoid creation and copying;
			*   returns nullptr if there's no next or previous block;
			* - ToAlloc()/ToGap() convert to alloc or gap. Assert is used to control type-safety;
			*   Pointer is to be returned to avoid creation and copying of handles;
			* - IsGap() return true if it's gap;
			* - GetStartOffset() : size_t returns start offset of the block
			*
			* AllocBlockHandle
			* - MakeFree() - free alloc completely; The handle cannot be used any more;
			* - Resize(size_t newNumBytes) - set new size for the allocated block;
			*   new size cannot be zero.
			*
			* GapBlockHandle
			* - AllocFromStart(size_t numBytes) : *AllocBlockHandle
			* - AllocFromEnd(size_t numBytes) : *AllockBlockHandle
			*   - allocate given number of bytes in the given gap
			************************************************************************************************/

			/************************************************************************************************
			* TODO:
			*
			*
			************************************************************************************************/

			/*************************************************************************************************
			* // TEMPRARILY COMMENTED: BECAUSED FREEZED AND UNCOMPILABLE
			struct AllocOrGapBlock;
			class AllocBlockList;

			class AllocOrGapBlockHandle
			{
			public:
				// the handle needs the default ctor because block pointer is not known at the construction time
				AllocOrGapBlockHandle() noexcept;
				AllocOrGapBlockHandle(AllocBlockList *pInList, AllocOrGapBlock *pInBlock) noexcept;

				AllocOrGapBlockHandle(const AllocOrGapBlockHandle& rhs) noexcept = default;
				AllocOrGapBlockHandle& operator=(const AllocOrGapBlockHandle& rhs) noexcept = default;				

			protected:
				AllocBlockList * const _pList;
				AllocOrGapBlock * const _pBlock;
			};
			bool operator==(const AllocOrGapBlockHandle& lhs, const AllocOrGapBlockHandle& rhs);
			bool operator!=(const AllocOrGapBlockHandle& lhs, const AllocOrGapBlockHandle& rhs);

			class AllocBlockHandle : public AllocOrGapBlockHandle
			{
			public:			
			};

			class GapBlockHandle : public AllocOrGapBlockHandle
			{
			};

			struct AllocOrGapBlock
			{
				using BlockList = std::list<AllocOrGapBlock>;				
				using BlockIteratorList = std::list<BlockList::iterator>;
				
				AllocList::iterator it_alloc_list_elem; // iterator to the alloc list elem
				size_t split_offset;

				AllocOrGapBlockHandle handle; // to be assigned to after construction
				BlockList::iterator it_block; // to be assigned to after construction
				// to be assigned to after construction;
				// equal to the AllocBlockList::end() if block is not actual
				BlockIteratorList::iterator it_handle_iter; 

				AllocOrGapBlock(AllocList::iterator InAllocListElem, size_t InSplitOffset);
				AllocOrGapBlock(const AllocOrGapBlock& rhs) = default;
				AllocOrGapBlock& operator=(const AllocOrGapBlock& rhs) = default;
			};						

			class AllocBlockList
			{
			public:
				AllocBlockList() = default;
				AllocBlockList(const AllocBlockList& rhs) = default;
				AllocBlockList& operator=(const AllocBlockList& rhs) = default;

				// Iterating ordered actual blocks only
				// ("actual" means that block region really exists, not degenerated handle only)
				AllocOrGapBlock::BlockIteratorList::const_iterator cbegin() const { return _orderedActualBlocks.cbegin(); }
				AllocOrGapBlock::BlockIteratorList::const_iterator cend() const { return _orderedActualBlocks.cend(); }
				AllocOrGapBlock::BlockIteratorList::const_iterator begin() const { return cbegin(); }
				AllocOrGapBlock::BlockIteratorList::const_iterator end() const { return cend(); }
				AllocOrGapBlock::BlockIteratorList::iterator begin() { return _orderedActualBlocks.begin(); }
				AllocOrGapBlock::BlockIteratorList::iterator end() { return _orderedActualBlocks.end(); }

				// Iterating all blocks
				AllocOrGapBlock::BlockList::const_iterator cbegin_blocks() const { return _blocks.cbegin(); }
				AllocOrGapBlock::BlockList::const_iterator cend_blocks() const { return _blocks.cend(); }
				AllocOrGapBlock::BlockList::iterator begin_blocks() { return _blocks.begin(); }
				AllocOrGapBlock::BlockList::iterator end_blocks() { return _blocks.end(); }

				// Is the given block actual
				bool IsActual(AllocOrGapBlock *pBlock);

				// Making allocation inside the given gap
				AllocOrGapBlock::BlockList::iterator AllocFromStart(AllocOrGapBlock *pInGap, size_t InSize);
				AllocOrGapBlock::BlockList::iterator AllocFromEnd(AllocOrGapBlock *pInGap, size_t InSize);

				// Resize the given alloc (to a greater or lesser size, but not to zero size)
				AllocOrGapBlock::BlockIteratorList::iterator ResizeAlloc(AllocOrGapBlock *pInAlloc, size_t InNewSize);

				// Free the given alloc completely (but without removing the handle)
				AllocOrGapBlock::BlockIteratorList::iterator Free(AllocOrGapBlock *pInAlloc);

			private:
				AllocOrGapBlock::BlockList::iterator _invertBlockStart(AllocOrGapBlock *pInBlock, size_t InSize);
				AllocOrGapBlock::BlockList::iterator _invertBlockEnd(AllocOrGapBlock *pInBlock, size_t InSize);
				AllocOrGapBlock::BlockList::iterator _pushBackBlock(AllocList::iterator InAllocListElem, size_t InSplitOffset);
				
				AllocOrGapBlock::BlockIteratorList::iterator _UpdateIteratorsBackward
				(
					AllocOrGapBlock::BlockIteratorList::iterator it_block_backward,
					AllocList::const_iterator it_alloc_elem
				);
				AllocOrGapBlock::BlockIteratorList::iterator _UpdateIteratorsForward
				(
					AllocOrGapBlock::BlockIteratorList::iterator it_block,
					size_t next_alloc_offset,
					AllocList::const_iterator it_alloc_elem
				);
				void _FixIterators(AllocOrGapBlock *pInBlock, const AllocList::AllocListResult& InResult);

				AllocOrGapBlock::BlockIteratorList			_orderedActualBlocks;
				AllocOrGapBlock::BlockList					_blocks;
				AllocList									_allocs;
			};
			*****/
		} // Container
	} // Util
} // Dv