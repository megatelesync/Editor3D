#ifndef _ArrayPtr_h_
#define _ArrayPtr_h_

#include <cassert>

namespace Dv
{
	namespace Util
	{
		namespace Container
		{
			// Should be passed by value,
			// ptr copy semantics used
			template<class ElemType> class ArrayPtr
			{
			public:
				ArrayPtr() : _pPtr(nullptr) {}
				ArrayPtr(ElemType *pPtrIn, int sizeIn) : _pPtr(pPtrIn), _sz(sizeIn) {}
				int GetSize() const { return _sz; }
				const ElemType *cbegin() const { return _pPtr; }
				const ElemType *cend() const { return _pPtr + _sz; }
				ElemType *begin() { return _pPtr; }
				ElemType *end() { return _pPtr + _sz; }
				const ElemType& operator[] (int index) const
				{ 
					assert(index < _sz);
					return _pPtr[index]; 
				}
				ElemType& operator[] (int index)
				{ 
					assert(index < _sz);
					return _pPtr[index]; 
				}
			private:
				ElemType *_pPtr;
				int _sz;
			};
		} // Container
	} // Util
} // Dv

#endif // _ArrayPtr_h_