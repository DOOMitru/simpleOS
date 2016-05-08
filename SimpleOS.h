/* Name  : Dumitru
** Class : CS 330 - 001
** Date  : Jan/Feb 2007
**
** Description: SimpleOS.h
Mimics a simple operating system with system calls. With the use of the file
system class it can install a disk, format it, mount it, display its contents,
change between file systems and finally unmount the file system.
******************************************************************************/

#ifndef SIMPLE_OS_H // avoid multiple declarations
#define SIMPLE_OS_H
#include "FileSystem.h"

// represents a disk object
struct DiskObj
{
	string diskName; // name of the disk
	FileSystem * fs; // the disk's file system
	bool mounted;    // is the disk mounted or not
	bool isCurrent;  // is the disk's fs the current fs
};

class SimpleOS
{
public:
	// constructor
	SimpleOS();
	// self explanatory functions
	void InstallDisk(string,int,fstream&);
	void FormatDisk(string,string,int,int,fstream&);
	void MountDisk(string,string,fstream&);
	void UnmountDisk(string,fstream&);
	void ShowMounts(fstream&);
	void ChangeFs(string,fstream&);
	void DumpFs(int,int,fstream&);
	void ShowFs(fstream&);

	void ImportFile(string,fstream&);
	void CpFile(string,string,fstream&);
	void RenameFile(string,string,fstream&);
	void ListFiles(fstream&);
	void ChgOwner(string,int,fstream&);
	void ChangePerm(string,string,fstream&);
	void ExportFile(string,fstream&);
private:
	DiskObj * diskTable[MAX_NUM_FS];

	int GetCurrFSindex();
};

#endif // SIMPLE_OS_H
