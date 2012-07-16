#include "Timer.h"

namespace ee
{
	Timer::Timer()
		: m_secondsPerCount(0.0), m_deltaTime(1.0), m_baseTime(0),
		  m_pausedTime(0), m_prevTime(0), m_currTime(0), m_isStopped(false)
	{
		__int64 countsPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER *)&countsPerSec);
		//Frequency = 1 / Time Period
		//Time Period = counts every second
		//Frequency = How many seconds per count
		m_secondsPerCount = 1.0 / (double) countsPerSec;
	}

	void Timer::Tick()
	{
		if (m_isStopped)
		{
			m_deltaTime = 0.0;
			return;
		}

		//Get the time this frame
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER *) &currTime);
		m_currTime = currTime;

		//Calculate the time difference between this frame and previous
		m_deltaTime = (m_currTime - m_prevTime) * m_secondsPerCount;
		//Set for next frame
		m_prevTime = m_currTime;

		//We need to make sure nonnegative. If the processor goes into power save mode,
		//or we get moved to another processor, delta time can become negative
		if (m_deltaTime < 0.0)
			m_deltaTime = 0.0;
	}

	void Timer::Reset()
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER *) &currTime);

		m_baseTime = currTime;
		m_prevTime = currTime; // No previous frame, so prev time is current time
		m_stopTime = 0;
		m_isStopped = false;
	}

	void Timer::Stop()
	{
		//Don't do anything if already stopped
		if (!m_isStopped)
		{
			__int64 currTime;
			QueryPerformanceCounter((LARGE_INTEGER *) &currTime);

			//Otherwise save time we are stopped at
			m_stopTime = currTime;
			m_isStopped = true;
		}
	}

	void Timer::Start()
	{
		__int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER *) &startTime);

		//Add the time elapsed between stop and start pairs
		
		//If timer is in a stopped state
		if (m_isStopped)
		{
			//Accumulate the paused time
			m_pausedTime += (startTime - m_stopTime);

			//Current prev time is invalid, set it back to current time
			m_prevTime = startTime;

			//We are no longer stopped
			m_stopTime = 0;
			m_isStopped = false;
		}
	}

	float Timer::GetDeltaTime() const
	{
		return (float) m_deltaTime;
	}

	float Timer::GetTotalTime() const
	{
		//Don't count time that has passed since stopped. In addition to this,
		//if we already had a pause, m_stopTime - m_baseTime includes paused time,
		//which we do not wish to count. We must subtract paused time from stop time

		if (m_isStopped)
		{
			return (float) (((m_stopTime - m_pausedTime) - m_baseTime) * m_secondsPerCount);
		}
		else
		{
			return (float) (((m_currTime - m_pausedTime) - m_baseTime) * m_secondsPerCount);
		}
	}

	bool Timer::IsStopped() const
	{
		return m_isStopped;
	}


}