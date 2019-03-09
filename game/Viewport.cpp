#include "pch.h"
#include "Viewport.h"

#include "ScreenMessage.h"

Viewport::Viewport()
{

}
Viewport::Viewport(int x, int y, int w, int h)
{
	X = x;
	Y = y;
	Width = w;
	Height = h;
}


Viewport::~Viewport()
{
}

void Viewport::PostMessage(Viewport* viewport, std::vector<Entity*>* ents, std::string msg)
{
	ScreenMessage* m = new ScreenMessage(viewport, ents, msg, msgfont);
	ents->push_back(m);
}
void Viewport::PostMessage(Viewport* viewport, std::vector<Entity*>* ents, bool drawmid, bool bg, bool relative, int dieafter, double x, double y, std::string msg)
{
	ScreenMessage* m = new ScreenMessage(viewport, ents, drawmid, bg, relative, dieafter, x, y, msg, msgfont);
	ents->push_back(m);
}