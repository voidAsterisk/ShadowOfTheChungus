#pragma once
#include <SDL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Entity.h"
class Model_OBJ;
class Viewport;
class Shader;
class Chungus :
	public Entity
{
	Model_OBJ* Model;
	Shader* shader;
public:
	Chungus(int value);
	virtual void Draw(Viewport vp, SDL_Renderer* ren);
};

