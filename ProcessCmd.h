/* Name  : Dumitru
** Class : CS 330 - 001
** Date  : Jan/Feb 2007
** 
** Description: ProcessCmd.h
This class is used to break a string into specific parts and it can then 
process the string as a command by being able to return the number of
arguments passed to the command, a specific argument and other parts of the
string. 
******************************************************************************/

#ifndef PROCESS_CMD_H // avoid multiple declarations
#define PROCESS_CMD_H

#include "constants.h"
#define MAX_NUM_PARAMS 4 // maximum number of command arguments

class ProcessCmd
{
public:
	// constructors
	ProcessCmd();
	ProcessCmd(string);
	// process a new string
	void NewCommand(string);
	// clear the current contents
	void Clear();
	// next 6 fn's return values of
	// specific parts of the string
	int GetUser();
	int GetTime();
	int GetNumParams();
	int GetCommandNo();
	string GetCommand();
	string GetParam(int);
	bool IsComment() { return lineIsComment; }
	// returns the original string
	string GetOriginal();
private:
	string mainStr, original, command;
	string param[MAX_NUM_PARAMS];
	int theTime, user, numParams;
	bool lineIsComment;
	// splits the string into it's parts
	void BreakApart();
};

#endif // PROCESS_CMD_H
