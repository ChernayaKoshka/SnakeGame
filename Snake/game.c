#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "math_custom.h"
#include "drawing.h"
#include "game.h"
#include "queue.h"

#define M_PI 3.141592653589793238462643383279502884197169399375105820974

BOOL Running = TRUE;

static Snake_t player = { 0 };

POINT food = { 0 };

int* BackBuffer;

BITMAPINFO BitMapInfo = { 0 };

HDC dcWindow = 0;

BOOL lockDirection = FALSE;

static double GTimePassed = 0;
static float SecondsPerTick = 0;
static __int64 GTimeCount = 0;

#if PSYCHADELIC
double snakeAngle = 0.0;
double backgroundAngle = M_PI;
double foodAngle = M_PI+0.5;
#else
int snakeColor = 0;
int backgroundColor = 0x00FFFFFF;
int foodColor = 0;
#endif

#if FOOD_ADDED_TO_BODY && PSYCHADELIC == 0
int bodyColors[MaxQueueSize] = { 0 };
#endif

#if FOOD_ADDED_TO_BODY && PSYCHADELIC
double bodyAngles[MaxQueueSize] = { 0.0 };
#endif

float InitFloatTime()
{
	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);
	SecondsPerTick = 1.0f / (float)Frequency.QuadPart;

	LARGE_INTEGER Counter;
	QueryPerformanceCounter(&Counter);
	GTimeCount = Counter.QuadPart;
	return SecondsPerTick;
}

float FloatTime()
{
	LARGE_INTEGER Counter;
	QueryPerformanceCounter(&Counter);

	__int64 Interval = Counter.QuadPart - GTimeCount;

	GTimeCount = Counter.QuadPart;
	double SecondsGoneBy = (double)Interval*SecondsPerTick;
	GTimePassed += SecondsGoneBy;

	return (float)GTimePassed;
}
/*
int GetColor(double shift, double* angle)
{
	*angle += 0.01;
	if (*angle > 2 * M_PI)
	{
		*angle = -(2 * M_PI);
	}

	return RGB(sin(*angle + 0 + shift) * 127 + 128, sin(*angle + 2 + shift) * 127 + 128, sin(*angle + 4 + shift) * 127 + 128);
}
*/

int GetColor(double* angle)
{
	*angle += 0.05;
	if (*angle > 2 * M_PI)
	{
		*angle = -(2 * M_PI);
	}

	return RGB(sin(*angle + 0) * 127 + 128, sin(*angle + 2) * 127 + 128, sin(*angle + 4) * 127 + 128);
}

BOOL BodyContainsPoint(POINT p)
{
	for (int i = 0; i < queue.count; i++)
	{
		int index = CalculateIndex(i + 1);
		if (p.x == queue.stackArray[index].x &&
			p.y == queue.stackArray[index].y)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CheckCollission()
{
	//border check

	//I have no idea why, but the width needs to be at least 120 to keep this check from freaking out......
	if (player.pos.x < 0 || player.pos.x > BUFFER_WIDTH - FOOD_WIDTH)
		return FALSE;

	if (player.pos.y < 0 || player.pos.y > BUFFER_HEIGHT - FOOD_WIDTH)
		return FALSE;

	BOOL skipNextCollission = FALSE;
	//food check
	if (player.pos.x == food.x && player.pos.y == food.y)
	{
#if FOOD_ADDED_TO_BODY && PSYCHADELIC == 0
		bodyColors[player.length + 1] = foodColor;
#endif

#if FOOD_ADDED_TO_BODY && PSYCHADELIC
		bodyAngles[player.length + 1] = foodAngle;
#endif

#if PSYCHADELIC == 0
		snakeColor = RGB(rand() % 255, rand() % 255, rand() % 255);
		foodColor = RGB(rand() % 255, rand() % 255, rand() % 255);
		backgroundColor = RGB(rand() % 255, rand() % 255, rand() % 255);
#endif
		push(player.pos);

		do
		{
			food.x = RandomInt(0, BUFFER_WIDTH - FOOD_WIDTH, FOOD_WIDTH);
			food.y = RandomInt(0, BUFFER_HEIGHT - FOOD_HEIGHT, FOOD_HEIGHT);
		} while (BodyContainsPoint(food)); //prevents spawn inside body

										   //just ate, no need for collission
		skipNextCollission = TRUE;
		player.length++;
	}

	//check for collision

	if (!skipNextCollission)
	{
		for (int i = 1; i < queue.count + 1; i++)
		{
			if (queue.count == 1) break;
			int index = CalculateIndex(i) - 1;
			if (index == queue.front) continue;
			if (player.pos.x == queue.stackArray[index].x &&
				player.pos.y == queue.stackArray[index].y)
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

int CalculateScreen(float timestep)
{
	wchar_t buf[20];
	swprintf_s(buf, 20, L"Score: %d", player.length);
	int len = lstrlenW((LPCWSTR)&buf);
	int xPos = 0;
	if (food.x > 0 && food.x < 50 && food.y == 0)
		xPos = 50;

	if (player.pos.x > 0 && player.pos.x < 50 && player.pos.y == 0)
		xPos = 50;
	TextOutW(dcWindow, xPos, 0, (LPCWSTR)&buf, len);

	if (timestep < TIMESTEP)
		return TRUE;


	memset(BackBuffer, 0, BUFFER_WIDTH * BUFFER_HEIGHT * 4); //4 = size of integer

#if PSYCHADELIC
	DrawRect(0, 0, BUFFER_WIDTH, BUFFER_HEIGHT, GetColor(&backgroundAngle), BackBuffer, BUFFER_WIDTH, BUFFER_HEIGHT);
#else
	DrawRect(0, 0, BUFFER_WIDTH, BUFFER_HEIGHT, backgroundColor, BackBuffer, BUFFER_WIDTH, BUFFER_HEIGHT);
#endif

	//move player
	switch (player.direction)
	{
	case Down:
		player.pos.y += FOOD_HEIGHT;
		break;
	case Up:
		player.pos.y -= FOOD_HEIGHT;
		break;
	case Left:
		player.pos.x -= FOOD_WIDTH;
		break;
	case Right:
		player.pos.x += FOOD_WIDTH;
		break;
	}
	lockDirection = FALSE;
	//push new head
	push(player.pos);
	//draw

	for (int i = 0; i < queue.count; i++)
	{
		int index = CalculateIndex(i + 1);

#if FOOD_ADDED_TO_BODY && PSYCHADELIC == 0
		DrawRect(queue.stackArray[index].x, queue.stackArray[index].y, FOOD_WIDTH, FOOD_HEIGHT, bodyColors[i], BackBuffer, BUFFER_WIDTH, BUFFER_HEIGHT);
#endif

#if PSYCHADELIC && FOOD_ADDED_TO_BODY == 0
		DrawRect(queue.stackArray[index].x, queue.stackArray[index].y, FOOD_WIDTH, FOOD_HEIGHT, GetColor(&snakeAngle), BackBuffer, BUFFER_WIDTH, BUFFER_HEIGHT);
#endif

#if PSYCHADELIC && FOOD_ADDED_TO_BODY
		double toPass = bodyAngles[i];
		DrawRect(queue.stackArray[index].x, queue.stackArray[index].y, FOOD_WIDTH, FOOD_HEIGHT, GetColor(&toPass), BackBuffer, BUFFER_WIDTH, BUFFER_HEIGHT);
		bodyAngles[i] = toPass;
#endif

#if PSYCHADELIC == 0 && FOOD_ADDED_TO_BODY == 0
		DrawRect(queue.stackArray[index].x, queue.stackArray[index].y, FOOD_WIDTH, FOOD_HEIGHT, snakeColor, BackBuffer, BUFFER_WIDTH, BUFFER_HEIGHT);
#endif
	}

	BOOL lost = CheckCollission();

	//pop back square of body
	pop();
	//draw food :)
#if PSYCHADELIC
	DrawRect(food.x, food.y, FOOD_WIDTH, FOOD_HEIGHT, GetColor(&foodAngle), BackBuffer, BUFFER_WIDTH, BUFFER_HEIGHT);
#else
	DrawRect(food.x, food.y, FOOD_WIDTH, FOOD_HEIGHT, foodColor, BackBuffer, BUFFER_WIDTH, BUFFER_HEIGHT);
#endif
	//display image
	StretchDIBits(dcWindow,
		0, 0, BUFFER_WIDTH, BUFFER_HEIGHT,
		0, 0, BUFFER_WIDTH, BUFFER_HEIGHT,
		BackBuffer, &BitMapInfo,
		DIB_RGB_COLORS, SRCCOPY);

	return lost;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT Result = 0;

	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (lockDirection && wParam != VK_ESCAPE) return Result;
		switch (wParam)
		{
		case VK_LEFT: case 0x41:
			if (player.direction != Right)
			{
				lockDirection = TRUE;
				player.direction = Left;
			}
			break;
		case VK_RIGHT: case 0x44:
			if (player.direction != Left)
			{
				lockDirection = TRUE;
				player.direction = Right;
			}
			break;
		case VK_UP: case 0x57:
			if (player.direction != Down)
			{
				lockDirection = TRUE;
				player.direction = Up;
			}
			break;
		case VK_DOWN: case 0x53:
			if (player.direction != Up)
			{
				lockDirection = TRUE;
				player.direction = Down;
			}
			break;
		case VK_ESCAPE:
			Running = FALSE;
			return Result;
		}
		return Result;
	case WM_ERASEBKGND:
		return TRUE;
	case WM_CLOSE:
	case WM_QUIT:
		Running = FALSE;
		return Result;
		break;
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

	BackBuffer = (int*)malloc(width*height * 4);

	return TRUE;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#if DEBUG
	OpenLog();
#endif

	if (!CreateGameWindow(hInstance, nShowCmd, BUFFER_WIDTH, BUFFER_HEIGHT))
		return EXIT_FAILURE;

	food.x = RandomInt(0, BUFFER_WIDTH - FOOD_WIDTH, FOOD_WIDTH);
	food.y = RandomInt(0, BUFFER_HEIGHT - FOOD_HEIGHT, FOOD_HEIGHT);

	InitFloatTime();

	float PrevTime = InitFloatTime();
	srand((unsigned int)time(NULL));

#if PSYCHADELIC == 0
	backgroundColor = RGB(rand() % 255, rand() % 255, rand() % 255);
	foodColor = RGB(rand() % 255, rand() % 255, rand() % 255);
#endif

	MSG msg;

	while (Running)
	{
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		float NewTime = FloatTime();

		Running = CalculateScreen(NewTime - PrevTime);

		//put check inside CalculateScreen() later... v
		if (NewTime - PrevTime > TIMESTEP) PrevTime = NewTime;
	}

	wchar_t buf[20];
	swprintf_s(buf, 20, L"Score: %d", player.length);
	int len = lstrlenW((LPCWSTR)&buf);
	Running = TRUE;
	while (Running)
	{
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		TextOutW(dcWindow, 0, 0, (LPCWSTR)&buf, len);
	}

	return EXIT_SUCCESS;
}