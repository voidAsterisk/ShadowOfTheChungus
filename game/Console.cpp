#include "pch.h"
#include "Console.h"


Console::Console()
{
	Line("Welcome to game");
	Line("This is the console");
}


Console::~Console()
{
}

void Console::Line(std::string line)
{
	Lines.push_back(line);
}