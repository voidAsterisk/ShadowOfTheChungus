#include "pch.h"
#include "Warp.h"


Warp::Warp(int warpid, int targetid, std::string targetlevel, SDL_Rect r)
{
	WarpID = warpid;
	TargetWarpID = targetid;
	TargetLevel = targetlevel;
	Rect = r;
}


Warp::~Warp()
{
}
