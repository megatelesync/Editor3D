#pragma once

#include <cassert>

namespace Dv
{
	namespace Util
	{
		// Convert a given ptr (of an arbitrary type) to a ptr of byte type
		template<class T>	
		char							*ToCharPtr		(T *ptr);
		template<class T> 
		const char						*ToCharPtr		(const T *ptr);
		// Convert a given ptr of byte type to another ptr
		template<class T>
		T								*FromCharPtr(char *ptr);

		template<class T>
		const T							*FromCharPtr(const char *ptr);

		// Copy one strided array to another strided array
		template<class DestTypeArg, class SrcTypeArg>
		void CopyStrided
		(
			DestTypeArg				*pDest,
			const SrcTypeArg		*pSrc,
			size_t					 InNumElems,
			size_t					 InDestStride,
			size_t					 InSrcStride
		);

		//
		// 1. Function:
		//		Copy strided array with zeroing extra destination elements
		// 2. Description:
		//		Copy elements from the src strided array of the given size to 
		// the destination strided array of the given size.
		// If size of the destination array is greater
		// then the size of the src array, fill the extra elements with zeroes.
		// 3. Restrictions:
		//		The length of the destination array must be greater or greater
		// to the length of the src array
		template<class DestTypeArg, class SrcTypeArg>
		void CopyStrided_Zeroed
		(
			DestTypeArg				*pDest,
			const SrcTypeArg		*pSrc,
			size_t					 InNumSrcElems,
			size_t					 InNumDestElems,
			size_t					 InDestStride,
			size_t					 InSrcStride
		);

		// Mk all components of the given strided array components to zero
		template<class DestTypeArg>
		void ZeroStrided
		(
			DestTypeArg				*pDest,
			size_t					 InNumComponents,
			size_t					 InDestStride
		);	

		// Set the given component of the given strided array to the given value
		// and return pointer to that component
		template<class DestTypeArg, class SrcTypeArg>
		DestTypeArg *SetStridedComponent
		(
			DestTypeArg						*pDest,
			size_t							 InComponentIndex,
			const SrcTypeArg&				 InValue,
			size_t							 InComponentStride = sizeof(DestTypeArg)
		);

		// Get pointer to the component with the given index
		template<class DestTypeArg>
		DestTypeArg *GetStridedComponentPtr
		(
			DestTypeArg						*pDest,
			size_t							 InComponentIndex,
			size_t							 InComponentStride = sizeof(DestTypeArg)
		);

		template<class DestTypeArg>
		const DestTypeArg *GetStridedComponentPtr
		(
			const DestTypeArg				*pDest,
			size_t							 InComponentIndex,
			size_t							 InComponentStride
		);

		// Add a number of bytes to a given ptr
		template<class T> T *AddPtr(T *ptr, int numBytes);
		template<> void *AddPtr(void *ptr, int numBytes);

		// 2D array utils
		inline int Array2DElemIndex(int InArrayWidth, int InRowIndex, int InColumnIndex);		

		template<class T>
		T *Array2DElemPointer(T *pArr, int InArrayWidth, int InRowIndex, int InColumnIndex);				

		// Copy 2D array from one location to another
		// (clamp array if necessary)
		template<class T>
		void CopyClampedArray2D
		(
			const T *pSrc, T *pDest, 
			int InSrcWidth, int InSrcHeight,
			int InDestWidth, int InDestHeight
		);





		// IMPL
		template<class T> char *ToCharPtr(T *ptr)
		{
			void *pVoid = static_cast<void *>(ptr);
			return static_cast<char *>(pVoid);
		}
		template<class T> const char *ToCharPtr(const T *ptr)
		{
			const void *pVoid = static_cast<const void *>(ptr);
			return static_cast<const char *>(pVoid);
		}
		template<class T>
		T *FromCharPtr(char *ptr)
		{
			void *pVoid = ptr;
			return static_cast<T *>(pVoid);
		}
		template<class T>
		const T *FromCharPtr(const char *ptr)
		{
			const void *pVoid = ptr;
			return static_cast<const T *>(pVoid);
		}

		template<class T> T *AddPtr(T *ptr, int numBytes)
		{
			auto pChar = ToCharPtr(ptr);
			return FromCharPtr<T>(pChar+numBytes);
		}

		template<> inline void *AddPtr(void *ptr, int numBytes)
		{
			return AddPtr(static_cast<char *>(ptr), numBytes);
		}

		template<class DestTypeArg, class SrcTypeArg>
		void CopyStrided
		(
			DestTypeArg				*pDest,
			const SrcTypeArg				*pSrc,
			size_t					 InNumElems,
			size_t					 InDestStride,
			size_t					 InSrcStride
		)
		{
			assert(pDest != nullptr);
			assert(pSrc != nullptr);
			assert(InNumElems > 0);
			for(int elem_index = 0; elem_index < InNumElems; elem_index++)
			{
				DestTypeArg *pDestElem = Util::AddPtr(pDest, InDestStride * elem_index);
				const SrcTypeArg *pSrcElem = Util::AddPtr(pSrc, InSrcStride * elem_index);
				*pDestElem = static_cast<DestTypeArg>(*pSrcElem);
			}
		}

		template<class DestTypeArg, class SrcTypeArg>
		void CopyStrided_Zeroed
		(
			DestTypeArg				*pDest,
			const SrcTypeArg		*pSrc,
			size_t					 InNumSrcElems,
			size_t					 InNumDestElems,
			size_t					 InDestStride,
			size_t					 InSrcStride
		)
		{
			assert(pDest != nullptr);
			assert(pSrc != nullptr);
			assert(InNumDestElems > 0);
			assert(InNumSrcElems > 0);			
			DestTypeArg *pDestElem = nullptr;
			SrcTypeArg *pSrcElem = nullptr;
			for(int elem_index = 0; elem_index < InNumSrcElems; elem_index++)
			{
				pDestElem = Util::AddPtr(pDest, InDestStride * elem_index);
				pSrcElem = Util::AddPtr(pSrc, InSrcStride * elem_index);
				*pDestElem = static_cast<DestTypeArg>(*pSrcElem);
			}
			pDestElem = Util::AddPtr(pDest, InDestStride * elem_index);			
			ZeroStrided(pDestElem, InNumDestElems - InNumSrcElems, InDestStride);
		}

		template<class DestTypeArg>
		void ZeroStrided
		(
			DestTypeArg				*pDest,
			size_t					 InNumComponents,
			size_t					 InDestStride
		)
		{
			assert(pDest != nullptr);
			for(int i = 0; i < InNumComponents; i++)
			{
				DestTypeArg *pDestComponent = Util::AddPtr(pDest, InDestStride * i);
				*pDestComponent = static_cast<DestTypeArg>(0);
			}
		}

		template<class DestTypeArg, class SrcTypeArg>
		DestTypeArg *SetStridedComponent
		(
			DestTypeArg						*pDest,
			size_t							 InComponentIndex,
			const SrcTypeArg&				 InValue,
			size_t							 InComponentStride
		)
		{
			assert(pDest != nullptr);
			assert(InComponentIndex >= 0);
			DestTypeArg *pComponent = GetStridedComponentPtr(pDest, InComponentIndex, InComponentStride);
			*pComponent = static_cast<DestTypeArg>(InValue);
			return pComponent;
		}

		template<class DestTypeArg>
		DestTypeArg *GetStridedComponentPtr
		(
			DestTypeArg						*pDest,
			size_t							 InComponentIndex,
			size_t							 InComponentStride
		)
		{
			return Util::AddPtr(pDest, InComponentIndex * InComponentStride);
		}

		template<class DestTypeArg>
		const DestTypeArg *GetStridedComponentPtr
		(
			const DestTypeArg				*pDest,
			size_t							 InComponentIndex,
			size_t							 InComponentStride
		)
		{
			return Util::AddPtr(pDest, InComponentIndex * InComponentStride);
		}

		template<class T>
		void CopyClampedArray2D
		(
			const T *pSrc, T *pDest, 
			int InSrcWidth, int InSrcHeight,
			int InDestWidth, int InDestHeight
		)
		{
			assert(pSrc != nullptr);
			assert(pDest != nullptr);
			assert(InSrcWidth >= 0);
			assert(InSrcHeight >= 0);
			assert(InDestWidth >= 0);
			assert(InDestHeight >= 0);
			const int end_width = InSrcWidth < InDestWidth ? InSrcWidth : InDestWidth;
			const int end_height = InSrcHeight < InDestHeight ? InSrcHeight : InDestHeight;
			for(int row_index = 0; row_index < end_height; row_index++)
			{
				for(int column_index = 0; column_index < end_width; column_index++)
				{
					typename T *pDestElem = Array2DElemPointer(pDest, InDestWidth, row_index, column_index); 
					typename const T *pSrcElem = Array2DElemPointer(pSrc, InSrcWidth, row_index, column_index); 
					*pDestElem = *pSrcElem;
				}
			}
		}

		inline int Array2DElemIndex(int InArrayWidth, int InRowIndex, int InColumnIndex)
		{
			assert(InColumnIndex < InArrayWidth);
			return InRowIndex * InArrayWidth + InColumnIndex;
		}

		template<class T>
		T *Array2DElemPointer(T *pArr, int InArrayWidth, int InRowIndex, int InColumnIndex)
		{
				assert(pArr != nullptr);
				assert(InRowIndex >= 0);
				assert(InColumnIndex >= 0);
				assert(InRowIndex < _height);
				assert(InColumnIndex < _width);
				return pArr[Array2DElemIndex(InArrayWidth, InRowIndex, InColumnIndex)];
		}
	} // Util
} // Dv