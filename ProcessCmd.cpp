/* Name  : Dumitru
** Class : CS 330 - 001
** Date  : Jan/Feb 2007
** 
** Description: ProcessCmd.cpp
******************************************************************************/

#ifndef PROCESS_CMD_CPP
#define PROCESS_CMD_CPP

#include "ProcessCmd.h"

// constructors
ProcessCmd::ProcessCmd()
{ Clear(); }

ProcessCmd::ProcessCmd(string str)
{ Clear(); NewCommand(str); }

// new command based on str
void ProcessCmd::NewCommand(string str)
{
	Clear();
	this->mainStr = str;
	this->original = str;
	BreakApart();
}

// clear the old string
void ProcessCmd::Clear()
{
	lineIsComment = false;
	mainStr = original = command = "";
	theTime = user = numParams = -1;
	for(int i = 0; i < MAX_NUM_PARAMS;i++)
		param[i] = "";
}

void ProcessCmd::BreakApart()
/******************************************************************************
Breaks apart the mainStr into it's corresponding parts, the user and time, the
command and the command parameters
******************************************************************************/
{
	vector<string> temp;          // holds individual parts of the string
	vector<string>::iterator itr; // used to transverse temp
	string tempStr;               // part to be pushed into temp
	int tempInt = -1;

	// strip off leading spaces from the string
	while(mainStr.at(0) == ' ')
		mainStr = mainStr.substr(1);

	while(mainStr.at(mainStr.length()-1) == ' ')
		mainStr = mainStr.substr(0,mainStr.size()-1);
	
	// search the strings for any comments ( ie. "#")
	for(int i = 0; i < (int)mainStr.length();i++)
	{
		if(mainStr.at(i) == '#')
		{
			if(i == 0)
				lineIsComment = true;
			tempInt = i; break;
		}
	}

	// trim the string of comments if it has any
	if(tempInt > 0)
	{
		if(mainStr.at(tempInt - 1) == ' ')
			mainStr = mainStr.substr(0,tempInt-1);
		else
			mainStr = mainStr.substr(0,tempInt);
	}

	tempInt = -1;
    do
    {
		// break off each part of the string
        tempInt =(int)mainStr.find_first_of(' ',0);
        tempStr = mainStr.substr(0,tempInt);
		// push each part onto temp
        temp.push_back(tempStr);
        mainStr = mainStr.substr(tempInt+1);
		// next while loop trims leading spaces
		// of the remaining string so we can then
		// search for the next space behind the 
		// first term in the string
		// .: the amount of spaces between the
		//    arguments doesn't matter
		while(mainStr.at(0) == ' ')
			mainStr = mainStr.substr(1);

    }while(tempInt != -1);
	
	// sort out the string by how many useful parts it has
	switch((int)temp.size())
	{
	case 7: // format
		theTime = atoi(temp.at(0).c_str());
		user = atoi(temp.at(1).c_str());
		command = temp.at(2);
		param[0] = temp.at(3);
		param[1] = temp.at(4);
		param[2] = temp.at(5);
		param[3] = temp.at(6);
		numParams = 4;
		break;
	case 5: // install, mount, dumpfs
		theTime = atoi(temp.at(0).c_str());
		user = atoi(temp.at(1).c_str());
		command = temp.at(2);
		param[0] = temp.at(3);
		param[1] = temp.at(4);
		numParams = 2;
		break;
	case 4: // cfs, unmount
		theTime = atoi(temp.at(0).c_str());
		user = atoi(temp.at(1).c_str());
		command = temp.at(2);
		param[0] = temp.at(3);
		numParams = 1;
		break;
	case 3: // showfs, showmounts
		theTime = atoi(temp.at(0).c_str());
		user = atoi(temp.at(1).c_str());
		command = temp.at(2);
		numParams = 0;
		break;
	default: // not a valid command
		numParams = -1;
	}
	
	// clear out the temp container
	for(itr = temp.begin(); itr < temp.end(); itr++)
	{ temp.pop_back(); }

	temp.clear();
}

int ProcessCmd::GetCommandNo()
/******************************************************************************
Returns:
1. the command index number if it is a valid command
2. -1 otherwise
******************************************************************************/
{
	// check command array
	for(int i = 0; i < SIZE_CMDS_ARRAY;i++)
	{
		if(command == CMDS[i])
			return i; // command found
	}

	return -1; // command not in array
}

/*######################### accessor functions###############################*/
int ProcessCmd::GetUser()
{ return this->user; }
int ProcessCmd::GetTime()
{ return this->theTime; }
int ProcessCmd::GetNumParams()
{ return this->numParams; }
string ProcessCmd::GetCommand()
{ return this->command; }
string ProcessCmd::GetParam(int index)
{ return this->param[index]; }
string ProcessCmd::GetOriginal()
{ return this->original; }

#endif // PROCESS_CMD_CPP
