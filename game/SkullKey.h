#pragma once
#include <map>
#include <vector>
#include "Entity.h"
class MouseState;
class Player;
class Viewport;
class SkullKey : public
	Entity
{
	MouseState* c_ms;
	MouseState* p_ms;
	Player* p;
	std::map<std::string, bool>* f;
	Viewport* v;
public:

	SkullKey(std::vector<Entity*>* entities, MouseState* currentMouseState, MouseState* previousMouseState, Player* player, std::map<std::string, bool>* flags, Viewport* vp);	~SkullKey();

	virtual void Update(double dt);

	virtual void Draw(Viewport viewport, SDL_Renderer* ren);
};