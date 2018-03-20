#pragma once

#include "Util_AllocList.h"

#include <list>

namespace Dv
{
	namespace Util
	{		
		namespace Container
		{
			/************************************************************************************************
			* Alloc handle list
			*
			* Список чередующихся свободных и аллоцированных блоков.
			* Важным свойством, обусловившим создание данного класса, является
			* сохранение валидности всех итераторов на элементы данного списка
			* (включая даже и удаленные) после выполнения над списком любой операции.
			* Для удаленных элементов указывается состояние, что они более не актуальны.
			* Удаленные элементы далее не должны перечисляться с помощью begin()/end().
			* Но итераторы на удаленные элементы сохраняются.
			************************************************************************************************/
			struct AllocHandleElem;
			class AllocHandleList;

			/************************************************************************************************
			* TODO: 
			************************************************************************************************/
			class AllocHandle
			{
			public:
				AllocHandle() : _pElem(nullptr), _pOwnerList(nullptr) {}
				AllocHandle(AllocHandleList *pInOwnerList, AllocHandleElem *pInElem) : _pOwnerList(pInOwnerList), _pElem(pInElem) {}				

				AllocHandle(const AllocHandle&) = default;
				AllocHandle& operator=(const AllocHandle&) = default;

				AllocHandle *GetNext() const;
				AllocHandle *GetPrevious() const;
				AllocHandle *invert_start(size_t splitOffset);
				AllocHandle *invert_end(size_t splitOffset);

				bool IsDangled() const;
				bool IsGap() const;
				size_t GetStartOffset() const;

			private:
				AllocHandleElem *_pElem;
				AllocHandleList *_pOwnerList;
			};

			struct AllocHandleElem
			{
				using List = std::list<AllocHandleElem>;
				using PtrList = std::list<AllocHandleElem *>;

				bool											bDangled;
				AllocHandle										handle;
				AllocList::alloc_elem_iterator					it_alloc_elem;
				PtrList::iterator								it_position;
				List::iterator									it_storage;

				AllocHandleElem();
				AllocHandleElem(AllocList::alloc_elem_iterator InItAllocElem);
			};			

			class AllocHandleList
			{
			public:
				using iterator = AllocHandleElem::PtrList::iterator;
				using const_iterator = AllocHandleElem::PtrList::const_iterator;
				using handle_iterator = AllocHandleElem::List::iterator;
				using const_handle_iterator = AllocHandleElem::List::const_iterator;

				// default ctor: size defaults to zero
				AllocHandleList();
				AllocHandleList(size_t InBufferSize);

				// buffer size in bytes
				size_t GetBufferSize() const;
				// total number of gaps and allocs
				size_t GetNumElems() const;
				
				const_iterator begin() const;
				const_iterator end() const;
				const_iterator cbegin() const;
				const_iterator cend() const;

				const_handle_iterator begin_handles() const;
				const_handle_iterator end_handles() const;

				// reset the list to contain only one gap of the given size
				// note: all handles will be marked as dangled
				void reset(size_t InBufferSize);
				iterator inverse_start(AllocHandleElem *pInHandle, size_t InSplitOffset);
				iterator inverse_end(AllocHandleElem *pInHandle, size_t InSplitOffset);

			private:
				iterator _insertAndUpdateHandle(AllocHandleElem *pInHandle, iterator where, const AllocList::AllocListResult& updateState);
				iterator _insertHandle(iterator where, AllocList::alloc_elem_iterator what);
				void _updateHandleAllocElems(AllocHandleElem *pInHandle, const AllocList::AllocListResult& updateState);

				AllocHandleElem::PtrList _order;
				AllocHandleElem::List _handles;
				AllocList _allocs;
			};

			/************************************************************************************************
			* IMPLEMENTATION
			************************************************************************************************/
			inline AllocHandleList::AllocHandleList()
			{
				reset(0);
			}

			inline AllocHandleList::AllocHandleList(size_t InBufferSize)
			{
				reset(InBufferSize);
			}

			inline size_t AllocHandleList::GetBufferSize() const
			{
				return _allocs.GetBufferSize();
			}

			inline size_t AllocHandleList::GetNumElems() const
			{
				return _allocs.size();
			}

			inline AllocHandleList::const_iterator AllocHandleList::begin() const
			{
				return _order.cbegin();
			}
			inline AllocHandleList::const_iterator AllocHandleList::end() const
			{
				return _order.cend();
			}
			inline AllocHandleList::const_iterator AllocHandleList::cbegin() const
			{
				return _order.cbegin();
			}
			inline AllocHandleList::const_iterator AllocHandleList::cend() const
			{
				return _order.cend();
			}
			inline AllocHandleList::const_handle_iterator AllocHandleList::begin_handles() const
			{
				return _handles.begin();
			}
			inline AllocHandleList::const_handle_iterator AllocHandleList::end_handles() const
			{
				return _handles.end();
			}
		} // Container
	} // Util
} // Dv