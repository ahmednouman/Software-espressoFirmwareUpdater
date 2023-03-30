#pragma once
#include <string>
#include <list>

using namespace std;

struct commandBase
{
	void virtual execute()=0;
	bool pass_next = false;
	list<pair<string,string>> virtual parse(string s);

};

struct commandStartup : public commandBase
{
	void execute() override;
	list<pair<string, string>> parse(string s) override;
};


struct commandFirmwareVersion : commandBase
{
	void execute() override;
};

struct commandFirmwareUpdate : commandBase
{
	void execute() override;

};


struct commandFilesVersion : commandBase
{
	void execute() override;

};