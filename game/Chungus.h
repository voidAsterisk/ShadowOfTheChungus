#pragma once
#include <SDL_mixer.h>
#include <SDL.h>
#include <vector>
#include <sstream>
#include "Entity.h"
//class Model_OBJ;
class Player;
class Viewport;
class Chungus :
	public Entity
{
	//Model_OBJ* Model;
	//Shader* shader;
	Player* p;
	Mix_Chunk* sfx_ding;
	Viewport* v;
	std::vector<Entity*>* e;
public:
	int Value;
	Chungus(Viewport* viewport, std::vector<Entity*>* entities, SDL_Renderer* ren, int value);
	~Chungus();
	virtual void Update(double dt);
};

