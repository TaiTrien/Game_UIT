#pragma once
#include "GameObject.h"
#include"Simon.h"
#include "FireHolding.h"
#define  WHIP_STATE_ATTACK 100
#define  WHIP_ANI_ATTACK_RIGHT    0
#define  WHIP_ANI_ATTACK_LEFT     1
#define	 WHIP_ANI_EMPTY	   2
#define WHIP_BBOX_WIDTH    23
#define WHIP_BBOX_HEIGHT   8
class Whip :public CGameObject
{
	DWORD timer = 0;
	Simon *simon;
	bool isWhipAttack;
public:

	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *colliable_objects = NULL);
	virtual void Render();
	void SetState(int stat);
	void CloneSimon(Simon *SIMON) { simon = SIMON; }
	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
};

#pragma once
