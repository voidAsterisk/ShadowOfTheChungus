#include "pch.h"
#include "Entity.h"

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

void Entity::Draw(SDL_Renderer* ren)
{
	if (Texture != nullptr) {
		SDL_Rect srcrect = { (int)AnimationX * Width, (int)AnimationY * Height, Width, Height };
		SDL_Rect dstrect = { X + viewport.X, Y + viewport.Y, Width * SCALE, Height * SCALE };
		SDL_SetTextureAlphaMod(Texture, Alpha);

		SDL_RenderCopy(ren, Texture, &srcrect, &dstrect);
	}
}

void Entity::DrawHealthBar(SDL_Renderer* ren)
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

struct Vec2
{
	double x;
	double y;
};

