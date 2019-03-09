#include "pch.h"
#include "Player.h"


Player::Player(std::vector<Entity*>* ents)
{
	Entity::Entity(ents);
	Solid = true;
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
