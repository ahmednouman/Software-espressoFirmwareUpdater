#include "stdafx.h"
#include "ChildProccess.h"
#include <windows.h>
#include <tchar.h>
#include "MyPipe.h"
#include "ChildSpecific.h"
#include "trim.cpp"

PROCESS_INFORMATION ChildProcess::pinfo;

void ChildProcess::CreateChildProcessTest()
{
	TCHAR cmdStr[] = _T("TestPipe3.exe");
	//TCHAR cmdStr[] = _T("UpgradeTool.exe /S /U"); //Run and upgrade

	STARTUPINFO startupInfo = { sizeof(startupInfo) };
	startupInfo.hStdError = MyPipe::hUpPipeWrite;
	startupInfo.hStdOutput = MyPipe::hUpPipeWrite;
	startupInfo.hStdInput = MyPipe::hDownPipeRead;
	startupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	startupInfo.wShowWindow = SW_SHOW;


	if (!CreateProcess(NULL, cmdStr, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &startupInfo, &pinfo))
	{
		//return -1;
		throw std::runtime_error("CreateProcess error");
	}

	//return 0;
}

static LPWSTR ConvertStringToLPWSTR(const std::string& instr)
{
	// Assumes std::string is encoded in the current Windows ANSI codepage
	int bufferlen = ::MultiByteToWideChar(CP_ACP, 0, instr.c_str(), instr.size(), NULL, 0);

	if (bufferlen == 0)
	{
		// Something went wrong. Perhaps, check GetLastError() and log.
		return 0;
	}

	// Allocate new LPWSTR - must deallocate it later
	LPWSTR widestr = new WCHAR[bufferlen + 1];

	::MultiByteToWideChar(CP_ACP, 0, instr.c_str(), instr.size(), widestr, bufferlen);

	// Ensure wide string is null terminated
	widestr[bufferlen] = 0;

	// Do something with widestr
	return widestr;
	//delete[] widestr;
}

void ChildProcess::CreateChildProcessReal()
{
	//TCHAR cmdStr[] = _T(CHILD_EXECUTABLE_S);
	//TCHAR cmdStr[] = _T("UpgradeTool.exe /S /U"); //Run and upgrade
	string c = CHILD_EXECUTABLE_FILENAME + string(" /S");

	//auto cmdStr= ConvertStringToLPWSTR(c);
	//auto cmdStr = ConvertStringToLPSTR(c);
	auto cmdStr = const_cast<char *>(c.c_str());
	STARTUPINFO startupInfo = { sizeof(startupInfo) };
	startupInfo.hStdError = MyPipe::hUpPipeWrite;
	startupInfo.hStdOutput = MyPipe::hUpPipeWrite;
	startupInfo.hStdInput = MyPipe::hDownPipeRead;
	startupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	startupInfo.wShowWindow = SW_HIDE;


	if (!CreateProcess(NULL, cmdStr, NULL, NULL, TRUE, NULL, NULL, NULL, &startupInfo, &pinfo))
	{
		//return -1;
		throw std::runtime_error("CreateProcess error");
	}

	//return 0;
}


void ChildProcess::TerminateChild()
{
	TerminateProcess(pinfo.hProcess, 0);

	// 500 ms timeout; use INFINITE for no timeout
	const DWORD result = WaitForSingleObject(pinfo.hProcess, 500);
	if (result == WAIT_OBJECT_0) {
		// Success
	}
	else {
		// Timed out or an error occurred
	}

	if (pinfo.hProcess != INVALID_HANDLE_VALUE) CloseHandle(pinfo.hProcess);
	if (pinfo.hThread != INVALID_HANDLE_VALUE) CloseHandle(pinfo.hThread);
}

