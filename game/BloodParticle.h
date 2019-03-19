#pragma once
#include <SDL.h>
#include "Entity.h"

class Viewport;

class BloodParticle : public
	Entity
{
public:
	BloodParticle(int x, int y);
	~BloodParticle();

	virtual void Draw(Viewport v, SDL_Renderer* ren);
};