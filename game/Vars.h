#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
static int SCALE = 2;
static int SCRW = 1280;
static int SCRH = 720;
static bool FULLSCRN = true;
static bool keys[1024] = { false };
static SDL_Renderer* renderer;
static TTF_Font* monsterfont;
static Viewport viewport = Viewport(0, 0, SCRW, SCRH);
static bool quit = false;
static std::string currentmap;
static Player* player;

static SDL_Window* window;
static TTF_Font* msgfont;
static SDL_Texture* tex_tilesheet;

enum GameStates {
	TitleMenu,
	Playing,
	Death,
	Paused,
	Options
};
static GameStates gamestate = TitleMenu;
static GameStates lastgamestate;

class MouseState
{
public:
	int X;
	int Y;
	int LeftDown;
	int RightDown;
};
static MouseState currentmousestate, previousmousestate;

static std::vector<Solid> solids;