#pragma once

#include "Util_AllocHandleList.h"

namespace Dv
{
	namespace Util
	{
		namespace Container
		{
			struct AtomicAllocElem
			{
				using List = std::list<AtomicAllocElem>;

				size_t		startOffset;
				AllocHandle *pAllocBlock;

				AtomicAllocElem(size_t InStartOffset, AllocHandle *pInAllocBlock);
			};

			/************************************************************************
			* Atomic alloc list
			*			
			* Содержит список элементарных аллокаций плюс список блоков.
			* Список элементарных аллокаций (atomic allocation) содержит только
			* аллокации и НЕ может содержать пустые блоки.
			* Набор последовательных аллокаций составляет блок аллокации.
			* Помимо блоков аллокаций список блоков содержит и пустые блоки.
			*
			* Contains list of atomic allocations and list of blocks.
			* List of atomic allocations contains only allocations and  
			* cannot contain empty blocks. Collection of consecutive allocations
			* makes allocation block. Apart from allocation blocks,
			* list of allocation blocks may contain empty blocks.
			************************************************************************/
			class AtomicAllocList
			{
			public:
				using iterator = AtomicAllocElem::List::iterator;
				using const_iterator = AtomicAllocElem::List::const_iterator;
				using block_iterator = AllocHandleList::const_iterator;

				AtomicAllocList();
				AtomicAllocList(size_t InTotalSize);

				// size of the buffer in bytes
				size_t GetBufferSize() const;
				// number of non-atomic filled or empty blocks
				size_t GetNumBlocks() const;
				// number of atomic allocs
				size_t GetNumAtomicAllocs() const;

				iterator begin();
				iterator end();
				const_iterator begin() const;
				const_iterator end() const;
				const_iterator cbegin() const;
				const_iterator cend() const;

				// non-atomic filled or empty blocks
				block_iterator blocks_begin() const;
				block_iterator blocks_end() const;

				void reset(size_t newSize);
				
				// Making new atomic allocation
				iterator alloc_start(iterator elem, size_t offset);
				iterator alloc_end(iterator elem, size_t offset);
				// Resizing the alloc
				void resize_start(iterator elem, size_t offset);
				void resize_end(iterator elem, size_t offset);
				// free the given alloc completely
				void free(iterator elem);

			private:
				AllocHandleList _blocks;
				AtomicAllocElem::List _atomicAllocs;
			};

			inline size_t AtomicAllocList::GetBufferSize() const
			{
				return _blocks.GetBufferSize();
			}

			inline size_t AtomicAllocList::GetNumBlocks() const
			{
				return _blocks.GetNumElems();
			}
			inline size_t AtomicAllocList::GetNumAtomicAllocs() const
			{
				return _atomicAllocs.size();
			}
			inline AtomicAllocList::iterator AtomicAllocList::begin()
			{
				return _atomicAllocs.begin();
			}
			inline AtomicAllocList::iterator AtomicAllocList::end()
			{
				return _atomicAllocs.end();
			}
			inline AtomicAllocList::const_iterator AtomicAllocList::begin() const
			{
				return _atomicAllocs.begin();
			}
			inline AtomicAllocList::const_iterator AtomicAllocList::end() const
			{
				return _atomicAllocs.end();
			}
			inline AtomicAllocList::const_iterator AtomicAllocList::cbegin() const
			{
				return _atomicAllocs.cbegin();
			}
			inline AtomicAllocList::const_iterator AtomicAllocList::cend() const
			{
				return _atomicAllocs.cend();
			}
			inline AtomicAllocList::block_iterator AtomicAllocList::blocks_begin() const
			{
				return _blocks.cbegin();
			}
			inline AtomicAllocList::block_iterator AtomicAllocList::blocks_end() const
			{
				return _blocks.cend();
			}
		} // Container
	} // Util
} // Dv