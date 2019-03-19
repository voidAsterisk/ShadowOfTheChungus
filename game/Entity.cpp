#include "pch.h"
#include "Entity.h"

#include "Viewport.h"
#include "BloodParticle.h"
#include <iostream>

Entity::Entity()
{
}

Entity::Entity(std::vector<Entity*>* ents)
{
	EntityListPointer = ents;
	id = "entity";
}

Entity::~Entity()
{
	if (Image != nullptr)
 		SDL_FreeSurface(Image);
	if (Texture != nullptr)
		SDL_DestroyTexture(Texture);
}
void Entity::LoadImage(SDL_Renderer* ren, std::string img)
{
	Image = SDL_LoadBMP(img.c_str());
	SDL_SetColorKey(Image, SDL_TRUE, SDL_MapRGB(Image->format, 0xFF, 0x00, 0xFF));
	Texture = SDL_CreateTextureFromSurface(ren, Image);
}

void Entity::Update(double dt)
{
	if (DeathTime != -1)
	{
		Alpha = 255 - 255 * ((double)(SDL_GetTicks() - SpawnTime) / (DeathTime - SpawnTime));
		if (SDL_GetTicks() >= DeathTime)
		{
			Alive = false;
		}
	}
}

void Entity::Draw(Viewport viewport, SDL_Renderer* ren)
{
	if (Texture != nullptr) {
		SDL_Rect srcrect = { (int)AnimationX * Width, (int)AnimationY * Height, Width, Height };
		SDL_Rect dstrect = { X + viewport.X, Y + viewport.Y, Width * SCALE, Height * SCALE };
		SDL_SetTextureAlphaMod(Texture, Alpha);

		SDL_RenderCopy(ren, Texture, &srcrect, &dstrect);
	}
}

void Entity::DrawHealthBar(Viewport viewport, SDL_Renderer* ren)
{
	// Draw health bar
	int h = 2 * SCALE;
	int w = Width * SCALE;
	SDL_Rect bgr = { X + viewport.X, Y + viewport.Y - h * 1.5f, w, h };
	SDL_Rect hpr = { X + viewport.X, Y + viewport.Y - h * 1.5f, w * (Health / MaxHealth), h };
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderFillRect(ren, &bgr);
	SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
	SDL_RenderFillRect(ren, &hpr);
}

void Entity::OnClick()
{

}

int Entity::ExperienceDrop()
{
	if (Level <= 50)
		return ((Level*Level*Level)*(100 - Level)) / 50;
	else if (Level >= 50 && Level <= 68)
		return ((Level*Level*Level)*(150 - Level)) / 50;
	else if (Level >= 68 && Level <= 98)
		return ((Level*Level*Level)*(1911 - 10 * Level) / 3);
	else
		return ((Level*Level*Level)*(160 - Level)) / 100;
}

SDL_Texture* Entity::LoadTexture(SDL_Renderer* ren, std::string img)
{
	SDL_Surface* surf = SDL_LoadBMP(img.c_str());
	SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 0xFF, 0x00, 0xFF));
	SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
	SDL_FreeSurface(surf);
	return tex;
}

bool Entity::lineLine(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {

	// calculate the direction of the lines
	float uA = ((x4 - x3)*(y1 - y3) - (y4 - y3)*(x1 - x3)) / ((y4 - y3)*(x2 - x1) - (x4 - x3)*(y2 - y1));
	float uB = ((x2 - x1)*(y1 - y3) - (y2 - y1)*(x1 - x3)) / ((y4 - y3)*(x2 - x1) - (x4 - x3)*(y2 - y1));

	// if uA and uB are between 0-1, lines are colliding
	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {

		// optionally, draw a circle where the lines meet
		float intersectionX = x1 + (uA * (x2 - x1));
		float intersectionY = y1 + (uA * (y2 - y1));
		return true;
	}
	return false;
}

bool Entity::lineRect(float x1, float y1, float x2, float y2, float rx, float ry, float rw, float rh) {

	// check if the line has hit any of the rectangle's sides
	// uses the Line/Line function below
	bool left = lineLine(x1, y1, x2, y2, rx, ry, rx, ry + rh);
	bool right = lineLine(x1, y1, x2, y2, rx + rw, ry, rx + rw, ry + rh);
	bool top = lineLine(x1, y1, x2, y2, rx, ry, rx + rw, ry);
	bool bottom = lineLine(x1, y1, x2, y2, rx, ry + rh, rx + rw, ry + rh);

	// if ANY of the above are true, the line
	// has hit the rectangle
	if (left || right || top || bottom) {
		return true;
	}
	return false;
}

void Entity::GenerateBloodSplatter(Viewport* viewport, double x, double y, double angle)
{
	for (int i = 0; i < rand() % 100 + 50; i++)
	{
		double a = rand();
		a *= 180 / M_PI;
		a += rand() % 30 - 15;
		a *= M_PI / 180;
		double nx = x + cos(angle + a) * (rand() % 50);
		double ny = y + sin(angle + a) * (rand() % 50);
		BloodParticle* b = new BloodParticle(nx, ny);
		EntityListPointer->push_back(b);
	}
}