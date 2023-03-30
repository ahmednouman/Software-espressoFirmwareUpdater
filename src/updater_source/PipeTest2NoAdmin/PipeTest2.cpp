// PipeTest2.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include <list>
#include <iostream>
#include <string>

#include "func.h"

using namespace std;


static void PrintCommandResults(list<list<pair<string, string>>> data) //example
{
	for (auto const& commandResult : data) {
		cout << "--command result--" << endl;
		cout << "--BEGIN--" << endl;
		for (auto const& item : commandResult) {
			cout << "name=" << item.first << " value=" << item.second << endl;
		}
		cout << "--END--" << endl;
	}
}



int main(int argc, char** argv)
{

	auto commandsList = argsParse(argc,argv);
	if (commandsList.size() == 0) return 0;
	

	auto commandResults=PipeWork(commandsList);

	//use commandResults for your needs
	//PrintCommandResults(commandResults);
	
	//example how to hardcode commands:

	//std::list<commandBase *> commandsList;
/*	commandsList.clear(); 
	commandsList.push_back(new commandFilesVersion());
	commandsList.push_back(new commandFirmwareVersion());
	commandResults = PipeWork(commandsList);
	PrintCommandResults(commandResults);
	*/
	return 0;

}
