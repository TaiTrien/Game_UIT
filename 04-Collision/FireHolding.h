#pragma once
#include "GameObject.h"
#define FIREHOLDING_BBOX_WIDTH  16
#define FIREHOLDING_BBOX_HEIGHT 32

#define HEART_BBOX_WIDTH 12
#define HEART_BBOX_HEIGHT 10

#define FIREHOLDING_STATE_NORMAL	0
#define FIREHOLDING_STATE_ATTACKED	100


#define FIREHOLDING_ANI_INDLE 0
#define FIREHOLDING_ANI_HEART 1
class FireHolding :public CGameObject
{
protected:
	bool isAttacked;
public:

	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *colliable_objects = NULL) { return; };
	virtual void Render();
	void SetState(int stat);
	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
};
