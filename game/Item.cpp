#include "pch.h"
#include "Item.h"
#include "HeldItem.h"
#include "Viewport.h"
#include "Player.h"
Item::Item()
{
}

Item::Item(std::vector<Entity*>* e, Viewport* viewport, Player* player, double x, double y, std::string name, std::string id) :
	Entity(e)
{
	p = player;
	v = viewport;
	X = x;
	Y = y;
	Name = name;
	Id = id;
	Respawn = false;
	IsSolid = false;
	sfx_pickup = Mix_LoadWAV("sfx/item.wav");
}

Item::~Item()
{
	Mix_FreeChunk(sfx_pickup);
}

void Item::Update(double dt)
{
	// If player touches item
	SDL_Rect pr = { p->X, p->Y, p->Width * SCALE, p->Height * SCALE };
	SDL_Rect r = { X, Y, Width * SCALE, Height * SCALE };

	if (SDL_HasIntersection(&pr, &r))
	{
		Alive = false;
		HeldItem it = HeldItem(Name, Id);
		p->AddItem(it, 1);
		Viewport::PostMessage(v, EntityListPointer, (std::string)"Obtained " + Name + "!");
		Mix_PlayChannel(-1, sfx_pickup, 0);
	}
}
