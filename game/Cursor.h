#pragma once
#include <SDL.h>
#include <vector>
#include "Entity.h"

enum CursorMode
{
	Crosshair,
	Pointer,
	None,
};
class Cursor :
	public Entity
{

	SDL_Texture* cur_crosshair;
	SDL_Texture* cur_pointer;
public:
	CursorMode Mode = Pointer;

	Cursor(std::vector<Entity*>* ents, SDL_Renderer* ren);
	~Cursor();

	virtual void Update(double dt);
	void Draw(SDL_Renderer* ren);
};
