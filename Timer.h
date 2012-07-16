//A class which can calculate time elapsed

#pragma once

#ifndef _TIMER_H_
#define _TIMER_H_

#include <Windows.h> //For QueryPerformance functions

namespace ee
{
	class Timer
	{
	public:
		Timer();

		//Calculate time elapsed since reset was called
		float GetTotalTime() const; //seconds
		//Get the time between frames
		float GetDeltaTime() const; //seconds

		//Whether the timer is stopped
		bool IsStopped() const;

		//Reset the timer, usually call before message loop
		void Reset();
		//Call when game is unpaused
		void Start();
		//Call when stopped
		void Stop();
		//Call every frame
		void Tick();

	private:
		double m_secondsPerCount;
		double m_deltaTime;

		__int64 m_baseTime; //The time the timer was last reset to
		__int64 m_pausedTime; //Keeps track of time elapsed during pause
		__int64 m_stopTime; //The time when the timer is stopped
		__int64 m_prevTime; //The time recorded at last tick
		__int64 m_currTime; //The timer recorded at current tick

		bool m_isStopped; //Whether the time has been stopped
	};
}

#endif