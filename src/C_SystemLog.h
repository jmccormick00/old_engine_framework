//-----------------------------------------------------------
// C_SystemLog.h
// Description:
//	Stores the system log.  Allows messages to be posted.
//	
//-----------------------------------------------------------

#ifndef _CSYSTEMLOG_
#define _CSYSTEMLOG_

#include <string>
#include <vector>

class C_SystemLog {
private:
	std::vector<std::string> d_Log;

	C_SystemLog();
	C_SystemLog(const C_SystemLog&);

	static C_SystemLog d_instance;

public:
	virtual ~C_SystemLog();

	void post(const std::string& msg);

	typedef std::vector<std::string>::reverse_iterator T_LogIterator;
	T_LogIterator getLogStart() { return d_Log.rbegin(); }
	T_LogIterator getLogEnd() { return d_Log.rend(); }
};

extern C_SystemLog* G_SystemLog;

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef DEBUGLOG
	#define DEBUGLOG(x) G_SystemLog->post(x);
	#endif
#else
	#ifndef DEBUGLOG
	#define DEBUGLOG(x) 
	#endif
#endif

#endif // _CSYSTEMLOG_
