#pragma once
#include <SDL_mixer.h>
#include "Entity.h"

class Player;
class MaximumHealth : public
	Entity
{
	Player* p;
	Mix_Chunk* sfx_pickup;
public:
	MaximumHealth(Player* player, double x, double y, std::string name);
	virtual ~MaximumHealth();

	virtual void Update(double dt);
};