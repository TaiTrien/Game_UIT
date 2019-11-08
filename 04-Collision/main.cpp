/* =============================================================
	INTRODUCTION TO GAME PROGRAMMING SE102
	
	SAMPLE 04 - COLLISION

	This sample illustrates how to:

		1/ Implement SweptAABB algorithm between moving objects
		2/ Implement a simple (yet effective) collision frame work

	Key functions: 
		CGame::SweptAABB
		CGameObject::SweptAABBEx
		CGameObject::CalcPotentialCollisions
		CGameObject::FilterCollision

		CGameObject::GetBoundingBox
		
================================================================ */

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <fstream>
#include <iostream>
#include <string> 
#include "debug.h"
#include "Game.h"
#include "GameObject.h"
#include "Textures.h"
#include "PlayScene.h"
#include "Simon.h"
#include "Whip.h"
#include "Dagger.h"
#include "FireHolding.h"
#include "Brick.h"
#include "Goomba.h"
using namespace std;
#define WINDOW_CLASS_NAME L"SampleWindow"
#define MAIN_WINDOW_TITLE L"04 - Collision"

#define BACKGROUND_COLOR D3DCOLOR_XRGB(0, 0, 0)
#define SCREEN_WIDTH 260
#define SCREEN_HEIGHT 220

#define TITLE_WIDTH 32
#define TITLE_HEIGHT 32


#define MAX_FRAME_RATE 120

#define SPRITE_TIME 100

#define ID_TEX_ENEMY 10
#define ID_TEX_MISC 20
#define ID_TEX_SIMON 30
#define ID_TEX_WHIP 40
#define ID_TEX_MAP 50
#define ID_TEX_FIRE 60
#define ID_TEX_BRICK 70
#define ID_TEX_ITEMS 80
#define ID_TEX_MAP_ITEMS 90

CGame *game;
Simon *simon;
PlayScene *playScene;
vector<vector<LPSPRITE>> map;
FireHolding *fire;
//CGoomba *goomba;

vector<LPGAMEOBJECT> objects;

class CSampleKeyHander: public CKeyEventHandler
{
	virtual void KeyState(BYTE *states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
};

CSampleKeyHander * keyHandler; 

void CSampleKeyHander::OnKeyDown(int KeyCode)
{
	DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	switch (KeyCode)
	{
	case DIK_SPACE:
		if (simon->getIsSitting()) return;
		simon->SetState(SIMON_STATE_JUMP);
		break;
	case DIK_A:
		simon->whip->SetState(WHIP_STATE_ATTACK);
		simon->SetState(SIMON_STATE_ATTACK);
		break;
	case DIK_S: // to use subweapons
		simon->SetState(SIMON_STATE_ATTACK); // like STATE THROWING LIKE STATE ATTACK
		if (simon->getIsUsingSubWeapons()) {
			simon->usingSubWeapons();
			objects.push_back(simon->dagger);
		}
		break;
	case DIK_LEFT:
		if (simon->getIsAttacking()) return;
		if (simon->getIsJumping()) return;
		simon->nx = -1;
		break;
	case DIK_RIGHT:
		if (simon->getIsAttacking()) return;
		if (simon->getIsJumping()) return;
		simon->nx = 1;
		break;
	}

}

void CSampleKeyHander::OnKeyUp(int KeyCode)
{
	DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);
	switch (KeyCode)
	{
	case DIK_DOWN:
		simon->SetState(SIMON_STATE_STAND_UP);
		break;
	}
}

void CSampleKeyHander::KeyState(BYTE *states)
{
	if (simon->GetState() == SIMON_STATE_DIE) return;
	if (game->IsKeyDown(DIK_DOWN))
		simon->SetState(SIMON_STATE_SITTING);
	else if (game->IsKeyDown(DIK_RIGHT)) {
		if (simon->getIsAttacking()) return;
		if (simon->getIsJumping()) return;
		simon->SetState(SIMON_STATE_WALKING_RIGHT);
	}
	else if (game->IsKeyDown(DIK_LEFT)) {
		if (simon->getIsAttacking()) return;
		if (simon->getIsJumping()) return;
		simon->SetState(SIMON_STATE_WALKING_LEFT);
	}
	else
		simon->SetState(SIMON_STATE_IDLE);
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

wchar_t* ConvertToWideChar(char* p) // hàm này covert string sang wchar_t*, coppy trên mạng
{
	wchar_t *r;
	r = new wchar_t[strlen(p) + 1];

	char *tempsour = p;
	wchar_t *tempdest = r;
	while (*tempdest++ = *tempsour++);

	return r;
}
void LoadResources()
{
	CTextures * textures = CTextures::GetInstance();

	ifstream infileTextures("text\\Textures.txt");
	int idTextures, R, G, B;
	string filePath;
	while (infileTextures) {
		infileTextures >> idTextures >> filePath >> R >> G >> B;
		textures->Add(idTextures, ConvertToWideChar((char*)filePath.c_str()), D3DCOLOR_XRGB(R, G, B));
	}	

	CSprites * sprites = CSprites::GetInstance();
	CAnimations * animations = CAnimations::GetInstance();
	
	
	LPDIRECT3DTEXTURE9 texSimon = textures->Get(ID_TEX_SIMON);
	LPDIRECT3DTEXTURE9 texWhip = textures->Get(ID_TEX_WHIP);
	LPDIRECT3DTEXTURE9 texMap = textures->Get(ID_TEX_MAP);
	LPDIRECT3DTEXTURE9 texFire = textures->Get(ID_TEX_FIRE);
	LPDIRECT3DTEXTURE9 texBrick = textures->Get(ID_TEX_BRICK);
	LPDIRECT3DTEXTURE9 texItems = textures->Get(ID_TEX_ITEMS);
	LPDIRECT3DTEXTURE9 texMapItems = textures->Get(ID_TEX_MAP_ITEMS);
	

	ifstream infile("text\\Sprites.txt"); // load all sprites
	int arr[5], idSprite;
	while (infile)
	{
		infile >> idSprite >> arr[0] >> arr[1] >> arr[2] >> arr[3] >> arr[4];
		if (arr[4] == ID_TEX_SIMON)
			sprites->Add(idSprite, arr[0], arr[1], arr[2], arr[3], texSimon);
		else if (arr[4] == ID_TEX_WHIP)
			sprites->Add(idSprite, arr[0], arr[1], arr[2], arr[3], texWhip);
		else if (arr[4] == ID_TEX_MAP)
			sprites->Add(idSprite, arr[0], arr[1], arr[2], arr[3], texMap);
		else if (arr[4] == ID_TEX_FIRE)
			sprites->Add(idSprite, arr[0], arr[1], arr[2], arr[3], texFire);
		else if (arr[4] == ID_TEX_BRICK)
			sprites->Add(idSprite, arr[0], arr[1], arr[2], arr[3], texBrick);
		else if (arr[4] == ID_TEX_ITEMS)
			sprites->Add(idSprite, arr[0], arr[1], arr[2], arr[3], texItems);
		else if (arr[4] == ID_TEX_MAP_ITEMS)
			sprites->Add(idSprite, arr[0], arr[1], arr[2], arr[3], texMapItems);
	}
	
	ifstream infileAni("text\\Animations.txt"); // load all animations
	LPANIMATION ani;
	int arr1[4], idAnimation;
	while (infileAni)
	{
		infileAni >> idAnimation >> arr1[0] >> arr1[1] >> arr1[2] >> arr1[3];
		ani = new CAnimation(SPRITE_TIME);

		for (int i = 0; i < 4; i++) {
			if (arr1[i] != -1) {
				ani->Add(arr1[i]);
			}
			animations->Add(idAnimation, ani);
		}
	}

	simon = new Simon();
	objects.push_back(simon);

	ifstream infile2("text\\Entrance.txt"); // load entrance
	int indexTitleMap; // index of title
	for (int d = 0; d < 5; d++) {
		vector<LPSPRITE> temp;
		for (int c = 0; c < 24; c++) {
			infile2 >> indexTitleMap;
			temp.push_back(sprites->Get(indexTitleMap));
		}
		map.push_back(temp);
	}
	infile2.close();

	simon->whip = new Whip();
	simon->whip->CloneSimon(simon);
	objects.push_back(simon->whip);

	for (int i = 1; i <= 5; i++) { // to create 5 fire holding
		fire = new FireHolding();
		fire->setIndexOfFireHolding(i);
		fire->SetPosition(120*i, 130);
		objects.push_back(fire);
	}
	
	for (int i = 0; i < 60; i++)
	{
		CBrick *brick = new CBrick();
		brick->SetPosition(0 + i*16.0f, 160);
		objects.push_back(brick);
	}

	// and Goombas 
	//for (int i = 0; i < 4; i++)
	//{
	//	goomba = new CGoomba();
	//	goomba->AddAnimation(701);
	//	goomba->AddAnimation(702);
	//	goomba->SetPosition(200 + i*60, 135);
	//	goomba->SetState(GOOMBA_STATE_WALKING);
	//	objects.push_back(goomba);
	//}

}

/*
	Update world status for this frame
	dt: time period between beginning of last frame and beginning of this frame
*/
void Update(DWORD dt)
{
	// We know that Mario is the first object in the list hence we won't add him into the colliable object list
	// TO-DO: This is a "dirty" way, need a more organized way 

	vector<LPGAMEOBJECT> coObjects;
	for (int i = 1; i < objects.size(); i++)
	{
		coObjects.push_back(objects[i]);
	}
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i]->isVanish) {
			objects.erase(objects.begin() + i);
		}
		objects[i]->Update(dt, &coObjects);
	}

	// Update camera to follow simon
	float cx, cy;
	simon->GetPosition(cx, cy);

	cx -= SCREEN_WIDTH / 2;
	cy -= SCREEN_HEIGHT / 2;

	CGame::GetInstance()->SetCamPos(cx, 0.0f /*cy*/);
}
// draw map temporarily
void DrawMap() {
	float camX = CGame::GetInstance()->getCamX();
	float camY = CGame::GetInstance()->getCamY();
	int startCol = camX / TITLE_WIDTH; // column to start drawing
	int endCol = (camX + SCREEN_WIDTH) / TITLE_WIDTH; //column to end drawing
	float x, y; // position to draw
	for (int i = 0; i <= 4; i++) {
		for (int j = startCol; j <= endCol; j++) {
			y = camY;
			x = TITLE_WIDTH * (j - startCol) + camX - (int)camX % TITLE_WIDTH;
			map[i][j]->Draw(x, y);
		}
		camY += TITLE_HEIGHT;
	}

}
/*
	Render a frame 
*/
void Render()
{
	LPDIRECT3DDEVICE9 d3ddv = game->GetDirect3DDevice();
	LPDIRECT3DSURFACE9 bb = game->GetBackBuffer();
	LPD3DXSPRITE spriteHandler = game->GetSpriteHandler();

	if (d3ddv->BeginScene())
	{
		// Clear back buffer with a color
		d3ddv->ColorFill(bb, NULL, BACKGROUND_COLOR);

		spriteHandler->Begin(D3DXSPRITE_ALPHABLEND);
		DrawMap();
		for (int i = 1; i < objects.size(); i++) {
			objects[i]->Render();
		}
		objects[0]->Render(); // to render simon above fire holdings


		spriteHandler->End();
		d3ddv->EndScene();
	}

	// Display back buffer content to the screen
	d3ddv->Present(NULL, NULL, NULL, NULL);
}

HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int ScreenWidth, int ScreenHeight)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;

	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	HWND hWnd =
		CreateWindow(
			WINDOW_CLASS_NAME,
			MAIN_WINDOW_TITLE,
			WS_OVERLAPPEDWINDOW, // WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			ScreenWidth,
			ScreenHeight,
			NULL,
			NULL,
			hInstance,
			NULL);

	if (!hWnd) 
	{
		OutputDebugString(L"[ERROR] CreateWindow failed");
		DWORD ErrCode = GetLastError();
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}

int Run()
{
	MSG msg;
	int done = 0;
	DWORD frameStart = GetTickCount();
	DWORD tickPerFrame = 1000 / MAX_FRAME_RATE;

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) done = 1;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		DWORD now = GetTickCount();

		// dt: the time between (beginning of last frame) and now
		// this frame: the frame we are about to render
		DWORD dt = now - frameStart;

		if (dt >= tickPerFrame)
		{
			frameStart = now;

			game->ProcessKeyboard();
			
			Update(dt);
			Render();
		}
		else
			Sleep(tickPerFrame - dt);
	}

	return 1;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd = CreateGameWindow(hInstance, nCmdShow, SCREEN_WIDTH, SCREEN_HEIGHT);

	game = CGame::GetInstance();
	game->Init(hWnd);

	keyHandler = new CSampleKeyHander();
	game->InitKeyboard(keyHandler);


	LoadResources();

	SetWindowPos(hWnd, 0, 0, 0, SCREEN_WIDTH*2, SCREEN_HEIGHT*2, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	Run();

	return 0;
}