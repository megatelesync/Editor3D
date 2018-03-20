#pragma once

#include <vector>
#include <chrono>
#include <boost/format.hpp>
#include <boost/accumulators/accumulators.hpp>
#include "../Log/Util_ILoggingDevice.h"

namespace Dv
{
	namespace Util
	{
		using StatTimer = std::chrono::high_resolution_clock;

		struct StatTimePoint
		{
			StatTimePoint(const std::string& InName, const StatTimer::time_point& InTimePoint);

			std::string                              name;
			StatTimer::time_point                    time_point;
		};
		inline StatTimePoint::StatTimePoint(const std::string& InName, const StatTimer::time_point& InTimePoint) :
			name(InName),
			time_point(InTimePoint)
		{
		}

		class RegisteredTimePoint
		{
		public:
			RegisteredTimePoint(const StatTimePoint *pInPoint) :
				_timePoint(*pInPoint) {}

			const StatTimePoint        *GetTimePoint            () const          { return &_timePoint; }

		private:
			StatTimePoint               _timePoint;
		};

		class TimingStat
		{
		public:
			TimingStat(const std::string& InStatName);

			// This functions are to be called before sending stat data and after stat data (about all timepoint) is set
			void BeginStat() {}
			void EndStat() {}

			const RegisteredTimePoint              *SetTimepoint                           (const std::string& InName, const RegisteredTimePoint *pInNullOrTimePoint);

			const std::string&                      GetName                                () const { return _statName; }

			size_t                                  GetNumTimepoints                       () const { return _timePoints.size(); }
			const RegisteredTimePoint              *GetTimePoint                           (size_t InIndex) const { return &(_timePoints[InIndex]); }

		private:
			std::string                             _statName;
			std::vector<RegisteredTimePoint>        _timePoints;
		};
		void SET_TIME_POINT(TimingStat *pTimingStat, const std::string& InName, const RegisteredTimePoint **ppRegisteredTimePoint);


		struct TimingStatLoggingSettings
		{
		};

		StatTimer::duration GetTimePointDeltaTime(const RegisteredTimePoint *pStart, const RegisteredTimePoint *pEnd);
		StatTimer::duration GetTimePointDeltaTime(const TimingStat *pInTimingStat, size_t InStartTimePointIndex, size_t InEndTimePointIndex);

		// Output all timepoints
		void LogTimingStat(Util::ILoggingDevice *pInLog, const TimingStat& InStat, const TimingStatLoggingSettings *pInSettings);
		void Log_DeltaTime(Util::ILoggingDevice *pInLog, const std::string &InTitle, const RegisteredTimePoint *pStart, const RegisteredTimePoint *pEnd, const TimingStatLoggingSettings *pInSettings);

		inline void SET_TIME_POINT(TimingStat *pTimingStat, const std::string& InName, const RegisteredTimePoint **ppRegisteredTimePoint)
		{
			assert(ppRegisteredTimePoint);
			*ppRegisteredTimePoint = pTimingStat->SetTimepoint(InName, *ppRegisteredTimePoint);
		}

		inline TimingStat::TimingStat(const std::string& InStatName) :
			_statName(InStatName)
		{
		}
		inline const RegisteredTimePoint *TimingStat::SetTimepoint(const std::string& InName, const RegisteredTimePoint *pInNullOrTimePoint)
		{
			auto timePoint_now = StatTimer::now();
			if (pInNullOrTimePoint)
			{
				return pInNullOrTimePoint;
			}
			else
			{
				StatTimePoint timePoint { InName, timePoint_now };
				auto it_registered = _timePoints.emplace(_timePoints.end(), &timePoint);
				return &(*it_registered);
			}
		}

		inline void Log_DeltaTime(Util::ILoggingDevice *pInLog, const std::string &InTitle, const RegisteredTimePoint *pStart, const RegisteredTimePoint *pEnd, const TimingStatLoggingSettings *pInSettings)
		{
			LogRecordScoper scoper { pInLog, InTitle, Util::EMessageType::Info };
			*pInLog << "Start: " << pStart->GetTimePoint()->name << Util::endl;
			*pInLog << "End: " << pEnd->GetTimePoint()->name << Util::endl;
			auto delta = GetTimePointDeltaTime(pStart, pEnd);
			auto delta_seconds = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(delta).count());
			auto delta_microSeconds = static_cast<int>(std::chrono::duration_cast<std::chrono::microseconds>(delta).count());
			auto delta_nanoSeconds = static_cast<int>(std::chrono::duration_cast<std::chrono::nanoseconds>(delta).count());
			*pInLog << "Secs = " << delta_seconds << "; MicroSecs(E-6) = " << delta_microSeconds << "; NanoSecs(E-9) = " << delta_nanoSeconds << Util::endl;
		}
		inline void LogTimingStat(Util::ILoggingDevice *pInLog, const TimingStat& InStat, const TimingStatLoggingSettings *pInSettings)
		{
			std::string header = (boost::format("%1% timing") % InStat.GetName()).str();
			LogRecordScoper scoper { pInLog, header, Util::EMessageType::Info };

			const auto numTimePoints = static_cast<int>(InStat.GetNumTimepoints() - 1);
			*pInLog << "TIMEPOINT COUNT: " << numTimePoints << Util::endl;

			if (numTimePoints > 1)
			{
				for (size_t i = 0; i < numTimePoints; i++)
				{
					auto pTimePoint = InStat.GetTimePoint(i);
					auto pNextTimePoint = InStat.GetTimePoint(i + 1);
					Log_DeltaTime(pInLog, (boost::format("TIMEPOINT DELTA No %1%") % i).str(), pTimePoint, pNextTimePoint, pInSettings);
				}
				Log_DeltaTime(pInLog, "TOTAL TIMEPOINT DELTA", InStat.GetTimePoint(0), InStat.GetTimePoint(numTimePoints - 1), pInSettings);
			}
		}

		inline StatTimer::duration GetTimePointDeltaTime(const RegisteredTimePoint *pStart, const RegisteredTimePoint *pEnd)
		{
			return pEnd->GetTimePoint()->time_point - pStart->GetTimePoint()->time_point;
		}

		inline StatTimer::duration GetTimePointDeltaTime(const TimingStat *pInTimingStat, size_t InStartTimePointIndex, size_t InEndTimePointIndex)
		{
			auto pStart = pInTimingStat->GetTimePoint(InStartTimePointIndex);
			auto pEnd = pInTimingStat->GetTimePoint(InEndTimePointIndex);
			return GetTimePointDeltaTime(pStart, pEnd);
		}
	}
}