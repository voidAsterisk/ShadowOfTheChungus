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
	std::string TakeItems = "none";
	ScreenMessage(Viewport* viewport, std::vector<Entity*>* ents, std::string msg);

	ScreenMessage(Viewport* viewport, std::vector<Entity*>* ents, bool drawmid, bool bg, bool relative, int dieafter, double x, double y, std::string msg);
	~ScreenMessage();

	virtual void Update(double dt);

	virtual void Draw(Viewport viewport, SDL_Renderer* ren);
};