#include "../../h/Container/Util_AtomicAllocList.h"

namespace Dv
{
	namespace Util
	{
		namespace Container
		{
			AtomicAllocElem::AtomicAllocElem(size_t InStartOffset, AllocHandle *pInAllocBlock) :
				startOffset(InStartOffset), pAllocBlock(pInAllocBlock)
			{
				assert(InStartOffset >= 0);
				assert(pInAllocBlock != nullptr);				
				assert(!pInAllocBlock->IsGap());
			}

			AtomicAllocList::AtomicAllocList() : _blocks(0)
			{
				_atomicAllocs.emplace_back(0, &((*_blocks.begin())->handle));
			}
			AtomicAllocList::AtomicAllocList(size_t InSize) :
				_blocks(InSize)
			{
				_atomicAllocs.emplace_back(0, &((*_blocks.begin())->handle));
			}
			void AtomicAllocList::reset(size_t newSize)
			{
				_blocks.reset(newSize);
				_atomicAllocs.clear();
				_atomicAllocs.emplace_back(0, &((*_blocks.begin())->handle));
			}

			AtomicAllocList::iterator AtomicAllocList::alloc_start(iterator elem, size_t offset)
			{
				auto it_new_alloc_block = elem->pAllocBlock->invert_start(offset);
				auto it_new_atomic_alloc = _atomicAllocs.emplace(elem, offset, it_new_alloc_block);
				return it_new_atomic_alloc;
			}
			AtomicAllocList::iterator AtomicAllocList::alloc_end(iterator elem, size_t offset)
			{
				auto it_new_alloc_block = elem->pAllocBlock->invert_end(offset);
				auto it_new_atomic_alloc = _atomicAllocs.emplace(std::next(elem), offset, it_new_alloc_block);
				return it_new_atomic_alloc;
			}
			void AtomicAllocList::resize_start(iterator elem, size_t offset)
			{
				auto it_new_alloc_block = elem->pAllocBlock->invert_start(offset);
			}
			void AtomicAllocList::resize_end(iterator elem, size_t offset)
			{
				auto it_new_alloc_block = elem->pAllocBlock->invert_end(offset);
			}
			void AtomicAllocList::free(iterator elem)
			{
				assert(!"Not yet impl: AtomicAllocList::free");
				//size_t alloc_sz = GetAllocSize(elem);
				//elem->pAllocBlock->invert(offset, alloc_sz);
				_atomicAllocs.erase(elem);
			}
		} // Container
	} // Util
} // Dv