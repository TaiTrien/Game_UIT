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
#include "debug.h"
#include "Game.h"
#include "GameObject.h"
#include "Textures.h"
#include "PlayScene.h"
#include "Simon.h"
#include "Whip.h"
#include "FireHolding.h"
#include "Brick.h"
#include "Goomba.h"

#define WINDOW_CLASS_NAME L"SampleWindow"
#define MAIN_WINDOW_TITLE L"04 - Collision"

#define BACKGROUND_COLOR D3DCOLOR_XRGB(0, 0, 0)
#define SCREEN_WIDTH 300
#define SCREEN_HEIGHT 220

#define MAX_FRAME_RATE 120

#define ID_TEX_MARIO 0
#define ID_TEX_ENEMY 10
#define ID_TEX_MISC 20
#define ID_TEX_SIMON 30
#define ID_TEX_WHIP 40
#define ID_TEX_MAP 50
#define ID_TEX_FIRE 60
#define ID_TEX_MAP_ITEMS 70
#define ID_TEX_ITEMS 80

CGame *game;
Whip *whip;
Simon *simon;
PlayScene *playScene;
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
	// disable control key when Mario die 
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

/*
	Load all game resources 
	In this example: load textures, sprites, animations and simon object

	TO-DO: Improve this function by loading texture,sprite,animation,object from file
*/
void LoadResources()
{
	CTextures * textures = CTextures::GetInstance();

	textures->Add(ID_TEX_MARIO, L"textures\\simon.png",D3DCOLOR_XRGB(255, 255, 255));
	textures->Add(ID_TEX_BBOX, L"textures\\bbox.png", D3DCOLOR_XRGB(255, 255, 255));
	textures->Add(ID_TEX_SIMON, L"textures\\simon.png", D3DCOLOR_XRGB(0, 128, 128));
	textures->Add(ID_TEX_WHIP, L"textures\\whip.png", D3DCOLOR_XRGB(0, 128, 128));
	textures->Add(ID_TEX_MAP, L"textures\\Map_entrance.png", D3DCOLOR_XRGB(0, 128, 128));
	textures->Add(ID_TEX_FIRE, L"textures\\Fireholding.png", D3DCOLOR_XRGB(34, 177, 76));
	textures->Add(ID_TEX_MAP_ITEMS, L"textures\\MapItems.png", D3DCOLOR_XRGB(34, 177, 76));
	textures->Add(ID_TEX_ITEMS, L"textures\\Items.png", D3DCOLOR_XRGB(255, 0, 255));

	CSprites * sprites = CSprites::GetInstance();
	CAnimations * animations = CAnimations::GetInstance();
	
	
	LPDIRECT3DTEXTURE9 texSimon = textures->Get(ID_TEX_SIMON);
	LPDIRECT3DTEXTURE9 texWhip = textures->Get(ID_TEX_WHIP);
	LPDIRECT3DTEXTURE9 texMap = textures->Get(ID_TEX_MAP);
	LPDIRECT3DTEXTURE9 texFire = textures->Get(ID_TEX_FIRE);
	LPDIRECT3DTEXTURE9 texMapItems = textures->Get(ID_TEX_MAP_ITEMS);
	LPDIRECT3DTEXTURE9 texItems = textures->Get(ID_TEX_ITEMS);

	ifstream infile("text\\Sprites.txt"); // load all sprite
	int arr[6];
	while (infile)
	{
		infile >> arr[0] >> arr[1] >> arr[2] >> arr[3] >> arr[4] >> arr[5];
		if (arr[5] == 0)
			sprites->Add(arr[0], arr[1], arr[2], arr[3], arr[4], texSimon);
		else if (arr[5] == 1)
			sprites->Add(arr[0], arr[1], arr[2], arr[3], arr[4], texWhip);
		else if (arr[5] == 2)
			sprites->Add(arr[0], arr[1], arr[2], arr[3], arr[4], texMap);
		else if (arr[5] == 3)
			sprites->Add(arr[0], arr[1], arr[2], arr[3], arr[4], texFire);
		else if (arr[5] == 4)
			sprites->Add(arr[0], arr[1], arr[2], arr[3], arr[4], texMapItems);
		else if (arr[5] == 5)
			sprites->Add(arr[0], arr[1], arr[2], arr[3], arr[4], texItems);
	}
	
	ifstream infileAni("text\\Animations.txt"); // load all animations except map
	LPANIMATION ani;
	int arr1[5];
	while (infileAni)
	{
		infileAni >> arr1[0] >> arr1[1] >> arr1[2] >> arr1[3] >> arr1[4];
		ani = new CAnimation(100);
		ani->Add(arr1[1]);
		for (int i = 2; i <= 4; i++) {
			if (arr1[i] != 0) {
				ani->Add(arr1[i]);
			}
		}
		animations->Add(arr1[0], ani);
	}

	for (int i = 0; i <= 23; i++) {  // for map
		ani = new CAnimation(100);
		ani->Add(i);
		animations->Add(i, ani);
	}


	ifstream infile2("text\\Entrance.txt"); // load entrance
	int mArray;
	for (int d = 0; d < 5; d++) {
		for (int c = 0; c < 24; c++) {
			infile2 >> mArray;
			PlayScene *entrance = new PlayScene();
			if (mArray!= 0)
			{
				entrance->AddAnimation(mArray);
				entrance->SetPosition(0 + c * 32.0f,  d * 32.0f);
				objects.push_back(entrance);
			}
		}
	}
	infile2.close();

	

	simon = new Simon();
	for (int i = 400; i <= 413; i++) {
		simon->AddAnimation(i);
	}
	
	simon->SetPosition(50.0f, 0);
	objects.push_back(simon);

	simon->whip = new Whip();
	simon->whip->CloneSimon(simon);
	for (int i = 800; i <= 805; i++) {
		simon->whip->AddAnimation(i);
	}
	simon->whip->AddAnimation(602);
	objects.push_back(simon->whip);
		
	

	

	for (int i = 1; i <= 5; i++) { // to create 5 fire holding
		fire = new FireHolding();
		fire->AddAnimation(900);
		fire->AddAnimation(1000); // for heart ani
		fire->AddAnimation(1001); // for upgrade item whip
		fire->AddAnimation(1002); // for knife
		fire->setIndexOfFireHolding(i);
		fire->SetPosition(120*i, 130);
		objects.push_back(fire);
	}
	

	for (int i = 0; i < 60; i++)
	{
		CBrick *brick = new CBrick();
		brick->AddAnimation(601);
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

		for (int i = 0; i < objects.size(); i++)
			objects[i]->Render();


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