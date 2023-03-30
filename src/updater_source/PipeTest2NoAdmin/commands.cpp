#include "stdafx.h"
#include "commands.h"
#include <string>
#include <stdio.h>
#include <windows.h>
#include <sstream>
#include <iostream>
#include "trim.cpp"
#include "variables.h"
#include "MyPipe.h"

using namespace std;

bool ContainsError(string s)
{
	if (s.find("error") != string::npos) return true;
	if (s.find("Error") != string::npos) return true;
	if (s.find("ERROR") != string::npos) return true;
	if (s.find("Fail") != string::npos) return true;
	if (s.find("fail") != string::npos) return true;
	if (s.find("FAIL") != string::npos) return true;

	return false;
}



list<pair<string, string>>  commandBase::parse(string s)
	{
		list<pair<string, string>> data;
		std::stringstream s_stream(s);
		string line;
		
		while (std::getline(s_stream, line, '\n')) {
			if (line.find("*****") != string::npos) {
				pass_next = false;
				continue;
			}

			if (line.find("Upgrading...") != string::npos) {
				pass_next = true;
				continue;
			}
			if (pass_next)
			{
				pass_next = false;
				cout << line << endl;
				if (ContainsError(line))
					
					data.push_back(pair<string, string>("error", line));
				else
					data.push_back(pair<string, string>("message",line));

				continue;
			}
			if (ContainsError(line) && DISPLAY_APP_ERRORS)
			{
				cout << line << endl;
				data.push_back(pair<string, string>("error", line));
				continue;
			}

			auto pos_space = line.find(": ");
			auto pos_tab = line.find(":\t");


			auto pos = string::npos;
			if (pos_space != string::npos)
				pos = pos_space;
			if (pos_tab != string::npos)
				pos = pos_tab;
			if (pos == string::npos) continue;

			if (pos == 0) continue;
			string proto_name = line.substr(0, pos);
			string proto_value = line.substr(pos + 2);

			string name = trim_copy(proto_name);
			string value = trim_copy(proto_value);

			if (name.length() == 0) continue;
			if (value.length() == 0) continue;
			if (value == "Unavailable") continue;
			//if (value == "Unknow") continue;
			std::replace(name.begin(), name.end(), ' ', '_'); // replace all 'x' to 'y'
			string result_line = name + "=" + value;
			cout << result_line << endl;
			data.push_back(pair<string, string>(name, value));
		}
		return data;

	}

	void commandStartup::execute()
	{

	}


	list<pair<string, string>>  commandStartup::parse(string s)
	{
		if (SHOW_STARTUP_MESSGAES)
		{
			return commandBase::parse(s);
		}
		std::stringstream s_stream(s);
		string line;
		list<pair<string, string>> data;
		while (std::getline(s_stream, line, '\n')) {

			if (ContainsError(line) && DISPLAY_APP_ERRORS)
			{
				cout << line << endl;
				data.push_back(pair<string, string>("message", line));
				continue;
			}

		}
		return data;

	}
	void commandFirmwareVersion::execute()
	{
		if (TEST_MODE)
			std::cout << "sent v" << endl;
		MyPipe::PipeSend("v\n");
	}
	/*
	void parse(string s) override
	{
	std::cout << "v parses result:" << endl << s << endl << "end" << endl;
	} */



	void commandFirmwareUpdate::execute()
	{
		if (TEST_MODE)
			std::cout << "sent u" << endl;
		MyPipe::PipeSend("u\n");
	}

	/*
	void parse(string s) override
	{
	std::cout << "u parses result:" << endl << s << endl << "end" << endl;
	} */


	void commandFilesVersion::execute()
	{
		if (TEST_MODE)
			std::cout << "sent f" << endl;
		MyPipe::PipeSend("f\n");
	}
	/*
	void parse(string s) override
	{
	std::cout << "f parses result:" << endl << s << endl << "end" << endl;
	} */
