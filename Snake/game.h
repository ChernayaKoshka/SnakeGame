#pragma once

#define FOOD_WIDTH 10
#define FOOD_HEIGHT 10

#define BUFFER_WIDTH 320
#define BUFFER_HEIGHT 300

#define TIMESTEP 0.1f

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