#include "../../h/Container/Util_AllocHandleList.h"

namespace Dv
{
	namespace Util
	{
		namespace Container
		{
			bool AllocHandle::IsDangled() const { return _pElem->bDangled; }
			bool AllocHandle::IsGap() const { assert(!IsDangled());  return _pElem->it_alloc_elem->bGap; }
			size_t AllocHandle::GetStartOffset() const { assert(!IsDangled()); return _pElem->it_alloc_elem->offset; }
			AllocHandle *AllocHandle::GetNext() const
			{
				auto it_next = std::next(_pElem->it_position);
				if (it_next == _pOwnerList->end())
				{
					return nullptr;
				}
				return &((*it_next)->handle);
			}
			AllocHandle *AllocHandle::GetPrevious() const
			{
				if (_pElem->it_position == _pOwnerList->begin())
				{
					return nullptr;
				}
				auto it_prev = std::prev(_pElem->it_position);
				return &((*it_prev)->handle);
			}
			AllocHandle *AllocHandle::invert_start(size_t splitOffset)
			{
				auto it_new_elem = _pOwnerList->inverse_start(_pElem, splitOffset);
				return &((*it_new_elem)->handle);
			}
			AllocHandle *AllocHandle::invert_end(size_t splitOffset)
			{
				auto it_new_elem = _pOwnerList->inverse_end(_pElem, splitOffset);
				return &((*it_new_elem)->handle);
			}

			AllocHandleElem::AllocHandleElem() : bDangled(true) {}
			AllocHandleElem::AllocHandleElem(AllocList::alloc_elem_iterator InItAllocElem) : 
				it_alloc_elem(InItAllocElem), bDangled(false) {}

			void AllocHandleList::reset(size_t InBufferSize)
			{
				assert(InBufferSize >= 0);
				for (auto &handle : _handles)
				{
					handle.bDangled = true;
				}
				_allocs.reset(InBufferSize);
				_order.clear();
				_handles.clear();
				_insertHandle(_order.begin(), _allocs.begin());
			}

			AllocHandleList::iterator AllocHandleList::inverse_start(AllocHandleElem *pInHandle, size_t InSplitOffset)
			{
				assert(pInHandle != nullptr);
				assert(InSplitOffset >= 0);
				assert(InSplitOffset < GetBufferSize());
				assert(!pInHandle->bDangled);
				auto invert_res = _allocs.invert_in_elem_from_its_start(pInHandle->it_alloc_elem, InSplitOffset);
				auto it_new_handle = _insertAndUpdateHandle(pInHandle, pInHandle->it_position, invert_res);
				return it_new_handle;
			}

			AllocHandleList::iterator AllocHandleList::inverse_end(AllocHandleElem *pInHandle, size_t InSplitOffset)
			{
				assert(pInHandle != nullptr);
				assert(InSplitOffset >= 0);
				assert(InSplitOffset < GetBufferSize());
				assert(!pInHandle->bDangled);
				auto invert_res = _allocs.invert_in_elem_from_its_end(pInHandle->it_alloc_elem, InSplitOffset);
				auto it_new_handle = _insertAndUpdateHandle(pInHandle, std::next(pInHandle->it_position), invert_res);
				return it_new_handle;
			}

			AllocHandleList::iterator  AllocHandleList::_insertAndUpdateHandle(AllocHandleElem *pInHandle, iterator where, const AllocList::AllocListResult& updateState)
			{				
				auto it_new_position = _insertHandle(where, updateState.iterator);
				_updateHandleAllocElems(pInHandle, updateState);
				return it_new_position;
			}

			void AllocHandleList::_updateHandleAllocElems(AllocHandleElem *pInHandle, const AllocList::AllocListResult& updateState)
			{
				if (updateState.bCurrentRemoved)
				{					
					pInHandle->it_alloc_elem = updateState.iterator;					
				}
				if (updateState.next_element != AllocList::EAllocListElemUpdate::Unchanged)
				{
					auto it_next_handle = std::next(pInHandle->it_position);
					assert(it_next_handle != _order.end());
					(*it_next_handle)->it_alloc_elem = updateState.iterator;					
				}
				if (updateState.previous_element != AllocList::EAllocListElemUpdate::Unchanged)
				{
					assert(pInHandle->it_position != _order.begin());
					auto it_prev_handle = std::prev(pInHandle->it_position);
					(*it_prev_handle)->it_alloc_elem = updateState.iterator;					
				}
			}

			AllocHandleList::iterator AllocHandleList::_insertHandle(iterator where, AllocList::alloc_elem_iterator what)
			{
				auto it_handle_elem = _handles.emplace(_handles.end(), what);
				auto pHandleElem = &(*it_handle_elem);
				pHandleElem->handle = AllocHandle(this, pHandleElem);
				auto it_new_position = _order.emplace(where, pHandleElem);
				pHandleElem->it_position = it_new_position;
				return it_new_position;
			}
		} // Container
	} // Util
} // Dv