#pragma once
#include <SDL.h>
#include <SDL_mixer.h>

#include <vector>
#include <map>
#include "Entity.h"

class HeldItem;
class Bullet;
class Viewport;
class Solid;
class Cursor;

class Player : public Entity
{
	Cursor* c;
	Mix_Chunk* sfx_shoot = NULL;
	Mix_Chunk* sfx_shotgun  = NULL;
	std::map<int, bool>* k;
	Viewport* v;
	SDL_Renderer* ren;
	std::vector<Solid>* s;
	double SprintVelocity;
	bool moving;
public:
	int Experience = 0;
	bool JustWarped = false;
	int TargetWarp = -1;

	int SelectedItem = -1;
	std::vector<HeldItem> Inventory;
	double DamageInflicted = 0;
	Player(Cursor* cursor, std::vector<Solid>* solids, std::vector<Entity*>* ents, std::map<int,bool>* keys, Viewport* viewport, SDL_Renderer* renderer);
	
	~Player();

	std::string GetSelectedItemId();

	void AddHealth(double hp)
	{
		Health += hp;
		if (Health > MaxHealth) Health = MaxHealth;
	}
	void AddItem(HeldItem item, int count);
	void RemoveItem(std::string item_id);
	int HasItem(std::string itemid);

	virtual void Update(double dt);

	void FireBullet();
	void FireUzi();
	void FireShotgun();

};