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
	friend class C_ConsoleCommands;
public:
	~C_Console();
	void open();	// Used to open the console
	void close();	// Used to close the console
	bool isActive() const  { return d_state == E_STATIC; }
	bool isVisible() const { return d_state != E_OFF; }

	void onBackSpace();
	void onTab();
	void onSpaceBar();
	void onEnter();
	void onKeyboardInput( const char c );

	// Returns the hash value of the command name.  If the value failed to insert, returns 0
	HashedValue registerCommand(const std::string& name, CommandPtr cmdImp);

	void onUpdate(const double& deltaSeconds);
	void onRender();

	void printCommands();

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

	// Internal Use
	enum State {
		E_OFF = 0,
		E_OPENING,
		E_CLOSING,
		E_STATIC
	};

	class C_ConsoleCommands : public I_ConsoleCommand {
		void onCommand(const HashedValue& cmd, const std::list<std::string>& args);
	};

	State d_state;
	float d_fAlpha;
	std::list<std::string> d_commandHistory;
	std::list<std::string> d_consoleOutput;
	std::list<std::string> d_arguments;
	CommandList d_commandList;

	// Console Commands
	HashedValue d_clearCommand, d_listCommands;
	
	static C_Console d_instance;

	std::string d_commandLine;	// the current command line
	std::string d_commandLineOutput; // The command line shown on the screen

	int d_iCursorBlinkTimer;	// Countdown to toggle cursor blink state
	bool d_bCursorOn;	// Is the cursor currently displayed?

	//If this is true, we have a string to execute on our next update cycle.  
	//We have to do this because otherwise the user could interrupt in the midst
	//of an onUpdate() call in another system.  This causes problems.
	bool d_bExecuteCmdOnUpdate;
};

extern C_Console* G_Console;

#endif // _CCONSOLE_
