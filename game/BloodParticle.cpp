#include "pch.h"
#include "BloodParticle.h"

#include "Viewport.h"

BloodParticle::BloodParticle(int x, int y)
{
	X = x;
	Y = y;
	DeathTime = SDL_GetTicks() + rand() % 15000;
	Fade = true;
	IsSolid = false;
}


BloodParticle::~BloodParticle()
{
}

void BloodParticle::Draw(Viewport v, SDL_Renderer* ren)
{
	SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
	SDL_RenderDrawPoint(ren, X + v.X, Y + v.Y);
}
