#pragma once

#include <Windows.h>


/*****************************************************************************************************************************
* TODO Fix bugs:
* 1. Hunk: only last alloc can be canceled
*****************************************************************************************************************************/

namespace Dv
{
	namespace Util
	{
		// Memory hunk
		//
		// Raw memory for POD types
		// (No calling destructors when deleting)
		//
		// Usage pattern1: Use as "dynamical automatic memory"
		class Hunk
		{
		public:
			// Use this lock class to lock the hunk
			friend class HunkLock;

			Hunk(void *pPtrIn, int numBytesIn) : 
				_pPtr(static_cast<BYTE*>(pPtrIn)), _pLastAlloc(static_cast<BYTE*>(pPtrIn)), _pCurrPtr(static_cast<BYTE*>(pPtrIn)),
				_numBytes(numBytesIn) {}
			~Hunk() {};

			// main allocation functions
			template<class T> 	T *Alloc(T val);
			template<class T>	T *AllocArray(T val, int numElems);
	
			UINT GetSizeInBytes() 			const	{ return _numBytes; }
			UINT GetOccupiedBytes()			const	{ return _pLastAlloc - _pPtr; }
			UINT GetFreeBytes()			const	{ _numBytes - GetOccupiedBytes(); }

			// Get allocated hunk of memory
			const void *GetMemory()			const	{ return _pPtr; }
			// Get pointer to the last allocation
			const void *GetLastAllocPtr()		const 	{ return _pLastAlloc; }

		private:
			// clear the last allocated on the hunk
			inline void				clearAlloc();
			inline void				allocate();

			int 					 _numBytes;
			BYTE 					*_pPtr;
			BYTE					*_pLastAlloc;
			BYTE					*_pCurrPtr;
		};

		class HunkLock
		{
		public:
			HunkLock(Hunk *pHunk) : _pHunk(pHunk) 	{ pHunk->allocate(); }
			virtual ~HunkLock()			{ _pHunk->clearAlloc(); }
			Hunk	*GetHunk() 			{ return _pHunk; }

		private:
			Hunk  *_pHunk;
		};




		/**************************************************************************************************************************************
		* IMPLEMENTATION
                 *************************************************************************************************************************************/
		template<class T> 	T *Hunk::Alloc(T val)
		{
			void *oldPtr = _pCurrPtr;
			_pCurrPtr += sizeof(val);
			return oldPtr;			
		}
		
		template<class T>	T *Hunk::AllocArray(T val, int numElems)
		{
			void *oldPtr = _pCurrPtr;
			_pCurrPtr += numElems * sizeof(val);
			return oldPtr;
		}

		inline void Hunk::clearAlloc()
		{
			_pCurrPtr = _pLastAlloc;
		}

		inline void Hunk::allocate()
		{
			_pLastAlloc = _pCurrPtr;
		}
	} // Util
} // Dv