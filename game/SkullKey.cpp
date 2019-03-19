#include "pch.h"
#include "SkullKey.h"

#include "MouseState.h"
#include "Player.h"
#include "Viewport.h"
SkullKey::SkullKey(std::vector<Entity*>* entities, MouseState* currentMouseState, MouseState* previousMouseState, Player* player, std::map<std::string, bool>* flags, Viewport* vp) :
	Entity(entities)
{
	Width = 16;
	Height = 18;

	c_ms = currentMouseState;
	p_ms = previousMouseState;
	p = player;
	f = flags;
	v = vp;
}


void SkullKey::Update(double dt)
{
	SDL_Point pp = { c_ms->X - v->X, c_ms->Y - v->Y };
	SDL_Rect r = { X, Y, Width * SCALE, Height * SCALE };
	if (p_ms->LeftDown == false &&
		c_ms->LeftDown == true &&
		p->GetSelectedItemId() == "holy_grail" &&
		(*f)["holy_grail_full"] == true)
	{
		(*f)["holy_grail_full"] = false;
		(*f)["portrait_open"] = true;
	}
}

void SkullKey::Draw(Viewport viewport, SDL_Renderer* ren)
{
	Entity::Draw(viewport, ren);
}