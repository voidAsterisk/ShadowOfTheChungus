#include "pch.h"
#include "Gate.h"

#include "Viewport.h"
#include "MouseState.h"
#include "Player.h"
#include "Entity.h"
#include "HeldItem.h"
#include <iostream>
Gate::Gate(std::map<std::string, bool>* flags, std::vector<Entity*>* entities, Viewport* viewport, Player* player, MouseState* currentmousestate, MouseState* previousmousestate, double x, double y, double w, double h)
{
	f = flags;
	e = entities;
	v = viewport;
	p = player;
	c_ms = currentmousestate;
	p_ms = previousmousestate;
	Rect = { (int)x, (int)y, (int)w, (int)h };
	Width = w;
	Height = h;
	X = x;
	Y = y;
	SCALE = 1;
	sfx_unlock = Mix_LoadWAV("sfx/unlock.wav");
}


Gate::~Gate()
{
	Mix_FreeChunk(sfx_unlock);
}

void Gate::Update(double dt)
{
	SDL_Point pt = { c_ms->X - v->X, c_ms->Y - v->Y };
	if (SDL_PointInRect(&pt, &Rect))
	{
		if (c_ms->LeftDown == true && p_ms->LeftDown == false)
		{
			if (p->Inventory.size() > 0)
			{
				if (NeededItem == p->Inventory[p->SelectedItem].Id)
				{
					Alive = false;
					p->RemoveItem(NeededItem);
					Mix_PlayChannel(-1, sfx_unlock, 0);
					if (Flag != "")
						(*f)[Flag] = true;
				}
				else
					Viewport::PostMessage(v, e, "Invalid key!");
			}
			else
			{
				Viewport::PostMessage(v, e, "You need a key!");
			}
		}
	}
}
void Gate::Draw(Viewport viewport, SDL_Renderer* ren)
{
	SDL_Rect r = { X + viewport.X, Y + viewport.Y, Width * SCALE, Height * SCALE };
	SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(ren, 255, 0, 0, 64);
	SDL_RenderFillRect(ren, &r);
}
