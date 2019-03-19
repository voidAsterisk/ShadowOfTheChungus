#include "pch.h"
#include "Fireball.h"

#include "Player.h"
#include "Solid.h"
#include "Viewport.h"

Fireball::Fireball(std::vector<Solid>* solids, Player* player, SDL_Renderer* renderer, std::vector<Entity*>* ents, double x, double y, double destX, double destY)
	: Entity(ents)
{
	s = solids;
	p = player;
	r = renderer;
	e = ents;
	X = x;
	Y = y;
	DestX = destX;
	DestY = destY;
	MaxVelocity = 0.18f;

	Width = 8;
	Height = 8;

	IsSolid = false;
	sfx_flame = Mix_LoadWAV("sfx/flame.wav");
	Mix_PlayChannel(-1, sfx_flame, 0);
}


Fireball::~Fireball()
{
	Entity::~Entity();
	Mix_FreeChunk(sfx_flame);
}

void Fireball::Update(double dt)
{
	double dx = DestX - X;
	double dy = DestY - Y;

	float length = sqrtf(dx*dx + dy * dy);

	if (length <= MaxVelocity * dt)
	{
		Alive = false;

		// Spawn shot decal
		Entity* d = new Entity(e);
		d->X = X;
		d->Y = Y;
		d->Width = 5;
		d->Height = 4;
		d->Fade = true;
		d->LoadImage(r, "gfx/" + (std::string)"shotdecal.bmp");
		d->IsSolid = false;
		d->DeathTime = SDL_GetTicks() + 10000;
		(*e).push_back(d);
		return;
	}

	dx /= length;
	dy /= length;
	X += dx * dt * MaxVelocity;
	Y += dy * dt * MaxVelocity;

	// If collides with player
	SDL_Rect pr = { p->X, p->Y, p->Width*SCALE, p->Height*SCALE };
	SDL_Rect r = { X, Y, Width *SCALE, Height * SCALE };

	if (SDL_HasIntersection(&pr, &r))
	{
		Alive = false;
		p->Health -= AttackDamage;
	}

	// Check for collisions against solids
	for (unsigned int i = 0; i < s->size(); i++)
	{
		if (SDL_HasIntersection(&r, &((*s)[i].Rect)))
		{
			Alive = false;
				return;
		}
	}
}