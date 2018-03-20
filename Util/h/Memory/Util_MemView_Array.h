#pragma once

#include <cstddef>
#include <iterator>
#include <cassert>
#include <type_traits>

namespace Dv
{
	namespace Util	
	{
		/********************************************************************************************************************************
		* Fwd decalrations
		********************************************************************************************************************************/
		template<class T>
		class MemView_FixedSizeArray;

		template<class T>
		class MemView_ExpandableArray;

		/*******************************************************************************************************************************
		* Base class of all array memory views.
		*
		* Inheritance is used only as the remedy to avoid copy-pasting;
		*
		* WARNING!!! USER CLASSED SHOULD NEVER INHERIT FROM THIS CLASS!!!
		*******************************************************************************************************************************/
		template<class T>
		class MemView_ArrayBase
		{
		public:
			// typedefs
			using value_type = typename T;
			using size_type = size_t;
			using difference_type = std::ptrdiff_t;
			using reference = value_type&;
			using const_reference = const value_type&;
			using pointer = value_type *;
			using const_pointer = const value_type *;
			using iterator = typename T *;
			using const_iterator = typename const T *;
			using reverse_iterator = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;
			using void_pointer = typename std::conditional<std::is_const<typename T>::value, const void *, void *>::type;

			// trivial destructor:
			~MemView_ArrayBase                                                     () = default;

			// default constructor
			constexpr MemView_ArrayBase                                            () = default;

			// ctor: initializes view to the specific memory region
			constexpr MemView_ArrayBase                                            (void_pointer InPtr, size_t InNumElems);

			// Move construction and move-assignment is redefined, so that we can
			// easily subclass classes that can have move semantics:
			//
			// IMPL NOTE:
			// Do NOT forget inside the move operator= to delete the old content, if it's owned.
			MemView_ArrayBase                                                      (MemView_ArrayBase<T>&& InOther);
			MemView_ArrayBase<T>&                  operator=                       (MemView_ArrayBase<T>&& InOther);

			// Copy state as-is
			MemView_ArrayBase                                                      (const MemView_ArrayBase<T>& InOther) = default;
			MemView_ArrayBase<T>&                  operator=                       (const MemView_ArrayBase<T>& InOther) = default;

			// Copy constructors with template argument:
			// We need it for copying <const T> views to <T> views;
			// In this case <const T>-view copy constructors and operators cannot be used,
			// because it will cause a conflict - two function with the same signature defined for <const T> views.
			template<class SourceT>
			MemView_ArrayBase                                                      (const MemView_ArrayBase<SourceT>& InOther);

			template<class SourceT>
			MemView_ArrayBase<T>&                  operator=                       (const MemView_ArrayBase<SourceT>& InOther);

			// checking that array is in valid state
			constexpr bool                         is_valid                        () const { return _ptr != nullptr; }

			// Data pointers:
			// - NEVER return nullptr - use validity checks to check that the view is in appropriate state
			constexpr T                           *data                            () const { assert(_ptr); return _ptr; }

			// Size in elements
			// WARNING: Array view DO can be empty (useful behaviour for expandable arrays)
			constexpr size_t                      size                             () const { assert(is_valid()); return _numElems; }
			constexpr bool                        empty                            () const { assert(is_valid()); return _numElems == 0; }

			// access by index
			constexpr T&                          operator                         [](size_t InIndex) const                         { assert(_ptr); assert(InIndex < _numElems); return _ptr[InIndex]; }
			constexpr T&                          at                               (size_t InIndex) const                           { assert(_ptr); return operator[](InIndex); }
			constexpr T&                          front                            () const                                         { assert(_ptr); assert(size() > 0);  return *_ptr; }
			constexpr T&                          back                             () const                                         { assert(_ptr); assert(size() > 0);  return _ptr[_numElems - 1]; }

			// iterator access
			iterator                              begin                            () const { assert(is_valid()); return _ptr; }
			iterator                              end                              () const { assert(is_valid()); return _ptr + _numElems; }
			const_iterator                        cbegin                           () const { assert(is_valid()); return _ptr; }
			const_iterator                        cend                             () const { assert(is_valid()); return _ptr + _numElems; }
			reverse_iterator                      rbegin                           () const { return reverse_iterator(end()); }
			reverse_iterator                      rend                             () const { return reverse_iterator(begin()); }
			const_reverse_iterator                crbegin                          () const { return rbegin(); }
			const_reverse_iterator                crend                            () const { return rend(); }

		protected: // protected operations section
			// constructing objects
			// WARNING!!! Object will be created using placement-new!!!
			// If your object has non-trivial destructor (for example, is POD type), you will need to deallocate it 
			// by CALLING DESTRUCTOR DIRECTLY!!! But NOT by using delete!!!!

			// constructing objects of the specified type, using the given Index (which is equal to offset for byte arrays)
			template<class TypeToCreate, class... Args>
			TypeToCreate                        *_emplace_overwrite_at                      (size_t InIndex, Args&&... args);

			// constructing objects of the container element type by newing (and overwriting the old one)
			template<class... Args>
			T                                   *_emplace_overwrite_at                      (size_t InIndex, Args&&... args);

			// Element construction
			void                                _default_construct_range                    (size_t InStartIndex, size_t InEndIndex);
			void                                _default_construct_all                      () { return _default_construct_range(0, _numElems); }
			void                                _default_construct                          (size_t InIndex);

			// fill: copy-assign given value to all elements of the given array
			void                                _fill                                       (const T& InValue);

			// Destruction functions:
			// This functions manually call the destructor for given range of elements;
			// Useful for implementations of owned containers.
			// (endIndex is always next element to last destroyed, like in iterators)
			void                                _destruct_range                             (size_t InStartIndex, size_t InEndIndex);

			// Destruct all elements
			void                                _destruct_all                                () { return _destruct_range(0, _numElems); }

			void                                _destruct                                    (size_t InIndex);

			// working with sizes: set number of elems without restrictions
			void                                _set_num_elems                               (size_t InNumNumElems) { _numElems = InNumElems; }

		protected:
			T             *_ptr                 = nullptr;
			size_t         _numElems            = 0;
		};

		template<class T> T *get_end_pointer(const MemView_ArrayBase<T>& InArray);

		template<class T> T *get_end_pointer(const MemView_ArrayBase<T>& InArray)
		{
			assert(InArray.is_valid());
			return InArray.data() + InArray.size();
		}

		/******************************************************************************************************************************
		* Memory fixed size array view
		*
		* Represents fixed-sized array that does NOT own memory pointed to
		*  - Does NOT own memory pointed to;
		*  - Does NOT allocate or deallocate memory;
		*  - DefaultConstructible;
		*  - Pointer semantics:
		*	1) BUT: May NOT be nullptr;
		*  	2) CopyConstructible, CopyAssignable - Copied other view state as-is;
		*  	3) NOT MoveAssignable, NOT MoveConstructible
		*	4) Pointer-semantics consantness (see Const-correctness)
		*  - Const correctness:
			1) Pointer-semantics constness: Const means "view is constant, NOT data it points to";
		*	2) All pointer-getting operations are constant (ever if operation returns non-const memory);
		*  - size cannot be changed during lifetime (unless overrided with copying);
		*  - destruction:
		*	1) Object destruction - see comments for the corresponding class
		*
		* Operations:
		* - No std equality operators provided because they look ambigious: 
		*  1) should we use reference or value equality semantics?
		*  2) Should we use bitwise memory equality, or check equality using user-defined operator==?
		*
		* Conversions:
		* - Conversions to other mem views:
		* 1) Part of memory may be converted to expandable array memory view;
		******************************************************************************************************************************/
		template<class T> 
		class MemView_FixedSizeArrayBase : public MemView_ArrayBase<T>
		{
		public:
			// trivial destructor:
			~MemView_FixedSizeArrayBase                                                           () = default;

			// default constructor:
			constexpr MemView_FixedSizeArrayBase                                                  () = default;

			// ctor: initializes view to the specific memory region
			constexpr MemView_FixedSizeArrayBase                                                  (void_pointer InPtr, size_t InNumElems);

			// Move operations should NEVER be deleted, 
			// because otherwise it will not BE able to use copying/moving with rvalues.
			MemView_FixedSizeArrayBase                                                            (MemView_FixedSizeArrayBase<T>&& InOther);
			MemView_FixedSizeArrayBase<T>&                  operator=                             (MemView_FixedSizeArrayBase<T>&& InOther);

			// Copy state as-is
			MemView_FixedSizeArrayBase                                                            (const MemView_FixedSizeArrayBase<T>& InOther) = default;
			MemView_FixedSizeArrayBase<T>&                  operator=                             (const MemView_FixedSizeArrayBase<T>& InOther) = default;

			// Copy assignment and constructors: See ArrayBase for comments
			template<class SourceT>
			MemView_FixedSizeArrayBase                                                            (const MemView_FixedSizeArrayBase<SourceT>& InOther);

			template<class SourceT>
			MemView_FixedSizeArrayBase<T>&                  operator=                             (const MemView_FixedSizeArrayBase<SourceT>& InOther);

			// max_size() for fixed-size array is always equal to its size
			// (max size is specified in elements)
			constexpr size_t                                max_size                              () const { return _numElems; }			
		};
		
		/******************************************************************************************************
		* Fixed sized-array view, that does NOT own memory;
		* 
		* Objects WILL not be destructed or constructed automatically
		* - Use for types without user destructors (remember about base class destructors),
		* or see guidelines on the ArrayBase class construction functions.
		******************************************************************************************************/
		template<class T>
		class MemView_FixedSizeArray : public MemView_FixedSizeArrayBase<T>
		{
		public:
			// trivial destructor:
			~MemView_FixedSizeArray() = default;

			// default constructor
			constexpr MemView_FixedSizeArray() = default;

			// ctor: initializes view to the specific memory region
			constexpr MemView_FixedSizeArray                                            (void_pointer InPtr, size_t InNumElems);

			// Move operations should NEVER be deleted, 
			// because otherwise it will not BE able to use copying/moving with rvalues.
			MemView_FixedSizeArray                                                      (MemView_FixedSizeArray<T>&& InOther) = default;
			MemView_FixedSizeArray<T>&                  operator=                       (MemView_FixedSizeArray<T>&& InOther) = default;

			// Copy state as-is
			MemView_FixedSizeArray                                                      (const MemView_FixedSizeArray<T>& InOther) = default;
			MemView_FixedSizeArray<T>&                  operator=                       (const MemView_FixedSizeArray<T>& InOther) = default;

			// Copy assignment and constructors: See ArrayBase for comments
			template<class SourceT>
			MemView_FixedSizeArray                                                      (const MemView_FixedSizeArray<SourceT>& InOther);

			template<class SourceT>
			MemView_FixedSizeArray<T>&                  operator=                       (const MemView_FixedSizeArray<SourceT>& InOther);

			// fill: must be used with POD types only, because it will not be deleted
			void                                        fill                            (const T& InValue);

			// See comment for array base
			template<class... Args>
			T                                          *emplace_overwrite_at            (size_t InIndex, Args&&... args);

		private:
			T             *_ptr           = nullptr;
			size_t         _numElems      = 0;
		};

		/***********************************************************************************************************
		* Owned fixed-size array
		*
		* FixedSize array that automatically destroys (using T destructor) all its elements;
		* At construction time, array is filled in such a fashion that each of its elements
		* is constructed (see concrete constructor comments, which constructors are used).
		*
		* WARNING!!! NEVER use over typed memory with T type 
		* (for example, do NOT use over std::array<T,n> or over std::vector<T>)
		*
		* Notes:
		* - Makes sense to use ever for POD type, to make them automatically-constructed;
		* - Move-only type:
		*  1) Copy construction and assignment is forbidden because it's dangerous:
		* no two views can have the same ptr - data pointed to is owned!!!
		*  2) Moving will release ownership of the source view
		**********************************************************************************************************/
		template<class T>
		class MemView_OwnedFixedSizeArray : public MemView_FixedSizeArrayBase<T>
		{
		public:
			// destructor:
			~MemView_OwnedFixedSizeArray();

			// Default ctor: creates object in invalid state
			constexpr                                       MemView_OwnedFixedSizeArray                  () = default;

			// ctor: initializes view to the specific memory region;
			// each element is constructed using default constructor.
			constexpr                                       MemView_OwnedFixedSizeArray                   (void_pointer InPtr, size_t InNumElems);

			// Forbid copy-construction and move-assignment (see comment for class)
			MemView_OwnedFixedSizeArray                                                                   (const MemView_OwnedFixedSizeArray<T>& InOther) = delete;
			MemView_OwnedFixedSizeArray<T>&                 operator=                                     (const MemView_OwnedFixedSizeArray<T>& InOther) = delete;

			// Move data by moving pointer
			MemView_OwnedFixedSizeArray                                                                   (MemView_OwnedFixedSizeArray<T>&& InOther) = default;
			MemView_OwnedFixedSizeArray<T>&                 operator=                                     (MemView_OwnedFixedSizeArray<T>&& InOther);

			// Construct element override old element with it:
			// WARNING!!! Previous element will be destructed using destructor before creating new elment.
			template<class... Args>
			T                                              *emplace_overwrite_at                          (size_t InIndex, Args&&... args);
		};

		template<class T>
		MemView_OwnedFixedSizeArray<T>::~MemView_OwnedFixedSizeArray()
		{
			_destruct_all();
		}

		template<class T>
		constexpr MemView_OwnedFixedSizeArray<T>::MemView_OwnedFixedSizeArray(void_pointer InPtr, size_t InNumElems) :
			MemView_FixedSizeArrayBase(InPtr, InNumElems)
		{
			static_assert(std::is_default_constructible<T>::value, "MemView_OwnedFixedSizeArray<T>::ctor(void_pointer InPtr, size_t InNumElems): T must conform is_default_constructible<T>");
			static_assert(std::is_destructible<T>::value, "MemView_OwnedFixedSizeArray<T>::ctor(void_pointer InPtr, size_t InNumElems): T must conform is_destructible<T>");
			_default_construct_all();
		}

		template<class T>
		MemView_OwnedFixedSizeArray<T>& MemView_OwnedFixedSizeArray<T>::operator=(MemView_OwnedFixedSizeArray<T>&& InOther)
		{
			_destruct_all();
			return MemView_FixedSizeArrayBase<T>::operator=(std::move(InOther));
		}

		template<class T>
		template<class... Args>
		T *MemView_OwnedFixedSizeArray<T>::emplace_overwrite_at(size_t InIndex, Args&&... args)
		{
			// Destroy element at the given index
			_destruct(InIndex);
			return _emplace_overwrite_at(InIndex, std::forward<Args>(args)...);
		}

		/*************************************************************************************************************
		* expandable array base class
		*************************************************************************************************************/
		template<class T>
		class MemView_ExpandableArrayBase : public MemView_ArrayBase<T>
		{
		public:
			// trivial destructor
			~MemView_ExpandableArrayBase() = default;

			// default constructor:
			constexpr MemView_ExpandableArrayBase() = default;

			// Move construct and move assign using ArrayBase method;
			MemView_ExpandableArrayBase                                                                (MemView_ExpandableArrayBase<T>&& InOther);
			MemView_ExpandableArrayBase<T>&                            operator=                       (MemView_ExpandableArrayBase<T>&& InOther);

			// ctor: create array with the given number of elements
			MemView_ExpandableArrayBase                                                                (size_t InMaxSize, void_pointer InPtr, size_t InNumElems = 0);

			// Copy state as-is
			MemView_ExpandableArrayBase                                                                (const MemView_ExpandableArrayBase<T>& InOther) = default;
			MemView_ExpandableArrayBase<T>&                            operator=                       (const MemView_ExpandableArrayBase<T>& InOther) = default;

			template<class SourceT>
			MemView_ExpandableArrayBase                                                                (const MemView_ExpandableArrayBase<SourceT>& InOther);

			template<class SourceT>
			MemView_ExpandableArrayBase<T>&                            operator=                       (const MemView_ExpandableArrayBase<SourceT>& InOther);

			// ctor: create as subview with the given set of elements,
			// elements can belong to the uninitialized part of the expandable array
			MemView_ExpandableArrayBase                                (const MemView_FixedSizeArray<T>& InFromView, size_t InNumElements, size_t InStartIndex = 0);
			MemView_ExpandableArrayBase                                (const MemView_ExpandableArray<T>& InFromView, size_t InNumElements, size_t InStartIndex = 0);

			// maximal size can be reduced, but not less than current size of the array:
			// 
			void                                                       reduce_max_size                 (size_t InNewMaxSize) { assert(InNewMaxSize <= _maxSize);  assert(InNewMaxSize >= size()); _maxSize = InNewMaxSize; }

			// max size is specified in elements:
			constexpr size_t                                           max_size                        () const { return _maxSize; }

			// insert copy of the element, and return address of the new element
			T                                                         *push_back                       (const T& InNewElement);

			// emplace new element back, increasing size by one
			template<class... Args>
			T                                                         *emplace_back                    (Args&&... args);

		protected:
			// Construct and emplace object, increasing size by the size of an object
			template<class TypeToCreate, class... Args>
			TypeToCreate                                             *_emplace_back                    (Args&&... args);

		protected:
			// Increase size, giving new value to size,
			// WARNING!!! New size must always be NOT less than the old size
			void                                                      _set_expanded_size               (size_t InNewSize) { assert(_numElems <= InNewSize); _resize(InNewSize); }

			// Decrease size, giving new value to size,
			// WARNING!!! New size must always be NOT greater than the old size
			void                                                      _set_diminished_size             (size_t InNewSize) { assert(InNewSize <= _numElems); _resize(InNewSize); }

		private:
			void                                                      _resize                          (size_t InNewSize)      { assert(InNewSize <= _maxSize); _numElems = InNewSize; }

			size_t                                                    _maxSize = 0;
		};		
		// ---------------------------------------------------------------------------------------
		// WARNING!!! This functions are applicable to all types of expandable array views,
		// including byte array views
		// ---------------------------------------------------------------------------------------
		// Freeze size of the expandable array, so it cannot grow anymore
		template<class T>
		void freeze_size(MemView_ExpandableArrayBase<T>& InArray);

		template<class T>
		size_t get_num_free_elements(MemView_ExpandableArrayBase<T>& InArray);

		// Increases size of the container by the given value;
		// Returns pointer to the first allocated element.
		template<class T, template<class> class ContainerType>
		T *expand_size_delta(ContainerType<T>& InArray, size_t InDelta);

		template<class T, template<class> class ContainerType>
		void diminish_size_delta(ContainerType<T>& InArray, size_t InDelta);

		template<class T, template<class> class ContainerType>
		void shrink_to_new_end(ContainerType<T>& InArray, void *pInNewEnd);

		/*************************************************************************************************************
		* The same as fixed size array, but that can be expanded from the end.
		*
		* Elements of expandable array can NOT be overwritten, 
		* as it makes no sense - you can use fixed-size array for that purpose.
		*************************************************************************************************************/
		template<class T>
		class MemView_ExpandableArray : public MemView_ExpandableArrayBase<T>
		{
		public:
			// trivial destructor
			~MemView_ExpandableArray                               () = default;

			// default constructor
			constexpr MemView_ExpandableArray                      () = default;

			// Moving: 
			// Moves expandable array to another.
			MemView_ExpandableArray                                                                (MemView_ExpandableArray<T>&& InOther) = default;
			MemView_ExpandableArray<T>&                            operator=                       (MemView_ExpandableArray<T>&& InOther) = default;

			// ctor: create array with the given number of elements
			MemView_ExpandableArray                                (size_t InMaxSize, void_pointer InPtr, size_t InNumElems = 0);
			// ctor: create as subview with the given set of elements,
			// elements can belong to the uninitialized part of the expandable array
			MemView_ExpandableArray                                (const MemView_FixedSizeArray<T>& InFromView, size_t InNumElements, size_t InStartIndex = 0);
			MemView_ExpandableArray                                (const MemView_ExpandableArray<T>& InFromView, size_t InNumElements, size_t InStartIndex = 0);

			// Copying is forbidden - it's dangerous to be able to copy expandable array, 
			// as it's dangerous to have two expandable views on the same array
			MemView_ExpandableArray                                                                (const MemView_ExpandableArray<T>& InOther) = default;
			MemView_ExpandableArray<T>&                            operator=                       (const MemView_ExpandableArray<T>& InOther) = default;

			// Copy construction with template argument (see base class for explanation)
			template<class SourceT>
			MemView_ExpandableArray                                                                (const MemView_ExpandableArray<SourceT>& InOther);

			template<class SourceT>
			MemView_ExpandableArray<T>&                             operator=                      (const MemView_ExpandableArray<SourceT>& InOther);

			// Increase size, giving new value to size,
			// WARNING!!! New size must always be NOT less than the old size
			void                                                    set_expanded_size              (size_t InNewSize) { _set_expanded_size(InNewSize); }

			// Decrease size, giving new value to size,
			// WARNING!!! New size must always be NOT greater than the old size
			void                                                    set_diminished_size            (size_t InNewSize) { _set_diminished_size(InNewSize); }

			// resizing array without restrictions
			// (you can set arbitrary size less or higher than current, 
			// but new size to set of cause must be less than maximal)
			void                                                    resize                         (size_t InNewNumElems) { _resize(InNewNumElems); }
		};

		/*************************************************************************************************************
		* Expandable array that owns all of its elements:
		* - Constructs elements;
		* - Use destructor to destroy elements;
		*************************************************************************************************************/
		template<class T>
		class MemView_OwnedExpandableArray : public MemView_ExpandableArrayBase<T>
		{
		public:
			// overload destructor to delete all its values
			~MemView_OwnedExpandableArray();

			// default constructor: creates object in invalid state
			constexpr MemView_OwnedExpandableArray() = default;

			// Moving is allowed
			MemView_OwnedExpandableArray                                                                (MemView_OwnedExpandableArray<T>&& InOther)     = default;
			MemView_OwnedExpandableArray<T>&                            operator=                       (MemView_OwnedExpandableArray<T>&& InOther);

			// constructor:
			// all elements are to be default-constructed
			MemView_OwnedExpandableArray                                                                (size_t InMaxSize, void_pointer InPtr, size_t InNumElems = 0);

			// Copying is forbidden
			MemView_OwnedExpandableArray                                                                (const MemView_OwnedExpandableArray<T>& InOther)  = delete;
			MemView_OwnedExpandableArray<T>&                            operator=                       (const MemView_OwnedExpandableArray<T>& InOther)  = delete;

			// Increase size, giving new value to size;
			// WARNING!!! New size must always be NOT less than the old size;
			// Extra elements are to be properly constructed.
			void                                                       set_expanded_size                (size_t InNewSize);

			// Decrease size, giving new value to size,
			// WARNING!!! New size must always be NOT greater than the old size;
			// Old elements beyond the new size are to be properly destructed.
			void                                                       set_deminished_size              (size_t InNewSize);

			// resizing array without restrictions
			// (you can set arbitrary size less or higher than current, 
			// but new size to set of cause must be less than maximal);
			// Extra elements are to be properly constructed,
			// old elements beyond the new size are to be properly destructed.
			void                                                        resize                          (size_t InNewNumElems);

		private:
		};

		template<class T>
		void MemView_OwnedExpandableArray<T>::set_expanded_size(size_t InNewSize)
		{
			_set_expanded_size(InNewSize);
			_default_construct_range(_numElems, InNewSize);
		}

		
		template<class T>
		void MemView_OwnedExpandableArray<T>::set_deminished_size(size_t InNewSize)
		{
			_destruct_range(InNewNumElems, _numElems);
			_set_diminished_size(InNewSize); 
		}

		template<class T>
		void MemView_OwnedExpandableArray<T>::resize(size_t InNewNumElems)
		{			
			if(InNewNumElems < _numElems)
			{
				_destruct_range(InNewNumElems, _numElems); 
				_resize(InNewNumElems);
			}
			else
			{
				if(InNewNumElems != _numElems)
				{
					_resize(InNewNumElems);
					_default_construct_range(_numElems, InNewNumElems);
				}
			}
		}

		template<class T>
		MemView_OwnedExpandableArray<T>::~MemView_OwnedExpandableArray()
		{
			_destruct_all();
		}

		template<class T>
		MemView_OwnedExpandableArray<T>& MemView_OwnedExpandableArray<T>::operator=(MemView_OwnedExpandableArray<T>&& InOther)
		{
			_destruct_all();
			MemView_ExpandableArrayBase<T>::operator=(std::move(InOther));
			return *this;
		}

		template<class T>
		MemView_OwnedExpandableArray<T>::MemView_OwnedExpandableArray(size_t InMaxSize, void_pointer InPtr, size_t InNumElems) :
			MemView_ExpandableArrayBase(InMaxSize, InPtr, InNumElems)
		{
			static_assert(std::is_destructible<T>::value, "MemView_OwnedExpandableArray<T>::ctor(size_t InMaxSize, void_pointer InPtr, size_t InNumElems) T Must conform is_destructible<T>");
			// WARNING!!! Objects of this class are NOT necessarily must conform is_default_constructible
			// (because may have zero size).
			_default_construct_all();
		}

		/*************************************************************************************************************
		*  MemView_ExpandableArray implementation
		*************************************************************************************************************/
		template<class T>
		MemView_ExpandableArray<T>::MemView_ExpandableArray(size_t InMaxSize, void_pointer InPtr, size_t InNumElems) :
			MemView_ExpandableArrayBase(InMaxSize, InPtr, InNumElems)
		{
		}
		
		template<class T>
		MemView_ExpandableArray<T>::MemView_ExpandableArray(const MemView_FixedSizeArray<T>& InFromView, size_t InNumElements, size_t InStartIndex) :
			MemView_ExpandableArrayBase(InFromView, InNumElements, InStartIndex)
		{
			
		}

		template<class T>
		MemView_ExpandableArray<T>::MemView_ExpandableArray(const MemView_ExpandableArray<T>& InFromView, size_t InNumElements, size_t InStartIndex) :
			MemView_ExpandableArrayBase(InFromView, InNumElements, InStartIndex)
		{
		}

		template<class T>
		template<class SourceT>
		MemView_ExpandableArray<T>::MemView_ExpandableArray(const MemView_ExpandableArray<SourceT>& InOther) :
			MemView_ExpandableArrayBase(InOther)
		{
		}

		template<class T>
		template<class SourceT>
		MemView_ExpandableArray<T>& MemView_ExpandableArray<T>::operator=(const MemView_ExpandableArray<SourceT>& InOther)
		{
			return MemView_ExpandableArrayBase<T>::operator=(InOther);
		}

		/*************************************************************************************************************
		*  MemView_ArrayBase implementation
		*************************************************************************************************************/
		template<class T>
		constexpr MemView_ArrayBase<T>::MemView_ArrayBase(void_pointer InPtr, size_t InNumElems) :
			_ptr(static_cast<T *>(InPtr)),
			_numElems(InNumElems)
		{
			assert(InPtr);
			// WARNING! Num of elems DO can be zero - see corresponding comment in the class declaration
		}

		template<class T>
		MemView_ArrayBase<T>::MemView_ArrayBase(MemView_ArrayBase<T>&& InOther) :
			_ptr(InOther._ptr),
			_numElems(InOther._numElems)
		{
			InOther._ptr = nullptr;
			// WE Need to reset size of the source container to avoid calling destructors
			InOther._numElems = 0;
		}

		template<class T>
		MemView_ArrayBase<T>& MemView_ArrayBase<T>::operator=(MemView_ArrayBase<T>&& InOther)
		{
			// HINT!!! Here we do NOT know, whether the array elements are owned,
			// so it's wrong to destroy elements here - 
			// it's to be done inside concrete class move constructor.
			_ptr = InOther._ptr;
			_numElems = InOther._numElems;
			InOther._ptr = nullptr;
			// WE Need to reset size of the source container to avoid calling destructors
			InOther._numElems = 0;
			return *this;
		}

		template<class T>
		template<class SourceT>
		MemView_ArrayBase<T>::MemView_ArrayBase(const MemView_ArrayBase<SourceT>& InOther) :
			_ptr(InOther._ptr),
			_numElems(InOther._numElems)
		{

		}

		template<class T>
		template<class SourceT>
		MemView_ArrayBase<T>& MemView_ArrayBase<T>::operator=(const MemView_ArrayBase<SourceT>& InOther)
		{
			_ptr = InOther.data();
			_numElems = InOther.size();
			return *this;
		}

		// fill: construct array of copies of the given value
		template<class T>
		void MemView_ArrayBase<T>::_fill(const T& InValue)
		{
			static_assert(std::is_copy_assignable<T>::value, "MemView_ArrayBase<T>::_fill(const T& InValue): T must conform is_copy_assignable<T>");
			for (size_t i = 0; i < size(); i++)
			{
				at(i) = InValue;
			}
		}

		template<class T>
		template<class... Args>
		T *MemView_ArrayBase<T>::_emplace_overwrite_at(size_t InIndex, Args&&... args)
		{
			// WARNING!!! Why this check does NOT work?
			//std::static_assert(std::is_constructible<T, Args ...>::value, "ArrrayBase::_emplace_overwrite_at: Object must be constructible with the given set of arguments");
			return new(&at(InIndex)) T(std::forward<Args>(args)...);
		}

		template<class T>
		template<class TypeToCreate, class... Args>
		TypeToCreate *MemView_ArrayBase<T>::_emplace_overwrite_at(size_t InIndex, Args&&... args)
		{
			// WARNING!!! Why this check does NOT work?
			//std::static_assert(std::is_constructible<TypeToCreate, Args ...>::value, "ArrrayBase::_emplace_overwrite_at: Object must be constructible with the given set of arguments");
			assert(InIndex * sizeof(T) + sizeof(TypeToCreate) <= _numElems * sizeof(T));
			return new (&at(InIndex)) TypeToCreate(std::forward<Args>(args)...);
		}

		template<class T>
		void MemView_ArrayBase<T>::_default_construct_range(size_t InStartIndex, size_t InEndIndex)
		{
			for (auto i = InStartIndex; i < InEndIndex; i++)
			{
				_default_construct(i);
			}
		}

		template<class T>
		void MemView_ArrayBase<T>::_default_construct(size_t InIndex)
		{
			static_assert(std::is_default_constructible<T>::value, "MemView_ArrayBase<T>::_default_construct(size_t InIndex): T must conform is_default_constructible<T>");
			_emplace_overwrite_at(InIndex);
		}

		template<class T>
		void MemView_ArrayBase<T>::_destruct_range(size_t InStartIndex, size_t InEndIndex)
		{
			// WARNING!!! Start index may be equal to end index, for case of zero-length arrays
			assert(InStartIndex <= InEndIndex);
			assert(InEndIndex <= _numElems);
			for (int i = InStartIndex; i < InEndIndex; i++)
			{
				_destruct(i);
			}
		}

		template<class T>
		void MemView_ArrayBase<T>::_destruct(size_t InIndex)
		{
			static_assert(std::is_destructible<T>::value, "MemView_ArrayBase<T>::_destruct(size_t InIndex): T must conform is_destructible");
			assert(InIndex < _numElems);
			at(InIndex).~T();
		}

		/*************************************************************************************************************
		*  MemView_FixedSizeArrayBase implementation
		*************************************************************************************************************/
		// ctor: initializes view to the specific memory region
		template<class T>
		constexpr MemView_FixedSizeArrayBase<T>::MemView_FixedSizeArrayBase(void_pointer InPtr, size_t InNumElems) :
			MemView_ArrayBase(InPtr, InNumElems)
		{
		}

		template<class T>
		MemView_FixedSizeArrayBase<T>::MemView_FixedSizeArrayBase(MemView_FixedSizeArrayBase<T>&& InOther) :
			MemView_ArrayBase(std::move(InOther))
		{
		}

		template<class T>
		MemView_FixedSizeArrayBase<T>& MemView_FixedSizeArrayBase<T>::operator=(MemView_FixedSizeArrayBase<T>&& InOther)
		{
			return MemView_ArrayBase::operator=(std::move(InOther));
		}

		template<class T>
		template<class SourceT>
		MemView_FixedSizeArrayBase<T>::MemView_FixedSizeArrayBase(const MemView_FixedSizeArrayBase<SourceT>& InOther) :
			MemView_ArrayBase(InOther.data(), InOther.size())
		{
		}

		template<class T>
		template<class SourceT>
		MemView_FixedSizeArrayBase<T>& MemView_FixedSizeArrayBase<T>::operator=(const MemView_FixedSizeArrayBase<SourceT>& InOther)
		{
			MemView_ArrayBase<T>::operator=(InOther);
			return *this;
		}

		/*************************************************************************************************************
		*  MemView_FixedSizeArray implementation
		*************************************************************************************************************/
		template<class T>
		constexpr MemView_FixedSizeArray<T>::MemView_FixedSizeArray(void_pointer InPtr, size_t InNumElems) :
			MemView_FixedSizeArrayBase(InPtr, InNumElems)
		{
		}

		template<class T>
		template<class SourceT>
		MemView_FixedSizeArray<T>::MemView_FixedSizeArray(const MemView_FixedSizeArray<SourceT>& InOther) :
			MemView_FixedSizeArrayBase(InOther.data(), InOther.size())
		{
		}

		template<class T>
		template<class SourceT>
		MemView_FixedSizeArray<T>& MemView_FixedSizeArray<T>::operator=(const MemView_FixedSizeArray<SourceT>& InOther)
		{
			MemView_FixedSizeArrayBase<T>::operator=(InOther);
			return *this;
		}

		template<class T>
		void MemView_FixedSizeArray<T>::fill(const T& InValue)
		{
			assert(is_valid());
			assert(std::is_pod<T>::value);
			_fill(InValue);
		}

		template<class T>
		template<class... Args>
		T *MemView_FixedSizeArray<T>::emplace_overwrite_at(size_t InIndex, Args&&... args)
		{
			return _emplace_overwrite_at(InIndex, std::forward<Args>(args)...);
		}

		/******************************************************************************************************
		* ExpandableArrayBase implementation
		******************************************************************************************************/
		template<class T>
		void freeze_size(MemView_ExpandableArrayBase<T>& InArray)
		{
			InArray.reduce_max_size(InArray.size());
		}

		template<class T, template<class> class ContainerType>
		T *expand_size_delta(ContainerType<T>& InArray, size_t InDelta)
		{
			auto pWhereToPlace = static_cast<void *>(get_end_pointer(InArray));
			InArray.set_expanded_size(InArray.size() + InDelta);
			return static_cast<T *>(pWhereToPlace);
		}

		template<class T, template<class> class ContainerType>
		void diminish_size_delta(ContainerType<T>& InArray, size_t InDelta)
		{
			assert(InArray.size() >= InDelta);
			InArray.set_diminished_size(InArray.size() - InDelta);
		}

		template<class T, template<class> class ContainerType>
		size_t get_size_to_end_pointer(ContainerType<T>& InArray, void *pInNewEnd)
		{
			return ( static_cast<T *>(pInNewEnd) - InArray.data() );
		}

		template<class T, template<class> class ContainerType>
		void shrink_to_new_end(ContainerType<T>& InArray, void *pInNewEnd)
		{
			auto newSize = get_size_to_end_pointer(InArray, pInNewEnd);
			InArray.set_diminished_size(newSize);
		}

		template<class T>
		size_t get_num_free_elements(MemView_ExpandableArrayBase<T>& InArray)
		{
			return InArray.max_size() - InArray.size();
		}

		template<class T>
		MemView_ExpandableArrayBase<T>::MemView_ExpandableArrayBase(size_t InMaxSize, void_pointer InPtr, size_t InNumElems) :
			MemView_ArrayBase(InPtr, InNumElems),
			_maxSize(InMaxSize)
		{
			assert(InNumElems <= InMaxSize);
		}


		template<class T>
		MemView_ExpandableArrayBase<T>::MemView_ExpandableArrayBase(MemView_ExpandableArrayBase<T>&& InOther) :
			MemView_ArrayBase(std::move(InOther)),
			_maxSize(InOther.max_size())
		{

		}


		template<class T>
		MemView_ExpandableArrayBase<T>& MemView_ExpandableArrayBase<T>::operator=(MemView_ExpandableArrayBase<T>&& InOther)
		{
			MemView_ArrayBase<T>::operator=(std::move(InOther));
			return *this;
		}

		template<class T>
		MemView_ExpandableArrayBase<T>::MemView_ExpandableArrayBase(const MemView_FixedSizeArray<T>& InFromView, size_t InNumElements, size_t InStartIndex) :
			MemView_ArrayBase(InPtr + InStartIndex, InNumElems),
			_maxSize(InFromView.max_size())
		{
			assert((InStartIndex + InNumElements) <= InFromView.max_size());
		}

		template<class T>
		MemView_ExpandableArrayBase<T>::MemView_ExpandableArrayBase(const MemView_ExpandableArray<T>& InFromView, size_t InNumElements, size_t InStartIndex) :
			MemView_ArrayBase(InPtr + InStartIndex, InNumElems),
			_maxSize(InFromView.max_size())
		{
			assert((InStartIndex + InNumElements) <= InFromView.max_size());
		}

		template<class T>
		template<class SourceT>
		MemView_ExpandableArrayBase<T>::MemView_ExpandableArrayBase(const MemView_ExpandableArrayBase<SourceT>& InOther) :
			MemView_ArrayBase(InOther),
			_maxSize(InOther.max_size())
		{
		}

		template<class T>
		template<class SourceT>
		MemView_ExpandableArrayBase<T>& MemView_ExpandableArrayBase<T>::operator=(const MemView_ExpandableArrayBase<SourceT>& InOther)
		{
			_maxSize = InOther.max_size();
			return MemView_ArrayBase::operator=<T>(InOther);
		}

		template<class T>
		T *MemView_ExpandableArrayBase<T>::push_back(const T& InNewElement)
		{
			assert(_numElems < _maxSize);
			auto oldSize = _numElems;
			_numElems++;
			at(oldSize) = InNewElement;
			return &at(oldSize);
		}

		template<class T>
		template<class... Args>
		T *MemView_ExpandableArrayBase<T>::emplace_back(Args&&... args)
		{
			assert(_numElems < _maxSize);
			auto oldNumElems = _numElems;
			_numElems++;
			auto ptr_to_return = _emplace_overwrite_at(oldNumElems, std::forward<Args>(args)...);
			return ptr_to_return;
		}

		template<class T>
		template<class TypeToCreate, class... Args>
		TypeToCreate *MemView_ExpandableArrayBase<T>::_emplace_back(Args&&... args)
		{
			static_assert(sizeof(T) == sizeof(char), "MemView_ExpandableArrayBase::_emplace_back<TypeToCreate>: this function is to be used only for byte-arrays");
			auto oldSize = _numElems;
			_numElems += sizeof(TypeToCreate);
			auto ptr_to_return = _emplace_overwrite_at<TypeToCreate>(oldSize, std::forward<Args>(args)...);
			return ptr_to_return;
		}
	} // Util
} // Dv