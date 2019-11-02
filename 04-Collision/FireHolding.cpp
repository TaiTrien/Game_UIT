#include "FireHolding.h"

void FireHolding::Render()
{
	animations[0]->Render(x, y, 255);
}
void FireHolding::GetBoundingBox(float &left, float &top, float &right, float &bottom) {
	left = x;
	top = y;
	right = x + 16.0f;
	bottom = y + 30.0f;
}