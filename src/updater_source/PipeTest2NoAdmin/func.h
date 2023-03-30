#pragma once
#include <string>
#include <list>
#include "commands.h"

using namespace std;

list<list<pair<string, string>>>  PipeWork(std::list<commandBase *> commandsList);
std::list<commandBase *> argsParse(int argc, char** argv);