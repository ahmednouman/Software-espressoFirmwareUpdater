#pragma once

#include <windows.h>
#include <string>
#include <list>
#include "commands.h"
using namespace std;

class MyPipe
{
public:
	static HANDLE hUpPipeRead;
	static HANDLE hUpPipeWrite;
	static HANDLE hDownPipeRead;
	static HANDLE hDownPipeWrite;
public:


	static void PipeInit();
	static void PipeSend(string s);

	static void ExitHandle();


	static string PipeReadAllUntilSymbolReady();
	static list<pair<string, string>> PipeReadAllUntilSymbolReadyParse(commandBase* command);
	static string PipeReadAll();

};