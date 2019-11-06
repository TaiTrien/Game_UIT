#pragma once

#include "GameObject.h"
#include "FireHolding.h"
#include "Whip.h"
#include "Simon.h"
class Simon;
#define DAGGER_SPEED				0.2f
#define  DAGGER_STATE_ATTACK		100
#define DAGGER_ANI_ATTACK_RIGHT		0
#define  DAGGER_ANI_ATTACK_LEFT		1

#define WHIP_BBOX_WIDTH    17
#define WHIP_BBOX_HEIGHT   9
class Dagger :public CGameObject
{
	DWORD timer = 0;
	Simon *simon;
	bool isDaggerAttack;
	int aniNow; // to know index of animations
public:
	Dagger() {
		this->AddAnimation(1002);
		this->AddAnimation(1003);
	}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *colliable_objects = NULL);
	virtual void Render();
	void SetState(int stat);
	void CloneSimon(Simon *SIMON) { simon = SIMON; }
	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
};

