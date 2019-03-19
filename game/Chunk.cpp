#include "pch.h"
#include "Chunk.h"


Chunk::Chunk(int x, int y, int w, int h)
{
	X = x;
	Y = y;
	Width = w;
	Height = h;
	Data = new int*[w];
	for (int i = 0; i < w; i++)
	{
		Data[i] = new int[h];
	}
}


Chunk::~Chunk()
{
	for (int i = 0; i < Width; i++)
			delete(Data[i]);

}