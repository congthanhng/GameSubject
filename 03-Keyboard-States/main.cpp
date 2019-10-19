#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "debug.h"
#include "Game.h"
#include "GameObject.h"
#include "Textures.h"
#include <time.h>

#define WINDOW_CLASS_NAME L"GameWindow"
#define MAIN_WINDOW_TITLE L"02"

#define BACKGROUND_COLOR D3DCOLOR_XRGB(0, 0, 0)
#define SURFACE_COLOR D3DCOLOR_XRGB(255, 255, 255)
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480
#define MAX_X 785
#define MAX_Y 440
#define SURFACE_HEIGHT 0


#define MAX_FRAME_RATE 60

CGame *game;
CSprite* sLeft;
CSprite* sRight;
CSprite* sBall;


LPDIRECT3DSURFACE9 surface = NULL;

int run = 0;
bool isEnd = false;
float v = 0.5;
bool up = false;
bool down = false;
LONG x = 0;
LONG y = 0;

float BallX;
float BallY;
float LeftX;
float LeftY;
float RightX;
float RightY;
float vx;
float vy;

void StartGame()
{
	if (run == 0 && isEnd == false)
	{
		run = 1;
		BallY = SURFACE_HEIGHT + 200 + rand() % (MAX_Y - 400 - SURFACE_HEIGHT + 1) - 12;
		BallX = MAX_X / 2;
	}
}

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
		StartGame();
		break;
	case DIK_ESCAPE:
		PostQuitMessage(0);
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
		StartGame();
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

void LoadResources()
{
	CTextures* textures = CTextures::GetInstance();

	textures->Add(1, L"Pong.png", D3DCOLOR_XRGB(0, 0, 0));

	LPDIRECT3DTEXTURE9 texPong = textures->Get(1);

	sLeft = new CSprite(0, 14, 14, 30, 105, texPong);
	sRight = new CSprite(0, 14, 14, 30, 105, texPong);
	sBall = new CSprite(2, 64, 64, 88, 88, texPong);

	BallX = MAX_X / 2;
	BallY = MAX_Y / 2;
	LeftX = 0;
	LeftY = SURFACE_HEIGHT;
	RightX = MAX_X - 17;
	RightY = SURFACE_HEIGHT;
	srand(time(NULL));
	if (rand() % 2 == 0)
	{
		vx = 0.2;
	}
	else
	{
		vx = -0.2;
	}
	if (rand() % 2 == 0)
	{
		vy = 0.2;
	}
	else
	{
		vy = -0.2;
	}
}


void SweptAABB(
	float ml, float mt, float mr, float mb,
	float dx, float dy,
	float sl, float st, float sr, float sb,
	float& t, float& nx, float& ny)
{
	float dx_entry, dx_exit, tx_entry, tx_exit;
	float dy_entry, dy_exit, ty_entry, ty_exit;

	float t_entry;
	float t_exit;

	t = -1;
	nx = ny = 0;

	if (dx == 0 && dy == 0) return;

	if (dx > 0)
	{
		dx_entry = sl - mr;
		dx_exit = sr - ml;
	}
	else if (dx < 0) {
		dx_entry = sr - ml;
		dx_exit = sl - mr;
	}

	if (dy > 0)
	{
		dy_entry = st - mb;
		dy_exit = sb - mt;
	}
	else if (dy < 0)
	{
		dy_entry = sb - mt;
		dy_exit = st - mb;
	}

	if (dx == 0)
	{
		tx_entry = -99999999999;
		tx_exit = 99999999999;
	}
	else
	{
		tx_entry = dx_entry / dx;
		tx_exit = dx_exit / dx;
	}

	if (dy == 0)
	{
		ty_entry = -99999999999;
		ty_exit = 99999999999;
	}
	else
	{
		ty_entry = dy_entry / dy;
		ty_exit = dy_exit / dy;
	}

	if ((tx_entry < 0 && ty_entry < 0) || tx_entry > 1 || ty_entry > 1) return;

	t_entry = max(tx_entry, ty_entry);
	t_exit = max(tx_exit, ty_exit);

	if (t_entry > t_exit) return;

	t = t_entry;

	if (tx_entry > ty_entry)
	{
		ny = 0;
		dx > 0 ? nx = -1 : nx = 1;
	}
	else
	{
		nx = 0;
		dy > 0 ? ny = -1 : ny = 1;
	}
}

struct Box
{
	float l, t, r, b;
};

void CreateBox(Box& box, float x, float y, float width, float height, float dx, float dy)
{
	if (dx > 0)
	{
		box.l = x;
		box.r = x + dx + width;
	}
	else
	{
		box.l = x + dx;
		box.r = x + width;
	}
	if (dy > 0)
	{
		box.t = y;
		box.b = y + dy + height;
	}
	else
	{
		box.t = y + dy;
		box.b = y + height;
	}
}

void Update(DWORD dt)
{
	if (up && LeftY > SURFACE_HEIGHT)
	{
		LeftY -= v * dt;
		if (LeftY < SURFACE_HEIGHT)
		{
			LeftY = SURFACE_HEIGHT;
		}
	}
	if (down && LeftY < MAX_Y - 91)
	{
		LeftY += v * dt;
		if (LeftY > MAX_Y - 91)
		{
			LeftY = MAX_Y - 91;
		}
	}

	RightY += y;
	if (RightY < SURFACE_HEIGHT)
	{
		RightY = SURFACE_HEIGHT;
	}
	if (RightY > MAX_Y - 91)
	{
		RightY = MAX_Y - 91;
	}

	float time = dt, nx = 1, ny = 1;

	if (run != 0)
	{
		float dx = vx * dt;
		float dy = vy * dt;

		Box ballBox;
		CreateBox(ballBox, BallX, BallY, 24, 24, dx, dy);

		if (ballBox.t < SURFACE_HEIGHT) // va cham bên trên
		{
			float t;
			SweptAABB(
				BallX, BallY, BallX + 24, BallY + 24,
				dx, dy,
				0, SURFACE_HEIGHT, MAX_X, SURFACE_HEIGHT,
				t, nx, ny
			);
			vy = -vy;
			if (t < time)
			{
				time = t;
			}
		}
		if (ballBox.b > MAX_Y) //chạm bên dưới
		{
			float t;
			SweptAABB(
				BallX, BallY, BallX + 24, BallY + 24,
				dx, dy,
				0, MAX_Y, MAX_X, MAX_Y,
				t, nx, ny
			);
			vy = -vy;
			if (t < time)
			{
				time = t;
			}
		}
		if (ballBox.l < 0) // va chạm trái
		{
			float t;
			SweptAABB(
				BallX, BallY, BallX + 24, BallY + 24,
				dx, dy,
				0, SURFACE_HEIGHT, 0, MAX_X,
				t, nx, ny
			);
			vx = 0.2;
			run = 0;
		
		}
		if (ballBox.r > MAX_X) // va chạm phải
		{
			float t;
			SweptAABB(
				BallX, BallY, BallX + 24, BallY + 24,
				dx, dy,
				0, SURFACE_HEIGHT, 0, MAX_X,
				t, nx, ny
			);
			vx = -0.2;
			run = 0;
			
		}
		if (ballBox.l < 16 && ballBox.t >= LeftY && ballBox.b <= LeftY + 91) // Left
		{
			float t;
			SweptAABB(
				BallX, BallY, BallX + 24, BallY + 24,
				dx, dy,
				0, LeftY, 16, LeftY + 91,
				t, nx, ny
			);
			vx = -vx;
			if (t < time)
			{
				time = t;
			}
		}
		if (ballBox.r > MAX_X - 16 && ballBox.t >= RightY && ballBox.b <= RightY + 91) // Right
		{
			float t;
			SweptAABB(
				BallX, BallY, BallX + 24, BallY + 24,
				dx, dy,
				MAX_X - 16, RightY, MAX_X, RightY + 91,
				t, nx, ny
			);
			vx = -vx;
			if (t < time)
			{
				time = t;
			}
		}
	}


	BallX += vx * time * run;
	BallY += vy * time * run;

}


void Render()
{
	LPDIRECT3DDEVICE9 d3ddv = game->GetDirect3DDevice();
	LPDIRECT3DSURFACE9 bb = game->GetBackBuffer();
	LPD3DXSPRITE spriteHandler = game->GetSpriteHandler();

	if (d3ddv->BeginScene())
	{
		
		d3ddv->ColorFill(bb, NULL, BACKGROUND_COLOR);
		

		spriteHandler->Begin(D3DXSPRITE_ALPHABLEND);

		sLeft->Draw(LeftX, LeftY);
		sRight->Draw(RightX, RightY);
		sBall->Draw(BallX, BallY);
		
		spriteHandler->End();
		d3ddv->EndScene();
	}

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
	if (surface != NULL)
	{
		surface->Release();
	}
	return 0;
}