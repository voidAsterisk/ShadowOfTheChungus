#include "pch.h"
#include "Player.h"

#include "Bullet.h"
#include "HeldItem.h"
#include "Viewport.h"
#include "Solid.h"

Player::Player(std::vector<Solid>* solids, std::vector<Entity*>* ents, bool** keys, Viewport* viewport, SDL_Renderer* renderer)
{
	Entity::Entity(ents);
	s = solids;
	ren = renderer;
	v = viewport;
	k = keys;
	IsSolid = true;
	id = "player";
	Level = 1;

	Width = 7;
	Height = 16;

	Health = MaxHealth = 10;
	NextAttack = -1;
}

Player::~Player()
{
}

void Player::FireBullet()
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	int nx = X + Width * SCALE / 2;
	int ny = Y + Height * SCALE / 2;
	Bullet* bl = new Bullet(v, this, s, ren, EntityListPointer, atan2(ny + v->Y - y, nx + v->X - x) + M_PI);
	bl->LoadImage(ren, "gfx/" + (std::string)"bullet.bmp");
	bl->X = nx;
	bl->Y = ny;
	EntityListPointer->push_back(bl);
}

std::string Player::GetSelectedItemId()
{
	if (SelectedItem == -1) return "none";
	else if (Inventory.size() == 0) return "none";
	else
		return Inventory[SelectedItem].Id;
}

void Player::AddItem(HeldItem item)
{
	if (SelectedItem == -1) SelectedItem = 0;

	for (int i = 0; i < Inventory.size(); i++)
	{
		if (item.Id == Inventory[i].Id)
		{
			Inventory[i].Count++;
			return;
		}
	}
	Inventory.push_back(item);
}

int Player::HasItem(std::string itemid)
{
	for (int i = 0; i < Inventory.size(); i++)
	{
		if (Inventory[i].Id == itemid)
		{
			return Inventory[i].Count;
		}
	}
	return 0;
}

void Player::Update(double dt)
{
	bool moving = false;
	if ((*k)[SDLK_w])
	{
		VelocityY = -MaxVelocity;
		moving = true;
	}
	else if ((*k)[SDLK_s])
	{
		VelocityY = MaxVelocity;
		moving = true;
	}
	else
		VelocityY = 0;


	if ((*k)[SDLK_a])
	{
		VelocityX = -MaxVelocity;
		moving = true;
	}
	else if ((*k)[SDLK_d])
	{
		VelocityX = MaxVelocity;
		moving = true;
	}
	else
		VelocityX = 0;

	X += VelocityX * dt;
	Y += VelocityY * dt;

	if (moving)
	{
		AnimationY = 1;
		AnimationX += 0.005 * dt;
		if (AnimationX > 2) AnimationX = AnimationX - (int)AnimationX;
	}
	else
	{
		AnimationX = 0;
		AnimationY = 0;
	}

	// Draw bullet from player to cursor
	if (SDL_GetTicks() >= NextAttack && NextAttack != -1)
	{
		FireBullet();
		if (Inventory[SelectedItem].Id == "uzi")
			NextAttack = SDL_GetTicks() + 100;
	}
}