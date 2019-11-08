#pragma once

#include "GameObject.h"
#include "FireHolding.h"
#include "Whip.h"
#include "Simon.h"
class Simon;
#define  WHIP_STATE_ATTACK 100
#define  WHIP_TYPE1_ANI_ATTACK_RIGHT    0
#define  WHIP_TYPE1_ANI_ATTACK_LEFT     1

#define  WHIP_TYPE2_ANI_ATTACK_RIGHT    2
#define  WHIP_TYPE2_ANI_ATTACK_LEFT     3

#define  WHIP_TYPE3_ANI_ATTACK_RIGHT    4
#define  WHIP_TYPE3_ANI_ATTACK_LEFT		5

#define	 WHIP_ANI_EMPTY	   6
#define WHIP_BBOX_WIDTH    23
#define WHIP_BBOX_HEIGHT   8
class Whip :public CGameObject
{
	DWORD timer = 0;
	Simon *simon;
	bool isWhipAttack;
	int levelWhip = 1;
	int aniNow; // to know index of animations
public:
	Whip() {
		for (int i = 800; i <= 805; i++) {
			this->AddAnimation(i);
		}
		this->AddAnimation(602);
	}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *colliable_objects = NULL);
	virtual void Render();
	void SetState(int stat);
	void CloneSimon(Simon *SIMON) { simon = SIMON; }
	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
	void levelUpWhip() { levelWhip++; }
};

#pragma once
