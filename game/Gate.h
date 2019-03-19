#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <vector>
#include <map>
#include "Entity.h"
class Viewport;
class MouseState;
class Player;
class Entity;
class Gate :
	public Entity
{
	MouseState* c_ms;
	MouseState* p_ms;
	Player* p;
	Viewport* v;
	std::vector<Entity*>* e;
	Mix_Chunk* sfx_unlock;
	std::map<std::string, bool>* f;
public:
	SDL_Rect Rect;
	std::string NeededItem;
	std::string Flag;
	Gate(std::map<std::string, bool>* flags, std::vector<Entity*>* entities, Viewport* viewport, Player* player, MouseState* currentmousestate, MouseState* previousmousestate, double x, double y, double w, double h);
	~Gate();
	virtual void Update(double);
	virtual void Draw(Viewport , SDL_Renderer*);
};

