#include "pch.h"
#include "ScreenMessage.h"


ScreenMessage::ScreenMessage(std::vector<Entity*>* ents, std::string msg, TTF_Font* font)
{
	Entity::Entity(ents);

	Message = msg;
	DeathTime = SDL_GetTicks() + 5000;
	X = 50;
	Y = viewport.Height - 50;
	Font = font;
	Fade = true;
	Z = 1;
}
ScreenMessage::ScreenMessage(std::vector<Entity*>* ents, bool drawmid, bool bg, bool relative, int dieafter, double x, double y, std::string msg, TTF_Font* font)
{
	Entity::Entity(ents);

	Message = msg;
	DeathTime = SDL_GetTicks() + dieafter;
	X = x;
	Y = y;
	Font = font;
	Fade = true;
	Background = bg;
	DrawMiddle = drawmid;
	Relative = relative;
}


ScreenMessage::~ScreenMessage()
{
}
