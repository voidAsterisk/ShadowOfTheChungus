#pragma once
#include <vector>
#include <string>

class Console
{

public:
	bool Toggled = false;
	std::vector<std::string> Lines;
	std::string Buffer = "";

	Console();
	~Console();

	void Line(std::string line);
};