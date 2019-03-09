#pragma once
#include <vector>
#include "Fonts.h"
class Entity;
class ScreenMessage;

class Viewport
{
public:
	double X, Y, Width, Height;
	Viewport();
	Viewport(int x, int y, int w, int h);
	~Viewport();

	static void PostMessage(Viewport* viewport, std::vector<Entity*>* ents, std::string msg);
	static void PostMessage(Viewport* viewport, std::vector<Entity*>* ents, bool drawmid, bool bg, bool relative, int dieafter, double x, double y, std::string msg);
};

