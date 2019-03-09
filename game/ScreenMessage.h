#pragma once
#include <vector>
#include "Entity.h"
#include "Vars.h"
class ScreenMessage : public
	Entity
{
public:
	std::string Message;
	TTF_Font* Font;
	bool Background = true;
	bool Relative = false;
	bool DrawMiddle = false;
	ScreenMessage(std::vector<Entity*>* ents, std::string msg, TTF_Font* font);

	ScreenMessage(std::vector<Entity*>* ents, bool drawmid, bool bg, bool relative, int dieafter, double x, double y, std::string msg, TTF_Font* font);
	~ScreenMessage();

	virtual void Update(double dt)
	{
		Entity::Update(dt);
		Y -= 0.01f * dt;
	}

	virtual void Draw(SDL_Renderer* ren)
	{
		SDL_Surface * s = TTF_RenderText_Solid(Font,
			Message.c_str(), { 255, 255, 255 });
		int w = s->w;
		int h = s->h;
		SDL_Rect expr;
		double x, y;
		if (Relative)
		{
			x = X + viewport.X;
			y = Y + viewport.Y;
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
};