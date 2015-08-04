
#include "C_SystemLog.h"
#include <fstream>
using namespace std;

C_SystemLog* G_SystemLog;
C_SystemLog C_SystemLog::d_instance;

C_SystemLog::C_SystemLog() {
	G_SystemLog = this;
}


C_SystemLog::~C_SystemLog() {
	#if defined _DEBUG
	{
		ofstream file("LogFile.txt");
		C_SystemLog::T_LogIterator logItr(getLogStart());
		C_SystemLog::T_LogIterator endItr(getLogEnd());

		for(; logItr != endItr; logItr++)
			file << (*logItr) << endl;
	}
	#endif
}


void C_SystemLog::post(const std::string& msg) {
	d_Log.push_back(msg);
}
