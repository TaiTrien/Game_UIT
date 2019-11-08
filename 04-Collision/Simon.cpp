#include <algorithm>
#include "debug.h"
#include "Simon.h"
#include "Game.h"
#include "FireHolding.h"
#include "Goomba.h"
#include "Brick.h"
#include "Whip.h"

void Simon::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
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


		//Collision logic with items
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{

			LPCOLLISIONEVENT e = coEventsResult[i]; 
			if (dynamic_cast<CBrick *>(e->obj)) // if e->obj is brick 
			{
				x += min_tx * dx + nx * 0.4f;		// nx*0.4f : need to push out a bit to avoid overlapping next frame
				y += min_ty * dy + ny * 0.4f;
				if (nx != 0) vx = 0;
				if (ny != 0) {
					vy = 0;
					isJumping = false;
				}
			}

			if (!dynamic_cast<CBrick *>(e->obj)) {
				x += dx;
				if (isJumping)
					y += dy;
			}

			if (dynamic_cast<FireHolding *>(e->obj)) // if e->obj is fire holdings 
			{
				FireHolding *fireHolding = dynamic_cast<FireHolding *>(e->obj);
				if (fireHolding->GetState() == FIREHOLDING_STATE_ATTACKED) {
					if (fireHolding->getIndex() == 2 || fireHolding->getIndex() == 4) {
						this->whip->levelUpWhip();
					}
					if (fireHolding->getIndex() == 5) {
						numberOfDagger+=3;
					}
						this->SetState(SIMON_STATE_EATING_ITEM);
						fireHolding->isVanish = true;
				}
			}
			return;
		}
		
	}
	
	for (UINT i = 0; i < coObjects->size(); i++)
	{

		if (dynamic_cast<FireHolding *>(coObjects->at(i)))//is fire holdings
		{
			FireHolding * fireHolding = dynamic_cast<FireHolding *>(coObjects->at(i));
			Whip *whip = dynamic_cast<Whip *>(coObjects->at(i));
			float l, t, r, b; // left top right bottom of simon
			float L, T, R, B; //left top right bottom of fireholdings
			GetBoundingBox(l, t, r, b);
			fireHolding->GetBoundingBox(L, T, R, B);
			if (t < B && b > T && r > L && l < R)
			{
				if (fireHolding->GetState() == FIREHOLDING_STATE_ATTACKED || this->GetState() == SIMON_STATE_ATTACK) {
					if (fireHolding->getIndex() == 2 || fireHolding->getIndex() == 4) {
						this->whip->levelUpWhip();
					}
					if (fireHolding->getIndex() == 5) {
						numberOfDagger += 3;
					}
					this->SetState(SIMON_STATE_EATING_ITEM);
					fireHolding->isVanish = true;
				}
					
			}
		}
	}
	
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

	}

	// clean up collision events


void Simon::Render()
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
	if (isAttacking) {
		if (animations[ani]->getCurrentFrame() == SIMON_ATTACK_MAX_FRAME)
		isAttacking = false;
	}
}

void Simon::SetState(int state)
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
			return;
		this->isAttacking = true;
		vx = 0;
		break;
	case SIMON_STATE_SITTING:
		if (isAttacking) return;
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
	case SIMON_STATE_EATING_ITEM:
		vx = 0;
		break;
	}
	
	
}

void Simon::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	left = x;
	top = y; 
	
	right = x + SIMON_BIG_BBOX_WIDTH;
	bottom = y + SIMON_BIG_BBOX_HEIGHT;	
	if (isSitting) {
		bottom -= 5;
	}
	
}
void Simon::usingSubWeapons() {
	if (isUsingSubWeapons) {
		dagger = new Dagger();
		dagger->SetPosition(this->x, this->y);
		dagger->SetState(DAGGER_STATE_ATTACK);
		dagger->nx = this->nx;
	}
}

bool Simon::getIsAttacking() {
	return isAttacking;
}
bool Simon::getIsJumping() {
	return isJumping;
}
bool Simon::getIsSitting() {
	return isSitting;
}
bool Simon::getIsUsingSubWeapons() {
	if (numberOfDagger > 0) {
		numberOfDagger--;
		isUsingSubWeapons = true;
	}
	else isUsingSubWeapons = false;
	return isUsingSubWeapons;
}

