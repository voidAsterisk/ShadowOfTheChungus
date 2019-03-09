#pragma once
#include <vector>
#include <SDL_ttf.h>
#include "Entity.h"

class Viewport;
class ScreenMessage : public
	Entity
{
	Viewport* vp;
public:
	std::string Message;
	TTF_Font* Font;
	bool Background = true;
	bool Relative = false;
	bool DrawMiddle = false;
	ScreenMessage(Viewport* viewport, std::vector<Entity*>* ents, std::string msg, TTF_Font* font);

	ScreenMessage(Viewport* viewport, std::vector<Entity*>* ents, bool drawmid, bool bg, bool relative, int dieafter, double x, double y, std::string msg, TTF_Font* font);
	~ScreenMessage();

	virtual void Update(double dt);

	virtual void Draw(SDL_Renderer* ren);
};