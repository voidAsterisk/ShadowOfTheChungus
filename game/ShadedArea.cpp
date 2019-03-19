#include "pch.h"
#include "ShadedArea.h"
#include "Shader.h"

ShadedArea::ShadedArea()
{

}
ShadedArea::ShadedArea(std::string vertexPath, std::string fragmentPath)
{
	shader = new Shader(vertexPath.c_str(), fragmentPath.c_str());
}


ShadedArea::~ShadedArea()
{
}
