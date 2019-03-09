#pragma once
#include <SDL.h>

SDL_Texture* LoadImage(SDL_Renderer* ren, std::string img)
{
	SDL_Surface* Image = SDL_LoadBMP(img.c_str());
	SDL_SetColorKey(Image, SDL_TRUE, SDL_MapRGB(Image->format, 0xFF, 0x00, 0xFF));
	return SDL_CreateTextureFromSurface(ren, Image);
}

int NextExp(int level)
{
	return round(0.04 * (level ^ 3) + 0.8 * (level ^ 2) + 2 * level);
}
int LevelToHP(int level)
{
	return round(0.04 * (level ^ 3) + 0.8 * (level ^ 2) + 2 * level);
}

bool lineLine(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {

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
bool lineRect(float x1, float y1, float x2, float y2, float rx, float ry, float rw, float rh) {

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

