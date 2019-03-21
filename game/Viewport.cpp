#include "pch.h"
#include "Viewport.h"
#include "Cursor.h"
#include "ScreenMessage.h"
#include <iostream>
Viewport::Viewport()
{
	
}
Viewport::Viewport(Cursor* cursor, int x, int y, int w, int h)
{
	
	c = cursor;
	X = x;
	Y = y;
	Width = w;
	Height = h;
}


Viewport::~Viewport()
{
}

void Viewport::Update(double dt)
{
	if (Focus != nullptr)
	{
		double dx = Focus->X - c->X + X;
		double dy = Focus->Y - c->Y + Y;
		double d = 500;

		if (dx > d) dx = d;
		if (dx < -d) dx = -d;
		if (dy > d) dy = d;
		if (dy < -d) dy = -d;

		c->X += dx;
		c->Y += dy;

		dx /= 9;
		dy /= 9;
		X = -Focus->X + Width / 2 + Focus->Width / 2 + dx;
		Y = -Focus->Y + Height / 2 + Focus->Height / 2 + dy;

		if (shaking)
		{
			shakeX += shakeamtX;
			shakeY += shakeamtY;
			if ((shakeX > 0 && shakeX > m) ||
				(shakeX < 0 && shakeX < -m)) shakeamtX = -shakeamtX;
			if ((shakeY > 0 && shakeY > m) ||
				(shakeY < 0 && shakeY < -m)) shakeamtY = -shakeamtY;
			shakeamtX *= 0.6f;
			shakeamtY *= 0.6f;
			if (abs(shakeamtX) < 0.01f &&
				abs(shakeamtY) < 0.01f) shaking = false;
			X += shakeamtX;
			Y += shakeamtY;
		}
	}
}
void Viewport::Shake(double magnitude)
{
	shaking = true; 
	m = magnitude;
	shakeamtX = rand() % 5 - 10;
	shakeamtY = rand() % 5 - 10;
}
void Viewport::PostMessage(Viewport* viewport, std::vector<Entity*>* ents, std::string msg)
{
	ScreenMessage* m = new ScreenMessage(viewport, ents, msg);
	m->id = "ui";
	ents->push_back(m);
}
void Viewport::PostMessage(Viewport* viewport, std::vector<Entity*>* ents, bool drawmid, bool bg, bool relative, int dieafter, double x, double y, std::string msg)
{
	ScreenMessage* m = new ScreenMessage(viewport, ents, drawmid, bg, relative, dieafter, x, y, msg);
	m->id = "ui";
	ents->push_back(m);
}