#pragma once
#include <SDL_mixer.h>
#include <sstream>
#include <vector>
#include "Entity.h"
class Viewport;
class Solid;
class Player;

class Bullet : public Entity
{
	SDL_Renderer* ren;
	std::vector<Solid>* s;
	Player* p;
	Viewport* v;

	std::vector< Mix_Chunk*> impact_sfx;
public:
	double lastX;
	double lastY;
	double Angle;
	Bullet(Viewport* viewport, Player* player, std::vector<Solid>* solids, SDL_Renderer* renderer, std::vector<Entity*>* ents, double angle);
	
	~Bullet();
	void PlayRandomImpactSound();
	virtual void Update(double dt);
};