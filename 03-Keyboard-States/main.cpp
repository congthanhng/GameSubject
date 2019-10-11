#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "debug.h"
#include "Game.h"
#include "GameObject.h"
#include "Textures.h"

#define WINDOW_CLASS_NAME L"GameWindow"
#define MAIN_WINDOW_TITLE L"BTT2"

#define BACKGROUND_COLOR D3DCOLOR_XRGB(0, 0, 0)
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480

#define MAX_FRAME_RATE 60

CGame *game;
CSprite* sLeft;
CSprite* sRight;
CSprite* sBall;

int run = 0;
float v = 0.5;
bool up = false;
bool down = false;
LONG x = 0;
LONG y = 0;

class CSampleKeyHander : public CKeyEventHandler
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
		run = 1;
		break;
	}
}

void CSampleKeyHander::OnKeyUp(int KeyCode)
{
	DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);
}

void CSampleKeyHander::KeyState(BYTE* states)
{
	if (game->IsKeyDown(DIK_UP))
	{
		up = true;
	}
	else
	{
		up = false;
	}
	if (game->IsKeyDown(DIK_DOWN))
	{
		down = true;
	}
	else
	{
		down = false;
	}
}

class CSampleMouseHander : public CMouseEventHandler
{
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
};

CSampleMouseHander* mouseHandler;

void CSampleMouseHander::OnKeyDown(int KeyCode)
{
	DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	switch (KeyCode)
	{
	case 0:
		run = 1;
		break;
	}
}

void CSampleMouseHander::OnKeyUp(int KeyCode)
{
	DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);
}

void CSampleMouseHander::KeyState(BYTE* states)
{

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

float BallX;
float BallY;
float LeftX;
float LeftY;
float RightX;
float RightY;
float vx;
float vy;

void LoadResources()
{
	CTextures* textures = CTextures::GetInstance();

	textures->Add(1, L"Bong.png", D3DCOLOR_XRGB(0, 0, 0));
	LPDIRECT3DTEXTURE9 texPong = textures->Get(1);

	sLeft = new CSprite(0, 14, 14, 30, 105, texPong);
	sRight = new CSprite(0, 14, 14, 30, 105, texPong);
	sBall = new CSprite(2, 64, 64, 88, 88, texPong);

	BallX = 395;
	BallY = 190;
	LeftX = 0;
	LeftY = 150;
	RightX = 770;
	RightY = 150;
	vx = 0.2;
	vy = -0.2;
}

/*
	Update world status for this frame
	dt: time period between beginning of last frame and beginning of this frame
*/

void Update(DWORD dt)
{
	BallX += vx * dt * run;
	BallY += vy * dt * run;
	if (BallX + 24 > 782)
	{
		run = 0;
	}

	if (BallX < 0)
	{
		run = 0;
	}

	if (BallY + 24 > 438)
	{
		vy = -vy;
	}
	if (BallY < 0)
	{
		vy = -vy;
	}
	if ((BallX < sLeft->GetX() + 18) && (BallY > sLeft->GetY()) && (BallY < sLeft->GetY() + 95))
	{
		vx = -vx;
	}
	if ((BallX + 28 > sRight->GetX()) && (BallY > sRight->GetY()) && (BallY < sRight->GetY() + 95))
	{
		vx = -vx;
	}
	if (up && LeftY > 0)
	{
		LeftY -= v * dt;
	}
	if (down && LeftY < 440 - 91)
	{
		LeftY += v * dt;
	}

	RightY += y;
	if (RightY < 0)
	{
		RightY = 0;
	}
	if (RightY > 440 - 91)
	{
		RightY = 440 - 91;
	}
}

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

		sLeft->Draw(LeftX, LeftY);
		sRight->Draw(RightX, RightY);
		sBall->Draw(BallX, BallY);

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
			WS_OVERLAPPEDWINDOW,
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

		DWORD dt = now - frameStart;

		if (dt >= tickPerFrame)
		{
			frameStart = now;

			game->ProcessKeyboard();
			game->ProcessMouse();
			y = mouseHandler->GetY();
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
	mouseHandler = new CSampleMouseHander();
	game->InitInput(keyHandler, mouseHandler);


	LoadResources();
	Run();

	return 0;
}