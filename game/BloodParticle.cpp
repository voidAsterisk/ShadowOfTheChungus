#include "pch.h"
#include "BloodParticle.h"

#include "Viewport.h"

BloodParticle::BloodParticle(Viewport* vp, int x, int y)
{
	viewport = vp;
	X = x;
	Y = y;
	DeathTime = SDL_GetTicks() + rand() % 15000;
	Fade = true;
	IsSolid = false;
}


BloodParticle::~BloodParticle()
{
}

void BloodParticle::Draw(SDL_Renderer* ren)
{
	SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
	SDL_RenderDrawPoint(ren, X + viewport->X, Y + viewport->Y);
}
