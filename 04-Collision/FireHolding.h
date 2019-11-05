#pragma once
#include "GameObject.h"
#define ITEMS_GRAVITY 0.002f
#define FIREHOLDING_BBOX_WIDTH  16
#define FIREHOLDING_BBOX_HEIGHT 32

#define HEART_BBOX_WIDTH 12
#define HEART_BBOX_HEIGHT 10

#define UPGRADE_ITEM_BBOX_WIDTH 17
#define UPGRADE_ITEM_BBOX_HEIGHT 17

#define KNIFE_BBOX_WIDTH 17
#define KNIFE_BBOX_HEIGHT 9

#define FIREHOLDING_STATE_NORMAL	0
#define FIREHOLDING_STATE_ATTACKED	100
#define FIREHOLDING_STATE_VANISH    200


#define FIREHOLDING_ANI_INDLE 0
#define FIREHOLDING_ANI_HEART 1
#define FIREHOLDING_ANI_UPGRADE_WHIP 2
#define FIREHOLDING_ANI_KNIFE_RIGHT 3
#define FIREHOLDING_ANI_KNIFE_LEFT 4


class FireHolding :public CGameObject
{
protected:
	bool isAttacked;
	int index;
	float vy;
public:

	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *colliable_objects);
	virtual void Render();
	void SetState(int stat);
	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
	void setIndexOfFireHolding(int x) { this->index = x; };
};
