#include "pch.h"

#include <SDL.h>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
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
#include <map>
#include <boost/tokenizer.hpp>
#include "Entity.h"
#include "Cursor.h"
#include "Console.h"
#include "HeldItem.h"
#include "Warp.h"
#include "Player.h"
#include "Bullet.h"
#include "Solid.h"
#include "BloodParticle.h"
#include "Viewport.h"
#include "Item.h"
#include "NPC.h"
#include "Trigger.h"
#include "MouseState.h"
#include "Gate.h"
#include "SkullKey.h"
#include "MinorHealth.h"
#include "MaximumHealth.h"
#include "Chunk.h"
#include "Fireball.h"
#include "ShadedArea.h"
#include "Shader.h"
#include "Chungus.h"
#pragma comment (lib, "SDL2")
#pragma comment (lib, "SDL2_ttf")
#pragma comment (lib, "glew32")
#pragma comment (lib, "glu32")
#pragma comment (lib, "opengl32")
#pragma comment (lib, "libfbxsdk-md")
Cursor* cursor;

SDL_GLContext mainContext;


TTF_Font * expfont;
TTF_Font* deathfont;
TTF_Font* titlefont;
TTF_Font* menufont;
TTF_Font* dmgfont;
TTF_Font* monsterfont;
TTF_Font* msgfont;

std::vector<SDL_Rect> hurt;
std::vector<SDL_Rect> heal;
std::vector<KillID> killedmapid = std::vector<KillID>();
std::vector<Warp> warps;
std::vector<Entity*> entities;
std::vector<Solid> solids;
std::vector<Trigger> triggers;
std::vector<ShadedArea> shadedareas;
Console cons;

std::map<std::string, bool> flags;
std::map<std::string, SDL_Texture*> textures;

Mix_Chunk* sfx_warp;
Mix_Chunk* sfx_saveload;
Mix_Chunk* sfx_savefailed;

static int SCALE = 2;
static int SCRW = 1280;
static int SCRH = 720;
static bool FULLSCRN = false;
static bool VSYNC = false;
std::string GAMENAME = "Sasha";
static std::map<int, bool> keys;
static SDL_Renderer* renderer;
static Viewport viewport;
static bool quit = false;
static std::string currentmap;
static Player* player;

static SDL_Window* window;
static SDL_Texture* tex_tilesheet;

enum GameStates {
	TitleMenu,
	Playing,
	Death,
	Paused,
	Options
};
static GameStates gamestate = TitleMenu;
static GameStates lastgamestate;
static MouseState currentmousestate, previousmousestate;

void SetGameState(GameStates gs);
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




class Enemy :
	public Entity
{
	bool OnDieExecuted = false;
	bool lineofsightbroken = false;
protected:
	int nextx;
	int nexty;
	int nextmove;
	bool Wander;
	int NextMoveMin;
	int NextMoveMax;
	bool IsAggro;
	double DistToPlayer;

	Player* p;
	std::vector<Solid>* s;
public:
	Enemy(std::vector<Solid>* solids, Player* player)
	{
		s = solids;
		p = player;
		id = "enemy";
		Level = 1;
		Name = "Enemy";
		Action = Standing;
		IsSolid = true;
		Wander = true;
		IsAggro = false;
		MaxVelocity = 0.02;
		NextMoveMin = 5000;
		NextMoveMax = 10000;
		AttackTimeout = 2500;
		AggroDistance = 15000;
		AttackDistance = 600;
		GetNextMove(NextMoveMin, NextMoveMax);
	}
	~Enemy() {}

	virtual void Update(double dt)
	{
		int x = X;
		int y = Y;
		int x2 = player->X;
		int y2 = player->Y;
		lineofsightbroken = false;
		for (int i = 0; i < s->size(); i++)
		{
			if (SDL_IntersectRectAndLine(&((*s)[i].Rect), &x, &y, &x2, &y2))
			{
				lineofsightbroken = true;
			}
		}

		if (Action != Dead)
		{
			double px = player->X + player->Width * SCALE / 2;
			double py = player->Y + player->Height * SCALE;
			DistToPlayer = ((px - X)*(px - X) + (py - Y)*(py - Y));
			if (Wander)
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
						GetNextMove(NextMoveMin, NextMoveMax);
					}
				}
			}
			if (IsAggro)
			{
				if (DistToPlayer > AggroDistance) Action = Standing;
				if (DistToPlayer < AggroDistance && DistToPlayer > AttackDistance)
				{
					Action = Aggro;
				}
				if (DistToPlayer < AttackDistance)
				{
					Action = Attacking;
				}
				if (Action == Aggro && !lineofsightbroken)
				{
					double px = p->X + p->Width * SCALE / 2 - X;
					double py = p->Y + p->Height * 0.75 * SCALE - Y;

					float length = sqrtf(px*px + py * py);

					if (length <= MaxVelocity)
					{
						Action = Standing;
					}

					px /= length;
					py /= length;
					X += px * dt * MaxVelocity;
					Y += py * dt * MaxVelocity;
				}
				if (Action == Aggro || Action == Attacking)
					if (lineofsightbroken)
						Action = Standing;
			}
		}
		if (Health <= 0)
		{
			Health = 0;
			Action = EntityAction::Dead;
			IsSolid = false;
			if (!OnDieExecuted)
			{
				OnDie();
				OnDieExecuted = true;
			}
		}

	}
	virtual void Draw(Viewport v, SDL_Renderer* ren)
	{
		Entity::Draw(viewport, ren);
		if (Health < MaxHealth && Action != EntityAction::Dead)
		{
			DrawHealthBar(viewport, ren);
		}
	}
	virtual void OnDie()
	{

	}
	void GetNextMove(int min, int max)
	{
		nextx = X + cos(rand()) * (rand() % 10);
		nexty = Y + sin(rand()) * (rand() % 10);
		nextmove = SDL_GetTicks() + rand() % max + min;
	}
};
class Critter : public Enemy
{
public:
	Critter(std::vector<Solid>* solids, Player* player, double x, double y) :
		Enemy(solids, player)
	{
		X = x;
		Y = y;
		id = "critter";
		Level = 1;
		Name = "Critter";
		MaxVelocity = 0.02;
		Width = 6;
		Height = 5;
	}
	virtual void Update(double dt)
	{
		Enemy::Update(dt);

		if (Action == EntityAction::Dead) // if dead
		{
			AnimationY = 2;
			AnimationX = 0;
		}
		else
		{
			if (Action == EntityAction::Moving || Action == EntityAction::Aggro) // if moving
			{
				AnimationY = 1;
				AnimationX += 0.01 * dt;
				if (AnimationX >= 2) AnimationX = 0;
			}
			else // If standing
			{
				AnimationY = 0;
				AnimationX = 0;
			}
		}
	}
	
};

class FireCritter : 
	public Enemy
{
public:
	FireCritter(std::vector<Solid>* solids, Player* player, double x, double y) :
		Enemy(solids, player)
	{
		X = x;
		Y = y;
		id = "fire_critter";
		Level = 5;
		Name = "Fire Critter";
		Action = Standing;
		MaxVelocity = 0.06;

		Width = 6;
		Height = 10;
		Health = 2;
		MaxHealth = 2;
		AttackTimeout = 400;
		AggroDistance = 160000;
		AttackDistance = 30000;
		IsAggro = true;
	}
	virtual void Update(double dt)
	{
		Enemy::Update(dt);
		if (Action == EntityAction::Dead) // if dead
		{
			AnimationY = 2;
			AnimationX = 0;
		}
		if (Action == Moving ||
			Action == Aggro)
		{
			AnimationY = 1;
			AnimationX += 0.01 * dt;
			if (AnimationX >= 2) AnimationX = 0;
		}
		if (Action == Standing ||
			Action == Attacking)
		{
			AnimationY = 0;
			AnimationX += 0.01 * dt;
			if (AnimationX >= 2) AnimationX = 0;
		}
		if (Action == EntityAction::Attacking || Action == EntityAction::Aggro)
		{
			if (NextAttack <= SDL_GetTicks())
			{
				// Launch fireball
				Fireball* fb = new Fireball(&solids, player, renderer, &entities, X, Y, player->X + player->Width * SCALE / 2, player->Y + player->Height * SCALE / 2);
				fb->LoadImage(renderer, "gfx/fireball.bmp");
				entities.push_back(fb);
				NextAttack = SDL_GetTicks() + AttackTimeout;
			}
		}
	}
};

class ZombieCritter : 
	public Enemy
{
public:
	ZombieCritter(std::vector<Solid>* solids, Player* player, double x, double y) :
		Enemy(solids, player)
	{
		X = x;
		Y = y;
		id = "zombie_critter";
		Level = 5;
		Name = "Zombie Critter";
		Action = Standing;
		Width = 6;
		Height = 5;
		Attack = 1;
		Health = 2;
		MaxHealth = 2;
		AttackTimeout = 2500;
		AggroDistance = 15000;
		AttackDistance = 600;
		Respawn = true;
		IsAggro = true;
	}
	virtual void Update(double dt)
	{
		Enemy::Update(dt);

		if (Action == EntityAction::Standing) // if STANDING
		{
			AnimationY = 0;
			AnimationX = 0;
		}
		if (Action == EntityAction::Dead) // if dead
		{
			AnimationY = 2;
			AnimationX = 0;
		}
		if (Action == Aggro)
		{
			AnimationY = 1;
			AnimationX += 0.01 * dt;
			if (AnimationX >= 2) AnimationX = 0;
		}
		if (Action == Attacking)
		{
			AnimationY = 0;
			AnimationX = 0;

			if (SDL_GetTicks() >= NextAttack)
			{
				p->Health -= AttackDamage;
				NextAttack = SDL_GetTicks() + AttackTimeout;
			}
		}
			
	}
	virtual void Draw(Viewport v, SDL_Renderer* ren)
	{
		Entity::Draw(viewport, ren);
		if (Health < MaxHealth && Action != EntityAction::Dead)
		{
			DrawHealthBar(viewport, ren);
		}
	}
};

class Skeleton :
	public Enemy
{
public:
	Skeleton(std::vector<Solid>* solids, Player* player, double x, double y) :
		Enemy(solids, player)
	{
		X = x;
		Y = y;
		id = "skeleton";
		Level = 5;
		Name = "Skeleton";
		Action = Standing;
		Width = 7;
		Height = 16;
		AttackDamage = 3;
		Health = 5;
		MaxHealth = 5;
		MaxVelocity = 0.12f;
		AttackTimeout = 2500;
		AggroDistance = 28000;
		AttackDistance = 600;
		Respawn = true;
		IsAggro = true;
		BloodSplatter = false;
	}
	virtual void Update(double dt)
	{
		Enemy::Update(dt);

		if (Action == EntityAction::Standing) // if STANDING
		{
			AnimationY = 0;
			AnimationX = 0;
		}
		if (Action == EntityAction::Dead) // if dead
		{
			AnimationY = 2;
			AnimationX = 0;
		}
		if (Action == Aggro)
		{
			AnimationY = 1;
			AnimationX += 0.01 * dt;
			if (AnimationX >= 2) AnimationX = 0;
		}
		if (Action == Attacking)
		{
			AnimationY = 0;
			AnimationX = 0;

			if (SDL_GetTicks() >= NextAttack)
			{
				p->Health -= AttackDamage;
				NextAttack = SDL_GetTicks() + AttackTimeout;
			}
		}

	}
	virtual void Draw(Viewport v, SDL_Renderer* ren)
	{
		Entity::Draw(viewport, ren);
		if (Health < MaxHealth && Action != EntityAction::Dead)
		{
			DrawHealthBar(viewport, ren);
		}
	}
};

class Zombie :
	public Enemy
{
public:
	Zombie(std::vector<Solid>* solids, Player* player, double x, double y) :
		Enemy(solids, player)
	{
		X = x;
		Y = y;
		id = "zombie";
		Level = 5;
		Name = "Zombie";
		Action = Standing;
		Width = 7;
		Height = 16;
		AttackDamage = 3;
		Health = 7;
		MaxHealth = 7;
		MaxVelocity = 0.11f;
		AttackTimeout = 2500;
		AggroDistance = 28000;
		AttackDistance = 600;
		Respawn = true;
		IsAggro = true;
	}
	virtual void Update(double dt)
	{
		Enemy::Update(dt);

		if (Action == EntityAction::Standing) // if STANDING
		{
			AnimationY = 0;
			AnimationX = 0;
		}
		if (Action == EntityAction::Dead) // if dead
		{
			AnimationY = 2;
			AnimationX = 0;
		}
		if (Action == Aggro)
		{
			AnimationY = 1;
			AnimationX += 0.01 * dt;
			if (AnimationX >= 2) AnimationX = 0;
		}
		if (Action == Attacking)
		{
			AnimationY = 0;
			AnimationX = 0;

			if (SDL_GetTicks() >= NextAttack)
			{
				p->Health -= AttackDamage;
				NextAttack = SDL_GetTicks() + AttackTimeout;
			}
		}

	}
	virtual void Draw(Viewport v, SDL_Renderer* ren)
	{
		Entity::Draw(viewport, ren);
		if (Health < MaxHealth && Action != EntityAction::Dead)
		{
			DrawHealthBar(viewport, ren);
		}
	}
};

class BigZombieCritter : 
	public Enemy
{
public:
	BigZombieCritter(std::vector<Solid>* solids, Player* player, double x, double y) :
		Enemy(solids, player)
	{
		X = x;
		Y = y;
		id = "big_zombie_critter";
		Level = 10;
		Name = "Big Zombie Critter";
		Action = Standing;
		MaxVelocity = 0.02;
		Width = 9;
		Height = 7;
		Attack = 1;
		Health = 7;
		MaxHealth = 7;
		AttackTimeout = 3000;
		AggroDistance = 160000;
		AttackDistance = 100000;
		Respawn = false;
	}
	virtual void Update(double dt)
	{
		Enemy::Update(dt);
		if (Action == EntityAction::Standing) // if standing
		{
			AnimationY = 0;
			AnimationX = 0;
		}
		if (Action == EntityAction::Dead) // if dead
		{
			AnimationY = 2;
			AnimationX = 0;
		}
		
		if (Action == Moving ||
			Action == Aggro)
		{
			AnimationY = 1;
			AnimationX += 0.01 * dt;
			if (AnimationX >= 2) AnimationX = 0;
		}
		if (Action == Attacking)
		{
			if (NextAttack <= SDL_GetTicks())
			{
				// Spawn fire critter
				FireCritter* f = new FireCritter(&solids, player, X + Width / 2, Y + Height / 2);
				f->LoadImage(renderer, "gfx/fire_critter.bmp");
				entities.push_back(f);
				NextAttack = SDL_GetTicks() + AttackTimeout;
			}
		}
	}
};


class KingCritter : 
	public Enemy
{
	int nextx;
	int nexty;
	int nextmove;

	
public:
	KingCritter(std::vector<Solid>* solids, Player* player, double x, double y) :
		Enemy(solids, player)
	{
		X = x;
		Y = y;
		id = "king_critter";
		Level = 5;
		Name = "King Critter";
		Action = Standing;
		MaxVelocity = 0.06;
		IsSolid = true;

		Width = 14;
		Height = 12;
		Health = 300;
		MaxHealth = 300;
		AttackTimeout = 150;
		AggroDistance = 180000;
		AttackDistance = 40000;

		IsAggro = true;
	}
	virtual void Update(double dt) 
	{ // Check line of sight
		
		Enemy::Update(dt);
		if (Action == EntityAction::Dead) // if dead
		{
			AnimationY = 2;
			AnimationX = 0;
		}
		if (Action == Moving ||
			Action == Aggro)
		{
			AnimationY = 1;
			AnimationX += 0.01 * dt;
			if (AnimationX >= 2) AnimationX = 0;
		}
		if (Action == Standing |
			Action == Attacking)
		{
			AnimationY = 0;
			AnimationX = 0;
		}

		if (Action == Attacking || Action == Aggro)
		{
			if (NextAttack <= SDL_GetTicks())
			{
				// Launch fireball
				Fireball* fb = new Fireball(&solids, player, renderer, &entities, X, Y, player->X + player->Width * SCALE / 2, player->Y + player->Height * SCALE / 2);
				fb->LoadImage(renderer, "gfx/fireball.bmp");
				entities.push_back(fb);
				NextAttack = SDL_GetTicks() + AttackTimeout;
			}
		}
	}
	virtual void Draw(Viewport v, SDL_Renderer* ren)
	{
		Entity::Draw(viewport, ren);
		if (Health < MaxHealth && Action != EntityAction::Dead)
		{
			DrawHealthBar(viewport, ren);
		}
	}
	virtual void OnDie()
	{
		if (OnDieFuncName == "drop_sword_and_health")
		{
			DropSwordAndHealth((Entity*)this);
		}
	}

	void DropSwordAndHealth(Entity* e)
	{
		double x = e->X + rand() % (int)e->Width - e->Width / 2;
		double y = e->Y + rand() % (int)e->Height - e->Height / 2;
		Item* i = new Item(&entities, &viewport, player, x + e->Width / 2, y + e->Height / 2, "Sword of Gryifningu", "g_sword");
		i->Width = 4;
		i->Height = 11;
		i->LoadImage(renderer, "gfx/g_sword.bmp");
		entities.push_back(i);
		x = e->X + rand() % (int)e->Width - e->Width / 2;
		y = e->Y + rand() % (int)e->Height - e->Height / 2;
		MaximumHealth* mh = new MaximumHealth(player, x + e->Width / 2, y + e->Height / 2, "Max Health");
		mh->Width = 9;
		mh->Height = 9;
		mh->LoadImage(renderer, "gfx/max_health.bmp");
		entities.push_back(mh);
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
				std::stringstream ss;
				ss << p->Inventory[i].Name;
				if (p->Inventory[i].Id == "holy_grail")
				{
					ss << (flags["holy_grail_full"] ? " (full)" : "(not full)");
				}
				if (p->Inventory[i].Count > 1)
					ss << " x" << p->Inventory[i].Count;
				SDL_Surface * s = TTF_RenderText_Solid(font,
					ss.str().c_str(), { 255, 255, 255 });
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


void LoadLevel(std::string filename)
{
	currentmap = filename;
	ClearEntities();
	entities.push_back(player);
	ClearChunks();
	solids.clear();
	warps.clear();
	hurt.clear();
	triggers.clear();
	shadedareas.clear();
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
			Solid s;
			s.Count = 1;
			s.Rect = { x, y, w, h };
			
				for (pugi::xml_node property = object.child("properties").first_child(); property; property = property.next_sibling())
				{
					std::string name = property.attribute("name").as_string();
					if (name == "key")
					{
						s.NeedsKey = property.attribute("value").as_string();
					}
					if (name == "count")
					{
						s.Count = property.attribute("value").as_int();
					}
					if (name == "flag")
					{
						s.NeedsFlag = property.attribute("value").as_string();
					}
					
				}
			solids.push_back(s);
		}
		if (name == "shaded")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int w = object.attribute("width").as_int();
			int h = object.attribute("height").as_int();
			int mapid = object.attribute("id").as_int();
			std::string shader;
			for (pugi::xml_node property = object.child("properties").first_child(); property; property = property.next_sibling())
			{
				std::string name = property.attribute("name").as_string();
				
				if (name == "shader")
				{
					shader = property.attribute("value").as_string();
				}

			}
			ShadedArea sh = ShadedArea("shaders/" + shader + ".vs", "shaders/" + shader + ".fs");
			sh.Rect = { x, y, w, h };
			shadedareas.push_back(sh);
		}
		if (name == "trigger")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int w = object.attribute("width").as_int();
			int h = object.attribute("height").as_int();
			std::string name = object.attribute("name").as_string();
			Trigger t;
			t.Rect = { x, y, w, h };

			for (pugi::xml_node property = object.child("properties").first_child(); property; property = property.next_sibling())
			{
				std::string name = property.attribute("name").as_string();
				if (name == "flag")
				{
					t.FlagName = property.attribute("value").as_string();
				}
				if (name == "value")
				{
					t.FlagValue = property.attribute("value").as_bool();
				}
				if (name == "needsitem")
				{
					t.NeedsItem = property.attribute("value").as_string();
				}
			}
			triggers.push_back(t);
		}
		if (name == "hurt")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int w = object.attribute("width").as_int();
			int h = object.attribute("height").as_int();
			
			hurt.push_back({ x, y, w, h });
		}
		if (name == "heal")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int w = object.attribute("width").as_int();
			int h = object.attribute("height").as_int();

			heal.push_back({ x, y, w, h });
		}
		if (name == "skull_key")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();

			SkullKey* k = new SkullKey(&entities, &currentmousestate, &previousmousestate, player, &flags, &viewport);
			k->X = x;
			k->Y = y;
			k->LoadImage(renderer, "gfx/skull.bmp");
			k->IsSolid = true;
			entities.push_back(k);

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
			Critter* crit = new Critter(&solids, player, x, y);
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
			KingCritter* crit = new KingCritter(&solids, player, x, y);
			crit->killID.MapID = mapid;
			crit->killID.Map = currentmap;
			crit->LoadImage(renderer, "gfx/king_critter.bmp"); for (pugi::xml_node property = object.child("properties").first_child(); property; property = property.next_sibling())
			{
				std::string name = property.attribute("name").as_string();

				if (name == "ondie")
				{
					crit->OnDieFuncName = property.attribute("value").as_string();
				}
			}
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
					Critter* crit = new Critter(&solids, player, nx, ny);
					crit->killID.MapID = -1;
					crit->killID.Map = currentmap;
					crit->LoadImage(renderer, "gfx/critter.bmp");
					entities.push_back(crit);
				}
				if (spawn_id == "fire_critter")
				{
					FireCritter* crit = new FireCritter(&solids, player, nx, ny);
					crit->killID.MapID = -1;
					crit->killID.Map = currentmap;
					crit->LoadImage(renderer, "gfx/fire_critter.bmp");
					entities.push_back(crit);
				}
				if (spawn_id == "zombie_critter")
				{
					ZombieCritter* crit = new ZombieCritter(&solids, player, nx, ny);
					crit->killID.MapID = -1;
					crit->killID.Map = currentmap;
					crit->LoadImage(renderer, "gfx/zombie_critter.bmp");
					entities.push_back(crit);
				}
				if (spawn_id == "skeleton")
				{
					Skeleton* s = new Skeleton(&solids, player, nx, ny);
					s->killID.MapID = -1;
					s->killID.Map = currentmap;
					s->LoadImage(renderer, "gfx/skeleton.bmp");
					entities.push_back(s);
				}
				if (spawn_id == "zombie")
				{
					Zombie* s = new Zombie(&solids, player, nx, ny);
					s->killID.MapID = -1;
					s->killID.Map = currentmap;
					s->LoadImage(renderer, "gfx/zombie.bmp");
					entities.push_back(s);
				}
				if (spawn_id == "big_zombie_critter")
				{
					BigZombieCritter* crit = new BigZombieCritter(&solids, player, nx, ny);
					crit->killID.MapID = -1;
					crit->killID.Map = currentmap;
					crit->LoadImage(renderer, "gfx/big_zombie_critter.bmp");
					entities.push_back(crit);
				}
				if (spawn_id == "king_critter")
				{
					KingCritter* crit = new KingCritter(&solids, player, nx, ny);
					crit->killID.MapID = -1;
					crit->killID.Map = currentmap;
					crit->LoadImage(renderer, "gfx/king_critter.bmp");
					entities.push_back(crit);
				}
			}
			
		}
		if (name == "npc")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int mapid = object.attribute("id").as_int();
			NPC* npc = new NPC(&flags, &entities, player, monsterfont, &viewport, cursor, x, y);
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
				if (name == "alt_dialog")
				{
					npc->AltDialog = property.attribute("value").as_string();
				}
				if (name == "dialog_item")
				{
					npc->AltDialogItem = property.attribute("value").as_string();
				}
				if (name == "name")
				{
					npc->Name = property.attribute("value").as_string();
				}
				if (name == "dialog_flag")
				{
					npc->AltDialogFlag = property.attribute("value").as_string();
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
			ZombieCritter* crit = new ZombieCritter(&solids, player, x, y);
			crit->killID.MapID = mapid;
			crit->killID.Map = currentmap;
			crit->LoadImage(renderer, "gfx/zombie_critter.bmp");
			entities.push_back(crit);
		}
		if (name == "skeleton")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int mapid = object.attribute("id").as_int();
			Skeleton* s = new Skeleton(&solids, player, x, y);
			s->killID.MapID = mapid;
			s->killID.Map = currentmap;
			s->LoadImage(renderer, "gfx/skeleton.bmp");
			entities.push_back(s);
		}
		if (name == "zombie")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int mapid = object.attribute("id").as_int();
			Zombie* z = new Zombie(&solids, player, x, y);
			z->killID.MapID = mapid;
			z->killID.Map = currentmap;
			z->LoadImage(renderer, "gfx/zombie.bmp");
			entities.push_back(z);
		}
		if (name == "big_zombie_critter")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int mapid = object.attribute("id").as_int();
			BigZombieCritter* crit = new BigZombieCritter(&solids, player, x, y);
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
			FireCritter* crit = new FireCritter(&solids, player, x, y);
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
			std::string removeitems = "none";
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
				if (name == "remove_items")
				{
					removeitems = property.attribute("value").as_string() == "" ? "none" : property.attribute("value").as_string();
				}
			}

			Warp wr = Warp(warpid, targetwarpid, targetlevel, { x, y, w, h });
			wr.RemoveItems = removeitems;
			warps.push_back(wr);

		}
		if (name == "gate")
		{
			int x = object.attribute("x").as_int();
			int y = object.attribute("y").as_int();
			int w = object.attribute("width").as_int();
			int h = object.attribute("height").as_int();
			int mapid = object.attribute("id").as_int();
			std::string neededitem = "";
			std::string flag;
			for (pugi::xml_node property = object.child("properties").first_child(); property; property = property.next_sibling())
			{
				std::string name = property.attribute("name").as_string();

				if (name == "needed_item")
				{
					neededitem = property.attribute("value").as_string();
				}
				if (name == "flag")
				{
					flag = property.attribute("value").as_string();
				}
			}

			Gate* g = new Gate(&flags, &entities, &viewport, player, &currentmousestate, &previousmousestate, x, y, w, h);
			g->IsSolid = true;
			g->NeededItem = neededitem;
			g->Flag = flag;
			g->killID.MapID = mapid;
			g->killID.Map = currentmap;
			entities.push_back(g);
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

			Item* i = new Item(&entities, &viewport, player, x, y, item_name, item_id);
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

			MinorHealth* i = new MinorHealth(player, x, y, "Minor Health");
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

			MaximumHealth* i = new MaximumHealth(player, x, y, "Max Health");
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
			Viewport::PostMessage(&viewport, &entities, text);
		}
		if (name == "chungus")
		{
			int value;
			for (pugi::xml_node property = object.child("properties").first_child(); property; property = property.next_sibling())
			{
				std::string name = property.attribute("name").as_string();

				if (name == "value")
				{
					value = property.attribute("value").as_int();
				}
			}
			Chungus* c = new Chungus(value);
			entities.push_back(c);
		}
		if (name == "info_flag")
		{
			std::string flag = "";
			bool value;
			for (pugi::xml_node property = object.child("properties").first_child(); property; property = property.next_sibling())
			{
				std::string name = property.attribute("name").as_string();

				if (name == "flag")
				{
					flag = property.attribute("value").as_string();
				}
				if (name == "value")
				{
					value = property.attribute("value").as_bool();
				}
			}
			flags[flag] = value;
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
			e->IsSolid = solid;
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
		unsigned int n;
		unsigned int k;

		/* Load killed map IDs */
		killedmapid.clear();
		saveFile.read((char*)&n, sizeof(n));
		for (int i = 0; i < n; i++)
		{
			saveFile.read((char*)&k, sizeof(k));
			char* buf = new char[k + 1];
			buf[k] = 0;
			saveFile.read(buf, k);
			KillID kid;
			kid.Map = buf;
			delete(buf);
			saveFile.read((char*)&kid.MapID, sizeof(kid.MapID));
			killedmapid.push_back(kid);
		}

		// load current map
		saveFile.read((char*)&n, sizeof(n));
		char* map = new char[n + 1];
		saveFile.read(map, n);
		map[n] = 0;
		LoadLevel(map);
		delete(map);

		/* Load player values */
		saveFile.read((char*)&player->X, sizeof(player->X));
		saveFile.read((char*)&player->Y, sizeof(player->Y));
		saveFile.read((char*)&player->JustWarped, sizeof(player->JustWarped));
		saveFile.read((char*)&player->Health, sizeof(player->Health));
		saveFile.read((char*)&player->MaxHealth, sizeof(player->MaxHealth));
		saveFile.read((char*)&player->SelectedItem, sizeof(player->SelectedItem));
		
		/* Load player inventory */
		player->Inventory.clear();
		saveFile.read((char*)&n, sizeof(n));
		for (int i = 0; i < n; i++)
		{
			saveFile.read((char*)&k, sizeof(k));
			char* id = new char[k + 1];
			id[k] = 0;
			saveFile.read(id, k);

			saveFile.read((char*)&k, sizeof(k));
			char* name = new char[k + 1];
			name[k] = 0;
			saveFile.read(name, k);

			HeldItem it = HeldItem(name, id);
			delete(id);
			delete(name);
			saveFile.read((char*)&it.Count, sizeof(it.Count));
			player->AddItem(it, 1);

			
		}

		/* Read flags */
		flags.clear();
		saveFile.read((char*)&n, sizeof(n));
		for (int i = 0; i < n; i++)
		{
			saveFile.read((char*)&k, sizeof(k));
			
			char* flag = new char[k + 1];
			flag[k] = 0;
			saveFile.read(flag, k);
			bool value;
			saveFile.read((char*)&value, sizeof(value));
			flags[flag] = value;
		}
		saveFile.close();
		
		SetGameState(GameStates::Playing);
	}
	else
	{
		Viewport::PostMessage(&viewport, &entities, "No saved game!");
	}
}

void SaveGame()
{
	// Save game
	std::ofstream saveFile("save.bin", std::ios::out | std::ios::binary);

	/* Save kill IDs */
	unsigned int n;
	unsigned int k;

	/* Save killed map IDs */
	n = killedmapid.size();
	saveFile.write((char*)&n, sizeof(n));
	for (int i = 0; i < n; i++)
	{
		KillID kid = killedmapid[i];
		k = kid.Map.size();
		saveFile.write((char*)&k, sizeof(k));
		saveFile.write(kid.Map.c_str(), k);
		saveFile.write((char*)&kid.MapID, sizeof(kid.MapID));
	}
	/* Save current map */
	n = currentmap.size();
	saveFile.write((char*)&n, sizeof(n));
	saveFile.write(currentmap.c_str(), currentmap.size());

	/* Save player values */
	saveFile.write((char*)&player->X, sizeof(player->X));
	saveFile.write((char*)&player->Y, sizeof(player->Y));
	saveFile.write((char*)&player->JustWarped, sizeof(player->JustWarped));
	saveFile.write((char*)&player->Health, sizeof(player->Health));
	saveFile.write((char*)&player->MaxHealth, sizeof(player->MaxHealth));
	saveFile.write((char*)&player->SelectedItem, sizeof(player->SelectedItem));
	
	/* Save player inventory */
	n = player->Inventory.size();
	saveFile.write((char*)&n, sizeof(n));
	for (int i = 0; i < n; i++)
	{
		HeldItem it = player->Inventory[i];
		k = it.Id.size();
		saveFile.write((char*)&k, sizeof(k));
		saveFile.write(it.Id.c_str(), k);
		k = it.Name.size();
		saveFile.write((char*)&k, sizeof(k));
		saveFile.write(it.Name.c_str(), k);
		saveFile.write((char*)&it.Count, sizeof(it.Count));
		
	}

	/* Save flags */
	n = flags.size();
	saveFile.write((char*)&n, sizeof(n));
	for (const auto &f : flags)
	{
		k = f.first.size();
		saveFile.write((char*)&k, sizeof(k));
		saveFile.write(f.first.c_str(), k);
		saveFile.write((char*)&f.second, sizeof(f.second));
	}

	saveFile.close();

	Mix_PlayChannel(-1, sfx_saveload, 0);
	SetGameState(GameStates::Playing);
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
			player->AddItem(h, 1);
		}
	}
	if (v[0] == "flag")
	{
		if (v.size() != 3)
			cons.Line("Usage: flag [name] [value]");
		else
		{
			if (v[2] == "false" || v[2] == "0")
				flags[v[1]] = false;
			if (v[2] == "true" || v[2] == "1")
				flags[v[1]] = true;
		}
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
				int nx = x * 32 + c->X * 32;
				int ny = y * 32 + c->Y * 32;
				if (nx < -viewport.X - 32 ||
					nx > -viewport.X + viewport.Width ||
					ny < -viewport.Y - 32 ||
					ny > -viewport.Y + viewport.Height) continue;

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
		if (keys[SDLK_w] || keys[SDLK_UP])
		{
			keys[SDLK_w] = false;
			keys[SDLK_UP] = false;
			SelectedOption--;
		}
		if (keys[SDLK_s] || keys[SDLK_DOWN])
		{
			keys[SDLK_s] = false;
			keys[SDLK_DOWN] = false;
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
		if (keys[SDLK_w] || keys[SDLK_UP])
		{
			keys[SDLK_w] = false;
			keys[SDLK_UP] = false;
			SelectedOption--;
		}
		if (keys[SDLK_s] || keys[SDLK_DOWN])
		{
			keys[SDLK_s] = false;
			keys[SDLK_DOWN] = false;
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
				if (flags["save_enabled"])
					SaveGame();
				else
					Mix_PlayChannel(-1, sfx_savefailed, 0);
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
	// Set flags
	flags[""] = false;
	flags["holy_grail_full"] = false;
	flags["portrait_open"] = false;
	flags["old_man_released"] = false;
	flags["noclip"] = false;
	flags["shadows"] = false;
	flags["save_enabled"] = true;

	
	srand(time(NULL));
	entities.clear();
	
	SDL_Event event;

	// init SDL

	SDL_Init(SDL_INIT_EVERYTHING);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
	TTF_Init();
	SDL_GL_SetSwapInterval(0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	window = SDL_CreateWindow(GAMENAME.c_str(),
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCRW, SCRH, SDL_WINDOW_OPENGL);
	SDL_SetWindowFullscreen(window, FULLSCRN);
	int oglIdx = -1;
	int nRD = SDL_GetNumRenderDrivers();
	for (int i = 0; i < nRD; i++)
	{
		SDL_RendererInfo info;
		if (!SDL_GetRenderDriverInfo(i, &info))
		{
			if (!strcmp(info.name, "opengl"))
			{
				oglIdx = i;
			}
		}
	}
	renderer = SDL_CreateRenderer(window, oglIdx, SDL_RENDERER_ACCELERATED  | VSYNC ? SDL_RENDERER_PRESENTVSYNC : 0);
	mainContext = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, mainContext);
	glewInit();
	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0f);                   // Set background depth to farthest

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, SCRW, 0, SCRH, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glDepthFunc(GL_LESS);
	glDepthRange(0.0f, 1.0f);

	sfx_warp = Mix_LoadWAV("sfx/warp.wav");
	sfx_saveload = Mix_LoadWAV("sfx/saveload.wav");

	sfx_savefailed = Mix_LoadWAV("sfx/savefailed.wav");
	
	cursor = new Cursor(&entities, renderer);
	viewport = Viewport(cursor, 0, 0, SCRW, SCRH);
	player = new Player(cursor, &solids, &entities, &keys, &viewport, renderer);
	viewport.Focus = player; 
	inventory = Inventory(player);
	player->LoadImage(renderer, "gfx/player.bmp");

	viewport.Width = SCRW;
	viewport.Height = SCRH;
	//glEnable(GL_LIGHTING);
	SDL_Surface* surf = SDL_LoadBMP("gfx/tilesheet.bmp");
	tex_tilesheet = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);

	entities.push_back(player);
	
	LoadLevel("map.tmx");

	SDL_ShowCursor(SDL_DISABLE);
	

	double minimum_fps_delta_time = (1000 / 6); // minimum 6 fps, if the computer is slower than this: slow down.
	double last_game_step = SDL_GetTicks(); // initial value

	expfont = TTF_OpenFont("fonts/arial.ttf", 9);
	monsterfont = TTF_OpenFont("fonts/arial.ttf", 12);
	deathfont = TTF_OpenFont("fonts/lunchds.ttf", 36);
	msgfont = TTF_OpenFont("fonts/biscuit.ttf", 17);
	titlefont = TTF_OpenFont("fonts/lunchds.ttf", 42);
	menufont = TTF_OpenFont("fonts/lunchds.ttf", 36);
	dmgfont = TTF_OpenFont("fonts/dmg.ttf", 36);

	SDL_StartTextInput();
	while (!quit)
	{
		double now = SDL_GetTicks();
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
					keys[event.key.keysym.sym] = true;
				if (gamestate == Playing || gamestate == Death)
				{
					if (keys[SDLK_F1])
						LoadGame();
				}
				if (gamestate == Playing)
				{
					if (keys[SDLK_F2])
					{
						if (flags["save_enabled"])
						{
							SaveGame();
							Viewport::PostMessage(&viewport, &entities, "Game saved!");
						}
						else
						{
							Viewport::PostMessage(&viewport, &entities, "Cannot save here!");
							Mix_PlayChannel(-1, sfx_savefailed, 0);
						}
					}
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
				keys[event.key.keysym.sym] = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					currentmousestate.LeftDown = true;
					if (!cons.Toggled && gamestate == Playing && player->GetSelectedItemId() == "pistol")
					{
						// Draw bullet from player to cursor						
						player->FireBullet();
					}
					else if (player->GetSelectedItemId() == "shotgun")
					{
						player->FireShotgun();

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
					player->NextAttack = -1;
				}
				else if (event.button.button == SDL_BUTTON_RIGHT)
				{
					currentmousestate.RightDown = false;
				}
				break;
			case SDL_MOUSEWHEEL:
				if (gamestate == Playing)
				{
					if (player->Inventory.size() > 1)
					{
						player->SelectedItem += event.wheel.y;
						if (player->SelectedItem >= player->Inventory.size()) player->SelectedItem = 0;
						if (player->SelectedItem < 0) player->SelectedItem = player->Inventory.size() - 1;
					}
				}
				break;
			}
		}
		
		if (last_game_step < now) {

			Uint32 delta_time = now - last_game_step;

			if (delta_time > minimum_fps_delta_time) {
				delta_time = minimum_fps_delta_time; // slow down if the computer is too slow
			}

			///SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			//SDL_RenderClear(renderer);
			
			glClearColor(0, 0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			//SDL_GL_SwapWindow(window);

			// Pointer modes
			if (gamestate == Playing)
			{
				if (player->GetSelectedItemId() == "pistol" ||
					player->GetSelectedItemId() == "uzi" ||
					player->GetSelectedItemId() == "shotgun")
					cursor->Mode = Crosshair;
				else
					cursor->Mode = Pointer;
			}
			if (gamestate == Death || gamestate == TitleMenu || gamestate == Paused)
				cursor->Mode = None;

			// Order entites by y
			std::sort(entities.begin(), entities.end(), [](Entity* a, Entity* b) {return a->Y + a->Height * SCALE < b->Y + b->Height * SCALE; });
			std::sort(entities.begin(), entities.end(), [](Entity* a, Entity* b) {return a->Z < b->Z; });
			
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
						Mix_PlayChannel(-1, sfx_warp, 0);
						
						if (warps[i].RemoveItems == "all")
						{
							player->Inventory.clear();
						}

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
				// Draw shaded areas
				for (int i = 0; i < shadedareas.size(); i++)
				{
					SDL_Rect r = shadedareas[i].Rect;
					//glUseProgram(shadedareas[i].shader->ID);

					glUseProgram(shadedareas[i].shader->ID);
					GLint t = glGetUniformLocation(shadedareas[i].shader->ID, "time");
					glUniform1f(t, SDL_GetTicks());
					t = glGetUniformLocation(shadedareas[i].shader->ID, "iResolution");
					glUniform2f(t, viewport.Width, viewport.Height);
					t = glGetUniformLocation(shadedareas[i].shader->ID, "viewport");
					glUniform2f(t, viewport.X, viewport.Y);
					
					glBegin(GL_QUADS);
					glVertex2f(r.x + viewport.X, r.y + viewport.Y);
					glVertex2f(r.x + viewport.X + r.w, r.y + viewport.Y);
					glVertex2f(r.x + viewport.X + r.w, r.y + viewport.Y + r.h);
					glVertex2f(r.x + viewport.X, r.y + viewport.Y + r.h);
					glEnd();
					glUseProgram(0);
				}
				
				for (unsigned int i = 0; i < entities.size(); i++)
				{
					if (!entities[i]->Alive) continue;
					if (entities[i]->id == "ui") continue;
					// If entity not inside screen then continue
					int nx = entities[i]->X;
					int ny = entities[i]->Y;
					if ((nx < -viewport.X - entities[i]->Width * SCALE ||
						nx > -viewport.X + viewport.Width ||
						ny < -viewport.Y - entities[i]->Height * SCALE ||
						ny > -viewport.Y + viewport.Height) &&
						entities[i]->id != "ui") continue;

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

						// If i is player then check against ever trigger
						if (entities[i]->id == "player")
						{
							for (int j = 0; j < triggers.size(); j++)
							{
								SDL_Rect p = { entities[i]->X, entities[i]->Y, entities[i]->Width * SCALE, entities[i]->Height * SCALE };
								
								if (SDL_HasIntersection(&p, &triggers[j].Rect))
								{
									if (triggers[j].NeedsItem != "")
									{
										if (player->HasItem(triggers[j].NeedsItem))
										{
											flags[triggers[j].FlagName] = triggers[j].FlagValue;
										}
									}
									else
									{
										
										flags[triggers[j].FlagName] = triggers[j].FlagValue;
									}
								}
							}
						}
						// Check against every hurt
						for (int j = 0; j < hurt.size(); j++)
						{
							SDL_Point p = { entities[i]->X + entities[i]->Width / 2, entities[i]->Y + entities[i]->Height };
							SDL_Rect b = hurt[j];
							if (SDL_PointInRect(&p, &b))
							{
								entities[i]->Health -= entities[i]->MaxHealth * 0.0002f * delta_time;
							}
						}
						// Check against every heal
						if (entities[i]->id == "player")
						{
							for (int j = 0; j < heal.size(); j++)
							{
								SDL_Point p = { entities[i]->X + entities[i]->Width / 2, entities[i]->Y + entities[i]->Height };
								SDL_Rect b = heal[j];
								if (SDL_PointInRect(&p, &b))
								{
									entities[i]->Health += entities[i]->MaxHealth * 0.0002f * delta_time;
									if (entities[i]->Health > entities[i]->MaxHealth)
										entities[i]->Health = entities[i]->MaxHealth;
								}
							}
						}
						// Start collisions 
						// Check against every and entity
						if (entities[i]->IsSolid)
						{
							for (int j = 0; j < entities.size(); j++)
							{
								if (i == j) continue;
								if (!entities[j]->IsSolid) continue;

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
									else if (r.w > r.h) // Solve vertically
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
									if (player->HasItem(solids[j].NeedsKey) >= solids[j].Count) continue;
									if (flags[solids[j].NeedsFlag]) continue;
									if (flags["noclip"]) continue;
								}
								SDL_Rect a = { entities[i]->X-1, entities[i]->Y-1, entities[i]->Width * entities[i]->SCALE+ 2, entities[i]->Height * entities[i]->SCALE + 2 };
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
						// End collisions
					}
					entities[i]->Draw(viewport, renderer);
				}
				
				
				if (flags["shadows"])
					for (unsigned int i = 0; i < solids.size(); i++)
					{
						SDL_Rect r = solids[i].Rect;

						if (r.x + r.w < -viewport.X ||
							r.x > -viewport.X + viewport.Width ||
							r.y + r.h < -viewport.Y ||
							r.y > -viewport.Y + viewport.Height) continue;
						// Cast each point away from player
						double angle;

						double vertices[4 * 4];
						// Top left
						int x = r.x;
						int y = r.y;
						angle = atan2(y - player->Y, x - player->X);
						int x2 = x + cos(angle) * 1000;
						int y2 = y + sin(angle) * 1000;
						vertices[0] = x;
						vertices[1] = y;
						vertices[2] = x2;
						vertices[3] = y2;
						// Top right
						x = r.x + r.w;
						y = r.y;
						angle = atan2(y - player->Y, x - player->X);
						x2 = x + cos(angle) * 1000;
						y2 = y + sin(angle) * 1000;
						vertices[4] = x;
						vertices[5] = y;
						vertices[6] = x2;
						vertices[7] = y2;
						// bottom left
						x = r.x;
						y = r.y + r.h;
						angle = atan2(y - player->Y, x - player->X);
						x2 = x + cos(angle) * 1000;
						y2 = y + sin(angle) * 1000;
						vertices[8] = x;
						vertices[9] = y;
						vertices[10] = x2;
						vertices[11] = y2;
						// bottom right
						x = r.x + r.w;
						y = r.y + r.h;
						angle = atan2(y - player->Y, x - player->X);
						x2 = x + cos(angle) * 1000;
						y2 = y + sin(angle) * 1000;
						vertices[12] = x;
						vertices[13] = y;
						vertices[14] = x2;
						vertices[15] = y2;
						SDL_RenderDrawLine(renderer, x + viewport.X, y + viewport.Y, x2 + viewport.X, y2 + viewport.Y);

						glBegin(GL_QUADS);

						glColor3f(0.0, 0, 0);

						glVertex2f(vertices[0] + viewport.X, vertices[1] + viewport.Y);
						glVertex2f(vertices[2] + viewport.X, vertices[3] + viewport.Y);
						glVertex2f(vertices[6] + viewport.X, vertices[7] + viewport.Y);
						glVertex2f(vertices[4] + viewport.X, vertices[5] + viewport.Y);

						glVertex2f(vertices[4] + viewport.X, vertices[5] + viewport.Y);
						glVertex2f(vertices[6] + viewport.X, vertices[7] + viewport.Y);
						glVertex2f(vertices[14] + viewport.X, vertices[15] + viewport.Y);
						glVertex2f(vertices[12] + viewport.X, vertices[13] + viewport.Y);

						glVertex2f(vertices[2] + viewport.X, vertices[3] + viewport.Y);
						glVertex2f(vertices[0] + viewport.X, vertices[1] + viewport.Y);
						glVertex2f(vertices[8] + viewport.X, vertices[9] + viewport.Y);
						glVertex2f(vertices[10] + viewport.X, vertices[11] + viewport.Y);

						glVertex2f(vertices[8] + viewport.X, vertices[9] + viewport.Y);
						glVertex2f(vertices[10] + viewport.X, vertices[11] + viewport.Y);
						glVertex2f(vertices[14] + viewport.X, vertices[15] + viewport.Y);
						glVertex2f(vertices[12] + viewport.X, vertices[13] + viewport.Y);
						glEnd();

						
					}

				for (unsigned int i = 0; i < entities.size(); i++)
				{
					if (entities[i]->id != "ui") continue;
					entities[i]->Update(delta_time);
					entities[i]->Draw(viewport, renderer);
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
					GAMENAME.c_str(), { 255, 0, 0 });
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
					GAMENAME.c_str(), { 255, 0, 0 });
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
			viewport.Update(delta_time);

			
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
						if (entities[i]->killID.MapID != -1)
							killedmapid.push_back(entities[i]->killID);
					}
					delete(entities[i]);
					continue;
				}

				if (entities[i]->Action == EntityAction::Dead)
				{
					if (entities[i]->killID.MapID != -1)
						killedmapid.push_back(entities[i]->killID);
				}
			}
			entities = ents;
			
			static float r = 0;
			r += 0.1f;
			
		
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glRotatef(r, 0, 1, 0);
			glBegin(GL_TRIANGLES);
			glVertex3f(0, 0, 0);
			glVertex3f(100, 0, 0);
			glVertex3f(100, 100, 0);
			glEnd();
			glPopMatrix();
		

			SDL_RenderPresent(renderer);

			last_game_step = now;
		}
		else {
			
		}

	}

	// cleanup SDL
	ClearEntities();

	Mix_FreeChunk(sfx_warp);
	Mix_FreeChunk(sfx_savefailed);
	Mix_FreeChunk(sfx_saveload);

	SDL_DestroyTexture(tex_tilesheet);
	SDL_GL_DeleteContext(mainContext);
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
	Mix_CloseAudio();
	SDL_Quit();

	return 0;
}