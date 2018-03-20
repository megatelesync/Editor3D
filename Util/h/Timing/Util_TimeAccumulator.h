#ifndef _dx11TimeAccumulator_h_
#define _dx11TimeAccumulator_h_

#include <Windows.h>

namespace Dv
{
	namespace dx11
	{
		struct TimeState
		{
			double				current_second;
			LARGE_INTEGER		lastTickEnd;
			LARGE_INTEGER		tickStart;
			LARGE_INTEGER		tickEnd;

			inline void UpdateTimeState();
			// get delta seconds passed from the end of the first tick and to the start of another
			double GetDeltaSeconds() const;

			TimeState() : lastTickEnd(), tickStart(), tickEnd(), current_second(0.0) {}
		};
		
		void FixTickStart	(TimeState *pState);
		void FixTickEnd		(TimeState *pState);





		// IMPLEMENTATION
		void inline TimeState::UpdateTimeState()
		{
			current_second += GetDeltaSeconds();
		}		
	} // dx11
} // Dv

#endif // _dx11TimeAccumulator_h_