#include "pch.h"
#include "Cursor.h"




Cursor::Cursor(std::vector<Entity*>* ents, SDL_Renderer* ren) :
	Entity(ents)
{
	id = "cursor";
	cur_crosshair = LoadTexture(ren, "gfx/crosshair.bmp");
	cur_pointer = LoadTexture(ren, "gfx/cursor.bmp");
}


Cursor::~Cursor()
{
}

void Cursor::Update(double dt)
{
	if (Mode == Crosshair)
	{
		Width = 9;
		Height = 9;
	}
	if (Mode == Pointer)
	{
		Width = 7;
		Height = 7;
	}
	if (Mode == None)
	{
		Width = 0;
		Height = 0;
	}

	int x, y;
	SDL_GetMouseState(&x, &y);


	X = x;
	Y = y;
}

void Cursor::Draw(SDL_Renderer* ren)
{

		
		if (Mode == Crosshair)
		{
			SDL_Rect dstrect = { (int)X - Width / 2, (int)Y - Height / 2, (int)(9 * SCALE), (int)(Height * SCALE) };
			SDL_RenderCopy(ren, cur_crosshair, NULL, &dstrect);
		}
		if (Mode == Pointer)
		{
			SDL_Rect dstrect = { (int)X, (int)Y, (int)(Width * SCALE), (int)(Height * SCALE) };
			SDL_RenderCopy(ren, cur_pointer, NULL, &dstrect);
		}

}
