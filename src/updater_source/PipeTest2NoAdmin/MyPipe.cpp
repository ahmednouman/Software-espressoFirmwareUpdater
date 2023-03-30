#include "stdafx.h"
#include "MyPipe.h"
#include <windows.h>
#include <iostream>
#include "variables.h"

HANDLE MyPipe::hUpPipeRead = INVALID_HANDLE_VALUE;
HANDLE MyPipe::hUpPipeWrite = INVALID_HANDLE_VALUE;
HANDLE MyPipe::hDownPipeRead = INVALID_HANDLE_VALUE;
HANDLE MyPipe::hDownPipeWrite = INVALID_HANDLE_VALUE;



	void MyPipe::PipeSend(string s)
	{
		DWORD byteWrite;

		WriteFile(hDownPipeWrite, s.c_str(), s.length(), &byteWrite, NULL);
	}



	void MyPipe::PipeInit()
	{
		/*
		hUpPipeRead = INVALID_HANDLE_VALUE;
		hUpPipeWrite = INVALID_HANDLE_VALUE;
		hDownPipeRead = INVALID_HANDLE_VALUE;
		hDownPipeWrite = INVALID_HANDLE_VALUE;  
		*/
		ExitHandle();

		SECURITY_ATTRIBUTES safety;
		safety.nLength = sizeof(SECURITY_ATTRIBUTES);
		safety.lpSecurityDescriptor = NULL;
		safety.bInheritHandle = TRUE;
		if (!CreatePipe(&hUpPipeRead, &hUpPipeWrite, &safety, 0))
		{
			//return -1;
			throw std::runtime_error("CreatePipe error");
		}


		if (!CreatePipe(&hDownPipeRead, &hDownPipeWrite, &safety, 0))
		{
			//return -1;
			throw std::runtime_error("CreatePipe error");
		}



//		return 0;
	}


	void MyPipe::ExitHandle()
	{
		/*if (!TerminateProcess(pinfo.hProcess, 0))
		{
		printf("TerminateProcess fail\n");
		}*/

		if (hUpPipeRead != INVALID_HANDLE_VALUE) {
			CloseHandle(hUpPipeRead);
			hUpPipeRead = INVALID_HANDLE_VALUE;
		}
		if (hUpPipeWrite != INVALID_HANDLE_VALUE) {
			CloseHandle(hUpPipeWrite);
			hUpPipeWrite = INVALID_HANDLE_VALUE;
		}
		if (hDownPipeRead != INVALID_HANDLE_VALUE) {
			CloseHandle(hDownPipeRead);
			hDownPipeRead = INVALID_HANDLE_VALUE;
		}
		if (hDownPipeWrite != INVALID_HANDLE_VALUE) {
			CloseHandle(hDownPipeWrite);
			hDownPipeWrite = INVALID_HANDLE_VALUE;
		}

		//if (pinfo.hProcess != INVALID_HANDLE_VALUE) CloseHandle(pinfo.hProcess);
		//if (pinfo.hThread != INVALID_HANDLE_VALUE) CloseHandle(pinfo.hThread);
	}

	string MyPipe::PipeReadAllUntilSymbolReady()
	{
		string accumulator;

		char buf[1024];
		unsigned long byteRead;

		while (true)
		{
			if (ReadFile(hUpPipeRead, buf, 1023, &byteRead, NULL) == FALSE)
			{
				throw std::runtime_error("ReadFile error");
			}
			if (byteRead == 0)
				continue;

			buf[byteRead] = 0;

			string piece = string(buf);




			accumulator = accumulator + piece;
			if (RAW_OUTPUT)
			{
				std::cout << piece  << endl;
			} else
			if (TEST_MODE)
			{
				std::cout << "program raw piece of response:" << endl;
				std::cout << piece << "END" << endl;
			}



			if (accumulator.length() >= 2)
			{
				if (accumulator.substr(accumulator.length() - 2, 2) == ">>")
				{

					break;
				}

			}
		}

		return accumulator;
	}


	list<pair<string, string>> MyPipe::PipeReadAllUntilSymbolReadyParse(commandBase* command)
	{

		char buf[1024];
		unsigned long byteRead;
		list<pair<string, string>>  data;
		while (true)
		{
			if (ReadFile(hUpPipeRead, buf, 1023, &byteRead, NULL) == FALSE)
			{
				throw std::runtime_error("ReadFile error");
			}
			if (byteRead == 0)
				continue;

			buf[byteRead] = 0;

			string piece = string(buf);

			if (RAW_OUTPUT)
			{
				std::cout << piece  << endl;
			} else
			if (TEST_MODE)
			{
				std::cout << "program raw piece of response:" << endl;
				std::cout << piece << "END" << endl;
			}


			if (command != NULL && !RAW_OUTPUT)
			{
				auto data_piece=command->parse(piece);
				for (auto const& item : data_piece) {
					data.push_back(item);
				}
			}

			if (piece.length() >= 2)
			{
				if (piece.substr(piece.length() - 2, 2) == ">>")
				{
					break;
				}

			}
		}

		return data;
	}

	string MyPipe::PipeReadAll()
	{

		char buf[1024];
		unsigned long byteRead;



		{
			if (ReadFile(hUpPipeRead, buf, 1023, &byteRead, NULL) == FALSE)
			{
				throw std::runtime_error("ReadFile error");
			}


			buf[byteRead] = 0;


			string piece = string(buf);
			//std::cout << "Read from pipe:" << piece;

			if (RAW_OUTPUT)
			{
				std::cout << piece  << endl;
			}
			else
			if (TEST_MODE)
			{
				std::cout << "program raw piece of response:" << endl;
				std::cout << piece  << endl;
			}
			return piece;

		}

	}

