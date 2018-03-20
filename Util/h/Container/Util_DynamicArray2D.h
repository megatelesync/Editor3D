#ifndef _Util_DynamicArray2D_h_
#define _Util_DynamicArray2D_h_

#include <cassert>
#include <memory>

#include "../CoreUtil/CoreUtil_Ptr.h"

namespace Dv
{
	namespace Util
	{
		namespace Container
		{
			template<class T>
			class DynamicArray2D
			{
			public:
				typedef T						value_type;
				typedef DynamicArray2D<T>		ThisType;

				DynamicArray2D							();
				DynamicArray2D							(int InWidth, int InHeight);				

				DynamicArray2D							(const ThisType& InOther);
				ThisType&				operator=		(const ThisType& InOther);

				size_t					size			()											const { return _width * _height; }

				// reset array size without preserving the old content
				void					Reset			(int InWidth, int InHeight);
				// resize the array with preserving the old content
				void					Resize			(int InWidth, int InHeight);

				// get size
				int						GetWidth		()											const;
				int						GetHeight		()											const;

				// Get element at given position
				const T&				At				(int InRowIndex, int inColumnIndex)			const;				
				T&						At				(int InRowIndex, int inColumnIndex);	

				// pointer to raw array data
				const T				  *data				()											const;				
				T					  *data				();				

			private:
				std::unique_ptr<T[]>			_pArr;
				int								_width;
				int								_height;

				// Get linear array element index corresponding given 2d array index pair
				int						_ElemIndex			(int InRowIndex, int InColumnIndex) const;					
				T					   *_ElemPointer		(int InRowIndex, int InColumnIndex) const;
				// Alloc internal array of a given size without preserving the old content
				void					_Alloc				(int InWidth, int InHeight);
			};





			// DynamicArray2D ctors IMPL
			template<class T>
			DynamicArray2D<T>::DynamicArray2D() : 
				_pArr(nullptr),
				_width(0),
				_height(0)
			{
			}

			template<class T>
			DynamicArray2D<T>::DynamicArray2D(int InWidth, int InHeight)				
			{
				_Alloc(InWidth, InHeight)
			}

			template<class T>
			DynamicArray2D<T>::DynamicArray2D(const ThisType& InOther) 
			{
				_Alloc(InOther.GetWidth(), InOther.GetHeight());		
				std::memcpy(_pArr.get(), InOther.data(), InOther.size());
			}

			template<class T>
			typename DynamicArray2D<T>::ThisType& DynamicArray2D<T>::operator=(const ThisType& InOther)
			{
				_Alloc(InOther.GetWidth(), InOther.GetHeight());		
				std::memcpy(_pArr.get(), InOther.data(), InOther.size());
				return *this;
			}

			// DynamicArray2D IMPL
			template<class T>
			void DynamicArray2D<T>::Reset(int InWidth, int InHeight)
			{
				_Alloc(InWidth, InHeight);
			}

			template<class T>
			void DynamicArray2D<T>::Resize(int InWidth, int InHeight)
			{
				int prev_width = _width;
				int prev_height = _height;
				std::unique_ptr<T[]> saved_arr(_pArr);
				_Alloc(InWidth, InHeight);
				CopyClampedArray2D(saved_arr.get(),_pArr.get(),prev_width,prev_height,InWidth,InHeight);
			}

			// DynamicArray2D getters impl
			template<class T>
			const T& DynamicArray2D<T>::At(int InRowIndex, int inColumnIndex) const
			{
				return &(*_ElemPointer(InRowIndex, inColumnIndex));
			}

			template<class T>
			T& DynamicArray2D<T>::At(int InRowIndex, int inColumnIndex)
			{
				return *_ElemPointer(InRowIndex, inColumnIndex);
			}

			template<class T>
			int DynamicArray2D<T>::GetWidth() const
			{
				return _width;
			}

			template<class T>
			int	DynamicArray2D<T>::GetHeight() const
			{
				return _height;
			}

			template<class T>
			const T *DynamicArray2D<T>::data() const
			{
				return _pArr.get();
			}

			template<class T>
			T *DynamicArray2D<T>::data()
			{
				return _pArr.get();
			}

			// DynamicArray2D private functions impl
			template<class T>
			int DynamicArray2D<T>::_ElemIndex(int InRowIndex, int InColumnIndex) const
			{
				return InRowIndex * _width + InColumnIndex;
			}

			template<class T>
			T *DynamicArray2D<T>::_ElemPointer(int InRowIndex, int InColumnIndex) const
			{
				assert(InRowIndex >= 0);
				assert(InColumnIndex >= 0);
				assert(InRowIndex < _height);
				assert(InColumnIndex < _width);
				return _pArr.get() + _ElemIndex(InRowIndex, InColumnIndex);
			}

			template<class T>
			void DynamicArray2D<T>::_Alloc(int InWidth, int InHeight)
			{				
				_width = InWidth;
				_height = InHeight;
				assert(InWidth > 0);
				assert(InHeight > 0);
				_pArr.reset(new T[_width * _height]);
			}
		} // Container
	} // Util
} // Dv

#endif // _Util_DynamicArray2D_h_