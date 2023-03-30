#pragma once

#include <windows.h>

class ChildProcess
{
private:
	static PROCESS_INFORMATION pinfo;
public:
	

	static void CreateChildProcessTest();
	static void CreateChildProcessReal();
	static void TerminateChild();
};