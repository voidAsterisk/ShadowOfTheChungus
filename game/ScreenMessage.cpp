#include "pch.h"
#include "ScreenMessage.h"

#include "Viewport.h"

ScreenMessage::ScreenMessage(Viewport* viewport, std::vector<Entity*>* ents, std::string msg, TTF_Font* font)
{
	Entity::Entity(ents);
	vp = viewport;
	Message = msg;
	DeathTime = SDL_GetTicks() + 5000;
	X = 50;
	Y = vp->Height - 50;
	Font = font;
	Fade = true;
	Z = 1;
}
ScreenMessage::ScreenMessage(Viewport* viewport, std::vector<Entity*>* ents, bool drawmid, bool bg, bool relative, int dieafter, double x, double y, std::string msg, TTF_Font* font)
{
	Entity::Entity(ents);
	vp = viewport;
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

void ScreenMessage::Update(double dt)
{
	Entity::Update(dt);
	Y -= 0.01f * dt;
}

void ScreenMessage::Draw(SDL_Renderer* ren)
{
	SDL_Surface * s = TTF_RenderText_Solid(Font,
		Message.c_str(), { 255, 255, 255 });
	int w = s->w;
	int h = s->h;
	SDL_Rect expr;
	double x, y;
	if (Relative)
	{
		x = X + vp->X;
		y = Y + vp->Y;
	}
	else
	{
		x = X;
		y = Y;
	}
	if (DrawMiddle)
	{
		x -= s->w / 2;
		y -= s->h / 2;
	}
	expr = { (int)x, (int)y, w, h };
	SDL_Texture* t = SDL_CreateTextureFromSurface(ren, s);
	SDL_FreeSurface(s);
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	if (Background) {
		SDL_RenderFillRect(ren, &expr);
		SDL_RenderCopy(ren, t, NULL, &expr);
	}
	SDL_DestroyTexture(t);
}