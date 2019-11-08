#include "FireHolding.h"

void FireHolding::Update(DWORD dt, vector<LPGAMEOBJECT> *colliable_objects) {
	// Calculate dx, dy 
	CGameObject::Update(dt);
	if (!isTouchGround && isAttacked) {
		dy = 0.5 * ITEMS_GRAVITY * dt * dt;
		y += dy;
	}
	for (UINT i = 0; i < colliable_objects->size(); i++)
	{
		if (dynamic_cast<CBrick *>(colliable_objects->at(i)))//is brick
		{
			CBrick * brick = dynamic_cast<CBrick *>(colliable_objects->at(i));
			float l, t, r, b; // left top right bottom of whip
			float L, T, R, B; //left top right bottom of fireholdings
			GetBoundingBox(l, t, r, b);
			brick->GetBoundingBox(L, T, R, B);
			if (t < B && b > T && r > L && l < R)
			{
				isTouchGround = true;
			}
			}
		}
	}
		


void FireHolding::Render()
{
	int ani;
	if (isAttacked) {
		switch (index) {
		case 1:
		case 3:
			ani = FIREHOLDING_ANI_HEART;
			break;
		case 2:
		case 4:
			ani = FIREHOLDING_ANI_UPGRADE_WHIP;
			break;
		case 5:
			ani = FIREHOLDING_ANI_KNIFE_RIGHT;
			break;
		}
	}
	else if (!isAttacked)
		ani = FIREHOLDING_ANI_INDLE;
	animations[ani]->Render(x, y, 255);
	RenderBoundingBox();
}
void FireHolding::GetBoundingBox(float &left, float &top, float &right, float &bottom) {

	left = x;
	top = y;
	if (isAttacked) {
		if (index == 1 || index == 3) {
			right = x + HEART_BBOX_WIDTH;
			bottom = y + HEART_BBOX_HEIGHT;
			return;
		}
		else if (index == 2 || index == 4) {
			right = x + UPGRADE_ITEM_BBOX_WIDTH;
			bottom = y + UPGRADE_ITEM_BBOX_HEIGHT;
			return;
		}
		else {
			right = x + KNIFE_BBOX_WIDTH;
			bottom = y + KNIFE_BBOX_HEIGHT;
			return;
		}
	}
	right = x + FIREHOLDING_BBOX_WIDTH;
	bottom = y + FIREHOLDING_BBOX_HEIGHT;
}
void FireHolding::SetState(int state) {

	CGameObject::SetState(state);

	switch (state) {
	case FIREHOLDING_STATE_ATTACKED:
		if (isAttacked) return;
			isAttacked = true;
			isTouchGround = false;
		break;
		//if (index == 1 || index == 3)
		//		/*y += (FIREHOLDING_BBOX_HEIGHT - HEART_BBOX_HEIGHT - 3);*/
		//	else if (index == 2 || index == 4)
		//		y += (FIREHOLDING_BBOX_HEIGHT - UPGRADE_ITEM_BBOX_HEIGHT);
		//	else if (index == 5)
		//		y+= (FIREHOLDING_BBOX_HEIGHT - UPGRADE_ITEM_BBOX_HEIGHT);
		//	break;
	}
}