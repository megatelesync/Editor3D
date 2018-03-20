#pragma once

#include "Util_MemView_Array.h"

/************************************************************************************************
* This header file includes special kinds of arrays that work on the byte level.
*
* Each such array includes additional operations for working on the byte level:
* - Creating objects of specified class by the given offset
*   (and, optionally, with the given alignment);
* - Getting void-pointers to their elements.
*
* This arrays are never aware of the content format,
* so they never call destructors on their elements.
* Actually, their elements are bytes.
*
* This arrays are NOT defined using array class specialization for uint8_t,
* because arrays of uint8_t are NOT necessarily byte arrays.
* Instead, byte arrays are implemented as subclasses of the corresponding array clases.
************************************************************************************************/

namespace Dv
{
	namespace Util
	{
		/***************************************************************************************************
		* Byte array template for const and non-const byte arrays;
		*
		* Useful in template-based code;
		* Most of type, however, template aliases to be used (see below the class definition)
		***************************************************************************************************/
		template<class T>
		class MemView_FixedSizeByteArrayTempl : public MemView_FixedSizeArray<T>
		{
		public:
			// trivial destructor:
			~MemView_FixedSizeByteArrayTempl                                                     () = default;

			// default constructor:
			MemView_FixedSizeByteArrayTempl                                                      () = default;

			// ctor: initializes view to the specific memory region
			constexpr MemView_FixedSizeByteArrayTempl                                            (void_pointer InPtr, size_t InNumElems);

			// Copy state as-is
			MemView_FixedSizeByteArrayTempl                                                      (const MemView_FixedSizeByteArrayTempl<T>& InOther) = default;
			MemView_FixedSizeByteArrayTempl<T>&                  operator=                       (const MemView_FixedSizeByteArrayTempl<T>& InOther) = default;

			// Move operations should NEVER be deleted, 
			// because otherwise it will not BE able to use copying/moving with rvalues.
			MemView_FixedSizeByteArrayTempl                                                      (MemView_FixedSizeByteArrayTempl<T>&& InOther) = default;
			MemView_FixedSizeByteArrayTempl<T>&                  operator=                       (MemView_FixedSizeByteArrayTempl<T>&& InOther) = default;
	
			// Copy constructors and operators for const views: 
			// (see array base for comments)
			template<class SourceT>
			MemView_FixedSizeByteArrayTempl                                                      (const MemView_FixedSizeByteArrayTempl<SourceT>& InOther);

			template<class SourceT>
			MemView_FixedSizeByteArrayTempl<T>&                  operator=                       (const MemView_FixedSizeByteArrayTempl<SourceT>& InOther);

			// Creating objects by the given offset
			// constructing objects of the specified type, using the given Index (which is equal to offset for byte arrays)
			template<class TypeToCreate, class... Args>
			TypeToCreate                                        *emplace_overwrite_at            (size_t InOffset, Args&&... args);

		private:
		};
		using MemView_FixedSizeByteArray                         = MemView_FixedSizeByteArrayTempl<uint8_t>;
		using MemView_ConstFixedSizeByteArray                    = MemView_FixedSizeByteArrayTempl<const uint8_t>;

		/*******************************************************************************************************
		* Expandable byte array templ
		*******************************************************************************************************/
		template<class T>
		class MemView_ExpandableByteArrayTempl : public MemView_ExpandableArray<T>
		{
		public:
			// trivial destructor:
			~MemView_ExpandableByteArrayTempl                                                       () = default;

			// default ctor: leaves object in invalid state
			MemView_ExpandableByteArrayTempl                                                        () = default;

			// ctor: initializes view to the specific memory region
			constexpr MemView_ExpandableByteArrayTempl                                              (size_t InMaxSize, void_pointer InPtr, size_t InNumElems = 0);

			// Move operations should NEVER be deleted, 
			// because otherwise it will not BE able to use copying/moving with rvalues.
			MemView_ExpandableByteArrayTempl                                                        (MemView_ExpandableByteArrayTempl<T>&& InOther) = default;
			MemView_ExpandableByteArrayTempl<T>&                  operator=                         (MemView_ExpandableByteArrayTempl<T>&& InOther) = default;

			// Copy state as-is
			MemView_ExpandableByteArrayTempl                                                        (const MemView_ExpandableByteArrayTempl<T>& InOther) = default;
			MemView_ExpandableByteArrayTempl<T>&                  operator=                         (const MemView_ExpandableByteArrayTempl<T>& InOther) = default;

			// Copy constructors and operators for const views: 
			// (see array base for comments)
			template<class SourceT>
			MemView_ExpandableByteArrayTempl                                                        (const MemView_ExpandableByteArrayTempl<SourceT>& InOther);

			template<class SourceT>
			MemView_ExpandableByteArrayTempl<T>&                  operator=                         (const MemView_ExpandableByteArrayTempl<SourceT>& InOther);

			// Creating objects by the given offset
			// constructing objects of the specified type, using the given Index (which is equal to offset for byte arrays)
			template<class TypeToCreate, class... Args>
			TypeToCreate                                         *emplace_overwrite_at              (size_t InOffset, Args&&... args);

			// Construct and add new object at the end of the array: see ExpandableArrayBase class doc
			template<class TypeToCreate, class... Args>
			TypeToCreate                                         *emplace_back                      (Args&&... args);

		private:
		};
		using MemView_ExpandableByteArray                         = MemView_ExpandableByteArrayTempl<uint8_t>;
		using MemView_ConstExpandableByteArray                    = MemView_ExpandableByteArrayTempl<const uint8_t>;

		template<class T, class V, template<class> class ByteArrayCont>
		V *expand_with_bytes(ByteArrayCont<T>& InArray, const V& InVal);

		template<class T, class V, template<class> class ByteArrayCont>
		V *expand_with_bytes(ByteArrayCont<T>& InArray, const V& InVal)
		{
			auto pTopPointer = static_cast<void *>(get_end_pointer(InArray));
			expand_size_delta(InArray, sizeof(InVal));
			memcpy(pTopPointer, &InVal, sizeof(InVal));
			return static_cast<V *>(pTopPointer);
		}

		/**************************************************************************************************
		* Allocator on ExpandableByteArray for type T;
		*
		* Always allocates, NEVER DEALLOCATES.
		**************************************************************************************************/
		template<class T>
		class ExpandableByteArrayAllocator
		{
		public:
			using value_type                                       = typename T;

			ExpandableByteArrayAllocator                           (MemView_ExpandableByteArrayTempl<T> *pInByteArray);

			ExpandableByteArrayAllocator                           () noexcept                                            = default;
			ExpandableByteArrayAllocator                           (const ExpandableByteArrayAllocator& InOther)          = default;
			ExpandableByteArrayAllocator<T>&       operator=       (const ExpandableByteArrayAllocator& InOther)          = default;
 
			T                                     *allocate        (size_t InNumElems);
			void                                   deallocate      (T *p, std::size_t num);

		private:
			MemView_ExpandableByteArrayTempl<T>                     *_pByteArray               = nullptr;
		};

		template<class T>
		ExpandableByteArrayAllocator<T>::ExpandableByteArrayAllocator(MemView_ExpandableByteArrayTempl<T> *pInByteArray) :
			_pByteArray(pInByteArray)
		{
			assert(pInByteArray);
		}

		template<class T>
		T *ExpandableByteArrayAllocator<T>::allocate(size_t InNumElems)
		{
			assert(_pByteArray);
			return expand_size_delta(*_pByteArray, sizeof(T) * InNumElems);
		}

		template<class T>
		void ExpandableByteArrayAllocator<T>::deallocate(T *p, std::size_t num)
		{
			// Nothing is to be done here
		}

		/**************************************************************************************************
		* MemView_FixedSizeByteArrayTempl implementation
		**************************************************************************************************/
		template<class T>
		constexpr MemView_FixedSizeByteArrayTempl<T>::MemView_FixedSizeByteArrayTempl(void_pointer InPtr, size_t InNumElems) :
			MemView_FixedSizeArray(InPtr, InNumElems)
		{
		}

		template<class T>
		template<class SourceT>
		MemView_FixedSizeByteArrayTempl<T>::MemView_FixedSizeByteArrayTempl(const MemView_FixedSizeByteArrayTempl<SourceT>& InOther) :
			MemView_FixedSizeArray(InOther)
		{
		}

		template<class T>
		template<class SourceT>
		MemView_FixedSizeByteArrayTempl<T>& MemView_FixedSizeByteArrayTempl<T>::operator=(const MemView_FixedSizeByteArrayTempl<SourceT>& InOther)
		{
			return MemView_FixedSizeArray<T>::operator=(InOther);
		}

		template<class T>
		template<class TypeToCreate, class... Args>
		TypeToCreate *MemView_FixedSizeByteArrayTempl<T>::emplace_overwrite_at(size_t InOffset, Args&&... args)
		{
			return _emplace_overwrite_at<TypeToCreate>(InOffset, std::forward<Args>(args)...);
		}

		/**************************************************************************************************
		* MemView_ExpandableByteArrayTempl implementation
		**************************************************************************************************/
		template<class T>
		constexpr MemView_ExpandableByteArrayTempl<T>::MemView_ExpandableByteArrayTempl(size_t InMaxSize, void_pointer InPtr, size_t InNumElems) :
			MemView_ExpandableArray(InMaxSize, InPtr, InNumElems)
		{
		}

		template<class T>
		template<class SourceT>
		MemView_ExpandableByteArrayTempl<T>::MemView_ExpandableByteArrayTempl(const MemView_ExpandableByteArrayTempl<SourceT>& InOther) :
			MemView_ExpandableArray(InOther)
		{
		}

		template<class T>
		template<class SourceT>
		MemView_ExpandableByteArrayTempl<T>& MemView_ExpandableByteArrayTempl<T>::operator=(const MemView_ExpandableByteArrayTempl<SourceT>& InOther)
		{
			return MemView_ExpandableArray<T>::operator=(InOther);
		}

		template<class T>
		template<class TypeToCreate, class... Args>
		TypeToCreate *MemView_ExpandableByteArrayTempl<T>::emplace_overwrite_at(size_t InOffset, Args&&... args)
		{
			return _emplace_overwrite_at<TypeToCreate>(InOffset, std::forward<Args>(args)...);
		}

		template<class T>
		template<class TypeToCreate, class... Args>
		TypeToCreate *MemView_ExpandableByteArrayTempl<T>::emplace_back(Args&&... args)
		{
			return _emplace_back<TypeToCreate>(std::forward<Args>(args)...);
		}
	} // Util
} // Dv