/* Name  : Dumitru
** Class : CS 330 - 001
** Date  : Jan/Feb 2007
** 
** Description: Phase I of S.O.S Project
Implements the overall management of the file systems.
It can install a new simulated disk, format the disk to create a file system,
load the information about the file system into the S.O.S (mount), and display
information about the mounted file systems and the contents of a particular
file system.
Instructions are loaded from a (ascii) text file and the output is logged in
a (ascii) log file. These files are passed to the program as arguments.
******************************************************************************/

#include "SimpleOS.h"   // SimpleOS class
#include "ProcessCmd.h" // ProcessCmds class
#include <time.h>       // for ctime()

/******************************************************************************
** void isPrintable(ProcessCmd & cmds, SimpleOS & sos, fstream & output );

Checks which command has to be processed according to cmds, checks for the 
correct number of parameters for the specified command and calls the
corresponding function of the sos.
******************************************************************************/
void Run(ProcessCmd&,SimpleOS&,fstream&);

/*################################# main ####################################*/
int main(int argc, char * argv[])
{
	// check for valid parameters
	// no need to create instances if the arguments are invalid
	if(argc != 3)
	{
		cout << "usage: program [inputFileName] [outputFileName]" << endl;
		return -1;
	}

	ProcessCmd cmds;       // used to process each command
	SimpleOS sos;          // simple operating system, performs commands
	fstream output, input; // for file input/output
	string cmd;            // temp string used to store command from file
	time_t theTime;        // used to record the current time

	// open input/output files
	output.open(argv[2],ios_base::out);
	input.open(argv[1],ios_base::in);
	 
	if(!input) // input file could not be opened
	{
		cout << argv[1] << " could not be opened." << endl
			<< "Program terminated." << endl;
		return 1; // exit program
	}

	if(!output) // log file could not be created
	{
		cout << argv[2] << " could not be opened." << endl
			<< "Programm terminated." << endl;
		return 1; // exit program
	}
	
	// initiate current time and record it
	time(&theTime);
	output << "SOS started on " << ctime(&theTime) << endl;

	// process input file
    while(getline(input,cmd))
	{
		if(!cmd.empty()) // skip empty lines
		{
			output << cmd << endl;
			cmds.NewCommand(cmd);
			if(!cmds.IsComment())
				Run(cmds,sos,output);
		}
	}

	// record the time
	output << endl << "SOS stopped on " << ctime(&theTime) << endl;
	
	// close input/output streams
	input.clear();  input.close();
	output.clear();	output.close();

	return 0;
} // END main()


void Run(ProcessCmd & cmds,SimpleOS & sos,fstream & output)
/******************************************************************************
Checks which command has to be processed according to cmds, checks for the 
correct number of parameters for the specified command and calls the
corresponding function of the sos.
******************************************************************************/
{
	switch(cmds.GetCommandNo())
	{
	case 0: // install: Install Disk 
	{
		// check for valid no. of parameters
		if(cmds.GetNumParams() != 2)
		{	// Error: Invalid Instruction
			output << ERROR[2] << endl; break;
		}
		// get the integer value of the string
		int diskSize = isValidNum(cmds.GetParam(1));
		// SimpleOS 'system call'
		sos.InstallDisk(cmds.GetParam(0),diskSize,output); break;
	}
	case 1: // format: Format  Disk
	{
		// check for valid no. of parameters
		if(cmds.GetNumParams() != 4)
		{	// Error: Invalid Instruction
			output << ERROR[2] << endl; break;
		}
		// get the integer values
		int fsSize   = isValidNum(cmds.GetParam(2));
		int maxFiles = isValidNum(cmds.GetParam(3));
		// SimpleOS 'system call'
		sos.FormatDisk(cmds.GetParam(0),cmds.GetParam(1),
					   fsSize,maxFiles,output);
		break;
	}
	case 2: // mount: Mount File System
	{
		// check for valid no. of parameters
		if(cmds.GetNumParams() != 2)
		{	// Error: Invalid Instruction
			output << ERROR[2] << endl; break;
		}
		// SimpleOS 'system call'
		sos.MountDisk(cmds.GetParam(0),cmds.GetParam(1),output); break;
	}
	case 3: // showmounts: Show Mounted File Systems
	{
		// check for valid no. of parameters
		if(cmds.GetNumParams() != 0)
		{	// Error: Invalid Instruction
			output << ERROR[2] << endl; break;
		}
		// SimpleOS 'system call'
		sos.ShowMounts(output);  break;
	}
	case 4: // cfs: Change File System
	{
		// check for valid no. of parameters
		if(cmds.GetNumParams() != 1)
		{	// Error: Invalid Instruction
			output << ERROR[2] << endl; break;
		}
		// SimpleOS 'system call'
		sos.ChangeFs(cmds.GetParam(0),output); break;
	}
	case 5: // dumpfs: Dump File System Contents
	{	
		// check for valid no. of parameters
		if(cmds.GetNumParams() != 2)
		{	// Error: Invalid Instruction
			output << ERROR[2] << endl; break;
		}
		// get integer values of parameters
		int startByte = isValidNum(cmds.GetParam(0));
		int numBytes  = isValidNum(cmds.GetParam(1));
		// SimpleOS 'system call'
		sos.DumpFs(startByte,numBytes,output); break;
	}
	case 6: // showfs: Show Current File System Info
	{
		// check for valid no. of parameters
		if(cmds.GetNumParams() != 0)
		{	// Error: Invalid Instruction
			output << ERROR[2] << endl; break;
		}
		// SimpleOS 'system call'
		sos.ShowFs(output); break;
	}
	case 7: // unmount: Unmount A File System
	{
		// check for valid no. of parameters
		if(cmds.GetNumParams() != 1)
		{
			// Error: Invalid Instruction
			output << ERROR[2] << endl; break;
		}
		// SimpleOS 'system call'
		sos.UnmountDisk(cmds.GetParam(0),output); break;
	}
	case 8:
	{
		if(cmds.GetNumParams() != 1)
		{
			output << ERROR[2] << endl; break;
		}
		sos.ImportFile(cmds.GetParam(0),output); break;
	}
	case 9:
	{
		if(cmds.GetNumParams() != 2)
		{
			output << ERROR[2] << endl; break;
		}
		sos.CpFile(cmds.GetParam(0),cmds.GetParam(1),output);
		break;
	}
	case 10:
	{
		if(cmds.GetNumParams() != 2)
		{
			output << ERROR[2] << endl; break;
		}
		sos.RenameFile(cmds.GetParam(0),cmds.GetParam(1),output);
		break;
	}
	case 11:
	{
		if(cmds.GetNumParams() != 0)
		{
			output << ERROR[2] << endl; break;
		}

		sos.ListFiles(output); break;
	}
	case 12:
	{
		if(cmds.GetNumParams() != 2)
		{
			output << ERROR[2] << endl; break;
		}
		int newOwner = isValidNum(cmds.GetParam(1));
		sos.ChgOwner(cmds.GetParam(0),newOwner,output); break;
	}
	case 13:
	{
		if(cmds.GetNumParams() != 2)
		{
			output << ERROR[2] << endl; break;
		}
		sos.ChangePerm(cmds.GetParam(0),cmds.GetParam(1),output);
		break;
	}
	case 14:
	{
		if(cmds.GetNumParams() != 1)
		{
			output << ERROR[2] << endl; break;
		}
		sos.ExportFile(cmds.GetParam(0),output); break;
	}
	// if it got here, the command is invalid
	// Error: Invalid Instruction
	default: output << ERROR[2] << endl;
	}
}
