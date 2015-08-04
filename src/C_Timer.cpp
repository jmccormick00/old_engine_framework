
#include "C_Timer.h"
#include <windows.h>

C_Timer::C_Timer()
: d_iBaseTime(0), d_iPauseTimeTotal(0), d_iPauseTime(0), d_dDeltaSeconds(-1.0),
  d_iPrevTime(0), d_iCurrTime(0), d_bPaused(false)
{
	_int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*) &countsPerSec);
	d_dSecondsPerCount = 1.0 / (double)countsPerSec;
}

void C_Timer::tick() {
	// If paused, set delta to zero
	if(d_bPaused) {
		d_dDeltaSeconds = 0.0;
		return;
	}

	// Get the time for this frame
	QueryPerformanceCounter((LARGE_INTEGER*) &d_iCurrTime);

	// Calculate delta
	//
	//				|<--d_dDeltaSeconds--->|
	// -------------*----------------------*--------------> Time
	//			d_iPrevTime			      Tick()
	d_dDeltaSeconds = (d_iCurrTime - d_iPrevTime) * d_dSecondsPerCount;

	d_iPrevTime = d_iCurrTime;

	// Force non negative numbers.  If the processor goes into power save
	// the time could be negative.
	if(d_dDeltaSeconds < 0.0)
		d_dDeltaSeconds = 0.0;
}


void C_Timer::reset() {
	QueryPerformanceCounter((LARGE_INTEGER*) &d_iCurrTime);

	d_iBaseTime = d_iCurrTime;
	d_iPrevTime = d_iCurrTime;
	d_iPauseTime = 0;
	d_bPaused = false;
}


void C_Timer::pause() {
	// If its already stopped, don't do anything
	if(!d_bPaused) {
		QueryPerformanceCounter((LARGE_INTEGER*) &d_iCurrTime);

		// Save the time we stopped at
		d_iPauseTime = d_iCurrTime;
		d_bPaused = true;
	}
}


void C_Timer::start() {
	if(d_bPaused) {
		// If we are resuming from a stopped state
		// accumalate the paused time
		//
		//				|<--d_iPauseTime--->|
		// -------------*-------------------*--------------> Time
		//			d_iStopTime			 start()
		QueryPerformanceCounter((LARGE_INTEGER*) &d_iCurrTime);
		d_iPauseTimeTotal += (d_iCurrTime - d_iPauseTime);

		// Reset the previous time
		d_iPrevTime = d_iCurrTime;
		d_bPaused = false;
		d_iPauseTime = 0;
	}

}


double C_Timer::getTime() const {
	// If we are paused, dont count the time since we have stopped
	//	
	// -----*----------------*--------------*----------> Time
	//	d_iBaseTime		d_iStopTime	    d_iCurrTime
	if(d_bPaused)
		return ((d_iPauseTime - d_iBaseTime) * d_dSecondsPerCount);

	// The (d_iCurrTime - d_iBaseTime) includes any paused time
	// So need to subract that out
	//						 |<--d_iPauseTime--->|
	// -----*----------------*-------------------*----------------*----> Time
	//	d_iBaseTime		d_iStopTime	           start()        d_iCurrTime
	else {
		QueryPerformanceCounter((LARGE_INTEGER*) &d_iCurrTime);
		return (((d_iCurrTime-d_iPauseTime)-d_iBaseTime) * d_dSecondsPerCount);
	}
}
