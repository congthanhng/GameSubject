#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#define WINDOW_CLASS_NAME "MainWin"
#define MAIN_WINDOW_TITLE "GameSubject"
#define BACKGROUND_COLOR D3DCOLOR_XRGB(255, 0, 0)
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_X GetSystemMetrics(SM_CXSCREEN)	//lấy kích thước màn hình chiều X
#define SCREEN_Y GetSystemMetrics(SM_CYSCREEN)	//lấy kích thước màn hình chiều Y

#define MAX_FRAME_RATE 20

//function prototype
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);				//Passes message information to the specified window procedure
HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int ScreenWidth, int ScreenHeight);	//init screen
int Run();																						//loop game

//Application entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd = CreateGameWindow(hInstance, nCmdShow, SCREEN_WIDTH, SCREEN_HEIGHT);
	Run();

	return 0;
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

//init screen
HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int ScreenWidth, int ScreenHeight)
{
	WNDCLASSEX wc;												//Contains window class information. sử dụng để đăng kí màn hình bằng function RegisterClassEx
	wc.cbSize = sizeof(WNDCLASSEX);								// size of structure 

	wc.style = CS_HREDRAW | CS_VREDRAW;							//resize kích thước màn hình theo chiều ngang(Horizontal) và dọc(vertical)
	wc.hInstance = hInstance;									// handle to instance 

	wc.lpfnWndProc = (WNDPROC)WinProc;							// points to window procedure 
	wc.cbClsExtra = 0;											// no extra class memory 
	wc.cbWndExtra = 0;											// no extra class memory 
	wc.hIcon = NULL;											// predefined app. icon 
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);					// predefined arrow 
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);		// white background brush 
	wc.lpszMenuName = NULL;										// name of menu resource 
	wc.lpszClassName = WINDOW_CLASS_NAME;						// name of window class 
	wc.hIconSm = NULL;											// small class icon 

	RegisterClassEx(&wc);										//đăng kí cửa sổ với các thông tin trên

	HWND hWnd =
		CreateWindow(
			WINDOW_CLASS_NAME,
			MAIN_WINDOW_TITLE,
			WS_OVERLAPPEDWINDOW, // WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP,
			(SCREEN_X - SCREEN_WIDTH) / 2,	//tọa độ x bắt đầu vẽ màn hình
			(SCREEN_Y - SCREEN_HEIGHT) / 2,	//tọa độ y bắt đầu vẽ màn hình
			SCREEN_WIDTH,					//độ ngang của màn hình
			SCREEN_HEIGHT,					//độ dọc của màn hình	
			NULL,							// no owner window 
			NULL,							// use class menu 
			hInstance,						// handle to application instance 
			NULL);							// no window-creation data 

	if (!hWnd)
	{
		OutputDebugString("[ERROR] CreateWindow failed");
		DWORD ErrCode = GetLastError();
		return FALSE;
	}

	// Show the window and send a WM_PAINT message to the window 
	// procedure. 
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}

//game loop
int Run()
{

	MSG msg;
	int done = 0;
	DWORD frameStart = GetTickCount();
	DWORD tickPerFrame = 1000 / MAX_FRAME_RATE;

	while (!done)
	{
		//OutputDebugString("[WARNING] you in a loop infinity");

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

		}
		else
			Sleep(tickPerFrame - dt);

	}

	return 1;
}