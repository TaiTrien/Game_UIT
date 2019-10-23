#pragma once
#include "GameObject.h"
class PlayScene :public CGameObject
{
public:
	PlayScene();
	void Render();
	virtual void GetBoundingBox(float &l, float &t, float &r, float &b);
	~PlayScene();
};
