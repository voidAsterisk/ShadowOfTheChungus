#include "pch.h"
#include "MaximumHealth.h"
#include "Player.h"

MaximumHealth::MaximumHealth(Player* player, double x, double y, std::string name)
{
	p = player;
	X = x;
	Y = y;
	Name = name;
	IsSolid = false;
	sfx_pickup = Mix_LoadWAV("sfx/health.wav");
}

MaximumHealth::~MaximumHealth()
{
	Entity::~Entity();
	Mix_FreeChunk(sfx_pickup);
}

void MaximumHealth::Update(double dt)
{
	// If players feet touches item
	SDL_Rect pr = { p->X, p->Y, p->Width * p->SCALE, p->Height * p->SCALE };
	SDL_Rect r = { X, Y, Width * SCALE, Height * SCALE };

	if (SDL_HasIntersection(&pr, &r) && p->Health != p->MaxHealth)
	{
		this->Alive = false;
		p->Health = p->MaxHealth;
		Mix_PlayChannel(-1, sfx_pickup, 0);
	}
}