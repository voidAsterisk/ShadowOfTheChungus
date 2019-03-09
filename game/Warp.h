#pragma once
#include <string>
#include <SDL.h>
class Warp
{
public:
	int WarpID;
	int TargetWarpID;
	std::string TargetLevel;
	SDL_Rect Rect;

	Warp(int warpid, int targetid, std::string targetlevel, SDL_Rect r);
	~Warp();
};