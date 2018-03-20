#pragma once

#include <memory>
#include "../CoreUtil/coreutil_assert.h"

namespace Dv
{
	namespace Util
	{
		// Enumerator const-ness means that it's enumerator to const
		// Return as std::unique_ptr<IEnumerator<T>>
		//
		// WARNING!!! Unlike its C# counterpart, this enumerator interface does NOT impose that you call MoveNext()
		// before accessing the first element.
		//
		// The enumerator returns POINTER to element T
		// 
		template<class T>
		class IEnumerator
		{
		public:
			/*******************************************************************************************************
			* SPECIAL FUNCTIONS
			*******************************************************************************************************/
			IEnumerator() = default;

			virtual                          ~IEnumerator() = default;

			// Cannot copy: it's virtual class!
			IEnumerator<T>(const IEnumerator<T>&) = delete;
			IEnumerator<T>&                   operator=             (const IEnumerator<T>&) = delete;

			// Cannot move: it's virtual class!
			IEnumerator<T>(const IEnumerator<T>&&) = delete;
			IEnumerator<T>&                   operator=             (const IEnumerator<T>&&) = delete;

			/*******************************************************************************************************
			* GETTING SIZE
			*******************************************************************************************************/
			virtual size_t                    GetSize() const = 0;

			/*******************************************************************************************************
			* MOVING
			*******************************************************************************************************/
			// false if end
			virtual bool                      MoveNext() = 0;

			/*******************************************************************************************************
			* GETTING CURRENT ELEMENT
			*******************************************************************************************************/
			virtual T                        *GetCurrent() = 0;
			virtual const T                  *GetCurrent() const = 0;
		};

		// Try to move to the first value equal to the given;
		// Return false if the given value is not visited while enumerating.
		template<class T>
		bool TryMoveTo(IEnumerator<T> *pEnumerator, const T *pInValue);

		// Get element by index (or return nullpr if index is greater than index of the max element)
		template<class T>
		T *GetByIndex(IEnumerator<T> *pEnumerator, size_t InIndex);

		template<class T>
		T *GetNullOrFirst(IEnumerator<T> *pEnumerator);

		/**************************************************************************************************
		* IEnumerator implementation that returns zero elements
		***************************************************************************************************/
		template<class T>
		class EmptyEnumerator : public IEnumerator<T>
		{
		public:
			EmptyEnumerator() = default;

			virtual size_t                    GetSize() const override { return 0; }
			virtual bool                      MoveNext() override { return false; }
			virtual T                        *GetCurrent() override { return Util::InvalidCall<T *>("GetCurrent() for empty enumerator should not be called."); }
			virtual const T                  *GetCurrent() const override { return Util::InvalidCall<T *>("GetCurrent() for empty enumerator should not be called."); }
		};
		template<class T>
		std::unique_ptr<IEnumerator<T>> MakeEnumeratorResult_Empty();

		/**************************************************************************************************
		* Enumerator of container of ptrs
		*
		* This enumerator enumerates all elements of the given container of type ContType,
		* that contains elements of type ptr to T.
		**************************************************************************************************/
		template<class ContType, class T>
		class PtrContEnumerator : public IEnumerator<T>
		{
		public:
			explicit                       PtrContEnumerator(ContType *pInCont);

			virtual size_t                 GetSize() const                      override { return _pCont->size(); }

			virtual bool                   MoveNext()                           override;
			virtual T                     *GetCurrent()                         override { return _current != _pCont->end() ? *_current : nullptr; }
			virtual const T               *GetCurrent() const                   override { return _current != _pCont->end() ? *_current : nullptr; }

		private:
			ContType                                      *_pCont;
			typename ContType::const_iterator              _current;
			typename ContType::const_iterator              _next;
		};

		/**************************************************************************************************
		* Enumerator of container of unique ptrs
		*
		* This enumerator enumerates all elements of the given container of type ContType,
		* that contains elements of type std::unique_ptr<T>.
		**************************************************************************************************/
		template<class ContType, class T>
		class UniquePtrContEnumerator : public Util::IEnumerator<T>
		{
		public:
			explicit                       UniquePtrContEnumerator(ContType *pInCont);

			virtual size_t                 GetSize() const                   override { return _pCont->size(); }

			// false if end
			virtual bool                   MoveNext()                           override;
			virtual T                     *GetCurrent()                         override { return _current != _pCont->end() ? _current->get() : nullptr; }
			virtual const T               *GetCurrent() const                   override { return _current != _pCont->end() ? _current->get() : nullptr; }

		private:
			ContType                                      *_pCont;
			typename ContType::const_iterator              _current;
			typename ContType::const_iterator              _next;
		};

		template<class ContType, class T>
		class MapContValueEnumerator : public Util::IEnumerator<T>
		{
		public:
			explicit                       MapContValueEnumerator(ContType *pInCont);

			virtual size_t                 GetSize() const                      override { return _pCont->size(); }

			// false if end
			virtual bool                   MoveNext()                           override;
			virtual T                     *GetCurrent()                         override { return _current != _pCont->end() ? _current->second : nullptr; }
			virtual const T               *GetCurrent() const                   override { return _current != _pCont->end() ? _current->second : nullptr; }

		private:
			ContType                                      *_pCont;
			typename ContType::const_iterator              _current;
			typename ContType::const_iterator              _next;
		};

		template<class ContType, class T>
		MapContValueEnumerator<ContType, T>::MapContValueEnumerator(ContType *pInCont) :
			_pCont(pInCont),
			_current(pInCont->begin())
		{
			if (_current != pInCont->end())
			{
				_next = std::next(_current);
			}
			else
			{
				_next = _current;
			}
		}

		template<class ContType, class T>
		bool MapContValueEnumerator<ContType, T>::MoveNext()
		{
			_current = _next;
			if (_current != _pCont->end())
			{
				_next = std::next(_current);
				return true;
			}
			else
			{
				return false;
			}
		}

		/************************************************************************************************
		* UniquePtrContEnumerator implementation
		*************************************************************************************************/
		template<class ContType, class T>
		UniquePtrContEnumerator<ContType, T>::UniquePtrContEnumerator(ContType *pInCont) :
			_pCont(pInCont),
			_current(pInCont->begin())
		{
			if(_current != pInCont->end())
			{
				_next = std::next(_current);
			}
			else
			{
				_next = _current;
			}
		}

		template<class ContType, class T>
		bool UniquePtrContEnumerator<ContType, T>::MoveNext()
		{
			_current = _next;
			if (_current != _pCont->end())
			{
				_next = std::next(_current);
				return true;
			}
			else
			{
				return false;
			}
		}

		/************************************************************************************************
		* PtrContEnumerator implementation
		*************************************************************************************************/
		template<class ContType, class T>
		PtrContEnumerator<ContType,T>::PtrContEnumerator(ContType *pInCont) :
			_pCont(pInCont),
			_current(pInCont->begin())
		{
			if (_current != pInCont->end())
			{
				_next = std::next(_current);
			}
			else
			{
				_next = _current;
			}
		}

		template<class ContType, class T>
		bool PtrContEnumerator<ContType, T>::MoveNext()
		{
			_current = _next;
			if (_current != _pCont->end())
			{
				_next = std::next(_current);
				return true;
			}
			else
			{
				return false;
			}
		}

		/**************************************************************************************************
		* EmptyEnumerator implementation
		***************************************************************************************************/
		template<class T>
		std::unique_ptr<IEnumerator<T>> MakeEnumeratorResult_Empty()
		{
			return std::unique_ptr<IEnumerator<T>>(new EmptyEnumerator<T>());
		}

		/*****************************************************************************************************************
		* IEnumerator implementation
		*****************************************************************************************************************/
		template<class T>
		bool TryMoveTo(IEnumerator<T> *pEnumerator, const T *pInValue)
		{
			while (true)
			{
				auto pCurr = pEnumerator->GetCurrent();
				if (nullptr == pCurr)
				{
					break;
				}
				if (pInValue == pCurr)
				{
					return true;
				}
				pEnumerator->MoveNext();
			}
			return false;
		}

		// Get element by index (or return nullpr if index is greater than index of the max element)
		template<class T>
		T *GetByIndex(IEnumerator<T> *pEnumerator, size_t InIndex)
		{
			auto index = 0;
			while (true)
			{
				auto pCurr = pEnumerator->GetCurrent();
				if (nullptr == pCurr)
				{
					break;
				}
				if (index == InIndex)
				{
					return pCurr;
				}
				index++;
				pEnumerator->MoveNext();
			}
			return nullptr;
		}

		template<class T>
		T *GetNullOrFirst(IEnumerator<T> *pEnumerator)
		{
			return GetByIndex(pEnumerator, 0);
		}
	} // Util
} // Dv