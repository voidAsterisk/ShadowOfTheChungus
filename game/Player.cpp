#include "pch.h"
#include "Player.h"

#include "Bullet.h"
#include "HeldItem.h"
#include "Viewport.h"
#include "Solid.h" 
#include "Cursor.h"
#include <iostream>

Player::Player(Cursor* cursor, std::vector<Solid>* solids, std::vector<Entity*>* ents, std::map<int, bool>* keys, Viewport* viewport, SDL_Renderer* renderer) :
	Entity(ents)
{
	c = cursor;
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
	SprintVelocity = MaxVelocity * 1.3;

	sfx_shoot = Mix_LoadWAV("sfx/shoot.wav");
	std::cout << SDL_GetError();
	sfx_shotgun = Mix_LoadWAV("sfx/shotgun.wav");
}

Player::~Player()
{
	Mix_FreeChunk(sfx_shotgun);
	Mix_FreeChunk(sfx_shoot);
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
	Mix_VolumeChunk(sfx_shoot, MIX_MAX_VOLUME * ((rand() % 5) * 0.1f + 0.4f));
	Mix_PlayChannel(-1, sfx_shoot, 0);
}

void Player::FireUzi()
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	double nx = X + Width * SCALE / 2;
	double ny = Y + Height * SCALE / 2;
	Bullet* bl = new Bullet(v, this, s, ren, EntityListPointer, atan2(ny + v->Y - y, nx + v->X - x) + M_PI + (rand() % 10 - 5)*M_PI / 180.0f);
	bl->LoadImage(ren, "gfx/" + (std::string)"bullet.bmp");
	bl->X = nx;
	bl->Y = ny;
	bl->DeathTime = SDL_GetTicks() + 40;
	EntityListPointer->push_back(bl);
	Mix_PlayChannel(-1, sfx_shoot, 0);
}

void Player::FireShotgun()
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	int nx;
	int ny;
	for (int i = 0; i < 6; i++)
	{
		nx = X + Width * SCALE / 2;
		ny = Y + Height * SCALE / 2;
		Bullet* bl = new Bullet(v, this, s, ren, EntityListPointer, atan2(ny + v->Y - y, nx + v->X - x) + M_PI + (rand() % 30 - 15)*M_PI/180.0f);
		bl->LoadImage(ren, "gfx/" + (std::string)"bullet.bmp");
		bl->X = nx;
		bl->Y = ny;
		bl->DeathTime = SDL_GetTicks() + 20;
		EntityListPointer->push_back(bl);
	}
	
	Mix_PlayChannel(-1, sfx_shotgun, 0);
}

std::string Player::GetSelectedItemId()
{
	if (SelectedItem == -1) return "none";
	else if (Inventory.size() == 0) return "none";
	else
		return Inventory[SelectedItem].Id;
}

void Player::AddItem(HeldItem item, int count)
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
	Inventory[Inventory.size() - 1].Count = count;
	if (Inventory.size() == 1) SelectedItem = 0;
}
void Player::RemoveItem(std::string item_id)
{
	for (int i = 0; Inventory.size(); i++)
	{
		if (Inventory[i].Id == item_id)
		{
			Inventory[i].Count--;
			if (Inventory[i].Count <= 0)
			{
				Inventory.erase(Inventory.begin() + i);
				if (SelectedItem == i)
					SelectedItem -= 1;
				if (SelectedItem < 0)
					SelectedItem = Inventory.size() - 1;
				return;
			}
		}
	}
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
	moving = false;

	double vel = MaxVelocity;
	if ((*k)[SDLK_LSHIFT])
		vel = SprintVelocity;


	if ((*k)[SDLK_w])
	{
		VelocityY = -vel;
		moving = true;
	}
	else if ((*k)[SDLK_s])
	{
		VelocityY = vel;
		moving = true;
	}
	else
		VelocityY = 0;


	if ((*k)[SDLK_a])
	{
		VelocityX = -vel;
		moving = true;
	}
	else if ((*k)[SDLK_d])
	{
		VelocityX = vel;
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
		FireUzi();
		if (Inventory[SelectedItem].Id == "uzi")
			NextAttack = SDL_GetTicks() + 120;
	}
}
