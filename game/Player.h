#pragma once
#include <SDL.h>
#include <vector>
#include "Entity.h"

class HeldItem;
class Bullet;
class Viewport;
class Solid;

class Player : public Entity
{
	bool** k;
	Viewport* v;
	SDL_Renderer* ren;
	std::vector<Solid>* s;
public:
	int Experience = 0;
	bool JustWarped = false;
	int TargetWarp = -1;

	bool HasGun = false;
	int SelectedItem = -1;
	std::vector<HeldItem> Inventory;
	double DamageInflicted = 0;
	Player(std::vector<Solid>* solids, std::vector<Entity*>* ents, bool** keys, Viewport* viewport, SDL_Renderer* renderer);
	
	~Player();

	std::string GetSelectedItemId();

	void AddHealth(double hp)
	{
		Health += hp;
		if (Health > MaxHealth) Health = MaxHealth;
	}
	void AddItem(HeldItem item);

	int HasItem(std::string itemid);

	virtual void Update(double dt);

	void FireBullet();
};