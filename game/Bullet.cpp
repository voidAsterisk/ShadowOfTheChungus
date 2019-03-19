#include "pch.h"
#include "Bullet.h"

#include "Viewport.h"
#include "Solid.h"
#include "Player.h"

Bullet::Bullet(Viewport* viewport, Player* player, std::vector<Solid>* solids, SDL_Renderer* renderer, std::vector<Entity*>* ents, double angle) :
	Entity(ents)
{
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

	impact_sfx.push_back(Mix_LoadWAV("sfx/dirt1.wav"));
	impact_sfx.push_back(Mix_LoadWAV("sfx/dirt2.wav"));
	impact_sfx.push_back(Mix_LoadWAV("sfx/dirt3.wav"));
	impact_sfx.push_back(Mix_LoadWAV("sfx/dirt4.wav"));
}

Bullet::~Bullet()
{
	for (int i = 0; i < impact_sfx.size(); i++)
	{
		Mix_FreeChunk(impact_sfx[i]);
	}
}

void Bullet::PlayRandomImpactSound()
{
	int n = impact_sfx.size() - 1;
	int k = rand() % n;

	Mix_VolumeChunk(impact_sfx[k], MIX_MAX_VOLUME * ((rand() % 5) * 0.1f + 0.4f));
	Mix_PlayChannel(-1, impact_sfx[k], 0);
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
		if (!(*EntityListPointer)[i]->Alive ||
			 (*EntityListPointer)[i]->Action == EntityAction::Dead) continue;

		if ((*EntityListPointer)[i]->id == "critter" ||
			(*EntityListPointer)[i]->id == "zombie_critter" ||
			(*EntityListPointer)[i]->id == "fire_critter" ||
			(*EntityListPointer)[i]->id == "king_critter" ||
			(*EntityListPointer)[i]->id == "big_zombie_critter" ||
			(*EntityListPointer)[i]->id == "skeleton" ||
			(*EntityListPointer)[i]->id == "zombie")
		{
			if (lineRect(lastX, lastY, X, Y, (*EntityListPointer)[i]->X, (*EntityListPointer)[i]->Y, (*EntityListPointer)[i]->Width * SCALE, (*EntityListPointer)[i]->Height * SCALE))
			{
				double ax = lastX;
				double ay = lastY;
				double bx = (*EntityListPointer)[i]->X;
				double by = (*EntityListPointer)[i]->Y;

				double a = atan2(by - ay, bx - ax);
				if ((*EntityListPointer)[i]->BloodSplatter)
					GenerateBloodSplatter(v, (*EntityListPointer)[i]->X + (*EntityListPointer)[i]->Width / 2, (*EntityListPointer)[i]->Y + (*EntityListPointer)[i]->Height, a);

				int dmg = rand() % 3 + 1;
				bool crit = rand() % 1000 < 50;
				std::stringstream ss;

				if (crit)
				{
					dmg = 5;
					//Viewport::PostMessage(v, EntityListPointer, "Critical hit!");
				}
				//ss << "-" << dmg;
				//Viewport::PostMessage(v, EntityListPointer, true, true, true, 1000, (*EntityListPointer)[i]->X + (*EntityListPointer)[i]->Width / 2, (*EntityListPointer)[i]->Y - (*EntityListPointer)[i]->Height, ss.str());
				(*EntityListPointer)[i]->Health -= dmg;
				Alive = false;
				p->DamageInflicted += dmg;

				// Special effect freeze the game for a little bit
				PlayRandomImpactSound();
				v->Shake(2);
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