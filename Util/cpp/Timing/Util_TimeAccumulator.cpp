#include "../../h/Timing/Util_TimeAccumulator.h"

namespace Dv
{
	namespace dx11
	{
		// TimeState
		void FixTickStart(TimeState *pState)
		{
			QueryPerformanceCounter(&(pState->tickStart));
		}

		void FixTickEnd(TimeState *pState)
		{
			pState->lastTickEnd = pState->tickEnd;
			QueryPerformanceCounter(&(pState->tickEnd));
		}

		double TimeState::GetDeltaSeconds() const
		{
			LARGE_INTEGER countsPerSec;
			QueryPerformanceFrequency(&countsPerSec);
			DWORD lowPartDelta = tickStart.LowPart - lastTickEnd.LowPart;
			return static_cast<double>(lowPartDelta)/countsPerSec.LowPart;
		}
	} // dx11
} // Dv