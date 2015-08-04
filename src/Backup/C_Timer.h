
#ifndef _CTIMER_
#define _CTIMER_

//#include <windows.h>

class C_Timer {
private:	
	__int64	d_iBaseTime,		// The base time of the timer.
			d_iPauseTimeTotal,	// The total time the timer was paused
			d_iPauseTime,		// The time marker when the timer was paused
			d_iPrevTime,		// The time during the last time step
			d_iCurrTime;		// Used to get the current time

	bool d_bPaused;

	double d_dSecondsPerCount;
	double d_dDeltaSeconds;

public:
	// Constructor
	C_Timer();
	virtual ~C_Timer() {}

	// Gets time in seconds since Reset() was called
	double getTime() const;

	// Gets the time in seconds since reset was called
	double getDeltaTime() const { return d_dDeltaSeconds; }

	// Called before message loop
	// Resets everything to zero and sets the current time
	void reset();

	// Called to unpause the timer
	void start();

	// Called to pause the timer
	void pause();

	// Called before the scene is updated, usually at the beginning of the game loop
	void tick();
};

#endif // _CTIME_
