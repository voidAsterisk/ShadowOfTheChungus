#pragma once
#include <string>
#include <SDL.h>
#include "Vars.h"

enum EntityAction
{
	Standing,
	Moving,
	Aggro,
	Attacking,
	Dead
};
class KillID
{
public:
	int MapID;
	std::string Map;
	KillID()
	{
		MapID = -1;
	}
	KillID(int mapid, std::string map)
	{
		MapID = mapid;
		Map = map;
	}

	bool operator==(const KillID& rhs)
	{
		return this->MapID == rhs.MapID && this->Map == rhs.Map;
	}

	bool operator!=(const KillID& rhs)
	{
		return this->MapID != rhs.MapID && this->Map != rhs.Map;
	}
};
class Entity
{
public:
	std::vector<Entity*>* EntityListPointer;
	int SCALE = 2;
	bool Alive = true;
	double X = 0;
	double Y = 0;
	double Z = 0;
	int Alpha = 255;
	bool Fade = false;
	bool Solid = false;
	int DeathTime = -1;
	int Level = -1;
	int Respawn = false;
	double VelocityX = 0;
	double VelocityY = 0;
	std::string id = "entity";
	std::string Name = "Entity";
	double Width;
	double Height;
	int SpawnTime = SDL_GetTicks();
	int NextAttack = 0;
	int Attack;
	int AttackTimeout;
	int AttackDamage = 1;
	double AttackDistance = -1;
	double AggroDistance = -1;
	double Health = 1;
	double MaxHealth = 1;
	KillID killID;

	double AnimationX = 0;
	double AnimationY = 0;

	double MaxVelocity = 0.25;

	SDL_Surface * Image = nullptr;
	SDL_Texture * Texture = nullptr;

	EntityAction Action;
	Entity();
	Entity(std::vector<Entity*>* ents);
	~Entity();
	virtual void LoadImage(SDL_Renderer* ren, std::string img);
	virtual void Update(double dt);
	virtual void Draw(SDL_Renderer* ren);
	virtual void DrawHealthBar(SDL_Renderer* ren);
	virtual int ExperienceDrop();
	virtual void OnClick();
	SDL_Texture* LoadTexture(SDL_Renderer* ren, std::string img);
};