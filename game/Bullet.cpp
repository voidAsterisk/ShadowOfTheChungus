#include "pch.h"
#include "Bullet.h"


Bullet::Bullet(std::vector<Entity*>* ents, double angle)
{
	Entity::Entity(ents);

	id = "bullet";
	MaxVelocity = 5;
	lastX = X;
	lastY = Y;

	Width = 2;
	Height = 2;
	Solid = false;
	Angle = angle;
	Fade = false;
	DeathTime = SDL_GetTicks() + 80;
}

Bullet::~Bullet()
{
}
