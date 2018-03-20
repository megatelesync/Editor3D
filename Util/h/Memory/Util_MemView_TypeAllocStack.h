#pragma once

#include "Util_MemView_ByteArray.h"

#include <type_traits>

/*********************************************************************************************************
* TODO:
* 1. Declare and implement resizable byte array;
* 2. Implement resizable byte array helper functions;
* 3. Implement constructors, destructors etc.
*********************************************************************************************************/

namespace Dv
{
	namespace Util
	{
		/*********************************************************************************************************
		* Concrete allocation headers of allocation stack must inherit from this hook.
		*
		* Concrete allocation header must always be POD types
		*
		* Template parameters:
		* 1. BaseHookTypeArg - type of the concrete base hook (may coincide with MemView_ListStack_HeaderBaseHook itself);
		* 2. MainObjectTypeArg - type of the main object;
		*********************************************************************************************************/
		template<class BaseHookTypeArg, class MainObjectTypeArg>
		class AllocStack_AllocHeaderHook
		{
		public:
			// destructor is NOT virtual, but that is NOT a problem,
			// because inherited objects of this class should NEVER be polymorphic
			~AllocStack_AllocHeaderHook                                        () = default;

			// default ctor: useful for concrete implementations
			AllocStack_AllocHeaderHook()
			{
				static_assert(std::is_base_of<AllocStack_AllocHeaderHook<BaseHookTypeArg, MainObjectTypeArg>, BaseHookTypeArg>::value, "BaseHookTypeArg type of AllocStack_AllocHeaderHook must always be inherited from AllocStack_AllocHeaderHook");
			}

			// ctor
			AllocStack_AllocHeaderHook                                             (BaseHookTypeArg *pInPrev)                                                                      : _pPrev(pInPrev) {}

			// Always copy constructible and copy-assignable:
			AllocStack_AllocHeaderHook                                             (const AllocStack_AllocHeaderHook<BaseHookTypeArg, MainObjectTypeArg> &)                        = default;

			// previous memory allocation (or nullptr)
			BaseHookTypeArg                                    *GetPrev              () const                                                                                      { return _pPrev; }

			// get main object (or nullptr)
			MainObjectTypeArg                                *GetMainObject        () const                                                                                      { return _pMainObject; }

			// Function to bind prev alloaction pointer:
			// To be used by the memory container class.
			void                                              BindPrev             (BaseHookTypeArg *pInPrev)                                                                      { _pPrev = pInPrev; }

			// Bind main object pointer
			void                                              BindMainObject       (MainObjectTypeArg *pInMainObject)                                                            { _pMainObject = pInMainObject; }

		private:
			BaseHookTypeArg                                    *_pPrev              = 0;
			MainObjectTypeArg                                *_pMainObject        = nullptr;
		};

		/***************************************************************************************************
		* Std allocation header
		****************************************************************************************************/
		template<class MainObjectTypeArg>
		class AllocStack_AllocHeader : public AllocStack_AllocHeaderHook<AllocStack_AllocHeader<MainObjectTypeArg>, MainObjectTypeArg>
		{
		public:
		};

		/***************************************************************************************************
		* Typed allocation stack
		*
		* Represents stack of typed and untyped memory allocations (allocations can be pushed and popped).
		* Poppable-chunk's main allocation is always typed 
		* (with subclass of the main type, provided as the template argument).
		* Extra allocations are untyped and may be added to the poppable chunk.
		* So, each poppable allocation may consists of a number of smaller allocations.
		*
		* Poppable allocation structure:
		* 1. Allocation header
		* 2. [Optionally] Some extra data
		* 3. [Optionally] Typed allocation
		* 4. [Optionally] Some extra data
		*
		* As you see, allocation may contain NO main class instance at all.
		*
		* Alignment:
		* - Alignment can be performed by adding pad-bytes as extra data.
		*
		* Destruction:
		* - Objects are destroyed at the container destruction time
		* ONLY in the SPECIAL OWNED stack implementation.
		* - Extra object are NEVER destroyed at the container destruction time.
		*
		* Iteration:
		* - Iterating the stack makes no sense, so the corresponding functionality is not provided.
		*
		* Template parameters:
		* 1. T - main object type;
		* 2. HeaderType - type of header to be used (must be a subclass of MemView_ListStack_HeaderBaseHook).
		*
		* TODO:
		* 1. Create classes: 
		* 1.1. byte array mem view (to make it accessible from this view)
		***************************************************************************************************/
		template<class T, class HeaderType = AllocStack_AllocHeader<T> >
		class MemView_TypedAllocStack
		{
		public:
			// typedefs
			using value_type                         = typename T;
			using reference                          = value_type&;
			using const_reference                    = const value_type&;
			using pointer                            = value_type *;
			using const_pointer                      = const value_type *;

			// byte array type
			using expandable_byte_array_type         = typename std::conditional<std::is_const<typename T>::value, MemView_ConstExpandableByteArray, MemView_ExpandableByteArray>::type;

			/**********************************************************************************************************************
			* Trivial destructor
			**********************************************************************************************************************/
			~MemView_TypedAllocStack                                                                                             () = default;

			/**********************************************************************************************************************
			* Default ctor: leaves stack in uninitialized state
			**********************************************************************************************************************/
			constexpr MemView_TypedAllocStack                                                                                    () = default;

			/**********************************************************************************************************************
			* Ctor: construct ointed to the given memory address and the given max size (IN BYTES)
			**********************************************************************************************************************/
			constexpr MemView_TypedAllocStack                                                                          (size_t InMaxSizeInBytes, void *pInPtr);

			/**********************************************************************************************************************
			* Validity checking
			**********************************************************************************************************************/
			constexpr bool                                                              is_valid                       () const                         { return _byteArray.is_valid(); }

			/**********************************************************************************************************************
			* Move construction and move assignment NEVER to be deleted
			**********************************************************************************************************************/
			MemView_TypedAllocStack                                                                                     (MemView_TypedAllocStack<T, HeaderType>&& InOther) = default;
			MemView_TypedAllocStack<T, HeaderType>&                                     operator=                       (MemView_TypedAllocStack<T, HeaderType>&& InOther) = default;

			/**********************************************************************************************************************
			* Checking poppability
			**********************************************************************************************************************/
			bool                                                                        empty                           () const { return _pTopAlloc == nullptr; }
			// Getting pointer to header of the top allocation (return nullptr if no alloc has been performed yet);
			HeaderType                                                                 *GetTopAlloc                     () const { return _pTopAlloc; }

			/**********************************************************************************************************************
			* Pop operations
			**********************************************************************************************************************/
			// Pop all allocations up to last header
			void                                                                       pop_memory                      ();

			/**********************************************************************************************************************
			* Push header only without any other allocations
			**********************************************************************************************************************/
			void                                                                       push_header                     (const HeaderType& InHeader = HeaderType());

			/**********************************************************************************************************************
			* Allocating header immediately followed by instance of (possibly subclass) of the main class.
			**********************************************************************************************************************/
			template<class... Args>
			T                                                                         *emplace_back_user_header        (const HeaderType& InHeader, Args&&... args);

			// Allocate instance of the given subclass for the given class.
			template<class SubclassOfT, class... Args>
			SubclassOfT                                                               *emplace_back_user_header        (const HeaderType& InHeader, Args&&... args);

			// Allocate copy of the given element on the stack;
			// Class must be a subclass of the given class.
			// Concrete class is allocated - slicing is avoided.
			template<class SubclassOfT>
			SubclassOfT                                                               *push_user_header                (const HeaderType& InHeader, const SubclassOfT& InValue);

			/**********************************************************************************************************************
			* Allocate main element, prepended with default header
			**********************************************************************************************************************/
			template<class... Args>
			T                                                                         *emplace_back                    (Args&&... args);

			// Allocate new stacked element of the given class;
			// WARNING!!! The class must be a subclass of the given class.
			template<class SubclassOfT, class... Args>
			SubclassOfT                                                               *emplace_back                    (Args&&... args);

			template<class SubclassOfT>
			SubclassOfT                                                               *push                            (const SubclassOfT& InValue);

			/**********************************************************************************************************************
			* Allocate only main element
			* WARNING!!! RESTRICTIONS:
			* - The header must already be allocated;
			* - No other main element may already be allocated.
			**********************************************************************************************************************/
			template<class... Args>
			T                                                                         *emplace_main_elem_back           (Args&&... args);

			template<class SubclassOfT, class... Args>
			SubclassOfT                                                               *emplace_main_elem_back           (Args&&... args);

			template<class SubclassOfT>
			SubclassOfT                                                               *push_main_elem                   (const SubclassOfT& InValue);

			/*************************************************************************************************************************
			* Allocating extra objects (in addition to current allocation)
			*************************************************************************************************************************/
			template<class TypeToCreate, class... Args>
			TypeToCreate                                                              *emplace_extra_back              (Args&&... args);
			template<class TypeToCreate>
			TypeToCreate                                                              *push_extra                      (const TypeToCreate& InValue);

		private:
			void                                                                      _SetTopAllocPointer              (void *pInPtr) { _pTopAlloc = static_cast<HeaderType *>(pInPtr); }

			expandable_byte_array_type                                                 _byteArray;
			HeaderType                                                                *_pTopAlloc                      = nullptr;
		};

		/*********************************************************************************************************************************************************
		* Disposing arrays of main-typed values:
		*
		* Arrays are always disposed as a sequence of allocations,
		* so destructing them is also to be perfomed in opposite order, with a sequence of elementary pop_memory() calls.
		**********************************************************************************************************************************************************/
		template<class AllocStackTypeArg, class MainObjectTypeArg>
		void pop_array_memory(AllocStackTypeArg& stack, size_t InArraySize, MainObjectTypeArg *pInObjectArrayPointer);

		template<class AllocStackTypeArg, class MainObjectTypeArg>
		MainObjectTypeArg *push_array(AllocStackTypeArg& stack, const MainObjectTypeArg& InValue, size_t InArraySize);

		template<class AllocStackTypeArg, class MainObjectTypeArg, class ... Args>
		MainObjectTypeArg *emplace_array(AllocStackTypeArg& stack, size_t InArraySize, Args&&... InArgs);

		// Destruct the main object, then unpop the last allocation;
		// Prerequisites: the given pointer must point to the last-allocated object (checked at runtime with assert);
		template<class HeaderBaseHookType, class MainObjectTypeArg, template<class, class> class HeaderType>
		void destruct_and_pop(MemView_TypedAllocStack<MainObjectTypeArg, HeaderType<HeaderBaseHookType, MainObjectTypeArg>>& stack, MainObjectTypeArg *pLastAllocMainObject);


		/*********************************************************************************************************************************************************
		* Disposing arrays of extra values:
		*
		* Arrays are always disposed as a sequence of extra allocations;
		* Poppable extra arrays are poppable with a single pop operation.
		**********************************************************************************************************************************************************/
		template<class AllocStackTypeArg, class MainObjectTypeArg>
		MainObjectTypeArg *push_extra_array(AllocStackTypeArg& stack, const MainObjectTypeArg& InValue, size_t InArraySize);

		template<class AllocStackTypeArg, class MainObjectTypeArg, class ... Args>
		MainObjectTypeArg *emplace_extra_array(AllocStackTypeArg& stack, size_t InArraySize, Args&&... InArgs);

		template<class AllocStackTypeArg, class MainObjectTypeArg>
		MainObjectTypeArg *push_poppable_extra_array(AllocStackTypeArg& stack, const MainObjectTypeArg& InValue, size_t InArraySize);

		template<class AllocStackTypeArg, class MainObjectTypeArg, class ... Args>
		MainObjectTypeArg *emplace_poppable_extra_array(AllocStackTypeArg& stack, size_t InArraySize, Args&&... InArgs);

		/************************************************************************************************************************
		* Allocator on the typed alloc stack;
		*
		* Deallocations MUST be performed in the order opposite to dellocations
		* (this is controlled at runtime using assert).
		************************************************************************************************************************/
		template<class MainObjectType, class HeaderType>
		class AllocStackAllocator
		{
		public:
			using value_type                                                                = typename MainObjectType;

			AllocStackAllocator                                                             () noexcept                                                                  = default;
			AllocStackAllocator                                                             (MemView_TypedAllocStack<MainObjectType, HeaderType> *pInStack);

			AllocStackAllocator                                                             (AllocStackAllocator<MainObjectType, HeaderType>& InOther)                   = default;
			AllocStackAllocator<MainObjectType, HeaderType>&             operator=          (AllocStackAllocator<MainObjectType, HeaderType>& InOther)                   = default;

			MainObjectType                                              *allocate           (size_t InNumElems);
			void                                                         deallocate         (MainObjectType *p, std::size_t num);

		private:
			MemView_TypedAllocStack<MainObjectType, HeaderType>         *_pStack                                                                                         = nullptr;
		};

		template<class MainObjectType, class HeaderType>
		AllocStackAllocator<MainObjectType, HeaderType>::AllocStackAllocator(MemView_TypedAllocStack<MainObjectType, HeaderType> *pInStack) :
			_pStack(pInStack)
		{
			assert(_pStack);
		}

		template<class MainObjectType, class HeaderType>
		MainObjectType *AllocStackAllocator<MainObjectType, HeaderType>::allocate(size_t InNumElems)
		{
			assert(!"Not yet implemented");
			emplace_array(*_pStack, InNumElems);
		}

		template<class MainObjectType, class HeaderType>
		void AllocStackAllocator<MainObjectType, HeaderType>::deallocate(MainObjectType *p, std::size_t num)
		{
			assert(p);
			pop_array_memory(*_pStack, num, p);
		}


		/************************************************************************************************************************
		* MemView_TypedAllocStack helper functions implementation
		************************************************************************************************************************/
		template<class AllocStackTypeArg, class MainObjectTypeArg>
		void pop_array_memory(AllocStackTypeArg& stack, size_t InArraySize, MainObjectTypeArg *pInObjectArrayPointer)
		{
			assert(InArraySize > 0);
			for (auto i = static_cast<int>(InArraySize); --i >= 0; )
			{
				assert(false == stack.empty());
				if (i == 0)
				{
					assert(stack.GetTopAlloc()->GetMainObject() == pInObjectArrayPointer);
				}
				// deallocate and pop
				stack.GetTopAlloc()->GetMainObject()->~MainObjectTypeArg();
				stack.pop_memory();
			}
		}

		template<class AllocStackTypeArg, class MainObjectTypeArg>
		MainObjectTypeArg *push_array(AllocStackTypeArg& stack, const MainObjectTypeArg& InValue, size_t InArraySize)
		{
			assert(InArraySize > 0);
			auto pArrayPointer = stack.push(InValue);
			for (auto i = 1; i < static_cast<int>(InArraySize); i++)
			{
				stack.push(InValue);
			}
			return pArrayPointer;
		}

		template<class AllocStackTypeArg, class MainObjectTypeArg, class ... Args>
		MainObjectTypeArg *emplace_array(AllocStackTypeArg& stack, size_t InArraySize, Args&&... InArgs)
		{
			assert(InArraySize > 0);
			auto pArrayPointer = stack.emplace_back(std::forward<Args>(InArgs)...);
			for (auto i = 1; i < static_cast<int>(InArraySize); i++)
			{
				stack.emplace_back(std::forward<Args>(InArgs)...);
			}
			return pArrayPointer;
		}

		template<class AllocStackTypeArg, class MainObjectTypeArg>
		MainObjectTypeArg *push_extra_array(AllocStackTypeArg& stack, const MainObjectTypeArg& InValue, size_t InArraySize)
		{
			assert(InArraySize > 0);
			auto pArrayPointer = stack.push_extra(InValue);
			for (auto i = 1; i < static_cast<int>(InArraySize); i++)
			{
				stack.push_extra(InValue);
			}
			return pArrayPointer;
		}

		template<class AllocStackTypeArg, class MainObjectTypeArg, class ... Args>
		MainObjectTypeArg *emplace_extra_array(AllocStackTypeArg& stack, size_t InArraySize, Args&&... InArgs)
		{
			assert(InArraySize > 0);
			auto pArrayPointer = stac.emplace_extra_back(std::forward<Args>(InArgs)...);
			for (auto i = 1; i < static_cast<int>(InArraySize); i++)
			{
				stack.emplace_extra_back(std::forward<Args>(InArgs)...);
			}
			return pArrayPointer;
		}

		template<class AllocStackTypeArg, class MainObjectTypeArg>
		MainObjectTypeArg *push_poppable_extra_array(AllocStackTypeArg& stack, const MainObjectTypeArg& InValue, size_t InArraySize)
		{
			return push_extra_array(stack, InValue, InArraySize);
		}

		template<class AllocStackTypeArg, class MainObjectTypeArg, class ... Args>
		MainObjectTypeArg *emplace_poppable_extra_array(AllocStackTypeArg& stack, size_t InArraySize, Args&&... InArgs)
		{
			return emplace_extra_array(stack, InArraySize, std::forward<Args>(InArgs)...);
		}

		template<class HeaderBaseHookType, class MainObjectTypeArg, template<class, class> class HeaderType>
		void destruct_and_pop(MemView_TypedAllocStack<MainObjectTypeArg, HeaderType<HeaderBaseHookType, MainObjectTypeArg>>& stack, MainObjectTypeArg *pLastAllocMainObject)
		{
			assert(pLastAllocMainObject == nullptr);
			assert(false == stack.empty());
			assert(pLastAllocMainObject == stack.GetTopAlloc()->GetMainObject());
			stack.GetTopAlloc()->GetMainObject()->~MainObjectTypeArg();
			stack.pop_memory();
		}

		/************************************************************************************************************************
		* MemView_TypedAllocStack implementation
		************************************************************************************************************************/
		template<class T, class HeaderType>
		constexpr MemView_TypedAllocStack<T, HeaderType>::MemView_TypedAllocStack(size_t InMaxSizeInBytes, void *pInPtr) :
			_byteArray(InMaxSizeInBytes, pInPtr)
		{
		}

		template<class T, class HeaderType>
		void MemView_TypedAllocStack<T, HeaderType>::pop_memory()
		{
			assert(_pTopAlloc);
			shrink_to_new_end(_byteArray, _pTopAlloc);
			_pTopAlloc = _pTopAlloc->GetPrev();
		}

		template<class T, class HeaderType>
		void MemView_TypedAllocStack<T, HeaderType>::push_header(const HeaderType& InHeader)
		{
			// WARNING!!! Header must be POD-type
			HeaderType headerValue(InHeader);
			// In this case, _pTopAlloc DO MAY BE nullptr
			// (in case of where we had no headers allocated before).
			headerValue.BindPrev(_pTopAlloc);

			_SetTopAllocPointer(get_end_pointer(_byteArray));
			expand_with_bytes(_byteArray, headerValue);
		}

		template<class T, class HeaderType>
		template<class... Args>
		T *MemView_TypedAllocStack<T, HeaderType>::emplace_back_user_header(const HeaderType& InHeader, Args&&... args)
		{
			return emplace_back_user_header<T>(InHeader, std::forward<Args>(args)...);
		}

		template<class T, class HeaderType>
		template<class SubclassOfT, class... Args>
		SubclassOfT *MemView_TypedAllocStack<T, HeaderType>::emplace_back_user_header(const HeaderType& InHeader, Args&&... args)
		{
			push_header(InHeader);
			return emplace_main_elem_back<SubclassOfT>(std::forward<Args>(args)...);
		}

		template<class T, class HeaderType>
		template<class SubclassOfT>
		SubclassOfT *MemView_TypedAllocStack<T, HeaderType>::push_user_header(const HeaderType& InHeader, const SubclassOfT& InValue)
		{
			return emplace_back_user_header<SubclassOfT>(InHeader, InValue);
		}

		template<class T, class HeaderType>
		template<class... Args>
		T *MemView_TypedAllocStack<T, HeaderType>::emplace_back(Args&&... args)
		{
			return emplace_back<T>(std::forward<Args>(args)...);
		}

		template<class T, class HeaderType>
		template<class SubclassOfT, class... Args>
		SubclassOfT *MemView_TypedAllocStack<T, HeaderType>::emplace_back(Args&&... args)
		{
			return emplace_back_user_header<SubclassOfT>(HeaderType(), std::forward<Args>(args)...);
		}

		template<class T, class HeaderType>
		template<class SubclassOfT>
		SubclassOfT *MemView_TypedAllocStack<T, HeaderType>::push(const SubclassOfT& InValue)
		{
			return push_user_header<SubclassOfT>(HeaderType(), InValue);
		}

		template<class T, class HeaderType>
		template<class... Args>
		T *MemView_TypedAllocStack<T, HeaderType>::emplace_main_elem_back(Args&&... args)
		{
			return emplace_main_elem_back<T>(std::forward<Args>(args)...);
		}

		template<class T, class HeaderType>
		template<class SubclassOfT, class... Args>
		SubclassOfT *MemView_TypedAllocStack<T, HeaderType>::emplace_main_elem_back(Args&&... args)
		{
			assert(_pTopAlloc);
			// Check that NO main element has been created in this allocation:
			assert(nullptr == _pTopAlloc->GetMainObject());
			auto pMainObject = _byteArray.emplace_back<SubclassOfT>(std::forward<Args>(args)...);
			_pTopAlloc->BindMainObject(pMainObject);
			return pMainObject;
		}

		template<class T, class HeaderType>
		template<class SubclassOfT>
		SubclassOfT *MemView_TypedAllocStack<T, HeaderType>::push_main_elem(const SubclassOfT& InValue)
		{
			return emplace_main_elem_back<SubclassOfT>(InValue);
		}

		template<class T, class HeaderType>
		template<class TypeToCreate, class... Args>
		TypeToCreate *MemView_TypedAllocStack<T, HeaderType>::emplace_extra_back(Args&&... args)
		{
			return _byteArray.emplace_back<TypeToCreate>(std::forward<Args>(args)...);
		}

		template<class T, class HeaderType>
		template<class TypeToCreate>
		TypeToCreate *MemView_TypedAllocStack<T, HeaderType>::push_extra(const TypeToCreate& InValue)
		{
			assert(_pTopAlloc);
			return expand_with_bytes(_byteArray, InValue);
		}
	} // Util
} // Dv