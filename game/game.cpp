#include "pch.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ios>
#include <pugixml.hpp>
#include <functional>
#include <algorithm>
#include<array>
#include <math.h>
#include <boost/tokenizer.hpp>
#include "Vars.h"
#include "Funcs.h"
#include "Entity.h"
#include "Cursor.h"
#include "Console.h"
#include "HeldItem.h"
#include "Warp.h"
#include "Player.h"
#include "Bullet.h"
#pragma comment (lib, "SDL2")
#pragma comment (lib, "SDL2_ttf")

std::vector<SDL_Rect> hurt;
std::vector<KillID> killedmapid = std::vector<KillID>();
std::vector<Warp> warps;

Console cons;

void SetGameState(GameStates gs);
void GenerateBloodSplatter(double x, double y, double angle);
void ClearEntities();

void SetGameState(GameStates gs)
{
	lastgamestate = gamestate;
	gamestate = gs;
}

void ClearEntities()
{
	for (unsigned int i = 0; i < entities.size(); i++)
	{
		if (entities[i]->id != "player" && entities[i] != nullptr)
		delete(entities[i]);
	}
	entities.clear();
}


Cursor* cursor;



class Item : public
	Entity
{
public:
	std::string Name;
	std::string Id;
	Item()
	{

	}
	Item(double x, double y, std::string name, std::string id)
	{
		X = x;
		Y = y;
		Name = name;
		Id = id;
		Respawn = false;
		Solid = false;
	}

	virtual void Update(double dt)
	{
		// If players feet touches item
		SDL_Point p = { player->X + player->Width * SCALE / 2, player->Y + player->Height * SCALE};
		SDL_Rect r = { X, Y, Width * SCALE, Height * SCALE };

		if (SDL_PointInRect(&p, &r))
		{
			Alive = false;
			HeldItem it = HeldItem(Name, Id);
			player->AddItem(it);
			PostMessage((std::string)"Obtained " + Name + "!");
		}
	}
};

class MinorHealth : public
	Entity
{
public:
	MinorHealth(double x, double y, std::string name)
	{
		X = x;
		Y = y;
		Name = name;
		Solid = false;
	}

	virtual void Update(double dt)
	{
		// If players feet touches item
		SDL_Point p = { player->X + player->Width * SCALE / 2, player->Y + player->Height * SCALE };
		SDL_Rect r = { X, Y, Width * SCALE, Height * SCALE };

		if (SDL_PointInRect(&p, &r) && player->Health != player->MaxHealth)
		{
			this->Alive = false;
			player->AddHealth(player->MaxHealth * 0.2f);
		}
	}
};
class MaximumHealth : public
	Entity
{
public:
	MaximumHealth(double x, double y, std::string name)
	{
		X = x;
		Y = y;
		Name = name;
		Solid = false;
	}

	virtual void Update(double dt)
	{
		// If players feet touches item
		SDL_Point p = { player->X + player->Width * SCALE / 2, player->Y + player->Height * SCALE };
		SDL_Rect r = { X, Y, Width * SCALE, Height * SCALE };

		if (SDL_PointInRect(&p, &r) && player->Health != player->MaxHealth)
		{
			this->Alive = false;
			player->Health = player->MaxHealth;
		}
	}
};
class Chunk
{
public:
	int X;
	int Y;
	int Width;
	int Height;
	int** Data;

	Chunk(int x, int y, int w, int h)
	{
		X = x;
		Y = y;
		Width = w;
		Height = h;
		Data = new int*[w];
		for (int i = 0; i < w; i++)
		{
			Data[i] = new int[h];
		}
	}
	~Chunk()
	{
		for (int i = 0; i < Width; i++)
				delete(Data[i]);
	}
};
std::vector<Chunk*> chunks;

void ClearChunks()
{
	for (int i = 0; i < chunks.size(); i++)
	{
		if (chunks[i] != nullptr)
			delete(chunks[i]);
	}
	chunks.clear();
}
class NPC : public
	Entity
{
public:
	std::string Dialog;

	NPC(double x, double y)
	{
		id = "npc";
		X = x;
		Y = y;
		Width = 7;
		Height = 16;
	}

	void OnClick()
	{
		Viewport::PostMessage("[" + Name + "] " + Dialog);
	}

	virtual void Draw(SDL_Renderer* ren)
	{
		Entity::Draw(ren);

		// Draw text if cursor rect intersects self rect
		SDL_Rect curr = { cursor->X, cursor->Y, cursor->Width, cursor->Height };
		SDL_Rect r = { X + viewport.X, Y + viewport.Y, Width * SCALE, Height * SCALE };

		if (SDL_HasIntersection(&curr, &r))
		{
			// Draw level and name
			SDL_Surface * s = TTF_RenderText_Solid(monsterfont,
				Name.c_str(), { 255, 255, 255 });
			int w = s->w;
			int h = s->h;
			SDL_Rect r = { X + viewport.X + Width * SCALE / 2 - w / 2, Y + viewport.Y - h*1.5f, w, h };
			SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
			SDL_FreeSurface(s);
			SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
			SDL_RenderFillRect(ren, &r);
			SDL_RenderCopy(renderer, t, NULL, &r);
			SDL_DestroyTexture(t);
		}
	}
};

class DustParticle : public Entity
{
public:
	double baseX;
	double baseY;
	int AddExp = 1;
	
	DustParticle(int exp, double bX, double bY)
	{
		id = "dustparticle";
		AddExp = exp;
		baseX = bX;
		baseY = bY + sin(rand()) * 5;
		VelocityX = (rand() % 50 - 25) * 0.01f;
		VelocityY = (rand() % 50 + 50) * 0.01f;
		MaxVelocity = 1;

		Width = 3;
		Height = 3;
	}
	virtual void Update(double dt)
	{
		VelocityY += 0.006;
		if (Y > baseY)
		{
			Y = baseY;
			VelocityY = -VelocityY /2;

			if (abs(VelocityX) > 0.0001f)
			{
				VelocityX *= 0.8f;
			}
			else
			{
				VelocityX = 0;
			}
			if (abs(VelocityY) < 0.01f)
			{
				VelocityY = 0;
			}
		}
		X += VelocityX;
		Y += VelocityY;

			SDL_Rect a = { player->X, player->Y, player->Width * SCALE, player->Height * SCALE };
			SDL_Rect b = { X, Y, Width * SCALE, Height * SCALE };
			SDL_Rect r;
			if (SDL_IntersectRect(&a, &b, &r))
			{
				Alive = false;
				
			}
	}
};

class Fireball :
	public Entity
{
public:
	double DestX;
	double DestY;

	Fireball(std::vector<Entity*>* ents, double x, double y, double destX, double destY)
	{
		Entity::Entity(ents);
		X = x;
		Y = y;
		DestX = destX;
		DestY = destY;
		MaxVelocity = 0.1;

		Width = 8;
		Height = 8;

		Solid = false;
	}

	virtual void Update(double dt)
	{
		double dx = DestX - Width * SCALE / 2 - X;
		double dy = DestY - Height * SCALE / 2 - Y;

		float length = sqrtf(dx*dx + dy * dy);

		if (length <= 1)
		{
			Alive = false;

			// Spawn shot decal
			Entity* d = new Entity(&entities);
			d->X = X;
			d->Y = Y;
			d->Width = 5;
			d->Height = 4;
			d->Fade = true;
			d->LoadImage(renderer, "gfx/" + (std::string)"shotdecal.bmp");
			d->Solid = false;
			d->DeathTime = SDL_GetTicks() + 10000;
			entities.push_back(d);
			return;
		}

		dx /= length;
		dy /= length;
		X += dx * dt * MaxVelocity;
		Y += dy * dt * MaxVelocity;

		// If collides with player
		SDL_Rect pr = { player->X, player->Y, player->Width*SCALE, player->Height*SCALE };
		SDL_Rect r = { X, Y, Width *SCALE, Height * SCALE };

		if (SDL_HasIntersection(&pr, &r))
		{
			Alive = false;
			player->Health -= AttackDamage;
		}

		// Check for collisions against solids
		for (unsigned int i = 0; i < solids.size(); i++)
		{
			if (SDL_HasIntersection(&r, &solids[i].Rect))
			{
				Alive = false;
				return;

			}
		}
	}
};


class Critter : public Entity
{
	int nextx;
	int nexty;
	int nextmove;
public:
	Critter(double x, double y)
	{
		X = x;
		Y = y;
		id = "critter";
		Level = 1;
		Name = "Critter";
		Action = Standing;
		MaxVelocity = 0.02;
		Solid = true;

		nextx = X + cos(rand()) * (rand() % 10);
		nexty = Y + sin(rand()) * (rand() % 10);
		nextmove = SDL_GetTicks() + rand() % 10000 + 5000;
		Action = Standing;

		Width = 6;
		Height = 5;
	}
	virtual void Update(double dt)
	{

		if (Action == Moving)
		{
			double dx = nextx - X;
			double dy = nexty - Y;

			float length = sqrtf(dx*dx + dy * dy);

			if (length <= MaxVelocity)
			{
				Action = Standing;
			}

			dx /= length;
			dy /= length;
			X += dx * dt * MaxVelocity;
			Y += dy * dt * MaxVelocity;
		}
		if (Action == Standing)
		{
			if (SDL_GetTicks() >= nextmove)
			{
				Action = Moving;
				nextx = X + rand() % 50 - 25;
				nexty = Y + rand() % 50 - 25;
				nextmove = SDL_GetTicks() + rand() % 10000 + 5000;
			}
		}
		

		if (Health <= 0)
		{
			Alive = false;
			// Spawn entity destruction
			for (int j = 0; j < ExperienceDrop(); j++)
			{
				player->Experience++;
				if (player->Experience == player->NextExperience)
				{
					player->Experience = 0;
					player->Level++;
					player->NextExperience = NextExp(player->Level);
				}
			}
		}
	}
	virtual void Draw(SDL_Renderer* ren)
	{
		Entity::Draw(ren);
		// Draw text if cursor rect intersects self rect
		SDL_Rect curr = { cursor->X, cursor->Y, cursor->Width, cursor->Height };
		SDL_Rect r = { X + viewport.X, Y + viewport.Y, Width * SCALE, Height * SCALE };

		if (SDL_HasIntersection(&curr, &r))
		{
			DrawHealthBar(ren);
		}
	}
};

class FireCritter : public Entity
{
	int nextx;
	int nexty;
	int nextmove;

	int AggroDist = 100000;
	int AttackDist = 40000;
public:
	FireCritter(double x, double y)
	{
		X = x;
		Y = y;
		id = "fire_critter";
		Level = 5;
		Name = "Fire Critter";
		Action = Standing;
		MaxVelocity = 0.06;
		Solid = true;

		nextx = X + cos(rand()) * (rand() % 10);
		nexty = Y + sin(rand()) * (rand() % 10);
		nextmove = SDL_GetTicks() + rand() % 10000 + 5000;
		Action = Standing;

		Width = 6;
		Height = 10;
		Attack = 1;
		Health = 1;
		MaxHealth = 1;
		AttackTimeout = 400;
	}
	virtual void Update(double dt)
	{
		// Get distance to player's feet
		double px = player->X + player->Width * SCALE / 2;
		double py = player->Y + player->Height * SCALE;

		double d = ((px - X)*(px - X) + (py - Y)*(py - Y));
		if (d < AggroDist && d > AttackDist)
		{
			Action = Aggro;
		}
		if (d < AttackDist)
		{
			Action = Attacking;
		}
		if (Action == Moving)
		{
			double dx = nextx - X;
			double dy = nexty - Y;

			float length = sqrtf(dx*dx + dy * dy);

			if (length <= MaxVelocity)
			{
				Action = Standing;
			}

			dx /= length;
			dy /= length;
			X += dx * dt * MaxVelocity;
			Y += dy * dt * MaxVelocity;
		}
		if (Action == Standing)
		{
			if (SDL_GetTicks() >= nextmove)
			{
				Action = Moving;
				nextx = X + rand() % 50 - 25;
				nexty = Y + rand() % 50 - 25;
				nextmove = SDL_GetTicks() + rand() % 10000 + 5000;
			}
		}
		if (Action == Aggro)
		{
			double dx = px - X - Width * SCALE / 2;
			double dy = py - Y - Height * SCALE;

			float length = sqrtf(dx*dx + dy * dy);
			dx /= length;
			dy /= length;
			X += dx * dt * MaxVelocity;
			Y += dy * dt * MaxVelocity;

			if (d > AggroDist) Action = Standing;
		}
		// Check line of sight
		int x = X;
		int y = Y;
		int x2 = player->X;
		int y2 = player->Y;
		for (int i = 0; i < solids.size(); i++)
		{
			if (SDL_IntersectRectAndLine(&solids[i].Rect, &x, &y, &x2, &y2))
			{
				Action = Standing;
			}
		}

		if (Action == Attacking || Action == Aggro)
		{
			if (NextAttack <= SDL_GetTicks())
			{
				// Launch fireball
				Fireball* fb = new Fireball(&entities, X, Y, player->X + player->Width * SCALE / 2, player->Y + player->Height * SCALE / 2);
				fb->LoadImage(renderer, "gfx/fireball.bmp");
				entities.push_back(fb);
				NextAttack = SDL_GetTicks() + AttackTimeout;
			}
			if (d > 20000) Action = Standing;

			
		}

		if (Health <= 0)
		{
			Alive = false;
		}


		AnimationY = 0;
		AnimationX += 0.01 * dt;
		if (AnimationX > 2) AnimationX = AnimationX - (int)AnimationX;
	}
	virtual void Draw(SDL_Renderer* ren)
	{
		Entity::Draw(ren);
		// Draw text if cursor rect intersects self rect
		SDL_Rect curr = { cursor->X, cursor->Y, cursor->Width, cursor->Height };
		SDL_Rect r = { X + viewport.X, Y + viewport.Y, Width * SCALE, Height * SCALE };

		if (SDL_HasIntersection(&curr, &r))
		{
			DrawHealthBar(ren);
		}
	}
};

class ZombieCritter : public Entity
{
	int nextx;
	int nexty;
	int nextmove;
public:
	ZombieCritter(double x, double y)
	{
		X = x;
		Y = y;
		id = "zombie_critter";
		Level = 5;
		Name = "Zombie Critter";
		Action = Standing;
		MaxVelocity = 0.02;
		Solid = true;

		nextx = X + cos(rand()) * (rand() % 10);
		nexty = Y + sin(rand()) * (rand() % 10);
		nextmove = SDL_GetTicks() + rand() % 10000 + 5000;
		Action = Standing;

		Width = 6;
		Height = 5;
		Attack = 1;
		Health = 2;
		MaxHealth = 2;
		AttackTimeout = 2500;
		Respawn = true;
	}
	virtual void Update(double dt)
	{
		// Get distance to player's feet
		double px = player->X + player->Width * SCALE / 2;
		double py = player->Y + player->Height * SCALE;

		double d = ((px - X)*(px - X) + (py - Y)*(py - Y));
		if (d < 15000 && d > 400)
		{
			Action = Aggro;
		}
		if (d < 500)
		{
			Action = Attacking;
		}
		if (Action == Moving)
		{
			double dx = nextx - X;
			double dy = nexty - Y;

			float length = sqrtf(dx*dx + dy * dy);

			if (length <= MaxVelocity)
			{
				Action = Standing;
			}

			dx /= length;
			dy /= length;
			X += dx * dt * MaxVelocity;
			Y += dy * dt * MaxVelocity;
		}
		if (Action == Standing)
		{
			if (SDL_GetTicks() >= nextmove)
			{
				Action = Moving;
				nextx = X + rand() % 50 - 25;
				nexty = Y + rand() % 50 - 25;
				nextmove = SDL_GetTicks() + rand() % 10000 + 5000;
			}
		}
		if (Action == Aggro || Action == Attacking)
		{
			// Check line of sight
			int x = X;
			int y = Y;
			int x2 = player->X;
			int y2 = player->Y;
			for (int i = 0; i < solids.size(); i++)
			{
				if (SDL_IntersectRectAndLine(&solids[i].Rect, &x, &y, &x2, &y2))
				{
					Action = Standing;
				}
			}
		}
		if (Action == Aggro)
		{
			double dx = px - X - Width * SCALE / 2;
			double dy = py - Y - Height * SCALE;

			float length = sqrtf(dx*dx + dy * dy);
			dx /= length;
			dy /= length;
			X += dx * dt * MaxVelocity;
			Y += dy * dt * MaxVelocity;

			if (d > 1500) Action = Standing;
		}
		if (Action == Attacking)
		{
			if (NextAttack <= SDL_GetTicks())
			{
				player->Health -= Attack;
				NextAttack = SDL_GetTicks() + AttackTimeout;
			}
			if (d > 1500) Action = Standing;
		}
		

		if (Health <= 0)
		{
			Alive = false;
		}
	}
	virtual void Draw(SDL_Renderer* ren)
	{
		Entity::Draw(ren);
		// Draw text if cursor rect intersects self rect
		SDL_Rect curr = { cursor->X, cursor->Y, cursor->Width, cursor->Height };
		SDL_Rect r = { X + viewport.X, Y + viewport.Y, Width * SCALE, Height * SCALE };

		if (SDL_HasIntersection(&curr, &r))
		{
			DrawHealthBar(ren);
		}
	}
};

class BigZombieCritter : public Entity
{
	int nextx;
	int nexty;
	int nextmove;
public:
	BigZombieCritter(double x, double y)
	{
		X = x;
		Y = y;
		id = "big_zombie_critter";
		Level = 10;
		Name = "Big Zombie Critter";
		Action = Standing;
		MaxVelocity = 0.02;
		Solid = true;

		nextx = X + cos(rand()) * (rand() % 10);
		nexty = Y + sin(rand()) * (rand() % 10);
		nextmove = SDL_GetTicks() + rand() % 10000 + 5000;
		Action = Standing;

		Width = 9;
		Height = 7;
		Attack = 1;
		Health = 7;
		MaxHealth = 7;
		AttackTimeout = 3000;
		AggroDistance = 80000;
		AttackDistance = 80000;
		Respawn = false;
	}
	virtual void Update(double dt)
	{
		// Get distance to player's feet
		double px = player->X + player->Width * SCALE / 2;
		double py = player->Y + player->Height * SCALE;

		double d = ((px - X)*(px - X) + (py - Y)*(py - Y));
		if (d < AggroDistance && d > AttackDistance)
		{
			Action = Aggro;
		}
		if (d < AttackDistance)
		{
			Action = Attacking;
		}
		if (Action == Moving)
		{
			double dx = nextx - X;
			double dy = nexty - Y;

			float length = sqrtf(dx*dx + dy * dy);

			if (length <= MaxVelocity)
			{
				Action = Standing;
			}

			dx /= length;
			dy /= length;
			X += dx * dt * MaxVelocity;
			Y += dy * dt * MaxVelocity;
		}
		if (Action == Standing)
		{
			if (SDL_GetTicks() >= nextmove)
			{
				Action = Moving;
				nextx = X + rand() % 50 - 25;
				nexty = Y + rand() % 50 - 25;
				nextmove = SDL_GetTicks() + rand() % 10000 + 5000;
			}
		}
		if (Action == Aggro || Action == Attacking)
		{
			// Check line of sight
			int x = X;
			int y = Y;
			int x2 = player->X;
			int y2 = player->Y;
			for (int i = 0; i < solids.size(); i++)
			{
				if (SDL_IntersectRectAndLine(&solids[i].Rect, &x, &y, &x2, &y2))
				{
					Action = Standing;
				}
			}
		}
		if (Action == Aggro)
		{
			double dx = px - X - Width * SCALE / 2;
			double dy = py - Y - Height * SCALE;

			float length = sqrtf(dx*dx + dy * dy);
			dx /= length;
			dy /= length;
			X += dx * dt * MaxVelocity;
			Y += dy * dt * MaxVelocity;

			if (d > AttackDistance) Action = Standing;
		}
		if (Action == Attacking)
		{
			if (NextAttack <= SDL_GetTicks())
			{
				// Spawn fire critter
				FireCritter* f = new FireCritter(X + Width / 2, Y + Height / 2);
				f->LoadImage(renderer, "gfx/fire_critter.bmp");
				entities.push_back(f);
				NextAttack = SDL_GetTicks() + AttackTimeout;
			}
			if (d > AggroDistance) Action = Standing;
		}


		if (Health <= 0)
		{
			Alive = false;
		}
	}
	virtual void Draw(SDL_Renderer* ren)
	{
		Entity::Draw(ren);
		// Draw text if cursor rect intersects self rect
		SDL_Rect curr = { cursor->X, cursor->Y, cursor->Width, cursor->Height };
		SDL_Rect r = { X + viewport.X, Y + viewport.Y, Width * SCALE, Height * SCALE };

		if (SDL_HasIntersection(&curr, &r))
		{
			DrawHealthBar(ren);
		}
	}
};

class KingCritter : public Entity
{
	int nextx;
	int nexty;
	int nextmove;

	int AggroDist = 180000;
	int AttackDist = 40000;
public:
	KingCritter(double x, double y)
	{
		X = x;
		Y = y;
		id = "king_critter";
		Level = 5;
		Name = "King Critter";
		Action = Standing;
		MaxVelocity = 0.06;
		Solid = true;

		nextx = X + cos(rand()) * (rand() % 10);
		nexty = Y + sin(rand()) * (rand() % 10);
		nextmove = SDL_GetTicks() + rand() % 10000 + 5000;
		Action = Standing;

		Width = 14;
		Height = 12;
		Attack = 1;
		Health = 300;
		MaxHealth = 300;
		AttackTimeout = 150;
	}
	virtual void Update(double dt)
	{
		// Get distance to player's feet
		double px = player->X + player->Width * SCALE / 2;
		double py = player->Y + player->Height * SCALE;

		double d = ((px - X)*(px - X) + (py - Y)*(py - Y));
		if (d < AggroDist && d > AttackDist)
		{
			Action = Aggro;
		}
		if (d < AttackDist)
		{
			Action = Attacking;
		}
		if (Action == Moving)
		{
			double dx = nextx - X;
			double dy = nexty - Y;

			float length = sqrtf(dx*dx + dy * dy);

			if (length <= MaxVelocity)
			{
				Action = Standing;
			}

			dx /= length;
			dy /= length;
			X += dx * dt * MaxVelocity;
			Y += dy * dt * MaxVelocity;
		}
		if (Action == Standing)
		{
			if (SDL_GetTicks() >= nextmove)
			{
				Action = Moving;
				nextx = X + rand() % 50 - 25;
				nexty = Y + rand() % 50 - 25;
				nextmove = SDL_GetTicks() + rand() % 10000 + 5000;
			}
		}
		if (Action == Aggro)
		{
			double dx = px - X - Width * SCALE / 2;
			double dy = py - Y - Height * SCALE;

			float length = sqrtf(dx*dx + dy * dy);
			dx /= length;
			dy /= length;
			X += dx * dt * MaxVelocity;
			Y += dy * dt * MaxVelocity;

			if (d > AggroDist) Action = Standing;
		}
		// Check line of sight
		int x = X;
		int y = Y;
		int x2 = player->X;
		int y2 = player->Y;
		for (int i = 0; i < solids.size(); i++)
		{
			if (SDL_IntersectRectAndLine(&solids[i].Rect, &x, &y, &x2, &y2))
			{
				Action = Standing;
			}
		}

		if (Action == Attacking || Action == Aggro)
		{
			if (NextAttack <= SDL_GetTicks())
			{
				// Launch fireball
				Fireball* fb = new Fireball(&entities, X, Y, player->X + player->Width * SCALE / 2, player->Y + player->Height * SCALE / 2);
				fb->LoadImage(renderer, "gfx/fireball.bmp");
				entities.push_back(fb);
				NextAttack = SDL_GetTicks() + AttackTimeout;
			}
			if (d > AttackDist) Action = Standing;


		}

		if (Health <= 0)
		{
			double x = X + rand() % (int)Width - Width / 2;
			double y = Y + rand() % (int)Height - Height / 2;
			Alive = false;
			Item* i = new Item(x + Width / 2, y + Height / 2, "Sword of Gryifningu", "g_sword");
			i->Width = 4;
			i->Height = 11;
			i->LoadImage(renderer, "gfx/g_sword.bmp");
			entities.push_back(i);

			x = X + rand() % (int)Width - Width / 2;
			y = Y + rand() % (int)Height - Height / 2;
			MaximumHealth* mh = new MaximumHealth(x + Width / 2, y + Height / 2, "Max Health");
			mh->Width = 9;
			mh->Height = 9;
			mh->LoadImage(renderer, "gfx/max_health.bmp");
			entities.push_back(mh);
			
		}


		//AnimationY = 0;
		//AnimationX += 0.01 * dt;
		//if (AnimationX > 2) AnimationX = AnimationX - (int)AnimationX;
	}
	virtual void Draw(SDL_Renderer* ren)
	{
		Entity::Draw(ren);
		// Draw text if cursor rect intersects self rect
		SDL_Rect curr = { cursor->X, cursor->Y, cursor->Width, cursor->Height };
		SDL_Rect r = { X + viewport.X, Y + viewport.Y, Width * SCALE, Height * SCALE };

		if (SDL_HasIntersection(&curr, &r))
		{
			DrawHealthBar(ren);
		}
	}
};

class Inventory
{
	Player* p;
public:
		Inventory()
		{

		}
		Inventory(Player* plyr)
		{
			p = plyr;
		}
		void Update(double dt)
		{

		}
		void Draw(SDL_Renderer* ren, TTF_Font* font)
		{
			//int j = ((p->Inventory.size() > 3) ? 3 : p->Inventory.size());
			int j = p->Inventory.size();
			for (int i = 0; i < j; i++)
			{
				
				SDL_Surface * s = TTF_RenderText_Solid(font,
					p->Inventory[i].Name.c_str(), { 255, 255, 255 });
				int w = s->w;
				int h = s->h;
				SDL_Rect expr = { viewport.Width - w, viewport.Height / 2 - h * i - j / 2, w, h };
				SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
				SDL_FreeSurface(s);
				if (i == player->SelectedItem)
				{
					SDL_SetRenderDrawColor(ren, 128, 64, 64, 255);
					SDL_RenderFillRect(ren, &expr);
				}
				else
				{
					SDL_SetRenderDrawColor(ren, 200, 200, 200, 255);
					SDL_RenderFillRect(ren, &expr);
				}
				SDL_RenderCopy(renderer, t, NULL, &expr);
				SDL_DestroyTexture(t);
			}
		}
};
Inventory inventory;


class BloodParticle : public
	Entity
{
public:
	BloodParticle(int x, int y)
	{
		X = x;
		Y = y;
		DeathTime = SDL_GetTicks() + rand() % 15000;
		Fade = true;
		Solid = false;
	}

	virtual void Draw(SDL_Renderer* ren)
	{
		SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
		SDL_RenderDrawPoint(ren, X + viewport.X, Y + viewport.Y);
	}
};


void LoadLevel(std::string filename)
{
	currentmap = filename;
	ClearEntities();
	entities.push_back(player);
	ClearChunks();
	solids.clear();
	warps.clear();
	hurt.clear();
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(("maps/" + filename).c_str());

	for (pugi::xml_node chunk = doc.child("map").child("layer").child("data").child("chunk"); chunk; chunk = chunk.next_sibling("chunk"))
	{
		int x = chunk.attribute("x").as_int();
		int y = chunk.attribute("y").as_int();
		int w = chunk.attribute("width").as_int();
		int h = chunk.attribute("height").as_int();

		std::stringstream stream(chunk.text().as_string());
		std::string line;

		Chunk* c = new Chunk(x, y, w, h);

		int i = 0;
		while (std::getline(stream, line)) { // Every line
			std::vector <std::string> tokens;

			// stringstream class check1 
			std::stringstream check1(line);

			std::string intermediate;

			// Tokenizing w.r.t. space ' ' 
			while (std::getline(check1, intermediate, ','))
			{
				tokens.push_back(intermediate);
			}
			for (int j = 0; j < tokens.size(); j++)
			{
				c->Data[i - 1][j] = atoi((const char*)tokens[j].c_str());
			}
			i++;
		}

		chunks.push_back(c);
	}
	for (pugi::xml_node object = doc.child("map").child("objectgroup").child("object"); object; object = object.next_sibling("object"))
	{
		std::string name = object.attribute("name").as_string();
		KillID k = KillID(object.attribute("id").as_int(), currentmap);
		if (std::find(killedmapid.begin(), killedmapid.end(), k) != killedmapid.end())
		{
			continue;
		}
		if (name == "solid")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int w = object.attribute("width").as_int();
			int h = object.attribute("height").as_int();
			int mapid = object.attribute("id").as_int();
			std::string name = object.attribute("name").as_string();
			Solid s;
			s.Rect = { x, y, w, h };
			
				for (pugi::xml_node property = object.child("properties").first_child(); property; property = property.next_sibling())
				{
					std::string name = property.attribute("name").as_string();
					if (name == "key")
					{
						s.NeedsKey = property.attribute("value").as_string();
					}
				}
			solids.push_back(s);
		}
		if (name == "hurt")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int w = object.attribute("width").as_int();
			int h = object.attribute("height").as_int();
			
			hurt.push_back({ x, y, w, h });
		}
		if (name == "player")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int mapid = object.attribute("id").as_int();
			player->killID.MapID = mapid;
			player->killID.Map = currentmap;
			player->X = x;
			player->Y = y;
		}
		if (name == "critter")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int mapid = object.attribute("id").as_int();
			Critter* crit = new Critter(x, y);
			crit->killID.MapID = mapid;
			crit->killID.Map = currentmap;
			crit->LoadImage(renderer, "gfx/critter.bmp");
			entities.push_back(crit);
		}
		if (name == "king_critter")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int mapid = object.attribute("id").as_int();
			KingCritter* crit = new KingCritter(x, y);
			crit->killID.MapID = mapid;
			crit->killID.Map = currentmap;
			crit->LoadImage(renderer, "gfx/king_critter.bmp");
			entities.push_back(crit);
		}
		if (name == "spawn")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int w = object.attribute("width").as_int();
			int h = object.attribute("height").as_int();
			int mapid = object.attribute("id").as_int();
			std::string spawn_id;
			int num;
			for (pugi::xml_node property = object.child("properties").first_child(); property; property = property.next_sibling())
			{
				std::string name = property.attribute("name").as_string();

				if (name == "spawn_id")
				{
					spawn_id = property.attribute("value").as_string();
				}
				if (name == "number")
				{
					num = property.attribute("value").as_int();
				}
			}
			for (int i = 0; i < num; i++)
			{
				int nx = x + (rand() % (int)(w - w * 0.05f) + w * 0.01f);
				int ny = y + (rand() % (int)(h - h * 0.05f) + h * 0.01f);

				if (spawn_id == "critter")
				{
					Critter* crit = new Critter(nx, ny);
					crit->killID.MapID = mapid;
					crit->killID.Map = currentmap;
					crit->LoadImage(renderer, "gfx/critter.bmp");
					entities.push_back(crit);
				}
				if (spawn_id == "fire_critter")
				{
					FireCritter* crit = new FireCritter(nx, ny);
					crit->killID.MapID = mapid;
					crit->killID.Map = currentmap;
					crit->LoadImage(renderer, "gfx/fire_critter.bmp");
					entities.push_back(crit);
				}
				if (spawn_id == "zombie_critter")
				{
					ZombieCritter* crit = new ZombieCritter(nx, ny);
					crit->killID.MapID = mapid;
					crit->killID.Map = currentmap;
					crit->LoadImage(renderer, "gfx/zombie_critter.bmp");
					entities.push_back(crit);
				}
				if (spawn_id == "big_zombie_critter")
				{
					BigZombieCritter* crit = new BigZombieCritter(nx, ny);
					crit->killID.MapID = mapid;
					crit->killID.Map = currentmap;
					crit->LoadImage(renderer, "gfx/big_zombie_critter.bmp");
					entities.push_back(crit);
				}
			}
			
		}
		if (name == "npc")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int mapid = object.attribute("id").as_int();
			NPC* npc = new NPC(x, y);
			npc->killID.MapID = mapid;
			npc->killID.Map = currentmap;
			npc->Name = object.attribute("type").as_string();
			
			for (pugi::xml_node property = object.child("properties").first_child(); property; property = property.next_sibling())
			{
				std::string name = property.attribute("name").as_string();

				if (name == "dialog")
				{
					npc->Dialog = property.attribute("value").as_string();
				}
				if (name == "name")
				{
					npc->Name = property.attribute("value").as_string();
				}
				if (name == "sprite")
				{
					npc->LoadImage(renderer, "gfx/" + (std::string)(property.attribute("value").as_string()));
				}
				if (name == "width")
				{
					npc->Width = property.attribute("value").as_int();
				}
				if (name == "height")
				{
					npc->Height = property.attribute("value").as_int();
				}
			}
			entities.push_back(npc);
		}
		if (name == "zombie_critter")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int mapid = object.attribute("id").as_int();
			ZombieCritter* crit = new ZombieCritter(x, y);
			crit->killID.MapID = mapid;
			crit->killID.Map = currentmap;
			crit->LoadImage(renderer, "gfx/zombie_critter.bmp");
			entities.push_back(crit);
		}
		if (name == "big_zombie_critter")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int mapid = object.attribute("id").as_int();
			BigZombieCritter* crit = new BigZombieCritter(x, y);
			crit->killID.MapID = mapid;
			crit->killID.Map = currentmap;
			crit->LoadImage(renderer, "gfx/big_zombie_critter.bmp");
			entities.push_back(crit);
		}
		if (name == "fire_critter")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int mapid = object.attribute("id").as_int();
			FireCritter* crit = new FireCritter(x, y);
			crit->killID.MapID = mapid;
			crit->killID.Map = currentmap;
			crit->LoadImage(renderer, "gfx/fire_critter.bmp");
			entities.push_back(crit);
		}
		if (name == "warp")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int w = object.attribute("width").as_int();
			int h = object.attribute("height").as_int();

			std::string targetlevel = "";
			int targetwarpid;
			int warpid;
			for (pugi::xml_node property = object.child("properties").first_child(); property; property = property.next_sibling())
			{
				std::string name = property.attribute("name").as_string();

				if (name == "target_level")
				{
					targetlevel = property.attribute("value").as_string();
				}
				if (name == "target_warp")
				{
					targetwarpid = property.attribute("value").as_int();
				}
				if (name == "warp_id")
				{
					warpid = property.attribute("value").as_int();
				}
			}

			Warp wr = Warp(warpid, targetwarpid, targetlevel, { x, y, w, h });
			warps.push_back(wr);

		}
		if (name == "item")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int mapid = object.attribute("id").as_int();
			int w, h;
			std::string item_id, item_name, sprite;
			for (pugi::xml_node property = object.child("properties").first_child(); property; property = property.next_sibling())
			{
				std::string name = property.attribute("name").as_string();

				if (name == "item_id")
				{
					item_id = property.attribute("value").as_string();
				}
				if (name == "item_name")
				{
					item_name = property.attribute("value").as_string();
				}
				if (name == "sprite")
				{
					sprite = property.attribute("value").as_string();
				}
				if (name == "width")
				{
					w = property.attribute("value").as_int();
				}
				if (name == "height")
				{
					h = property.attribute("value").as_int();
				}
			}

			Item* i = new Item(x, y, item_name, item_id);
			i->Width = w;
			i->Height = h;
			i->killID.MapID = mapid;
			i->killID.Map = currentmap;
			i->LoadImage(renderer, "gfx/" + sprite);
			entities.push_back(i);

		}
		if (name == "minor_health")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int mapid = object.attribute("id").as_int();

			MinorHealth* i = new MinorHealth(x, y, "Minor Health");
			i->Width = 6;
			i->Height = 7;
			i->killID.MapID = mapid;
			i->killID.Map = currentmap;
			i->LoadImage(renderer, "gfx/minor_health.bmp");
			entities.push_back(i);

		}
		if (name == "max_health")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int mapid = object.attribute("id").as_int();

			MaximumHealth* i = new MaximumHealth(x, y, "Max Health");
			i->Width = 9;
			i->Height = 9;
			i->killID.MapID = mapid;
			i->killID.Map = currentmap;
			i->LoadImage(renderer, "gfx/max_health.bmp");
			entities.push_back(i);

		}
		if (name == "info_message")
		{
			std::string text = "";
			for (pugi::xml_node property = object.child("properties").first_child(); property; property = property.next_sibling())
			{
				std::string name = property.attribute("name").as_string();

				if (name == "text")
				{
					text = property.attribute("value").as_string();
				}
			}
			PostMessage(text);
		}
		if (name == "static_sprite")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			std::string sprite;
			int w, h;
			bool solid;
			for (pugi::xml_node property = object.child("properties").first_child(); property; property = property.next_sibling())
			{
				std::string name = property.attribute("name").as_string();

				if (name == "sprite")
				{
					sprite = property.attribute("value").as_string();
				}
				if (name == "width")
				{
					w = property.attribute("value").as_int();
				}
				if (name == "height")
				{
					h = property.attribute("value").as_int();
				}
				if (name == "solid")
				{
					solid = property.attribute("value").as_bool();
				}
			}
			Entity* e = new Entity(&entities);
			e->X = x;
			e->Y = y;
			e->Width = w;
			e->Height = h;
			e->Solid = solid;
			e->LoadImage(renderer, "gfx/"+sprite);
			entities.push_back(e);
		}

			
			

	}
	for (int i = 0; i < warps.size(); i++)
	{
		if (warps[i].WarpID == player->TargetWarp)
		{
			player->X = warps[i].Rect.x + warps[i].Rect.w / 2 - player->Width * SCALE / 2;
			player->Y = warps[i].Rect.y - player->Height * SCALE + warps[i].Rect.h / 2;
			break;
		}
	}
}


void LoadGame()
{
	
	std::ifstream saveFile("save.bin", std::ios::in | std::ios::binary);
	if (saveFile.is_open())
	{
		ClearEntities();
		
		
		saveFile.close();
	}
	else
	{
		PostMessage("No saved game!");
	}
}

void SaveGame()
{
	// Save game
	std::ofstream saveFile("save.bin", std::ios::out | std::ios::binary);

	

	saveFile.close();
}
void ExecuteCommand(std::string command)
{
	if (command == "clear")
	{
		cons.Lines.clear();
	}

	if (command == "load")
	{
		LoadGame();
	}

	if (command == "save")
	{
		SaveGame();
	}
	std::stringstream ss = std::stringstream(command);
	std::string s;
	std::vector<std::string> v;
	while (getline(ss, s, ' ')) {
		v.push_back(s);
	}
	if (v[0] == "warp")
	{
		if (v.size() != 3)
			cons.Line("Usage: warp [map] [spawn warp id]");
		else
		{
			player->TargetWarp = atoi(v[2].c_str());
			LoadLevel(v[1]);		
			player->JustWarped = true;

		}
	}
	if (v[0] == "killall")
	{
		if (v.size() != 2)
			cons.Line("Usage: killall [id]");
		else
		{
			for (int i = 0; i < entities.size(); i++)
			{
				if (entities[i]->id == v[1])
					entities[i]->Alive = false;
			}
		}
	}
	if (v[0] == "give")
	{
		if (v.size() != 3)
			cons.Line("Usage: give [name] [id]");
		else
		{
			HeldItem h = HeldItem(v[1], v[2]);
			player->AddItem(h);
		}
	}
}

void GenerateBloodSplatter(double x, double y, double angle)
{
	for (int i = 0; i < rand() % 100 + 50; i++)
	{
		double a = rand();
		a *= 180 / M_PI;
		a += rand() % 30 - 15;
		a *= M_PI / 180;
		double nx = x + cos(angle + a) * (rand() % 50);
		double ny = y + sin(angle + a) * (rand() % 50);
		BloodParticle* b = new BloodParticle(nx, ny);
		entities.push_back(b);
	}
}

void Respawn()
{
	player->TargetWarp = -1;
	LoadLevel("map.tmx");
	SetGameState(GameStates::Playing);
	player->Health = player->MaxHealth;
	player->Inventory.clear();
	player->SelectedItem = -1;
	player->NextAttack = -1;
	killedmapid.clear();
}

void DrawMap(std::vector<Chunk*> ch)
{
	for (int i = 0; i < ch.size(); i++)
	{

		Chunk* c = ch[i];

		for (int x = 0; x < c->Width; x++)
		{
			for (int y = 0; y < c->Height; y++)
			{
				const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
				const unsigned FLIPPED_VERTICALLY_FLAG = 0x40000000;
				const unsigned FLIPPED_DIAGONALLY_FLAG = 0x20000000;
				const unsigned ROTATED_90_FLAG = 0xA0000000;
				const unsigned ROTATED_180_FLAG = 0xC000000;
				const unsigned ROTATED_270_FLAG = 0x6000000;

				unsigned j = c->Data[y][x];

				// XXX: Only flips vertically
				bool flipHorizontal = (j & FLIPPED_HORIZONTALLY_FLAG);
				bool flipVertical = (j & FLIPPED_VERTICALLY_FLAG);
				bool flipDiagonal = (j & FLIPPED_DIAGONALLY_FLAG);

				j = j & ~(FLIPPED_HORIZONTALLY_FLAG |
					FLIPPED_VERTICALLY_FLAG |
					FLIPPED_DIAGONALLY_FLAG);

				SDL_RendererFlip flip = SDL_FLIP_NONE;
				int angle = 0;

				if (flipHorizontal) flip = SDL_FLIP_HORIZONTAL;
				if (flipVertical) flip = SDL_FLIP_VERTICAL;
				if (flipDiagonal) flip = (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
				if (flipDiagonal && flipHorizontal) angle = 90;
				if (flipDiagonal && flipVertical) angle = 270;

				if (j == 0) continue; j -= 1;

				/*
				double nx = x * 32 + c->X * 32;
				double ny = y * 32 + c->Y * 32;
				if (nx < player->X - viewport.Width / 2 - 32 ||
					nx > player->X + viewport.Width / 2 ||
					ny < player->Y - viewport.Height / 2 - 64 ||
					ny > player->Y + viewport.Height / 2) continue;
				*/
				SDL_Rect dest = { x * 32 + viewport.X + c->X * 32, y * 32 + viewport.Y + c->Y * 32, 32, 32 };
				SDL_Rect src = { (j % 16) * 32, (j / 16) * 32, 32, 32 };
				SDL_RenderCopyEx(renderer, tex_tilesheet, &src, &dest, angle, NULL, flip);
			}
		}
	}
}

class MainMenu
{
public:
	std::vector<std::string> Options;
	int SelectedOption;
	MainMenu()
	{
		Options.push_back("New Game");
		Options.push_back("Load Game");
		Options.push_back("Options");
		Options.push_back("Quit");
		SelectedOption = 0;
	}
	void Update()
	{
		if (keys[SDLK_w])
		{
			keys[SDLK_w] = false;
			SelectedOption--;
		}
		if (keys[SDLK_s])
		{
			keys[SDLK_s] = false;
			SelectedOption++;
		}

		if (keys[SDLK_RETURN] || keys[SDLK_SPACE])
		{
			switch (SelectedOption)
			{
			case 0:
				Respawn();
				break;
			case 1:
				LoadGame();
				break;
			case 2:
				SetGameState(GameStates::Options);
				break;
			case 3:
				quit = true;
				break;
			}
		}
		keys[SDLK_RETURN] = false;
		keys[SDLK_SPACE] = false;

		if (SelectedOption < 0) SelectedOption = Options.size() - 1;
		if (SelectedOption >= Options.size()) SelectedOption = 0;
	}
	void Draw(SDL_Renderer* ren, TTF_Font* font)
	{
		for (int i = 0; i < Options.size(); i++)
		{
			SDL_Surface * s = TTF_RenderText_Solid(font,
				Options[i].c_str(), { 255, 0, 0 });
			int w = s->w;
			int h = s->h;
			SDL_Rect expr = { SCRW / 2 - w / 2, SCRH / 2 + i * 50, w, h };

			if (i == SelectedOption)
			{
				SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
				SDL_RenderFillRect(ren, &expr);
			}

			SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
			SDL_FreeSurface(s);
			SDL_RenderCopy(renderer, t, NULL, &expr);
			SDL_DestroyTexture(t);
		}
	}
};
MainMenu mainmenu;

class PauseMenu
{
public:
	std::vector<std::string> Options;
	int SelectedOption;
	PauseMenu()
	{
		Options.push_back("Continue");
		Options.push_back("Save");
		Options.push_back("Load");
		Options.push_back("Options");
		Options.push_back("Quit");
		SelectedOption = 0;
	}
	void Update()
	{
		if (keys[SDLK_w])
		{
			keys[SDLK_w] = false;
			SelectedOption--;
		}
		if (keys[SDLK_s])
		{
			keys[SDLK_s] = false;
			SelectedOption++;
		}

		if (keys[SDLK_RETURN] || keys[SDLK_SPACE])
		{
			switch (SelectedOption)
			{
			case 0:
				SetGameState(GameStates::Playing);
				break;
			case 1:
				SaveGame();
				break;
			case 2:
				LoadGame();
				break;
			case 3:
				SetGameState(GameStates::Options);
				break;
			case 4:
				quit = true;
			}
		}
		keys[SDLK_RETURN] = false;
		keys[SDLK_SPACE] = false;

		if (SelectedOption < 0) SelectedOption = Options.size() - 1;
		if (SelectedOption >= Options.size()) SelectedOption = 0;
	}
	void Draw(SDL_Renderer* ren, TTF_Font* font)
	{
		for (int i = 0; i < Options.size(); i++)
		{
			SDL_Surface * s = TTF_RenderText_Solid(font,
				Options[i].c_str(), { 255, 0, 0 });
			int w = s->w;
			int h = s->h;
			SDL_Rect expr = { SCRW / 2 - w / 2, SCRH / 2 + i * 50, w, h };

			if (i == SelectedOption)
			{
				SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
				SDL_RenderFillRect(ren, &expr);
			}

			SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
			SDL_FreeSurface(s);
			SDL_RenderCopy(renderer, t, NULL, &expr);
			SDL_DestroyTexture(t);
		}
	}
};
PauseMenu pausemenu;

class OptionsMenu
{
public:
	std::vector<std::string> Options;
	int SelectedOption;
	OptionsMenu()
	{
		UpdateOptions();
	}
	void UpdateOptions()
	{
		Options.clear();
		std::stringstream ss;
		ss << "Window mode: ";
		ss << (FULLSCRN ? "Fullscreen" : "Windowed");
		Options.push_back(ss.str());
		Options.push_back("Back");
		SelectedOption = 0;
	}
	void Update()
	{
		
		if (keys[SDLK_w])
		{
			
			keys[SDLK_w] = false;
			SelectedOption--;
		}
		if (keys[SDLK_s])
		{
			keys[SDLK_s] = false;
			SelectedOption++;
		}

		if (keys[SDLK_RETURN] || keys[SDLK_SPACE])
		{
			switch (SelectedOption)
			{
			case 0:
				FULLSCRN = !FULLSCRN;
				keys[SDLK_RETURN] = false;
				keys[SDLK_SPACE] = false;
				SDL_SetWindowFullscreen(window, FULLSCRN);
				UpdateOptions();
				break;
			case 1:
				keys[SDLK_RETURN] = false;
				keys[SDLK_SPACE] = false;
				SetGameState(lastgamestate);
				break;
			}
		}

		if (SelectedOption < 0) SelectedOption = Options.size() - 1;
		if (SelectedOption >= Options.size()) SelectedOption = 0;
	}
	void Draw(SDL_Renderer* ren, TTF_Font* font)
	{
		for (int i = 0; i < Options.size(); i++)
		{
			SDL_Surface * s = TTF_RenderText_Solid(font,
				Options[i].c_str(), { 255, 0, 0 });
			int w = s->w;
			int h = s->h;
			SDL_Rect expr = { SCRW / 2 - w / 2, SCRH / 2 + i * 50, w, h };

			if (i == SelectedOption)
			{
				SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
				SDL_RenderFillRect(ren, &expr);
			}

			SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
			SDL_FreeSurface(s);
			SDL_RenderCopy(renderer, t, NULL, &expr);
			SDL_DestroyTexture(t);
		}
	}
};
OptionsMenu optionsmenu;


int main(int argc, char ** argv)
{
	srand(time(NULL));
	entities.clear();

	
	SDL_Event event;
	int x = 288;
	int y = 208;

	// init SDL

	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	window = SDL_CreateWindow("Shadow of the Chungus",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCRW, SCRH, 0);
	SDL_SetWindowFullscreen(window, FULLSCRN);
	renderer = SDL_CreateRenderer(window, -1, 0);

	player = new Player(&entities);
	inventory = Inventory(player);
	cursor = new Cursor(&entities, renderer);
	
	player->LoadImage(renderer, "gfx/player.bmp");

	viewport.Width = SCRW;
	viewport.Height = SCRH;

	SDL_Surface* surf = SDL_LoadBMP("gfx/tilesheet.bmp");
	tex_tilesheet = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);

	entities.push_back(player);
	
	LoadLevel("map.tmx");

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_ShowCursor(SDL_DISABLE);
	

	Uint32 minimum_fps_delta_time = (1000 / 30); // minimum 6 fps, if the computer is slower than this: slow down.
	Uint32 last_game_step = SDL_GetTicks(); // initial value

	TTF_Font * expfont = TTF_OpenFont("fonts/arial.ttf", 9);
	monsterfont = TTF_OpenFont("fonts/arial.ttf", 12);
	TTF_Font* deathfont = TTF_OpenFont("fonts/lunchds.ttf", 36);
	msgfont = TTF_OpenFont("fonts/biscuit.ttf", 17);
	TTF_Font* titlefont = TTF_OpenFont("fonts/lunchds.ttf", 42);
	TTF_Font* menufont = TTF_OpenFont("fonts/lunchds.ttf", 36);
	TTF_Font* dmgfont = TTF_OpenFont("fonts/dmg.ttf", 36);

	SDL_StartTextInput();
	while (!quit)
	{
		Uint32 now = SDL_GetTicks();
		previousmousestate = currentmousestate;
		SDL_GetMouseState(&currentmousestate.X, &currentmousestate.Y);
		while (SDL_PollEvent(&event)) {

			switch (event.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_TEXTINPUT:
				if (cons.Toggled)
				{
					if (event.text.text[0] != SDLK_BACKQUOTE && !((event.text.text[0] == 'c' || event.text.text[0] == 'C') && (event.text.text[0] == 'v' || event.text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL))
					{
						//Append character
						cons.Buffer += event.text.text;
					}
				}
				break;
			case SDL_KEYDOWN:
				if (gamestate == Death)
				{
					Respawn();
				}
				else
				{
					if (event.key.keysym.sym < 1024)
						keys[event.key.keysym.sym] = true;
				}
					if (event.key.keysym.sym == SDLK_BACKQUOTE)
					{
						cons.Toggled = !cons.Toggled; continue;
					}
					if (keys[SDLK_ESCAPE])
					{
						keys[SDLK_ESCAPE] = false;
						if (gamestate == Playing)
							SetGameState(GameStates::Paused);
						else if (gamestate = Paused)
							SetGameState(GameStates::Playing);
					}
					if (cons.Toggled)
					{

						int sym = event.key.keysym.sym;
						if (sym == SDLK_RETURN && cons.Buffer != "")
						{
							cons.Line(cons.Buffer);
							ExecuteCommand(cons.Buffer);
							cons.Buffer = "";
						}
						//Handle backspace
						if (event.key.keysym.sym == SDLK_BACKSPACE && cons.Buffer.length() > 0)
						{
							//lop off character
							cons.Buffer.pop_back();
						}

					}
				break;
			case SDL_KEYUP:
				if (event.key.keysym.sym < 1024)
					keys[event.key.keysym.sym] = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					currentmousestate.LeftDown = true;
					if (!cons.Toggled && gamestate == Playing && player->GetSelectedItemId() == "pistol")
					{
						// Draw bullet from player to cursor
						int x, y;
						SDL_GetMouseState(&x, &y);
						Bullet* bl = new Bullet(&entities, atan2(player->Y + viewport.Y - y, player->X + viewport.X - x) + M_PI);
						bl->LoadImage(renderer, "gfx/" + (std::string)"bullet.bmp");
						bl->X = player->X;
						bl->Y = player->Y;
						entities.push_back(bl);

					}
					else if (player->GetSelectedItemId() == "uzi")
					{
						player->NextAttack = 0;
					}
				}
				else if (event.button.button == SDL_BUTTON_RIGHT)
				{
					currentmousestate.RightDown = true;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					currentmousestate.LeftDown = false;
					if (gamestate == Playing)
					{
						player->NextAttack = -1;
					}
				}
				else if (event.button.button == SDL_BUTTON_RIGHT)
				{
					currentmousestate.RightDown = false;
				}
				break;
			case SDL_MOUSEWHEEL:
				if (player->Inventory.size() > 1)
				{
					player->SelectedItem += event.wheel.y;
					player->SelectedItem %= player->Inventory.size();
				}
				break;
			}
		}
		
		if (last_game_step < now) {

			Uint32 delta_time = now - last_game_step;

			if (delta_time > minimum_fps_delta_time) {
				delta_time = minimum_fps_delta_time; // slow down if the computer is too slow
			}

			// Pointer modes
			if (gamestate == Playing)
			{
				if (player->GetSelectedItemId() == "pistol" ||
					player->GetSelectedItemId() == "uzi")
					cursor->Mode = Crosshair;
				else
					cursor->Mode = Pointer;
			}
			if (gamestate == Death || gamestate == TitleMenu || gamestate == Paused)
				cursor->Mode = None;

			// Order entites by y
			std::sort(entities.begin(), entities.end(), [](Entity* a, Entity* b) {return a->Y + a->Height * SCALE < b->Y + b->Height * SCALE; });
			std::sort(entities.begin(), entities.end(), [](Entity* a, Entity* b) {return a->Z < b->Z; });
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);

			if (player->Health <= 0)
			{
				SetGameState(GameStates::Death);
			}

			if (gamestate == Playing)
			{

				// Check if player hit a warp
				for (int i = 0; i < warps.size(); i++)
				{
					// Get player's feet
					double px = player->X + player->Width * SCALE / 2;
					double py = player->Y + player->Height * SCALE;
					SDL_Point p = { px, py };
					if (SDL_PointInRect(&p, &warps[i].Rect) && !player->JustWarped)
					{
						
						player->TargetWarp = warps[i].TargetWarpID;
						player->JustWarped = true;
						LoadLevel(warps[i].TargetLevel);
					}
				}
				
				bool touchingAnyWarp = false;
				for (int i = 0; i < warps.size(); i++)
				{
					// Get player's feet
					double px = player->X + player->Width * SCALE / 2;
					double py = player->Y + player->Height * SCALE;
					SDL_Point p = { px, py };
					if (SDL_PointInRect(&p, &warps[i].Rect))
					{
						touchingAnyWarp = true;
					}
				}
				if (!touchingAnyWarp) player->JustWarped = false;

				DrawMap(chunks);
				
				for (unsigned int i = 0; i < entities.size(); i++)
				{
					if (!entities[i]->Alive) continue;

					if (!cons.Toggled)
					{
						entities[i]->Update(delta_time);

						// Check if the entity has been clicked on 
						SDL_Point mouse_pt = { currentmousestate.X, currentmousestate.Y };
						SDL_Rect mouse_r = { entities[i]->X + viewport.X, entities[i]->Y + viewport.Y, entities[i]->Width * SCALE, entities[i]->Height * SCALE };
						if ((currentmousestate.RightDown == true && previousmousestate.RightDown == false) &&
							SDL_PointInRect(&mouse_pt, &mouse_r))
						{
							entities[i]->OnClick();
						}
						// Check against every hurt
						for (int j = 0; j < hurt.size(); j++)
						{
							SDL_Point p = { entities[i]->X + entities[i]->Width, entities[i]->Y + entities[i]->Height };
							SDL_Rect b = hurt[j];
							if (SDL_PointInRect(&p, &b))
							{
								entities[i]->Health -= entities[i]->MaxHealth * 0.0005f;
							}
						}
						// Check against every and entity
						if (entities[i]->Solid)
						{
							for (int j = 0; j < entities.size(); j++)
							{
								if (entities[j]->id == "player") continue;
								if (i == j) continue;
								if (!entities[j]->Solid) continue;

								SDL_Rect a = { entities[i]->X, entities[i]->Y, entities[i]->Width * entities[i]->SCALE, entities[i]->Height * entities[i]->SCALE };
								SDL_Rect b = { entities[j]->X, entities[j]->Y, entities[j]->Width * entities[j]->SCALE, entities[j]->Height * entities[j]->SCALE };
								SDL_Rect r;

								double amidx = a.x + a.w / 2;
								double amidy = a.y + a.h / 2;
								double bmidx = b.x + b.w / 2;
								double bmidy = b.y + b.h / 2;
								if (SDL_IntersectRect(&a, &b, &r))
								{
									if (r.w < r.h) // Solive horizontally
									{
										if (amidx < bmidx) // If to the left then push left
										{
											entities[i]->X = b.x - a.w;
										}
										else
										{
											entities[i]->X = b.x + b.w;
										}
									}
									if (r.w > r.h) // Solve vertically
									{
										if (amidy < bmidy) // If above push up
										{
											entities[i]->Y = b.y - a.h;
										}
										else
										{
											entities[i]->Y = b.y + b.h;
										}
									}
								}
							}

							for (int j = 0; j < solids.size(); j++)
							{
								if (entities[i]->id == "player")
								{
									if (player->HasItem(solids[j].NeedsKey)) continue;
								}
								SDL_Rect a = { entities[i]->X, entities[i]->Y, entities[i]->Width * entities[i]->SCALE, entities[i]->Height * entities[i]->SCALE + 1 };
								SDL_Rect b = solids[j].Rect;
								SDL_Rect r;

								double amidx = a.x + a.w / 2;
								double amidy = a.y + a.h / 2;
								double bmidx = b.x + b.w / 2;
								double bmidy = b.y + b.h / 2;
								if (SDL_IntersectRect(&a, &b, &r))
								{
									if (r.w < r.h) // Solive horizontally
									{
										if (amidx < bmidx) // If to the left then push left
										{
											entities[i]->X = b.x - a.w;
										}
										else
										{
											entities[i]->X = b.x + b.w;
										}
									}
									if (r.w > r.h) // Solve vertically
									{
										if (amidy < bmidy) // If above push up
										{
											entities[i]->Y = b.y - a.h;
										}
										else
										{
											entities[i]->Y = b.y + b.h;
										}
									}
								}
							}
						}
					}
					entities[i]->Draw(renderer);
				}
				
			}
			else if (gamestate == Death)
			{
				std::string s = "You died!";
				SDL_Surface * dsurface = TTF_RenderText_Solid(deathfont,
					s.c_str(), { 255, 0, 0 });
				int dw = dsurface->w;
				int dh = dsurface->h;
				SDL_Rect expr = { SCRW / 2 - dw / 2, SCRH / 2 - dh / 2, dw, dh };
				SDL_Texture* dtex = SDL_CreateTextureFromSurface(renderer, dsurface);
				SDL_FreeSurface(dsurface);
				SDL_RenderCopy(renderer, dtex, NULL, &expr);
				SDL_DestroyTexture(dtex);
			}
			else if (gamestate == TitleMenu)
			{
				// Draw title menu
				SDL_Surface * s = TTF_RenderText_Solid(titlefont,
					"Shadow of the Chungus", { 255, 0, 0 });
				int w = s->w;
				int h = s->h;
				SDL_Rect expr = { SCRW / 2 - w / 2, SCRH / 4, w, h };
				SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
				SDL_FreeSurface(s);
				SDL_RenderCopy(renderer, t, NULL, &expr);
				SDL_DestroyTexture(t);

				mainmenu.Update();
				mainmenu.Draw(renderer, menufont);
			}
			else if (gamestate == GameStates::Options)
			{
			// Draw title menu
			SDL_Surface * s = TTF_RenderText_Solid(titlefont,
				"Options", { 255, 0, 0 });
			int w = s->w;
			int h = s->h;
			SDL_Rect expr = { SCRW / 2 - w / 2, SCRH / 4, w, h };
			SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
			SDL_FreeSurface(s);
			SDL_RenderCopy(renderer, t, NULL, &expr);
			SDL_DestroyTexture(t);

			optionsmenu.Update();
			optionsmenu.Draw(renderer, menufont);
			}
			
			else if (gamestate == Paused)
			{
				// Draw title menu
				SDL_Surface * s = TTF_RenderText_Solid(titlefont,
					"Shadow of the Chungus", { 255, 0, 0 });
				int w = s->w;
				int h = s->h;
				SDL_Rect expr = { SCRW / 2 - w / 2, SCRH / 4, w, h };
				SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
				SDL_FreeSurface(s);
				SDL_RenderCopy(renderer, t, NULL, &expr);
				SDL_DestroyTexture(t);

				pausemenu.Update();
				pausemenu.Draw(renderer, menufont);
			}
			// Draw console
			double dx = player->X - cursor->X + viewport.X;
			double dy = player->Y - cursor->Y + viewport.Y;
			double d = 500;

			if (dx > d) dx = d;
			if (dx < -d) dx = -d;
			if (dy > d) dy = d;
			if (dy < -d) dy = -d;

			dx /= 9;
			dy /= 9;

			viewport.X = -player->X + SCRW / 2 + player->Width / 2 + dx;
			viewport.Y = -player->Y +SCRH / 2 + player->Height / 2 + dy;

			
			cursor->Update(delta_time);

			// Draw player inventory
			if (gamestate == Playing)
			{
				inventory.Update(delta_time);
				inventory.Draw(renderer, msgfont);

				// Draw health on left side of screen
				int hpw = 150;
				int hph = 10;
				int hpx = 10;
				int hpy = 10;
				int outline = 3;
				SDL_Rect r = { 10, 10, hpw, hph };
				SDL_Rect outlr = { 10 - outline, 10 - outline, hpw + outline * 2, hph + outline * 2 };
				SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
				SDL_RenderFillRect(renderer, &outlr);
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				SDL_RenderFillRect(renderer, &r);
				r.w = hpw * ((double)player->Health / player->MaxHealth);
				if (r.w <= 0) r.w = 0;
				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
				SDL_RenderFillRect(renderer, &r);
			}

			cursor->Draw(renderer);
			if (cons.Toggled)
			{
				for (unsigned int i = 0; i < cons.Lines.size(); i++)
				{
					if (cons.Lines[i] == "") continue;

					SDL_Surface * lnsurface = TTF_RenderText_Solid(expfont,
						cons.Lines[i].c_str(), { 255, 255, 255 });
					if (!lnsurface) continue;
					int lnw = lnsurface->w;
					int lnh = lnsurface->h;
					SDL_Rect expr = { 0, lnh * i, lnw, lnh };
					SDL_Texture* lntex = SDL_CreateTextureFromSurface(renderer, lnsurface);
					SDL_FreeSurface(lnsurface);
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
					SDL_RenderFillRect(renderer, &expr);
					SDL_RenderCopy(renderer, lntex, NULL, &expr);
					SDL_DestroyTexture(lntex);
				}

				if (cons.Buffer != "")
				{
					SDL_Surface * lnsurface = TTF_RenderText_Solid(expfont,
						cons.Buffer.c_str(), { 255, 255, 255});
					int lnw = lnsurface->w;
					int lnh = lnsurface->h;
					SDL_Rect expr = { 0, SCRH - lnh, lnw, lnh };
					SDL_Texture* lntex = SDL_CreateTextureFromSurface(renderer, lnsurface);
					SDL_FreeSurface(lnsurface);
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
					SDL_RenderFillRect(renderer, &expr);
					SDL_RenderCopy(renderer, lntex, NULL, &expr);
					SDL_DestroyTexture(lntex);
				}
			}
			
			// Draw score
			/*
			std::stringstream ss;
			ss << player->DamageInflicted;
			SDL_Surface * lnsurface = TTF_RenderText_Solid(dmgfont,
				ss.str().c_str(), { 0, 255, 0 });
			int lnw = lnsurface->w;
			int lnh = lnsurface->h;
			SDL_Rect r = { SCRW - lnw, 0, lnw, lnh };
			SDL_Texture* lntex = SDL_CreateTextureFromSurface(renderer, lnsurface);
			SDL_FreeSurface(lnsurface);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderFillRect(renderer, &r);
			SDL_RenderCopy(renderer, lntex, NULL, &r);
			SDL_DestroyTexture(lntex);
			*/
			/*
			// Draw experience
			std::stringstream ss;
			ss << "Experience: " << player->Experience << "/" << player->NextExperience;
			SDL_Surface * expsurface = TTF_RenderText_Solid(expfont,
				ss.str().c_str(), { 0, 0, 0 });
			int expw = expsurface->w;
			int exph = expsurface->h;
			SDL_Rect expr = { SCRW - expw, 0, expw, exph };
			SDL_Texture* exptex = SDL_CreateTextureFromSurface(renderer, expsurface);
			SDL_FreeSurface(expsurface);
			SDL_RenderCopy(renderer, exptex, NULL, &expr);
			// Draw level
			ss = std::stringstream();
			ss << "Level " << player->Level;
			SDL_Surface * lvlsurface = TTF_RenderText_Solid(expfont,
				ss.str().c_str(), { 0, 0, 0 });
			int lvlw = expsurface->w;
			int lvlh = expsurface->h;
			SDL_Rect lvlr = { SCRW - lvlw, exph, lvlw, lvlh };
			SDL_Texture* lvltex = SDL_CreateTextureFromSurface(renderer, lvlsurface);
			SDL_FreeSurface(lvlsurface);
			SDL_RenderCopy(renderer, lvltex, NULL, &lvlr);
			*/

			
			
			// Remove non alive entities
			std::vector<Entity*> ents;
			for (unsigned int i = 0; i < entities.size(); i++)
			{
				if (entities[i]->Alive)
				{
					ents.push_back(entities[i]);
				}
				else
				{
					if (!entities[i]->Respawn && entities[i]->killID.MapID != -1 && !(std::find(killedmapid.begin(), killedmapid.end(), entities[i]->killID) != killedmapid.end()))
					{
						killedmapid.push_back(entities[i]->killID);
					}
					delete(entities[i]);
				}

			}
			entities = ents;

			SDL_RenderPresent(renderer);

			last_game_step = now;
		}
		else {
			
		}

	}

	// cleanup SDL
	ClearEntities();
	SDL_DestroyTexture(tex_tilesheet);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_CloseFont(deathfont);
	TTF_CloseFont(monsterfont);
	TTF_CloseFont(expfont);
	TTF_CloseFont(msgfont);
	TTF_CloseFont(titlefont);
	TTF_CloseFont(menufont);
	TTF_CloseFont(dmgfont);
	TTF_Quit();
	SDL_Quit();

	return 0;
}