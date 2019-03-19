#pragma once
#include <SDL.h>
#include <string>
class Trigger
{
public:
	SDL_Rect Rect;
	std::string FlagName;
	std::string NeedsItem;
	bool FlagValue;
	Trigger();
	~Trigger();
};

