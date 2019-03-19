#pragma once
#include <vector>
#include <SDL_ttf.h>
class Entity;
class ScreenMessage;
class Cursor; 
class Viewport
{
	Cursor* c;
	bool shaking = false;
	double m = 0;
	double shakeX = 0;
	double shakeY = 0;
	double shakeamtX = 0;
	double shakeamtY = 0;
public:
	double X, Y, Width, Height;
	Entity* Focus;
	Viewport();
	Viewport(Cursor* cursor,int x, int y, int w, int h);
	void Update(double dt);
	void Shake(double magnitude);
	~Viewport();

	static void PostMessage(Viewport* viewport, std::vector<Entity*>* ents, std::string msg);
	static void PostMessage(Viewport* viewport, std::vector<Entity*>* ents, bool drawmid, bool bg, bool relative, int dieafter, double x, double y, std::string msg);
};

