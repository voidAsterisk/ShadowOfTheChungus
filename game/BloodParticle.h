#pragma once
#include <SDL.h>
#include "Entity.h"

class Viewport;

class BloodParticle : public
	Entity
{
	Viewport* viewport;
public:
	BloodParticle(Viewport* vp, int x, int y);
	~BloodParticle();

	virtual void Draw(SDL_Renderer* ren);
};