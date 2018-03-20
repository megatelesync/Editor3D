#include "../../h/CoreUtil/CoreUtil_Bytes.h"
#include "../../h/CoreUtil/CoreUtil_Ptr.h"

namespace Dv
{
	namespace Util
	{
		void SetByOffset(void *pInDest, size_t InOffset, const void *pInSrc, size_t InSize)
		{
			auto pPtr = Util::AddPtr(pInDest, InOffset);
			std::memcpy(pPtr, pInSrc, InSize);
		}
	}
}