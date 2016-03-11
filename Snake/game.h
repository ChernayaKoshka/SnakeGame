#pragma once

#define FoodWidth 10
#define FoodHeight 10

#define BufferWidth 640
#define BufferHeight 480

typedef struct tagSnake
{
	POINT pos;
	int direction;
	int length;
}Snake_t;

typedef enum tageDirection
{
	Down = 0,
	Up = 1,
	Left = 2,
	Right = 3
}eDirection;