#include "pch.h"
#include "Chungus.h"
#include "Viewport.h"
#include "Model_OBJ.h"
#include "Shader.h"
Chungus::Chungus(int value)
{
	Model = new Model_OBJ();
	Model->Load("models/chungus.obj");
	shader = new Shader("shaders/default.vs", "shaders/default.fs");
}

void Chungus::Draw(Viewport vp, SDL_Renderer* ren)
{
	
	Model->Draw();
	glUseProgram(0);
}
