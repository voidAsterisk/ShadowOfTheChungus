#include "pch.h"
#include "Solid.h"


Solid::Solid()
{
}

Solid::Solid(SDL_Rect* r)
{
	Rect.x = r->x;
	Rect.y = r->y;
	Rect.w = r->w;
	Rect.h = r->h;
}


Solid::~Solid()
{
}
