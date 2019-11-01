#include "Whip.h"
#include"Simon.h"



void Whip::Update(DWORD time, vector<LPGAMEOBJECT>*colliable_object)
{
	
	if (simon->nx > 0)
	{
		if (animations[WHIP_ANI_ATTACK_RIGHT]->getCurrentFrame() == 0)
		{
			this->x = simon->x - 7.2f;
			this->y = simon->y + 5.0f;
		}
		else if (animations[WHIP_ANI_ATTACK_RIGHT]->getCurrentFrame() == 1)
		{
			this->x = simon->x - 11.0f;
			this->y = simon->y;
		}
		else if (animations[WHIP_ANI_ATTACK_RIGHT]->getCurrentFrame() == 2)
		{
			this->x = simon->x + 18.0f;
			this->y = simon->y + 5.5f;
		}
		else {
			this->x = simon->x - 7.2f;
			this->y = simon->y + 5.0f;
		}
	}
	else if (simon->nx < 0)
	{
		if (animations[WHIP_ANI_ATTACK_LEFT]->getCurrentFrame() == 0)
		{
			this->x = simon->x + 19.5f;
			this->y = simon->y + 5.0f;
		}
		else if (animations[WHIP_ANI_ATTACK_LEFT]->getCurrentFrame() == 1)
		{
			this->x = simon->x + 11.2f;
			this->y = simon->y;
		}
		else if (animations[WHIP_ANI_ATTACK_LEFT]->getCurrentFrame() == 2)
		{
			this->x = simon->x - 18.5f;
			this->y = simon->y + 5.5f;
		}
		else {
			this->x = simon->x + 19.5f;
			this->y = simon->y + 5.0f;
		}
		
	}


}
void Whip::Render()
{
	int ani = WHIP_ANI_EMPTY;
	if (isWhipAttack)
	{
		if (simon->nx > 0)
			ani = WHIP_ANI_ATTACK_RIGHT;
		else if(simon->nx < 0)
			ani = WHIP_ANI_ATTACK_LEFT;
	}
	animations[ani]->Render(x, y, 255);
	if (animations[ani]->getCurrentFrame() == 3)
		isWhipAttack = false;

}
void Whip::SetState(int stat)
{
	if (this->isWhipAttack) return;
	CGameObject::SetState(stat);
	switch (state = WHIP_STATE_ATTACK)
	{
	case WHIP_STATE_ATTACK:
		if (isWhipAttack) return;
		isWhipAttack = true;
		break;
	}
}
void Whip::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	if (simon->nx > 0)
	{
		if (animations[WHIP_ANI_ATTACK_RIGHT]->getCurrentFrame() == 0)
		{
			left = x;
			top = y;
			right = x + 9.0f;
			bottom = y + 24.0f;
		}
		else if (animations[WHIP_ANI_ATTACK_RIGHT]->getCurrentFrame() == 1)
		{
			left = x;
			top = y;
			right = x + 17.0f;
			bottom = y + 19.0f;
		}
		else if (animations[WHIP_ANI_ATTACK_RIGHT]->getCurrentFrame() == 2)
		{
			left = x;
			top = y;
			right = x + 23.0f;
			bottom = y + 8.0f;
		}
	}
	if (simon->nx < 0)
	{
		if (animations[WHIP_ANI_ATTACK_LEFT]->getCurrentFrame() == 0)
		{
			left = x;
			top = y;
			right = x + 9.0f;
			bottom = y + 24.0f;
		}
		else if (animations[WHIP_ANI_ATTACK_LEFT]->getCurrentFrame() == 1)
		{
			left = x;
			top = y;
			right = x + 17.0f;
			bottom = y + 19.0f;
		}
		else if (animations[WHIP_ANI_ATTACK_LEFT]->getCurrentFrame() == 2)
		{
			left = x;
			top = y;
			right = x + 23.0f;
			bottom = y + 8.0f;
		}
	}

}
