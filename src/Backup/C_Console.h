//======================================================================
// C_Console.cpp 
// Author: James McCormick
// Discription:
//	The system for handling the engine console system
//======================================================================
#ifndef _CCONSOLE_
#define _CCONSOLE_

#include "C_HashedString.h"
#include "C_SmartPointer.h"
#include <list>

class I_ConsoleCommand : public C_SmartPointerObject {
public:
	~I_ConsoleCommand() {}
	virtual void onCommand(const HashedValue& cmd, const std::list<std::string>& args) = 0;
};
typedef C_SmartPointer<I_ConsoleCommand> CommandPtr;

class C_Console  // TODO - finish this class
{
public:
	~C_Console();
	void open();	// Used to open the console
	void close();	// Used to close the console
	bool isActive() const  { return d_bActive; }

	void onBackSpace();
	void onTab();
	void onSpaceBar();
	void onEnter();
	void onKeyboardInput( const char c );

	void registerCommand(const C_HashedString& cmdID, CommandPtr cmdImp);

	void onUpdate(const double& deltaSeconds);
	void onRender();

private:
	C_Console();
	C_Console(const C_Console&);
	void processCommand();
	void print(const std::string& strText);
	
	struct CommandPair {
		C_HashedString name;
		CommandPtr cmdPtr;
		CommandPair(const C_HashedString& id, CommandPtr imp) : name(id), cmdPtr(imp) {}
	};
	typedef std::list<CommandPair> CommandList;

	bool d_bActive;
	std::list<std::string> d_commandHistory;
	CommandList d_commandList;

	static C_Console d_instance;

	//RECT d_ConsoleOutputRect;	//Where results get shown
	//RECT d_ConsoleInputRect;	//Where input is entered

	std::string d_commandLine;	// the current command line

	int d_iConsoleInputSize;	// Height of the input console window

	int d_iCursorBlinkTimer;	// Countdown to toggle cursor blink state
	bool d_bCursorOn;	// Is the cursor currently displayed?

	//If this is true, we have a string to execute on our next update cycle.  
	//We have to do this because otherwise the user could interrupt in the midst
	//of an onUpdate() call in another system.  This causes problems.
	bool d_bExecuteCmdOnUpdate;
};

extern C_Console* G_Console;

#endif // _CCONSOLE_
