#pragma once
#include <SDL_mixer.h>
#include <string>

#include "Entity.h"
class Player;
class MinorHealth : public
	Entity
{
	Mix_Chunk* sfx_pickup;
	Player* p;
public:
	MinorHealth(Player* player, double x, double y, std::string name);
	virtual ~MinorHealth();

	virtual void Update(double dt);
};