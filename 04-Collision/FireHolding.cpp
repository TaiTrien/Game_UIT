#include "FireHolding.h"

void FireHolding::Update(DWORD dt, vector<LPGAMEOBJECT> *colliable_objects) {
	// Calculate dx, dy 
	CGameObject::Update(dt);
	vy += ITEMS_GRAVITY * dt;
}
void FireHolding::Render()
{
	int ani = 0;
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
		if (index == 1 || index == 3)
				y += (FIREHOLDING_BBOX_HEIGHT - HEART_BBOX_HEIGHT - 3);
			else if (index == 2 || index == 4)
				y += (FIREHOLDING_BBOX_HEIGHT - UPGRADE_ITEM_BBOX_HEIGHT);
			else if (index == 5)
				y+= (FIREHOLDING_BBOX_HEIGHT - UPGRADE_ITEM_BBOX_HEIGHT);
			break;
	}
}