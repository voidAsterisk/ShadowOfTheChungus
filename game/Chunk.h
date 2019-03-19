#pragma once
class Chunk
{
public:
	int X;
	int Y;
	int Width;
	int Height;
	int** Data;

	Chunk(int x, int y, int w, int h);
	~Chunk();
};