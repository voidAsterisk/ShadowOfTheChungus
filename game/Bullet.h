#pragma once
#include <sstream>
#include <vector>
#include "Funcs.h"
class Entity;
class Viewport;
class Solid;
class Player;

class Bullet : public Entity
{
	SDL_Renderer* ren;
	std::vector<Solid>* s;
	Player* p;
	Viewport* v;
public:
	double lastX;
	double lastY;
	double Angle;
	Bullet(Viewport* viewport, Player* player, std::vector<Solid>* solids, SDL_Renderer* renderer, std::vector<Entity*>* ents, double angle);
	
	~Bullet();

	virtual void Update(double dt);
};