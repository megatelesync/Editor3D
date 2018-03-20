#ifndef _Util_Range_h_
#define _Util_Range_h_

#include <list>
#include <vector>
#include <memory>

namespace Dv
{
	namespace Util
	{
		template<class IteratorTypeArg, class ConstIteratorTypeArg>
		class Range
		{
		public:
			Range						();
			Range						(const IteratorTypeArg& firstIn, const IteratorTypeArg& lastIn);			

			typedef						Range<IteratorTypeArg,ConstIteratorTypeArg>				ThisType;
			typedef						IteratorTypeArg											IteratorType;
			typedef						ConstIteratorTypeArg									ConstIteratorType;

			typedef						typename IteratorType::reference						reference;
			typedef						typename IteratorType::value_type						value_type;
			typedef						typename IteratorType::pointer							pointer;
			typedef						typename IteratorType::difference_type					difference_type;
			
			IteratorTypeArg				begin			()								{ return _first; }
			IteratorTypeArg				end				()								{ return _last; }
			ConstIteratorTypeArg		cbegin			()								{ return _first; }
			ConstIteratorTypeArg		cend			()								{ return _last; }

		private:
			IteratorTypeArg				_first;
			IteratorTypeArg				_last;
		};

		template<class ContType> 
		Range<typename ContType::iterator, typename ContType::const_iterator> MakeRange(const ContType& cont);

		template<class ContType>
		Range<typename ContType::const_iterator, typename ContType::const_iterator> MakeConstRange(const ContType& cont);

		// Range function impl
		template<class ContType> 
		Range<typename ContType::iterator, typename ContType::const_iterator> MakeRange(const ContType& cont)
		{
			return Range<typename ContType::iterator, typename ContType::const_iterator>(cont.begin(), cont.end());
		}

		template<class ContType> 
		Range<typename ContType::const_iterator, typename ContType::const_iterator> MakeConstRange(const ContType& cont)
		{
			return Range<typename ContType::const_iterator, typename ContType::const_iterator>(cont.cbegin(), cont.cend());
		}

		// Range impl
		template<class IteratorTypeArg, class ConstIteratorTypeArg>
		Range<IteratorTypeArg, ConstIteratorTypeArg>::Range()
		{
		}

		template<class IteratorTypeArg, class ConstIteratorTypeArg>
		Range<IteratorTypeArg, ConstIteratorTypeArg>::Range(const IteratorTypeArg& firstIn, const IteratorTypeArg& lastIn) :
			_first(firstIn), _last(lastIn)
		{
		}
	} // Util
} // Dv

#endif // _Util_Range_h_