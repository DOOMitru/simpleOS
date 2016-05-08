/* Name  : Dumitru
** Class : CS 330 - 001
** Date  : Jan/Feb 2007
** 
** Description: constants.h
Contains all preprocessor directories, as well as constants and global 
functions needed.
******************************************************************************/
#ifndef CONSTANTS_H // Avoid multiple definitions
#define CONSTANTS_H

/*############################### INCLUDES ##################################*/
#include <iostream> // I/O, "right" flag
#include <iomanip>  // setw()
#include <fstream>  // file I/O
#include <vector>   // vector container
#include <string>   // string class
using namespace std;// standard namespace

/*############################### CONSTANTS #################################*/
// allowed file system types
const string FS_TYPE[] = 
{
	"SFS-1.0" // 0
};

// allowed S.O.S commands
const string CMDS[] =
{
	"install",		// 0
	"format",		// 1
	"mount",		// 2
	"showmounts",	// 3
	"cfs",			// 4
	"dumpfs",		// 5
	"showfs",		// 6
	"unmount",      // 7
	"import",       // 8
	"copy",         // 9
	"rename",       // 10
	"dir",          // 11
	"owner",        // 12
	"permission",   // 13
	"export"        // 14
};

// available error messages
const string ERROR[] = {
	"", // empty to keep index no = to error no
	"*ERROR  1: System error",
	"*ERROR  2: Invalid instruction",
	"*ERROR  3: Invalid parameter",
	"*ERROR  4: Cannot create diskfile",
	"*ERROR  5: Unsupported filesystem type",
	"*ERROR  6: Invalid filesystem name",
	"*ERROR  7: Cannot format disk",
	"*ERROR  8: Insufficient room in filesystem",
	"*ERROR  9: Maximum files in filesystem",
	"*ERROR 10: Cannot update filesystem",
	"*ERROR 11: Cannot read filesystem",
	"*ERROR 12: Invalid filesystem",
	"*ERROR 13: Mount table is full",
	"*ERROR 14: Filesystem is not mounted",
	"*ERROR 15: No filesystems are mounted",
	"*ERROR 16: No filesystem has been specified",
	"*ERROR 17: Filesystem is already mounted",
	"*ERROR 18: Filesystem is in use",
	"*ERROR 19: Disk is already installed",
	"*ERROR 20: No disks installed",
	"*ERROR 21: Disk not installed",
	
	"*ERROR 22: File cannot be read",
	"*ERROR 23: File cannot be created",
	"*ERROR 24: File cannot be written",
	"*ERROR 25: Specified minifile does not exist",
	"*ERROR 26: Specified minifile already exists",
	"*ERROR 27: Specified minifile cannot be written",
	"*ERROR 28: Cannot read disk"
};
// next 3 lines: constants for the number of entries in each array
#define SIZE_ERROR_ARRAY   (sizeof(ERROR)/sizeof(string))
#define SIZE_CMDS_ARRAY    (sizeof(CMDS)/sizeof(string))
#define SIZE_FS_TYPE_ARRAY (sizeof(FS_TYPE)/sizeof(string))

#define MAX_DISK_BYTE_SIZE 100000 // max no. bytes for any disk
#define MAX_NUM_FS         10     // max no. file systems in S.O.S
#define MAX_NUM_FILES      500    // max no. of files on disk

/*############################### FUNCTIONS #################################*/
/******************************************************************************
** bool isPrintable(string str,int maxLength, bool isFsName);
Returns:
1. true if a) str is a valid disk name       (isFsName must be false)
           b) str is a valid filesystem name (isFsName must be true)
2. false otherwise
******************************************************************************/
inline bool isValidName(string str,int maxLength, bool isFsName)
{
	// to hold no. of '.' or ':' characters
	int numPeriods = 0, numColons = 0;
	// make sure the length is:
	// -- greater than 0 and
	// -- less than maxLength
	if((int)str.length() > maxLength || (int)str.length() == 0)
		return false;
	// make sure the first character is a letter
	if(!isalpha(str.at(0)))
		return false;
	// check the rest of the string
	for(int i = 0;i < (int)str.length(); i++)
	{
		// check for periods, at most 1 period allowed
		if(str.at(i) == '.')
		{
			if(numPeriods > 0)
				return false;
			else numPeriods++;
		}
		// if it's a file system name it can have at most 1 ':'
		else if(str.at(i) == ':' && isFsName)
		{
			if(numColons > 0)
				return false;
			else numColons++;
		}
		// if it's not ".", it must be alphanumeric(a,A-z,Z ; 0-9)
		else if(!isalnum(str.at(i)))
			return false;
	}
	// if it is a file system name the last char must be a ':'
	if(isFsName)
	{
		if(str.at((int)str.length()-1)!= ':')
			return false;
	}
	// if it got to this point, we got a valid name
	return true;
}
/******************************************************************************
** int isValidNum(string str);
Returns:
1. -1 if str is not made up of all digits
2. the integer value of str 
******************************************************************************/
inline int isValidNum(string str)
{
	if(str.empty()) // theStr == ""
		return -1;

	// each char in the string MUST be a digit, otherwise return -1
	for(int i = 0; i < (int)str.length(); i++)
		if(!isdigit(str.at(i)))
			return -1;
	// integer value of the string
	return atoi(str.c_str());
}
/******************************************************************************
** bool isFsType(string str);
Returns:
1. true if str is == to any of the allowed file system types
2. false otherwise
******************************************************************************/
inline bool isFsType(string str)
{
	// if it's in the array, then it's valid
	for(int i = 0; i < SIZE_FS_TYPE_ARRAY; i++)
		if(str.compare(FS_TYPE[i]))
			return true;

	return false; // not in array
}
/******************************************************************************
** bool isPrintable(char ch);
Returns:
1. true if ch is one of the allowed printable characters
2. false otherwise
******************************************************************************/
inline bool isPrintable(char ch)
{
	return (ch >= '!' && ch <= '~') ? true : false;
}
#endif // CONSTANTS_H
