#include "C_Console.h"
#include "C_Graphics.h"
#include "C_Application.h"

#define MAXHISTORY 25
#define LINEHEIGHT 15
C_Console* G_Console;
C_Console C_Console::d_instance;

C_Console::C_Console() : d_state(E_OFF), d_fAlpha(1.0f)  {
	G_Console = this;
	
	CommandPtr p = new C_ConsoleCommands();
	d_clearCommand = registerCommand("clear", p);
	d_listCommands = registerCommand("listCommands", p);
}

C_Console::~C_Console() {
	d_commandHistory.clear();
	d_consoleOutput.clear();
}


void C_Console::C_ConsoleCommands::onCommand(const HashedValue& cmd, const std::list<std::string>& args) {
	if(G_Console->d_clearCommand == cmd) {
		G_Console->d_commandHistory.clear();
		G_Console->d_consoleOutput.clear();
	} else if(G_Console->d_listCommands == cmd) {
		G_Console->printCommands();
	}
}


void C_Console::onRender() {
	std::list<std::string>::iterator endItr, itr;
	endItr = d_consoleOutput.end();
	short y = G_Application->getWindow()->getHeight() / 2;
	//d_commandLineOutput = ">>> " + d_commandLine;
	d_commandLineOutput.assign(">>> ");
	d_commandLineOutput.append(d_commandLine);
	G_Graphics->drawText(d_commandLineOutput, D3DXCOLOR(1.0f, 0.0f, 0.0f, d_fAlpha), 0, y, 700, y + LINEHEIGHT);

	for(itr = d_consoleOutput.begin(); itr != endItr; ++itr) {
		y -= LINEHEIGHT;
		G_Graphics->drawText((*itr), D3DXCOLOR(1.0f, 0.0f, 0.0f, d_fAlpha), 0, y, 700, y + LINEHEIGHT); 
	}
}

void C_Console::printCommands() {
	CommandList::iterator iter, endIter;
	endIter = d_commandList.end();
	for(iter = d_commandList.begin(); iter != endIter; ++iter) {
		print("    " + iter->name.getStr());
	}
}

void C_Console::onUpdate(const double& deltaSeconds) {
	switch(d_state) {
	case E_OFF:
		break;
	case E_OPENING:
		d_fAlpha += (float)deltaSeconds;
		if(d_fAlpha > 1.0f) {
			d_fAlpha = 1.0f;
			d_state = E_STATIC;
		}
		break;
	case E_CLOSING:
		d_fAlpha -= (float)deltaSeconds;
		if(d_fAlpha < 0.0f) {
			d_fAlpha = 0.0f;
			d_state = E_OFF;
		}
		break;
	case E_STATIC:
		break;
	}
}

void C_Console::onBackSpace() {
	if(d_commandLine.length() > 0) 
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
	d_arguments.clear();
    CommandList::iterator iter, endIter;

	if(d_commandLine.size() == 0) return;

	std::string::size_type index = d_commandLine.find_first_of("abcdefghijklmnopqrstuvwxyz.ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890");
	d_commandLine.erase(0, index);

	print(d_commandLine);

    // add to commandline history
    d_commandHistory.push_back(d_commandLine);
    if(d_commandHistory.size() > MAXHISTORY) d_commandHistory.erase(d_commandHistory.begin());

    // tokenize
	std::string::size_type next_space = 0;
	index = 0;
    while(next_space != std::string::npos) {
        // push word
        next_space = d_commandLine.find(' ', index);
        d_arguments.push_back(d_commandLine.substr(index, next_space));

        // increment index
		index = next_space + 1;
    }

    // look for the command
	bool foundIt = false;
	endIter = d_commandList.end();
    for(iter = d_commandList.begin(); iter != endIter; ++iter) {
		if(iter->name.getStr().compare(*d_arguments.begin()) == 0) {
			d_arguments.erase(d_arguments.begin());
			iter->cmdPtr->onCommand(iter->name.getHashValue(), d_arguments);
			foundIt = true;
			break;
        }
    }

	if(!foundIt) {
		print("   " + *d_arguments.begin() + " - command not recognized!");
	}

	d_commandLine.clear();
}

void C_Console::print(const std::string& strText) {
	if(d_consoleOutput.size() > MAXHISTORY)
		d_consoleOutput.pop_back();
	d_consoleOutput.push_front(strText);
}

void C_Console::open() {
	d_state = E_OPENING;
}

void C_Console::close() {
	d_state = E_CLOSING;
}


HashedValue C_Console::registerCommand(const std::string& name, CommandPtr cmdImp) {
	CommandList::const_iterator iter;
	C_HashedString temp(name.c_str());
	// make sure its not in there first
	for(iter = d_commandList.begin(); iter != d_commandList.end(); ++iter) {
		if(iter->name.getHashValue() == temp.getHashValue())
			return 0;
    }

	d_commandList.push_back(CommandPair(temp, cmdImp));
	return temp.getHashValue();
}