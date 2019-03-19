#pragma once
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <map>
#include "Entity.h"

class Viewport;
class Cursor;
class Player;

class NPC : public
	Entity
{
	TTF_Font* Font;
	Viewport* v;
	Cursor* c;
	Player* p;
	Mix_Chunk* sfx_dialog;
	std::map<std::string, bool>* f;
public:
	std::string Dialog = "";
	std::string AltDialog = "";
	std::string AltDialogItem = "";
	std::string AltDialogFlag = "";

	NPC(std::map<std::string, bool>* flags, std::vector<Entity*>* entities, Player* player, TTF_Font* font, Viewport* viewport, Cursor* cursor, double x, double y);
	~NPC();
	virtual void OnClick();

	virtual void Draw(Viewport vp, SDL_Renderer* ren);
};