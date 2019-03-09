#pragma once
#include <SDL.h>
#include <string>

class Solid
{
public:
	SDL_Rect Rect;
	std::string NeedsKey;
	Solid();
	Solid(SDL_Rect* r);

	~Solid();
};