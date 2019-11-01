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
		simon->SetState(SIMON_STATE_ATTACK);
		whip->SetState(WHIP_STATE_ATTACK);
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

	CSprites * sprites = CSprites::GetInstance();
	CAnimations * animations = CAnimations::GetInstance();
	
	
	LPDIRECT3DTEXTURE9 texSimon = textures->Get(ID_TEX_SIMON);
	LPDIRECT3DTEXTURE9 texWhip = textures->Get(ID_TEX_WHIP);
	LPDIRECT3DTEXTURE9 texMap = textures->Get(ID_TEX_MAP);
	LPDIRECT3DTEXTURE9 texFire = textures->Get(ID_TEX_FIRE);
	LPDIRECT3DTEXTURE9 texMapItems = textures->Get(ID_TEX_MAP_ITEMS);

	ifstream infile("text\\Sprites.txt");
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
	}
	


	LPDIRECT3DTEXTURE9 texMisc = textures->Get(ID_TEX_MISC);
	sprites->Add(20001, 408, 225, 424, 241, texMisc);

	LPDIRECT3DTEXTURE9 texEnemy = textures->Get(ID_TEX_ENEMY);
	sprites->Add(30001, 5, 14, 21, 29, texEnemy);
	sprites->Add(30002, 25, 14, 41, 29, texEnemy);

	sprites->Add(30003, 45, 21, 61, 29, texEnemy); // die sprite

	LPANIMATION ani;

	ani = new CAnimation(100);	// idle big right
	ani->Add(10001);
	animations->Add(400, ani);

	ani = new CAnimation(100);	// idle big left
	ani->Add(10004);
	animations->Add(401, ani);

	ani = new CAnimation(100);	// idle small right
	ani->Add(10021);
	animations->Add(402, ani);

	ani = new CAnimation(100);	// idle small left
	ani->Add(10031);
	animations->Add(403, ani);

	ani = new CAnimation(100);	// walk right big
	ani->Add(10001);
	ani->Add(10002);
	ani->Add(10003);
	animations->Add(500, ani);

	ani = new CAnimation(100);	// // walk left big
	ani->Add(10004);
	ani->Add(10005);
	ani->Add(10006);
	animations->Add(501, ani);

	ani = new CAnimation(100);	// walk right small
	ani->Add(10021);
	ani->Add(10022);
	ani->Add(10023);
	animations->Add(502, ani);

	ani = new CAnimation(100);	// walk left small
	ani->Add(10031);
	ani->Add(10032);
	ani->Add(10033);
	animations->Add(503, ani);

	ani = new CAnimation(100); // attack right 
	ani->Add(10010);
	ani->Add(10011);
	ani->Add(10012);
	ani->Add(10001);
	animations->Add(504, ani);

	ani = new CAnimation(100); // attack left 
	ani->Add(10013);
	ani->Add(10014);
	ani->Add(10015);
	ani->Add(10004);
	animations->Add(505, ani);

	ani = new CAnimation(100);
	ani->Add(1030);
	animations->Add(506, ani); // jumping right

	ani = new CAnimation(100); // jumping left 
	ani->Add(1031);
	animations->Add(507, ani);

	ani = new CAnimation(100); // sit attack right 
	ani->Add(1041);
	ani->Add(1042);
	ani->Add(1043);
	ani->Add(1030);
	animations->Add(508, ani);

	ani = new CAnimation(100); // sit attack left 
	ani->Add(1044);
	ani->Add(1045);
	ani->Add(1046);
	ani->Add(1031);
	animations->Add(509, ani);

	ani = new CAnimation(100); // whip type 1 attack right 
	ani->Add(2001);
	ani->Add(2002);
	ani->Add(2003);
	ani->Add(2060); // empty sprite
	animations->Add(800, ani);

	ani = new CAnimation(100); // whip type 1 attack left 
	ani->Add(2004);
	ani->Add(2005);
	ani->Add(2006);
	ani->Add(2060);
	animations->Add(801, ani);

	ani = new CAnimation(100);
	ani->Add(2060);
	animations->Add(602, ani);

	ani = new CAnimation(100);		// Mario die
	ani->Add(10099);
	animations->Add(599, ani);

	ani = new CAnimation(100);		// brick
	ani->Add(200);
	animations->Add(601, ani);

	ani = new CAnimation(300);		// Goomba walk
	ani->Add(30001);
	ani->Add(30002);
	animations->Add(701, ani);

	ani = new CAnimation(1000);		// Goomba dead
	ani->Add(30003);
	animations->Add(702, ani);

	for (int i = 0; i <= 23; i++) {  // for map
		ani = new CAnimation(100);
		ani->Add(i);
		animations->Add(i, ani);
	}
	ani = new CAnimation(100);
	for (int i = 0; i <= 1; i++) {  // for fire holding
		ani->Add(i + 100);
		animations->Add(700, ani);
	}


	ifstream infile2("text\\Entrance.txt");
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
	simon->AddAnimation(400);		// idle right big
	simon->AddAnimation(401);		// idle left big
	simon->AddAnimation(402);		// idle right small
	simon->AddAnimation(403);		// idle left small

	simon->AddAnimation(500);		// walk right big
	simon->AddAnimation(501);		// walk left big
	simon->AddAnimation(502);		// walk right small
	simon->AddAnimation(503);		// walk left big
	simon->AddAnimation(504);		// attack right
	simon->AddAnimation(505);		// attack left
	simon->AddAnimation(506);		// jump right
	simon->AddAnimation(507);		// jump left
	simon->AddAnimation(508);		// sit attack right
	simon->AddAnimation(509);		// sit attack left
	simon->SetPosition(50.0f, 0);
	objects.push_back(simon);

	whip = new Whip();
	whip->CloneSimon(simon);
	whip->AddAnimation(800);
	whip->AddAnimation(801);
	whip->AddAnimation(602);
	objects.push_back(whip);

	for (int i = 1; i <= 3; i++) {
		fire = new FireHolding();
		fire->AddAnimation(700);
		fire->SetPosition(150*i + 16.0f, 130);
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
	//whip->CloneSimon(simon);
	vector<LPGAMEOBJECT> coObjects;
	for (int i = 1; i < objects.size(); i++)
	{
		coObjects.push_back(objects[i]);
	}

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(dt,&coObjects);
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