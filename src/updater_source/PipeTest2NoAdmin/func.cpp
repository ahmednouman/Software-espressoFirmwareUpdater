#include "stdafx.h"
#include "func.h"
#include <stdio.h>
#include <windows.h>
#include <string>
#include <tchar.h>
#include <tlhelp32.h>
#include <iostream>
//#include <sstream>
#include <list>

#include "MyPipe.h"
#include "variables.h"
#include "commands.h"
#include "ChildProccess.h"
#include "ChildSpecific.h"
#include "trim.cpp"

static void ExecuteCmd(LPSTR command, INT wait_timeout)
{
	/*WCHAR strCmd[MAX_PATH];
	lstrcpyW(strCmd, command);*/

	STARTUPINFO startupInfo = { sizeof(startupInfo) };
	//startupInfo.dwFlags =STARTF_USESTDHANDLES;
	startupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	startupInfo.wShowWindow = SW_HIDE;
	if (TEST_MODE)
		startupInfo.wShowWindow = SW_SHOW;
	PROCESS_INFORMATION pinfo;
	if (!CreateProcess(NULL, command, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startupInfo, &pinfo))
	{
		//return -1;
		throw std::runtime_error("CreateProccess error (ExecuteCmd)");
	}
	//WaitForInputIdle(pinfo.hProcess, wait_timeout);
	WaitForSingleObject(pinfo.hProcess, wait_timeout);

	CloseHandle(pinfo.hProcess);
	CloseHandle(pinfo.hThread);
	//return 0;
}

std::list<commandBase *> argsParse(int argc, char** argv)
{
	std::list<commandBase *> commandsList;
	if (argc <2)
	{
		std::cout << "Console firmware tool. \n Usage: \n consoleupgrade.exe [command1] [command2] [command3] [option1] [option2] [option3] [custom_exe_file]  \n \n Use arguments: \n f - command get file versions \n v - command get device firmware versions \n u - command update firmware \n hideerrors - option add this option to disable error messages displaying. \n showstartmessages - option display all application output before commands execution (hidden by default). \n raw - option display all appication output, no filtration, no parsing.\n \n example: \n consoleupgrade.exe v \n consoleupgrade.exe u hideerrors \n \n custom exe file to control: \n consoleupgrade.exe u customfilename.exe \n ";
		return commandsList;
	}

	int errorsCount = 0;
	for (int i = 1; i < argc; i++)
	{
		string arg = string(argv[i]);

		if (arg == "f")
		{
			commandsList.push_back(new commandFilesVersion());
		}
		else
			if (arg == "v")
			{
				commandsList.push_back(new commandFirmwareVersion());
			}
			else
				if (arg == "u")
				{
					commandsList.push_back(new commandFirmwareUpdate());
				}
				else
					if (arg == "hideerrors")
					{
						DISPLAY_APP_ERRORS = false;
					}
					else
						if (arg == "raw")
						{
							RAW_OUTPUT = true;
						}
						else
							if (arg == "showstartmessages")
							{
								SHOW_STARTUP_MESSGAES = true;
							}
							else
								if (arg == "testchildexe")
								{
									TEST_CHILD_EXE = true;
								}
								else
									if (arg == "testmode")
									{
										TEST_MODE = true;
									}
									else
									if (arg.find('.') != string::npos)
									{
										CHILD_EXECUTABLE_FILENAME = arg; //custom exe file
									} else
									{
										std::cout << "Option " << arg << " not supporded." << endl;
										errorsCount++;
									}
	}
	if (errorsCount > 0)
		commandsList.clear();
	return commandsList;
}


static list<list<pair<string, string>>> ExecuteCommandsList(std::list<commandBase *> commandsList)
{
	list<list<pair<string,string>>> commandResults;

	MyPipe::PipeReadAllUntilSymbolReadyParse(new commandStartup());

	for (auto command : commandsList) {
		try
		{
			command->execute();

			auto currentCommandResult=MyPipe::PipeReadAllUntilSymbolReadyParse(command);

			commandResults.push_back(currentCommandResult);

		}
		catch (const std::exception& e)
		{
			std::cout << " error '"
				<< e.what() << "'\n";
		}
	}
	return commandResults;
}




static DWORD FindProcessId(const std::string& processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(processesSnapshot, &processInfo);
	if (!processName.compare(processInfo.szExeFile))
	{
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processesSnapshot, &processInfo))
	{
		if (!processName.compare(processInfo.szExeFile))
		{
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processesSnapshot);
	return 0;
}


list<list<pair<string, string>>>  PipeWork(std::list<commandBase *> commandsList)
{
	unsigned long dwID;

	string taskkill1 = string("taskkill /F /IM ") + string(CHILD_EXECUTABLE_FILENAME);

	list<list<pair<string, string>>> commandResults;

	while (auto pid = FindProcessId((CHILD_EXECUTABLE_FILENAME)))
	{


		if (TEST_MODE)
		{
			cout << "proccess " << CHILD_EXECUTABLE_FILENAME << "already running" << endl;
			cout << "Executing external command: " << taskkill1 << endl;

		}
		//auto wtaskkill = widen(taskkill1);
		auto taskkill=const_cast<char *>(taskkill1.c_str());
		ExecuteCmd(taskkill, 20000);
	}

	try
	{


		MyPipe::PipeInit();

		if (/*TEST_MODE ||*/ TEST_CHILD_EXE)
			ChildProcess::CreateChildProcessTest();
		else
			ChildProcess::CreateChildProcessReal();

		commandResults=ExecuteCommandsList(commandsList);



	}
	catch (const std::exception& e)
	{
		std::cout << " error '"
			<< e.what() << "'\n";
	}

	ChildProcess::TerminateChild();
	MyPipe::ExitHandle();
	return commandResults;
}
