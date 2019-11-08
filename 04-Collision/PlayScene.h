#pragma once
#include "GameObject.h"
#include <fstream>

class PlayScene :public CGameObject
{

public:
	PlayScene() {};
	void Render();
	virtual void GetBoundingBox(float &l, float &t, float &r, float &b);
	~PlayScene();
};
