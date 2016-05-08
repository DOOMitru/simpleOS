/* Name  : Dumitru
** Class : CS 330 - 001
** Date  : Jan/Feb 2007
** 
** Description: FileSystem.h
Mimics a simple file system manager for an operating system. It will manipulate
a file system stored on a simulated disk ( a normal binary file ).
It can install a new disk, format it with a new file system, load the info
about the file system into the operating system (mount) and display info
about the mounted file systems and their particular contents.
******************************************************************************/

#ifndef FILE_SYSTEM_H // avoid multiple declarations
#define FILE_SYSTEM_H

#include "constants.h"

// represents a file in the file index table
struct fileObj
{
	char name[15];   // file name
	int startByte;   // where it starts on disk
	int length;      // how long the file is
	int owner;       // owner of the file, default 0
	int permissions; // permissions on the file
};
// represents the file system header 
struct fsHeader
{
	char ver[8];   // version of the file system
	int maxFiles;  // maximum number of files on disk
	int maxBytes;  // maximum number of bytes of disk
	int usedFiles; // how many files are currently on disk
	int usedBytes; // how many bytes of maxbytes have been used
};

class FileSystem
{
public:
	// default constructor
	FileSystem();
	// install a new disk 
	void Install(string,int,fstream&);
	// format a specific disk
	void Format(string,string,int,int,fstream&);
	// show the disk contents
	void Dump(string,int,int,fstream&);
	// set the file system name
	void SetName(string);

	void ImportFile(string,string,fstream&);
	void Dir(fstream&);
	void Owner(string,int,fstream&);
	void Rename(string,string,string,fstream&);
	void Export(string,string,fstream&);
	void Cp(string,string,string,fstream&);

	// next 6 functions return the values of
	// specific members of the class
	string GetName();
	string GetType();
	int GetNumMaxBytes();
	int GetNumMaxFiles();
	int GetNumUsedFiles();
	int GetNumUsedBytes();
private:
	fileObj fiTable[MAX_NUM_FILES]; // file index table
	fsHeader header;
	string name; // name of the file system

	int WriteHeader(fstream&);
	int WriteFiTable(fstream&);
};

#endif // FILE_SYSTEM_H
