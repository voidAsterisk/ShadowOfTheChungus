#include "pch.h"
#include "MinorHealth.h"

#include "Player.h"

MinorHealth::MinorHealth(Player* player, double x, double y, std::string name)
{
	p = player;
	X = x;
	Y = y;
	Name = name;
	IsSolid = false;
	sfx_pickup = Mix_LoadWAV("sfx/health.wav");
}


MinorHealth::~MinorHealth()
{
	Entity::~Entity();
	Mix_FreeChunk(sfx_pickup);
}

void MinorHealth::Update(double dt)
{
	// If players feet touches item
	SDL_Rect pr = { p->X, p->Y, p->Width * p->SCALE, p->Height * p->SCALE };
	SDL_Rect r = { X, Y, Width * SCALE, Height * SCALE };

	if (SDL_HasIntersection(&pr, &r) && p->Health != p->MaxHealth)
	{
		this->Alive = false;
		p->Health += 2;
		Mix_PlayChannel(-1, sfx_pickup, 0);
	}
}