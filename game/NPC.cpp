#include "pch.h"
#include "NPC.h"
#include "Viewport.h"
#include "Cursor.h"
#include "Player.h"
#include <iostream>
NPC::NPC(std::map<std::string, bool>* flags, std::vector<Entity*>* entities, Player* player, TTF_Font* font, Viewport* viewport, Cursor* cursor, double x, double y) :
	Entity(entities)
{
	f = flags;
	p = player;
	c = cursor;
	v = viewport;
	Font = font;
	id = "npc";
	X = x;
	Y = y;
	Width = 7;
	Height = 16;

	sfx_dialog = Mix_LoadWAV("sfx/dialog.wav");
}


NPC::~NPC()
{
	Mix_FreeChunk(sfx_dialog);
}

void NPC::OnClick()
{
	std::cout << (*f)["old_man_released"];
	if (AltDialog == "")
	{
		Viewport::PostMessage(v, EntityListPointer, "[" + Name + "] " + Dialog);
		Mix_PlayChannel(-1, sfx_dialog , 0);
	}
	if (AltDialogItem != "" && AltDialogFlag == "")
	{
		if (p->HasItem(AltDialogItem))
		{
			Viewport::PostMessage(v, EntityListPointer, "[" + Name + "] " + AltDialog);
			Mix_PlayChannel(-1, sfx_dialog, 0);
		}
		else
		{
			Viewport::PostMessage(v, EntityListPointer, "[" + Name + "] " + Dialog);
			Mix_PlayChannel(-1, sfx_dialog, 0);
		}
	}
	if (AltDialogFlag != "" && AltDialogItem == "")
	{
		if ((*f)[AltDialogFlag])
		{
			Viewport::PostMessage(v, EntityListPointer, "[" + Name + "] " + AltDialog);
			Mix_PlayChannel(-1, sfx_dialog, 0);
		}
		else
		{
			Viewport::PostMessage(v, EntityListPointer, "[" + Name + "] " + Dialog);
			Mix_PlayChannel(-1, sfx_dialog, 0);
		}
	}
}

void NPC::Draw(Viewport vp, SDL_Renderer* ren)
{
	Entity::Draw(vp, ren);

	// Draw text if cursor rect intersects self rect
	SDL_Rect curr = { c->X, c->Y, c->Width, c->Height };
	SDL_Rect r = { X + v->X, Y + v->Y, Width * SCALE, Height * SCALE };

	if (SDL_HasIntersection(&curr, &r))
	{
		// Draw level and name
		SDL_Surface * s = TTF_RenderText_Solid(Font,
			Name.c_str(), { 255, 255, 255 });
		int w = s->w;
		int h = s->h;
		SDL_Rect r = { X + v->X + Width * SCALE / 2 - w / 2, Y + v->Y - h * 1.5f, w, h };
		SDL_Texture* t = SDL_CreateTextureFromSurface(ren, s);
		SDL_FreeSurface(s);
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
		SDL_RenderFillRect(ren, &r);
		SDL_RenderCopy(ren, t, NULL, &r);
		SDL_DestroyTexture(t);
	}
}