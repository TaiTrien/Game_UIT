#pragma once
#include "GameObject.h"
#include "Whip.h"
class Whip;
#define SIMON_WALKING_SPEED		0.1f 
//0.1f
#define SIMON_JUMP_SPEED_Y		0.5f
#define SIMON_JUMP_DEFLECT_SPEED 0.2f
#define SIMON_GRAVITY			0.002f
#define SIMON_DIE_DEFLECT_SPEED	 0.5f
#define SIMON_STATE_IDLE			0
#define SIMON_STATE_WALKING_RIGHT	100
#define SIMON_STATE_WALKING_LEFT	200
#define SIMON_STATE_JUMP			300
#define SIMON_STATE_DIE				400
#define SIMON_STATE_ATTACK			500
#define SIMON_STATE_SITTING			600
#define SIMON_STATE_STAND_UP		700
#define SIMON_STATE_EATING_ITEM     800

#define SIMON_ANI_BIG_IDLE_RIGHT		0
#define SIMON_ANI_BIG_IDLE_LEFT			1
#define SIMON_ANI_SMALL_IDLE_RIGHT		2
#define SIMON_ANI_SMALL_IDLE_LEFT			3

#define SIMON_ANI_BIG_WALKING_RIGHT			4
#define SIMON_ANI_BIG_WALKING_LEFT			5
#define SIMON_ANI_SMALL_WALKING_RIGHT		6
#define SIMON_ANI_SMALL_WALKING_LEFT		7
#define SIMON_ANI_JUMP_RIGHT				10
#define SIMON_ANI_JUMP_LEFT					11
#define SIMON_ANI_SIT_ATTACK_RIGHT			12
#define SIMON_ANI_SIT_ATTACK_LEFT			13


#define SIMON_ANI_ATTACK_LEFT		9
#define SIMON_ANI_ATTACK_RIGHT		8

#define	SIMON_LEVEL_SMALL	1
#define	SIMON_LEVEL_BIG		2

#define SIMON_BIG_BBOX_WIDTH  15
#define SIMON_BIG_BBOX_HEIGHT 27
#define SIMON_SIT_BBOX_WIDTH  25
#define SIMON_SIT_BBOX_HEIGHT  23

#define SIMON_SMALL_BBOX_WIDTH  13
#define SIMON_SMALL_BBOX_HEIGHT 15

#define SIMON_ATTACK_MAX_FRAME	3
#define SIMON_UNTOUCHABLE_TIME 5000


class Simon : public CGameObject
{
	int level;
	int untouchable;
	bool isAttacking;
	bool isJumping;
	bool isSitting;
	DWORD untouchable_start;
public: 
	Whip * whip;
	Simon() : CGameObject()
	{
		level = SIMON_LEVEL_BIG;
		untouchable = 0;
	}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *colliable_objects);
	virtual void Render();
	void SetState(int state);
	void SetLevel(int l) {level = l;}
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount(); }
	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
	bool getIsAttacking();
	bool getIsJumping();
	bool getIsSitting();
	void cloneWhip(Whip *WHIP) { whip = WHIP; }
};