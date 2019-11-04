#include "FireHolding.h"

void FireHolding::Render()
{
	int ani = 0;
	if (isAttacked)
		ani = FIREHOLDING_ANI_HEART;
	animations[ani]->Render(x, y, 255);
	RenderBoundingBox();
}
void FireHolding::GetBoundingBox(float &left, float &top, float &right, float &bottom) {

	left = x;
	
	if (isAttacked) {
		top = y;
		right = x + HEART_BBOX_WIDTH;
		bottom = y + HEART_BBOX_HEIGHT;
		return;
	}
	top = y;
	right = x + FIREHOLDING_BBOX_WIDTH;
	bottom = y + FIREHOLDING_BBOX_HEIGHT;
}
void FireHolding::SetState(int state) {

	CGameObject::SetState(state);

	switch (state) {
	case FIREHOLDING_STATE_ATTACKED:
		if (isAttacked) return;
			isAttacked = true;
			y += (FIREHOLDING_BBOX_HEIGHT - HEART_BBOX_HEIGHT - 3);
	}
}