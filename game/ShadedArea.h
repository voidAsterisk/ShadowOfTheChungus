#pragma once
#include <GL/glew.h>
#include <SDL.h>
#include <string>
class Shader;
class ShadedArea
{
public:
	SDL_Rect Rect;
	Shader* shader;
	ShadedArea();
	ShadedArea(std::string vertexPath, std::string fragmentPath);
	~ShadedArea();
};

