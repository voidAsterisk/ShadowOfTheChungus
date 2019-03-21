#include "pch.h"
#include "Chungus.h"
#include "Viewport.h"
#include "Player.h"
//#include "Model_OBJ.h"
//#include "Shader.h"
Chungus::Chungus(Viewport* viewport, std::vector<Entity*>* entities, SDL_Renderer* ren, int value) :
	Entity(entities)
{
	e = entities;
	v = viewport;
	// Find player
	for (int i = 0; i < entities->size(); i++)
	{
		if ((*entities)[i]->id == "player")
		{
			p = (Player*)(*entities)[i];
		}
	}
	//Model = new Model_OBJ();
	//Model->Load("models/chungus.obj");
	//shader = new Shader("shaders/default.vs", "shaders/default.fs");
	switch (value)
	{
	case 1:
	case 2:
	case 3:
		LoadImage(ren, "gfx/chungus1.bmp");
		Width = 10;
		Height = 9;
		break;
	case 4:
	case 5:
	case 6:
	case 7:
		LoadImage(ren, "gfx/chungus5.bmp");
		Width = 15;
		Height = 13;
		break;
	default:
		LoadImage(ren, "gfx/chungus20.bmp");
		Width = 15;
		Height = 19;
		break;
	}
	Value = value;
	sfx_ding = Mix_LoadWAV("sfx/ding.wav");
}
Chungus::~Chungus()
{
	Mix_FreeChunk(sfx_ding);
}
void Chungus::Update(double dt)
{
	Entity::Update(dt);
	Y += sin(SDL_GetTicks() * 0.001f) * 0.002f * dt;

	// If rect intersects with player rect
	SDL_Rect r = { X, Y, Width * SCALE, Height * SCALE };
	SDL_Rect pr = { p->X, p->Y, p->Width * p->SCALE, p->Height * p->SCALE };
	if (SDL_HasIntersection(&r, &pr))
	{
		Alive = false;
		p->Chunguses += Value;
		Mix_PlayChannel(-1, sfx_ding, 0);
		
		std::stringstream ss;
		ss << "+" << Value << " chungus";
		Viewport::PostMessage(v, e, ss.str());
	}

}
