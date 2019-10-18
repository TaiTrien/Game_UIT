﻿#include <algorithm>
#include "debug.h"

#include "Simon.h"
#include "Game.h"

#include "Goomba.h"

void CSimon::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	// Calculate dx, dy 
	CGameObject::Update(dt);

	// Simple fall down
	vy += SIMON_GRAVITY*dt;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state!=SIMON_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

	// reset untouchable timer if untouchable time has passed
	if ( GetTickCount() - untouchable_start > SIMON_UNTOUCHABLE_TIME) 
	{
		untouchable_start = 0;
		untouchable = 0;
	}
	//if (isSitting) {
	//	y += 0.2; //Update tọa độ cho sprite ngồi
	//}
	// No collision occured, proceed normally
	if (coEvents.size()==0)
	{
		x += dx; 
		y += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);

		// block 
		x += min_tx*dx + nx*0.4f;		// nx*0.4f : need to push out a bit to avoid overlapping next frame
		y += min_ty*dy + ny*0.4f;
		
		if (nx!=0) vx = 0;
		if (ny != 0)
		{
			vy = 0;
			isJumping = false;
		}


		// Collision logic with Goombas
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CGoomba *>(e->obj)) // if e->obj is Goomba 
			{
				CGoomba *goomba = dynamic_cast<CGoomba *>(e->obj);

				// jump on top >> kill Goomba and deflect a bit 
				if (e->ny < 0)
				{
					if (goomba->GetState()!= GOOMBA_STATE_DIE)
					{
						goomba->SetState(GOOMBA_STATE_DIE);
						vy = -SIMON_JUMP_DEFLECT_SPEED;
					}
				}
				else if (e->nx != 0)
				{
					if (untouchable==0)
					{
						if (goomba->GetState()!=GOOMBA_STATE_DIE)
						{
							if (level > SIMON_LEVEL_SMALL)
							{
								level = SIMON_LEVEL_SMALL;
								StartUntouchable();
							}
							else 
								SetState(SIMON_STATE_DIE);
						}
					}
				}
			}
		}
	}

	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CSimon::Render()
{
	int ani;
		if (isJumping) {
			if (nx > 0) {
				if (isAttacking) {
						ani = SIMON_ANI_ATTACK_RIGHT;
				}
				else ani = SIMON_ANI_JUMP_RIGHT;
			}
			else {
				if (isAttacking) {
					ani = SIMON_ANI_ATTACK_LEFT;
				}
				else ani = SIMON_ANI_JUMP_LEFT;
			}
		}
		else if (!isJumping) {
			if (vx == 0)
			{
				if (nx > 0) {
					if (isAttacking) {
						if (isSitting)
						{
							ani = SIMON_ANI_SIT_ATTACK_RIGHT;
						}
						else 
							ani = SIMON_ANI_ATTACK_RIGHT;
					}
					else if(isSitting) {
						ani = SIMON_ANI_JUMP_RIGHT; // VÌ SPRITE JUMP GIỐNG SPRITE NGỒI
					}
					else ani = SIMON_ANI_BIG_IDLE_RIGHT;
				}
				else
				{
					if (isAttacking) {
						if (isSitting) {
							ani = SIMON_ANI_SIT_ATTACK_LEFT;
						}
						else
							ani = SIMON_ANI_ATTACK_LEFT;
						//else ani = SIMON_ANI_ATTACK_LEFT;
					}
					else if (isSitting) {
						ani = SIMON_ANI_JUMP_LEFT; // VÌ SPRITE JUMP GIỐNG SPRITE NGỒI
					}
					else
						ani = SIMON_ANI_BIG_IDLE_LEFT;
				}
			}
			else if (vx > 0)
				ani = SIMON_ANI_BIG_WALKING_RIGHT;
			else ani = SIMON_ANI_BIG_WALKING_LEFT;
		}
		
	int alpha = 255;
	if (untouchable) alpha = 128;
	animations[ani]->Render(x, y, alpha);
	RenderBoundingBox();
	if (isAttacking && animations[ani]->getCurrentFrame() == SIMON_ATTACK_MAX_FRAME) {
			isAttacking = false;
	}
}

void CSimon::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case SIMON_STATE_WALKING_RIGHT:
		if (this->isJumping || this->isSitting) {
			vx = 0;
		}
		else if (this->isAttacking) {
			vx = 0;
		}
		else
			vx = SIMON_WALKING_SPEED;
		nx = 1;
		break;
	case SIMON_STATE_WALKING_LEFT:
		if (this->isJumping || this->isSitting) {
			vx = 0;
		}
		else if (this->isAttacking) {
			vx = 0;
		}
		else
			vx = -SIMON_WALKING_SPEED;
		nx = -1;
		break;
	case SIMON_STATE_JUMP:
		if (isJumping)
			break;
		isJumping = true;
		vy = -SIMON_JUMP_SPEED_Y;
		break;
	case SIMON_STATE_IDLE:
		vx = 0;
		isSitting = false;
		break;
	case SIMON_STATE_DIE:
		vy = -SIMON_DIE_DEFLECT_SPEED;
		break;
	case SIMON_STATE_ATTACK:
		if (this->isAttacking)
			break;
		this->isAttacking = true;
		break;
	case SIMON_STATE_SITTING:
		if (this->isSitting)
			break;
		vx = 0;
		y += 5;
		this->isSitting = true;
		break;
	case SIMON_STATE_STAND_UP:
		y -= 5;
		isSitting = false;
		break;
	}
	
}

void CSimon::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	left = x;
	top = y; 
	
	right = x + SIMON_BIG_BBOX_WIDTH;
	bottom = y + SIMON_BIG_BBOX_HEIGHT;	
	if (isSitting) {
		bottom -= 5;
	}
	
}

