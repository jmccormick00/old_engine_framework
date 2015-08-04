#include "C_Console.h"

#define MAXHISTORY 30
C_Console* G_Console;
C_Console C_Console::d_instance;

C_Console::C_Console() : d_bActive(false) {
	G_Console = this;
}

C_Console::~C_Console() {
	// TODO - implement
}

void C_Console::onRender() {
	// TODO - implement
}

void C_Console::onUpdate(const double& deltaSeconds) {
	// TODO - implement
}

void C_Console::onBackSpace() {
	d_commandLine.pop_back();
}

void C_Console::onTab() {
	// TODO - implement
}

void C_Console::onSpaceBar() {
	d_commandLine.push_back(' ');
}

void C_Console::onEnter() {
	if(d_commandLine.size() > 0) 
		processCommand();
}
	
void C_Console::onKeyboardInput( const char c ) {
	d_commandLine.push_back(c);
}


void C_Console::processCommand() {
    std::string::size_type index = 0;
    std::list<std::string> arguments;
    CommandList::const_iterator iter;

    // add to text buffer
	print(d_commandLine);

    // add to commandline history
    d_commandHistory.push_back(d_commandLine);
    if(d_commandHistory.size() > MAXHISTORY) d_commandHistory.erase(d_commandHistory.begin());

    // tokenize
    while(index != std::string::npos) {
        // push word
        std::string::size_type next_space = d_commandLine.find(' ');
        arguments.push_back(d_commandLine.substr(index, next_space));

        // increment index
        if(next_space != std::string::npos) index = next_space + 1;
        else break;
    }

    // execute (look for the command)
    for(iter = d_commandList.begin(); iter != d_commandList.end(); ++iter) {
		if(iter->name.getStr() == *arguments.begin()) {
			arguments.erase(arguments.begin());
			iter->cmdPtr->onCommand(iter->name.getHashValue(), arguments);
        }
    }
}

void C_Console::print(const std::string& strText) {
	// TODO - implement
}

void C_Console::open() {
	d_bActive = true;
}

void C_Console::close() {

}


void C_Console::registerCommand(const C_HashedString& cmdID, CommandPtr cmdImp) {
	CommandList::const_iterator iter;
	// make sure its not in there first
	for(iter = d_commandList.begin(); iter != d_commandList.end(); ++iter) {
		if(iter->name.getHashValue() == cmdID.getHashValue())
			return;
    }

	d_commandList.push_back(CommandPair(C_HashedString(cmdID), cmdImp)); 
}