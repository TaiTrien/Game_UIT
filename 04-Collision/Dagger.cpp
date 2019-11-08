#include "Dagger.h"
#include"Simon.h"
#include "debug.h"


void Dagger::Update(DWORD dt, vector<LPGAMEOBJECT>*colliable_object)
{
	if (nx > 0)
	{
		vx = DAGGER_SPEED;
	}
	else if (nx < 0)
	{
		vx = -DAGGER_SPEED;
	}
	if (isDaggerAttack) {
		dx = dt * vx;
		x += dx;
	}

	for (UINT i = 0; i < colliable_object->size(); i++)
	{

		if (dynamic_cast<FireHolding *>(colliable_object->at(i)))//is fire holdings
		{
			FireHolding * fireHolding = dynamic_cast<FireHolding *>(colliable_object->at(i));
			float l, t, r, b; // left top right bottom of whip
			float L, T, R, B; //left top right bottom of fireholdings
			GetBoundingBox(l, t, r, b);
			fireHolding->GetBoundingBox(L, T, R, B);
			if (t < B && b > T && r > L && l < R)
			{
				if (fireHolding->GetState() != FIREHOLDING_STATE_ATTACKED) {
					fireHolding->SetState(FIREHOLDING_STATE_ATTACKED);
					this->isDaggerAttack = false;
				}
					
			}
		}
	}
}


void Dagger::Render()
{
	int ani;
	if (isDaggerAttack)
	{
		if (nx > 0)
			ani = DAGGER_ANI_ATTACK_RIGHT;
		else if (nx < 0)
			ani = DAGGER_ANI_ATTACK_LEFT;

	}
	else return;
	aniNow = ani;
	animations[ani]->Render(x, y, 255);
}
void Dagger::SetState(int stat)
{
	if (this->isDaggerAttack) return;
	CGameObject::SetState(stat);
	switch (state)
	{
	case DAGGER_STATE_ATTACK:
		if (isDaggerAttack) return;
		isDaggerAttack = true;
		break;
	}
}
void Dagger::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	int deltaLength = 0;
	if (nx > 0)
	{
		if (animations[aniNow]->getCurrentFrame() == 0)
		{
			left = x;
			top = y;
			right = x + 9.0f + deltaLength;
			bottom = y + 24.0f + deltaLength;
		}
		else if (animations[aniNow]->getCurrentFrame() == 1)
		{
			left = x;
			top = y;
			right = x + 17.0f + deltaLength;
			bottom = y + 19.0f + deltaLength;
		}
		else if (animations[aniNow]->getCurrentFrame() == 2)
		{
			left = x;
			top = y;
			right = x + 23.0f + deltaLength;
			bottom = y + 8.0f + deltaLength;
		}
	}
	if (nx < 0)
	{
		if (animations[aniNow]->getCurrentFrame() == 0)
		{
			left = x;
			top = y;
			right = x + 9.0f + deltaLength;
			bottom = y + 24.0f + deltaLength;
		}
		else if (animations[aniNow]->getCurrentFrame() == 1)
		{
			left = x;
			top = y;
			right = x + 17.0f + deltaLength;
			bottom = y + 19.0f + deltaLength;
		}
		else if (animations[aniNow]->getCurrentFrame() == 2)
		{
			left = x;
			top = y;
			right = x + 23.0f + deltaLength;
			bottom = y + 8.0f + deltaLength;
		}
	}

}
