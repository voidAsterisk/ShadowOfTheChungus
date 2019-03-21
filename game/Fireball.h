#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <vector>
#include "Entity.h"

class Player;
class Solid;
class Viewport;
class Fireball :
	public Entity
{
	SDL_Renderer* r;
	Player* p;
	std::vector<Entity*>* e;
	std::vector<Solid>* s;
	Mix_Chunk* sfx_flame;
	Mix_Chunk* sfx_out;
public:
	double DestX;
	double DestY;

	Fireball(std::vector<Solid>* solids, Player* player, SDL_Renderer* renderer,std::vector<Entity*>* ents, double x, double y, double destX, double destY);
	~Fireball();
	virtual void Update(double dt);
};