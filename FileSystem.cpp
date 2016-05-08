/* Name  : Dumitru
** Class : CS 330 - 001
** Date  : Jan/Feb 2007
** 
** Description: FileSystem.cpp
This file implements the functions of the file system class.
******************************************************************************/

#ifndef FILE_SYSTEM_CPP // avoid multiple declarations
#define FILE_SYSTEM_CPP

#include "FileSystem.h"

FileSystem::FileSystem()
/******************************************************************************
Default constructor. Sets the default values of the file system.
******************************************************************************/
{
	name = "";
	header.maxBytes = 0;
	header.maxFiles = 0;
	header.usedFiles = 0;
	header.usedBytes = 0;
	strcpy(header.ver,"");
	for(int i = 0; i < MAX_NUM_FILES;i++)
	{
		strcpy(fiTable[i].name,"");
		fiTable[i].length = 0;
		fiTable[i].owner = 0;
		fiTable[i].permissions = 0;
		fiTable[i].startByte = -1;
	}
}

void FileSystem::Install(string dName, int nBytes, fstream & output)
/******************************************************************************
Pre: dName and nBytes are valid 
Installs a new disk with disk name = dName, and size = nBytes
******************************************************************************/
{
	char nullChar = '\0';
	fstream outFile;
	outFile.open(dName.c_str(),ios_base::out | ios_base::binary);
	
	// check if we can create a new file
	if(!outFile)
	{	// Error: Cannot create diskfile
		output << ERROR[4] << endl; return;
	}

	// write out null values on disk
	for(int i = 0; i < nBytes; i++)
		outFile.write(&nullChar,1);
	header.maxBytes = nBytes;

	// close file stream
	outFile.clear(); outFile.close();
}

void FileSystem::Format(string dName, string fsType,
						int nBytes, int nFiles, fstream & output)
/******************************************************************************
Pre: dName, fsType, nBytes, nFiles are valid 
Formats disk 'dName' with the 'fsTye' file system.  The disk can hold up to 
nFiles of files.
******************************************************************************/
{
	int indexPointer = 0;
	// default file to be written out
	char defaultFile[] = "passwords.dat\0";
	char fileContents[] = "root:\n";

	fstream outFile;
	outFile.open(dName.c_str(),ios_base::in | ios_base::out | ios_base::binary);
	
	// check if we can read the disk
	if(!outFile)
	{	// Error: Cannot format disk
		output << ERROR[7] << endl; return;
	}
	
	if(nBytes < (int)((sizeof(header) 
		+ sizeof(fiTable[0]) * nFiles) + sizeof(char) * 7))
	{	// Error: Invalid parameter
		output << ERROR[3] << endl; return;
	}

	// initialize the file system header
	strcpy(header.ver,fsType.c_str());
	header.maxBytes = nBytes;
	header.maxFiles = nFiles;
	header.usedFiles = 1;
	header.usedBytes = sizeof(header)
                       + sizeof(fileObj) * header.maxFiles
                       + sizeof(char) * 7;

	// set values for the first file to be on disk
	strcpy(fiTable[0].name,defaultFile);
	fiTable[0].length = sizeof(char) * 7;
	fiTable[0].owner = 0;
	fiTable[0].permissions = 0;
	fiTable[0].startByte = sizeof(header) + sizeof(fileObj) * header.maxFiles;

	// write the header file to disk
	indexPointer = WriteHeader(outFile);

	// write the file index on disk
	indexPointer = WriteFiTable(outFile);
	
	outFile.seekp(indexPointer);
	// write the default file contents
	outFile.write(reinterpret_cast<const char*>(&fileContents),sizeof(char)*7);

	// close file stream
	outFile.clear(); outFile.close();
}

void FileSystem::Dump(string dName, int startByte, int nBytes, fstream & output)
/******************************************************************************
Pre: dName, startByte and nBytes are valid
Displays the contents on the disk from startByte to (startByte + nBytes)
It displays each 4 bytes as an integer value as well as each each byte as a
character value.
******************************************************************************/
{
	fstream inputFile;
	char tempCh;
	char theBuffer[4]; // buffer for input 
	int tempInt = 0;

	// cant read past the end of the file
	if((startByte + nBytes) > header.maxBytes)
	{	// Error: Invalid parameter
		output << ERROR[3] << endl; return;
	}

	inputFile.open(dName.c_str(),ios_base::in | ios_base::binary);
	
	// check disk is available
	if(!inputFile)
	{	// Error: Cannot read filesystem
		output << ERROR[11] << endl; return;
	}
	output << "++++++++++++++++++++++++++++++++++++++++++" << endl;
	
	// display contents
	for(
		int i = startByte;
		(i < (startByte + nBytes))&&(i < header.maxBytes);
		i += 4
		)
	{
		// display the integer value of the 4 bytes
		output << "+" << setw(8) << right << i << "|";
		inputFile.seekg(i);
		inputFile.read(theBuffer,4);
		tempInt = (*(reinterpret_cast<int*>(theBuffer)));
		output << setw(12); // formatting
		if(tempInt < 0)
			output << "0";
		else output << tempInt; 
		output << "|";
		
		// displays contents as characters
		for(int j = i; j < (i+4); j++)
		{
			inputFile.seekg(j);
			inputFile.read(theBuffer,1);
			tempCh = (*(reinterpret_cast<char*>(theBuffer)));
			// formatting
			if(j == i)
				output << setw(5);
			else output << setw(4);
			// display the character value if  it's a valid char
			if(isPrintable(tempCh))
				output << tempCh;
			else output << "   ";
		}
		
		output << " +" << endl;
	}

	output << "++++++++++++++++++++++++++++++++++++++++++" << endl;
	// close file stream
	inputFile.clear(); inputFile.close();
}

/*######################### accessor functions###############################*/
void FileSystem::SetName(string str) { name = str;              }
int  FileSystem::GetNumMaxBytes()    { return header.maxBytes;  }
int  FileSystem::GetNumUsedFiles()   { return header.usedFiles; }
int  FileSystem::GetNumUsedBytes()   { return header.usedBytes; }
int  FileSystem::GetNumMaxFiles()    { return header.maxFiles;  }
string FileSystem::GetType()         { return header.ver;       }
string FileSystem::GetName()         { return name;             }

/*########################### PHASE II ######################################*/

void FileSystem::ImportFile(string fName, string dName, fstream& output)
{
	int byteCount = 0, indexPointer = 0;
	char fileBuffer[MAX_DISK_BYTE_SIZE];
	char readBuffer[1];

	fstream inputFile, outputFile;

	if(header.usedBytes == header.maxFiles)
	{
		output << ERROR[9] << endl; return;
	}

	inputFile.open(fName.c_str(), ios_base::in | ios_base::binary);
	outputFile.open(dName.c_str(), ios_base::in | ios_base::out | ios_base::binary);

	if(!inputFile)
	{
		output << ERROR[22] << endl; return;
	}

	if(!outputFile)
	{
		output << ERROR[28] << endl; return;
	}

	inputFile.seekg(ios_base::beg);

	while(inputFile.read(readBuffer,1))
	{
		fileBuffer[byteCount] = readBuffer[0];
		if(byteCount >= (header.maxBytes - header.usedBytes))
		{
			output << ERROR[8] << endl; return;
		}
		byteCount++;
	}
	inputFile.clear(); inputFile.close();

	strcpy(fiTable[header.usedFiles].name,fName.c_str());
	fiTable[header.usedFiles].owner = 0;
	fiTable[header.usedFiles].permissions = 0;
	fiTable[header.usedFiles].startByte = header.usedBytes;
	fiTable[header.usedFiles].length = byteCount;

	header.usedBytes += byteCount;
	header.usedFiles++;
	
	indexPointer = WriteHeader(outputFile);
	indexPointer = WriteFiTable(outputFile);

	outputFile.seekp(fiTable[header.usedFiles-1].startByte);
	outputFile.write(reinterpret_cast<const char*>(&fileBuffer),byteCount);
    
	outputFile.clear(); outputFile.close();
}

void FileSystem::Dir(fstream& output)
{
	output << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl
           << "+ Filename          Size   Owner Properties               +" << endl;

	for(int i = 0; i < header.usedFiles; i++)
	{
		int temp = 0;
		for(temp = 0; fiTable[i].name[temp] != '\0';temp++);

		output << "+ "
		       << fiTable[i].name << setw(22 - temp) << right
		       << fiTable[i].length << setw(8) << right
               << fiTable[i].owner << left << " " 
			   << fiTable[i].permissions 
			   << setw(25) << right << "+" << endl;
	}

	output << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
}

void FileSystem::Owner(string fName, int newOwner, fstream& output)
{
	int index = -1;
	for(int i = 0; i < header.usedFiles;i++)
	{
		if(fiTable[i].name == fName)
		{
			index = i; break;
		}
	}

	if(index == -1)
	{
		output << ERROR[25] << endl; return;
	}

	fiTable[index].owner = newOwner;
}

void FileSystem::Rename(string dName, string f1, string f2, fstream& output)
{
	int index = -1;
	fstream o;
	for(int i = 0; i < header.usedFiles;i++)
	{
		if(fiTable[i].name == f1)
		{
			index = i; break;
		}
	}

	if(index == -1)
	{
		output << ERROR[25] << endl; return;
	}

	o.open(dName.c_str(),ios_base::in | ios_base::out | ios_base::binary);

	if(!o)
	{
		output << ERROR[28] << endl; return;
	}
    
	strcpy(fiTable[index].name,f2.c_str());
	index = WriteFiTable(o);
}

void FileSystem::Export(string fName, string dName, fstream& output)
{
	int index = -1;
	char readBuffer[MAX_DISK_BYTE_SIZE];
	fstream inputFile, outputFile;

	inputFile.open(dName.c_str(),ios_base::in | ios_base::binary);

	if(!inputFile)
	{
		output << ERROR[28] << endl; return;
	}

	for(int i = 0; i < header.usedFiles;i++)
	{
		if(fiTable[i].name == fName)
		{
			index = i; break;
		}
	}

	if(index == -1)
	{
		output << ERROR[25] << endl; return;
	}

	outputFile.open(fName.c_str(),ios_base::out | ios_base::binary);
	
	if(!outputFile)
	{
		output << ERROR[23] << endl; return;
	}

	inputFile.seekg(fiTable[index].startByte);
	inputFile.read(readBuffer,fiTable[index].length);

	outputFile.seekp(ios_base::beg);
	outputFile.write(reinterpret_cast<const char*>(&readBuffer),fiTable[index].length);
    
	inputFile.clear(); inputFile.close();
	outputFile.clear(); outputFile.close();
}

void FileSystem::Cp(string dName, string f1, string f2, fstream& output)
{
	int index = -1, indexPointer = 0;
	char readBuffer[MAX_DISK_BYTE_SIZE];
	fstream inputFile;

	if(header.usedFiles == header.maxFiles)
	{
		output << ERROR[9] << endl; return;
	}

	for(int i = 0; i < header.usedFiles;i++)
	{
		if(fiTable[i].name == f2)
		{
			output << ERROR[26] << endl; return;
		}
	}

	for(int i = 0; i < header.usedFiles;i++)
	{
		if(fiTable[i].name == f1)
		{
			index = i; break;
		}
	}

	if(index == -1)
	{
		output << ERROR[25] << endl; return;
	}

	if(((fiTable[index].length)*2) > (header.maxBytes - header.usedBytes))
	{
		output << ERROR[8] << endl; return;
	}

	inputFile.open(dName.c_str(),ios_base::in | ios_base::out | ios_base::binary);
	if(!inputFile)
	{
		output << ERROR[28] << endl; return;
	}

	cout << fiTable[index].startByte << endl
		<< fiTable[index].length << endl;

	inputFile.seekg(fiTable[index].startByte);
	inputFile.read(readBuffer,fiTable[index].length);

	strcpy(fiTable[header.usedFiles].name,f2.c_str());
	fiTable[header.usedFiles].owner = fiTable[index].owner;
	fiTable[header.usedFiles].permissions = fiTable[index].permissions;
	fiTable[header.usedFiles].startByte = header.usedBytes;
	fiTable[header.usedFiles].length = fiTable[index].length;

	header.usedBytes += fiTable[index].length;
	header.usedFiles++;

	indexPointer = WriteHeader(inputFile);
	indexPointer = WriteFiTable(inputFile);

	inputFile.seekp(fiTable[header.usedFiles-1].startByte);
	inputFile.write(reinterpret_cast<const char*>(&readBuffer),fiTable[index].length);

	inputFile.clear(); inputFile.close();
}

int FileSystem::WriteHeader(fstream& o)
{
	o.seekp(ios_base::beg);
	o.write(reinterpret_cast<const char*>(&header.ver),sizeof(char)*8);
	o.write(reinterpret_cast<const char*>(&header.maxFiles), sizeof(int));
	o.write(reinterpret_cast<const char*>(&header.maxBytes), sizeof(int));
	o.write(reinterpret_cast<const char*>(&header.usedFiles),sizeof(int));
	o.write(reinterpret_cast<const char*>(&header.usedBytes),sizeof(int));

	return (o.tellp());
}

int FileSystem::WriteFiTable(fstream& o)
{
	o.seekp(24); // write it after the header
	for(int i = 0; i < header.maxFiles; i++)
	{
		o.write(reinterpret_cast<const char*>(&fiTable[i].name)
			,sizeof(char)*16);
		o.write(reinterpret_cast<const char*>(&fiTable[i].startByte)
			,sizeof(int));
		o.write(reinterpret_cast<const char*>(&fiTable[i].length)
			,sizeof(int));
		o.write(reinterpret_cast<const char*>(&fiTable[i].owner)
			,sizeof(int));
		o.write(reinterpret_cast<const char*>(&fiTable[i].permissions)
			,sizeof(int));
	}
	return (o.tellp());
}
#endif // FILE_SYSTEM_CPP
