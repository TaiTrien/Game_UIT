#include "PlayScene.h"

void PlayScene::Render() {
	animations[0]->Render(x, y);
}

void PlayScene::GetBoundingBox(float &l, float &t, float &r, float &b)
{
	return;

}

PlayScene::~PlayScene() {}
