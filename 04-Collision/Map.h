#pragma once
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <fstream>
#include <vector>

#include "Sprites.h"
#include "Textures.h"

using namespace std;
class CMap
{
	vector <LPSPRITE> objects;

public:
	CMap();
	void LoadResources();
	~CMap();

private:

};

CMap::CMap()
{
}

CMap::~CMap()
{
}
