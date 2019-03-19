#pragma once
#include <SDL_mixer.h>
#include <string>
#include <vector>
#include "Entity.h"
class Player;
class Item : public
	Entity
{
	Player* p;
	Viewport* v;
	Mix_Chunk* sfx_pickup;
public:
	std::string Name;
	std::string Id;
	Item();
	Item(std::vector<Entity*>* e, Viewport* viewport, Player* player, double x, double y, std::string name, std::string id);
	~Item();
	virtual void Update(double dt);
};