#pragma once
#include "GameObject.h"
class FireHolding :public CGameObject
{
public:

	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *colliable_objects = NULL) { return; };
	virtual void Render();
	void SetState(int stat) { return; };
	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
};
