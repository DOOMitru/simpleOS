/* Name  : Dumitru
** Class : CS 330 - 001
** Date  : Jan/Feb 2007
** 
** Description: SimpleOS.cpp
Implements the functions of the simple operating system class.
******************************************************************************/

#ifndef SIMPLE_OS_CPP // avoid multiple declarations
#define SIMPLE_OS_CPP

#include "SimpleOS.h"

SimpleOS::SimpleOS()
/******************************************************************************
Default constructor. Sets the default values of the operating system.
******************************************************************************/
{	// assign default values
	for(int i = 0; i < MAX_NUM_FS;i++)
		diskTable[i] = NULL;
}

void SimpleOS::InstallDisk(string dName, int nBytes, fstream & output)
/******************************************************************************
Checks for valid parameters and if they are valid, it installs the disk refered
to by dName on the operating system.
******************************************************************************/
{
	int index = -1;
	// check for valid disk name
	if(!isValidName(dName,15,false))
	{	// Error: Invalid Parameter
		output << ERROR[3] << endl;
		return;
	}
	// check the disk size
	// should be > 0 and < MAX_DISK_BYTE_SIZE
	if(nBytes <= 0 || nBytes > MAX_DISK_BYTE_SIZE)
	{	// Error: Invalid Parameter
		output << ERROR[3] << endl;
		return;
	}
	// check if the dName disk is already installed
	for(int i = 0; i < MAX_NUM_FS; i++)
	{
		if(diskTable[i] != NULL && (dName == diskTable[i]->diskName))
		{	// Error: Disk is already installed
			output << ERROR[19] << endl; return;
		}
	}
	// check if we have enough space to install a new disk
	for(int i = 0; i < MAX_NUM_FS; i++)
	{
		if(diskTable[i] == NULL)
		{
			index = i; break;
		}
	}

	if(index == -1)
	{	// Error: Mount table is full
		output << ERROR[13] << endl; return;
	}

	// if we got here we can install a new disk at [index]
	diskTable[index] = new DiskObj;
	diskTable[index]->diskName = dName;
	diskTable[index]->fs = new FileSystem;
	diskTable[index]->fs->SetName("");
	diskTable[index]->fs->Install(dName,nBytes,output);
	diskTable[index]->mounted = false;
	diskTable[index]->isCurrent = false;
}

void SimpleOS::FormatDisk(string dName, string fsType,
			int nBytes, int nFiles, fstream & output)
/******************************************************************************
Checks for valid parameters and if they are valid it formats the disk refered
to by dName with the file system type fsType with a maximum no of bytes nBytes
and a maximum number of files nFiles
******************************************************************************/
{
	int index = -1;
	// check for valid disk name
	if(!isValidName(dName,15,false))
	{	// Error: Invalid Parameter
		output << ERROR[3] << endl; return;
	}
	// check for valid filesystem type
	if(isFsType(fsType))
	{	// Error: Unsupported Filesystem Type
		output << ERROR[5] << endl; return;
	}
	// check for number of files
	if(nFiles > MAX_NUM_FILES || nFiles < 1)
	{	// Error: Invalid Parameter
		output << ERROR[3] << endl; return;
	}
	// checks if the disk is installed 
	for(int i = 0; i < MAX_NUM_FS; i++)
	{
		if((diskTable[i] != NULL) && (diskTable[i]->diskName == dName))
		{
			index = i; break;
		}
	}
	if(index == -1)
	{	// Error: Disk not installed
		output << ERROR[21] << endl; return;
	}

	if(nBytes > diskTable[index]->fs->GetNumMaxBytes())
	{	// Error: Invalid Parameter
		output << ERROR[3] << endl; return;
	}
	
	diskTable[index]->fs->Format(dName,fsType,nBytes,nFiles,output);
}

void SimpleOS::MountDisk(string dName, string fsName, fstream & output)
/******************************************************************************
Checks for valid parameters and if they are valid, it mounts the file system
refered to by fsName on the disk refered to by dName
******************************************************************************/
{
	bool diskTableFull = true;
	int index = -1;
	// check for valid disk name
	if(!isValidName(dName,15,false))
	{	// Error: Invalid Parameter
		output << ERROR[3] << endl;
		return;
	}
	// check for valid filesystem name
	if(!isValidName(fsName,15,true))
	{	// Error: Invalid Parameter
		output << ERROR[3] << endl;
		return;
	}
	// check if there's enough room on the mount table
	for(int i = 0; i < MAX_NUM_FS;i++)
	{
		if(diskTable[i] == NULL)
		{
			diskTableFull = false;
			break;
		}
	}
	if(diskTableFull)
	{	// Error: Mount table is full
		output << ERROR[13] << endl;
		return;
	}
	
	// --- IF THE FILE TABLE IS NOT FULL

	// if fsName is valid, it has a ':' char at the end
	// remove the ':'
	fsName = fsName.substr(0,fsName.length()-1);

	// look for the disk refered to by dName
	for(int i = 0; i < MAX_NUM_FS; i++)
	{
		if(diskTable[i] != NULL && (diskTable[i]->diskName == dName))
		{
			index = i; break;
		}
	}

	if(index == -1) // disk not found
	{	// Error: Cannot read filesystem
		output << ERROR[11] << endl; return;
	}

	// disk found:
	// check if it is already mounted or not
	if(diskTable[index]->mounted)
	{	// Error: Filesystem is already mounted
		output << ERROR[17] << endl; return;
	}

	// mount the disk
	diskTable[index]->mounted  = true;
	diskTable[index]->isCurrent = false;
	diskTable[index]->fs->SetName(fsName);
}

void SimpleOS::UnmountDisk(string fsName, fstream& output)
/******************************************************************************
Checks for valid parameters and if they are valid, unmounts the file system
refered to by fsName from the operating system.
******************************************************************************/
{
	int index = -1;
	// check for valid file system name
	// if not valid, it's not in the system to start with
	if(!isValidName(fsName,15,true))
	{	// Error: Invalid parameter
		output << ERROR[3] << endl; return;
	}
	// if it is a valid file system name, it has a ':'
	// at the end, strip it
	fsName = fsName.substr(0,fsName.length()-1);
	
	// look for the file system in the table 
	for(int i = 0; i < MAX_NUM_FS; i++)
	{
		if(diskTable[i] != NULL && (diskTable[i]->fs->GetName() == fsName))
		{
			index = i; break;
		}
	}

	if(index == -1) // not in table
	{	// Error: Filesystem is not mounted
		output << ERROR[14] << endl; return;
	}

	// if the this file system is the current one
	// it cannot be unmounted
	if(diskTable[index]->isCurrent)
	{	// Error: Filesystem is in use
		output << ERROR[18] << endl; return;
	}
	
	// unmount file system
	diskTable[index]->mounted = false;
}

void SimpleOS::ShowMounts(fstream & output)
/******************************************************************************
Displays the mounted file systems.
******************************************************************************/
{
	output << "+++++++++++++++++++++++++++++++++++++++++++++++" << endl
		 << "+ FSId FS_Name         FS_Disk                +" << endl;
	
	for(int i = 0; i < MAX_NUM_FS; i++)
	{	// if it's mounted, display it
		if(diskTable[i] != NULL && diskTable[i]->mounted)
		{	// formatting and output
			output << "+" << setw(5) << right << i << " "
				<< left << setw(16) << diskTable[i]->fs->GetName()
				<< setw(17) << diskTable[i]->diskName;
			
			// indicate if the file system is the current file system
			if(diskTable[i]->isCurrent) output << "+++++ +";
			else output << right <<  setw(7) << "+";
			output << endl;
		}
	}

	output << "+++++++++++++++++++++++++++++++++++++++++++++++" << endl;
}

void SimpleOS::ChangeFs(string fsName, fstream & output)
/******************************************************************************
Check for valid input and if it is valid, change the current file system to
the file system refered by fsName
******************************************************************************/
{
	int index = -1;
	// check for valid filesystem name
	if(!isValidName(fsName,15,true))
	{	// Error: Invalid Parameter
		output << ERROR[3] << endl;
		return;
	}
	// if fsName is valid, it has a ':' char at the end
	// remove the ':'
	fsName = fsName.substr(0,fsName.length()-1);

	// look for the file system 
	for(int i = 0; i < MAX_NUM_FS; i++)
	{
		if(diskTable[i] != NULL)
		{
			if(diskTable[i]->fs->GetName() == fsName)
			{
				index = i; break;
			}
		}
	}

	if(index == -1 || !(diskTable[index]->mounted))
	{	// Error: Filesystem is not mounted
		output << ERROR[14] << endl; return;
	}
	
	// find the current file system and change its state
	for(int i = 0; i < MAX_NUM_FS; i++)
	{
		if(diskTable[i] != NULL && diskTable[i]->isCurrent)
		{
			diskTable[i]->isCurrent = false;
			break;
		}
	}
	
	// set new current file system
	diskTable[index]->isCurrent = true;
}

void SimpleOS::DumpFs(int startByte, int nBytes, fstream & output)
/******************************************************************************
Check for valid parameters and if they are valid, display the contents of the
disk from startByte to (startByte + nBytes)
******************************************************************************/
{
	int index = -1;
	// check for valid startByte and nBytes
	// they should be >= 0 and divisible by 4
	if(startByte < 0 || nBytes < 0)
	{
		// Error: Invalid Parameter
		output << ERROR[3] << endl;
		return;
	}
	if((startByte % 4 != 0) || (nBytes % 4 != 0))
	{
		// Error: Invalid Parameter
		output << ERROR[3] << endl;
		return;
	}
	// find the current file system
	for(int i = 0; i < MAX_NUM_FS; i++)
	{
		if(diskTable[i] != NULL && (diskTable[i]->isCurrent))
		{
			index = i; break;
		}
	}
	
	if(index == -1)
	{	// Error: No filesystem has been specified
		output << ERROR[16] << endl; return;
	}
	
	// display contents
	diskTable[index]->fs->Dump(diskTable[index]->diskName,startByte,nBytes,output);
}

void SimpleOS::ShowFs(fstream & output)
/******************************************************************************
Displays the information about the current file system
******************************************************************************/
{
	int index = -1;
	float percent = -1.0;
	
	// find the current file system
	for(int i = 0; i < MAX_NUM_FS; i++)
	{
		if(diskTable[i] != NULL && diskTable[i]->isCurrent)
		{
			index = i; break;
		}
	}

	if(index == -1)
	{	// Error: No filesystem has been specified
		output << ERROR[16] << endl; return;
	}
	
	// display header
	output << "++++++++++++++++++++++++++++++++++++++"
		 << "+++++++++++++++++++++++++++++++++++++" << endl;
	output << "+ Filesystem       Type    UsedF  MaxF"
		 << " UsedB  MaxB %Used Diskfile         +" << endl;

	// calculate the percent of used bytes
	percent = ((float)diskTable[index]->fs->GetNumUsedBytes()/
		(float)diskTable[index]->fs->GetNumMaxBytes()) * 100;

	// format and output information
	output << "+ " << setw(16) << left << diskTable[index]->fs->GetName() << right
		<< setw(8) << diskTable[index]->fs->GetType() << " " 
		<< setw(5) << right << diskTable[index]->fs->GetNumUsedFiles()
		<< setw(6) << diskTable[index]->fs->GetNumMaxFiles()
		<< setw(6) << diskTable[index]->fs->GetNumUsedBytes()
		<< setw(6) << diskTable[index]->fs->GetNumMaxBytes()
		<< setw(6) << (int)percent
		<< " " << diskTable[index]->diskName 
		<< setw(18-(int)(diskTable[index]->diskName).length()) << right << "+" << endl;

	// display footer
	output << "++++++++++++++++++++++++++++++++++++++"
		 << "+++++++++++++++++++++++++++++++++++++" << endl;
}

/*########################### PHASE II ######################################*/

void SimpleOS::ImportFile(string fName, fstream& output)
{
	int index = -1;

	index = GetCurrFSindex();

	if(index == -1)
	{
		output << ERROR[16] << endl; return;
	}

	if(!isValidName(fName,15,false))
	{
		output << ERROR[3] << endl; return;
	}

	diskTable[index]->fs->ImportFile(fName,diskTable[index]->diskName,output);
}

void SimpleOS::CpFile(string f1, string f2, fstream& output)
{
	int index = -1;
	if(!isValidName(f1,15,false) || !isValidName(f2,15,false) || f1 == f2)
	{
		output << ERROR[3] << endl; return;
	}

	index = GetCurrFSindex();

	if(index == -1)
	{
		output << ERROR[16] << endl; return;
	}

	diskTable[index]->fs->Cp(diskTable[index]->diskName,f1,f2,output);
}

void SimpleOS::RenameFile(string f1, string f2, fstream& output)
{
	int index = -1;

	if(!isValidName(f1,15,false) || !isValidName(f2,15,false))
	{
		output << ERROR[3] << endl; return;
	}
	
	index = GetCurrFSindex();

	if(index == -1)
	{
		output << ERROR[16] << endl; return;
	}

	diskTable[index]->fs->Rename(diskTable[index]->diskName,f1,f2,output);
}

void SimpleOS::ListFiles(fstream& output)
{
	int index = -1;

	index = GetCurrFSindex();

	if(index == -1)
	{
		output << ERROR[16] << endl; return;
	}

	diskTable[index]->fs->Dir(output);
}

void SimpleOS::ChgOwner(string fName, int newOwner, fstream& output)
{
	int index = -1;

	if(newOwner < 0 || (!isValidName(fName,15,false)))
	{
		output << ERROR[3] << endl; return;
	}

	index = GetCurrFSindex();

	if(index == -1)
	{
		output << ERROR[16] << endl; return;
	}

	diskTable[index]->fs->Owner(fName,newOwner,output);
}

void SimpleOS::ChangePerm(string fName, string newPerms, fstream& output)
{

}

void SimpleOS::ExportFile(string fName, fstream& output)
{
	int index = -1;

	if(!isValidName(fName,15,false))
	{
		output << ERROR[3] << endl; return;
	}

	index = GetCurrFSindex();

	if(index == -1)
	{
		output << ERROR[16] << endl; return;
	}

	diskTable[index]->fs->Export(fName,diskTable[index]->diskName,output);
}

int SimpleOS::GetCurrFSindex()
{
	for(int i = 0; i < MAX_NUM_FS;i++)
	{
		if(diskTable[i] != NULL)
		{
			if(diskTable[i]->isCurrent)
				return i;
		}
	}

	return -1;
}
#endif // SIMPLE_OS_CPP
