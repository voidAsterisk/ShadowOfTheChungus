#include "pch.h"
#include "Bullet.h"

#include "Entity.h"
#include "Viewport.h"
#include "Solid.h"
#include "Player.h"

Bullet::Bullet(Viewport* viewport, Player* player, std::vector<Solid>* solids, SDL_Renderer* renderer, std::vector<Entity*>* ents, double angle)
{
	Entity::Entity(ents);
	v = viewport;
	p = player;
	s = solids;
	ren = renderer;
	id = "bullet";
	MaxVelocity = 5;
	lastX = X;
	lastY = Y;

	Width = 2;
	Height = 2;
	IsSolid = false;
	Angle = angle;
	Fade = false;
	DeathTime = SDL_GetTicks() + 80;
}

Bullet::~Bullet()
{
}

void Bullet::Update(double dt)
{
	Entity::Update(dt);
	if (!Alive)
	{
		// Spawn shot decal
		Entity* d = new Entity(EntityListPointer);
		d->X = X;
		d->Y = Y;
		d->Width = 5;
		d->Height = 4;
		d->IsSolid = false;
		d->Fade = true;
		d->LoadImage(ren, "gfx/" + (std::string)"shotdecal.bmp");
		d->DeathTime = SDL_GetTicks() + 10000;
		EntityListPointer->push_back(d);
		return;
	}

	double dx = cos(Angle);
	double dy = sin(Angle);

	lastX = X;
	lastY = Y;
	X += dx * dt * MaxVelocity;
	Y += dy * dt * MaxVelocity;

	// Check for collisions against other entitise
	for (unsigned int i = 0; i < EntityListPointer->size(); i++)
	{
		if (!(*EntityListPointer)[i]->Alive) continue;

		if ((*EntityListPointer)[i]->id == "critter" ||
			(*EntityListPointer)[i]->id == "zombie_critter" ||
			(*EntityListPointer)[i]->id == "fire_critter" ||
			(*EntityListPointer)[i]->id == "king_critter" ||
			(*EntityListPointer)[i]->id == "big_zombie_critter")
		{
			if (lineRect(lastX, lastY, X, Y, (*EntityListPointer)[i]->X, (*EntityListPointer)[i]->Y, (*EntityListPointer)[i]->Width * SCALE, (*EntityListPointer)[i]->Height * SCALE))
			{
				double ax = lastX;
				double ay = lastY;
				double bx = (*EntityListPointer)[i]->X;
				double by = (*EntityListPointer)[i]->Y;

				double a = atan2(by - ay, bx - ax);
				GenerateBloodSplatter(v, EntityListPointer, (*EntityListPointer)[i]->X + (*EntityListPointer)[i]->Width / 2, (*EntityListPointer)[i]->Y + (*EntityListPointer)[i]->Height, a);

				int dmg = rand() % 3 + 1;
				bool crit = rand() % 1000 < 50;
				std::stringstream ss;

				if (crit)
				{
					dmg = 5;
					Viewport::PostMessage(v, EntityListPointer, "Critical hit!");
				}
				ss << "-" << dmg;
				Viewport::PostMessage(v, EntityListPointer, true, true, true, 1000, (*EntityListPointer)[i]->X + (*EntityListPointer)[i]->Width / 2, (*EntityListPointer)[i]->Y - (*EntityListPointer)[i]->Height, ss.str());
				(*EntityListPointer)[i]->Health -= dmg;
				Alive = false;
				p->DamageInflicted += dmg;
				return;

			}

		}
	}
	// Check for collisions against solids
	for (unsigned int i = 0; i < s->size(); i++)
	{
		if (lineRect(lastX, lastY, X, Y, (*s)[i].Rect.x, (*s)[i].Rect.y, (*s)[i].Rect.w, (*s)[i].Rect.h))
		{
			Alive = false;
			return;

		}
	}

}