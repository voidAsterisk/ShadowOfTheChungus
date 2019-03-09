#pragma once
#include <sstream>
class Bullet : public Entity
{
public:
	double lastX;
	double lastY;
	double Angle;
	Bullet(std::vector<Entity*>* ents, double angle);
	
	~Bullet();

	virtual void Update(double dt)
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
			d->Solid = false;
			d->Fade = true;
			d->LoadImage(renderer, "gfx/" + (std::string)"shotdecal.bmp");
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
					GenerateBloodSplatter((*EntityListPointer)[i]->X + (*EntityListPointer)[i]->Width / 2, (*EntityListPointer)[i]->Y + (*EntityListPointer)[i]->Height, a);

					int dmg = rand() % 3 + 1;
					bool crit = rand() % 1000 < 50;
					std::stringstream ss;

					if (crit)
					{
						dmg = 5;
						Viewport::PostMessage(EntityListPointer, "Critical hit!");
					}
					ss << "-" << dmg;
					Viewport::PostMessage(EntityListPointer, true, true, true, 1000, (*EntityListPointer)[i]->X + (*EntityListPointer)[i]->Width / 2, (*EntityListPointer)[i]->Y - (*EntityListPointer)[i]->Height, ss.str());
					(*EntityListPointer)[i]->Health -= dmg;
					Alive = false;
					player->DamageInflicted += dmg;
					return;

				}

			}
		}
		// Check for collisions against solids
		for (unsigned int i = 0; i < solids.size(); i++)
		{
			if (lineRect(lastX, lastY, X, Y, solids[i].Rect.x, solids[i].Rect.y, solids[i].Rect.w, solids[i].Rect.h))
			{
				Alive = false;
				return;

			}
		}

	}
};