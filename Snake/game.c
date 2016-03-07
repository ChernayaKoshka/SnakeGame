#include <Windows.h>
#include <stdio.h>
#include "math_custom.h"
#include "drawing.h"


#define FoodWidth 10
#define FoodHeight 10

BOOL Running = TRUE;

typedef struct tagSnake 
{
	POINT pos;
	int direction;
	int length;
}Snake_t;

POINT food = { 0 };

typedef enum eDirection
{
	Down = 0,
	Up = 1,
	Left = 2,
	Right = 3
};

static Snake_t player = { 0 };

int BufferWidth = 640;;
int BufferHeight = 480;

int* BackBuffer;

BITMAPINFO BitMapInfo = { 0 };

HDC dcWindow = 0;

static double GTimePassed = 0;
static float SecondsPerTick = 0;
static __int64 GTimeCount = 0;

float Sys_InitFloatTime()
{
	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);
	SecondsPerTick = 1.0f / (float)Frequency.QuadPart;

	LARGE_INTEGER Counter;
	QueryPerformanceCounter(&Counter);
	GTimeCount = Counter.QuadPart;
	return SecondsPerTick;
}

float Sys_FloatTime()
{
	LARGE_INTEGER Counter;
	QueryPerformanceCounter(&Counter);

	__int64 Interval = Counter.QuadPart - GTimeCount;

	GTimeCount = Counter.QuadPart;
	double SecondsGoneBy = (double)Interval*SecondsPerTick;
	GTimePassed += SecondsGoneBy;

	return (float)GTimePassed;
}

int CalculateScreen(float timestep)
{
	memset(BackBuffer, 0xFF, BufferWidth * BufferHeight * 4); //4 = size of integer

	switch (player.direction)
	{
	case Down:
		player.pos.y += FoodHeight;
		break;
	case Up:
		player.pos.y -= FoodHeight;
		break;
	case Left:
		player.pos.x -= FoodWidth;
		break;
	case Right:
		player.pos.x += FoodWidth;
		break;
	}

	if (player.pos.x < 0 || player.pos.x > BufferWidth-FoodWidth)
	{
		Running = FALSE;
		return 0;
	}

	if (player.pos.y < 0 || player.pos.y>BufferHeight-FoodHeight)
	{
		Running = FALSE;
		return 0;
	}

	if (player.pos.x == food.x && player.pos.y==food.y)
	{
		player.length++;
		food.x = RandomInt10(0, BufferWidth);
		food.y = RandomInt10(0, BufferHeight);
	}

	//draw player
	DrawRect(player.pos.x, player.pos.y, FoodWidth, FoodHeight, 0x000F707F, BackBuffer, BufferWidth, BufferHeight);

	//draw food :)
	DrawRect(food.x, food.y, FoodWidth, FoodHeight, 0, BackBuffer, BufferWidth, BufferHeight);

	//display image
	StretchDIBits(dcWindow,
		0, 0, BufferWidth, BufferHeight,
		0, 0, BufferWidth, BufferHeight,
		BackBuffer, &BitMapInfo,
		DIB_RGB_COLORS, SRCCOPY);


	//debug
	wchar_t buf[100];
	swprintf_s(buf, 20, L"Score: %d", player.length);
	int len = lstrlenW(&buf);
	TextOutW(dcWindow, 0, 0, &buf, len);

	/*swprintf_s(buf, 20, L"Food X: %d", food.x);
	len = lstrlenW(&buf);
	TextOutW(dcWindow, 0, 20, &buf, len);

	swprintf_s(buf, 20, L"Food Y: %d", food.y);
	len = lstrlenW(&buf);
	TextOutW(dcWindow, 0, 40, &buf, len);*/
	//end debug

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, WPARAM lParam)
{
	LRESULT Result = 0;

	switch (uMsg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:
			player.direction = Left;
			break;
		case VK_RIGHT:
			player.direction = Right;
			break;
		case VK_UP:
			player.direction = Up;
			break;
		case VK_DOWN:
			player.direction = Down;
			break;
		case VK_ESCAPE:
			Running = FALSE;
			return Result;
		}
		return Result;
	case WM_ERASEBKGND:
		return TRUE;
	case WM_CLOSE:
		Running = FALSE;
		return Result;
	case WM_COMMAND:
		return Result;
	case WM_PAINT:
	default:
		Result = DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return Result;
}

BOOL CreateGameWindow(HINSTANCE hInstance, int nShowCmd, int width, int height)
{
	RECT r = { 0 };
	r.right = width;
	r.bottom = height;
	AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW | WS_VISIBLE, FALSE);

	//define window
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	wc.lpszClassName = L"snakegame";

	if (!RegisterClassEx(&wc))
		return FALSE;

	HWND hwndWindow = CreateWindowExW(
		0,
		L"snakegame",
		L"Snake Game",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		r.right - r.left,
		r.bottom - r.top,
		NULL,
		NULL,
		hInstance,
		0);

	if (!hwndWindow) 
		return FALSE;

	BitMapInfo.bmiHeader.biSize = sizeof(BitMapInfo.bmiHeader);
	BitMapInfo.bmiHeader.biWidth = width;
	BitMapInfo.bmiHeader.biHeight = -height;
	BitMapInfo.bmiHeader.biPlanes = 1;
	BitMapInfo.bmiHeader.biBitCount = 32;
	BitMapInfo.bmiHeader.biCompression = BI_RGB;

	dcWindow = GetDC(hwndWindow);

	BackBuffer = (int*)malloc(width*height*4);

	return TRUE;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

	if (!CreateGameWindow(hInstance, nShowCmd, BufferWidth, BufferHeight))
		return EXIT_FAILURE;

	Sys_InitFloatTime();

	float PrevTime = Sys_InitFloatTime();
	float TimeAccumulated = 0;

	MSG msg;

	while (Running)
	{
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		float NewTime = Sys_FloatTime();
		if (NewTime-PrevTime > 0.15) 
		{
			CalculateScreen(NewTime - PrevTime);
			PrevTime = NewTime;
		}
	}

	return EXIT_SUCCESS;
}

