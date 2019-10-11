#pragma once
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#define KEYBOARD_BUFFER_SIZE 1024

class CKeyEventHandler
{
public:
	virtual void KeyState(BYTE *state) = 0;
	virtual void OnKeyDown(int KeyCode) = 0;
	virtual void OnKeyUp(int KeyCode) = 0;
};

typedef CKeyEventHandler * LPKEYEVENTHANDLER;

class CMouseEventHandler
{
	LONG currentXpos;
	LONG currentYpos;
	LONG currentZpos;
public:
	virtual void KeyState(BYTE* state) = 0;
	virtual void OnKeyDown(int KeyCode) = 0;
	virtual void OnKeyUp(int KeyCode) = 0;
	LONG GetX() { return currentXpos; }
	LONG GetY() { return currentYpos; }
	LONG GetZ() { return currentZpos; }
	void SetCurrentPos(LONG x, LONG y, LONG z) { this->currentXpos = x; this->currentYpos = y; this->currentZpos = z; }
};

typedef CMouseEventHandler* LPMOUSEEVENTHANDLER;

class CGame
{
	static CGame * __instance;
	HWND hWnd;

	LPDIRECT3D9 d3d = NULL;
	LPDIRECT3DDEVICE9 d3ddv = NULL;

	LPDIRECT3DSURFACE9 backBuffer = NULL;
	LPD3DXSPRITE spriteHandler = NULL;

	LPDIRECTINPUT8       di;
	LPDIRECTINPUTDEVICE8 keyboard;
	LPDIRECTINPUTDEVICE8 mouse;

	BYTE  keyStates[256];
	DIMOUSESTATE mouse_state;

	DIDEVICEOBJECTDATA keyEvents[KEYBOARD_BUFFER_SIZE];

	LPKEYEVENTHANDLER keyHandler;
	LPMOUSEEVENTHANDLER mouseHandler;

public:
	void InitInput(LPKEYEVENTHANDLER keyHandler, LPMOUSEEVENTHANDLER mouseHandler);
	void Init(HWND hWnd);
	void Draw(float x, float y, LPDIRECT3DTEXTURE9 texture, int left, int top, int right, int bottom);

	int IsKeyDown(int KeyCode);
	bool IsMouseDown(int KeyCode);
	void ProcessKeyboard();
	void ProcessMouse();

	LPDIRECT3DDEVICE9 GetDirect3DDevice() { return this->d3ddv; }
	LPDIRECT3DSURFACE9 GetBackBuffer() { return backBuffer; }
	LPD3DXSPRITE GetSpriteHandler() { return this->spriteHandler; }

	static CGame * GetInstance();

	~CGame();
};


