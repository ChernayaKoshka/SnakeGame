#pragma once

#define FOOD_ADDED_TO_BODY 1
#define PSYCHADELIC 1

#define FOOD_WIDTH 20
#define FOOD_HEIGHT 20

#define BUFFER_WIDTH 720
#define BUFFER_HEIGHT 720

#define TIMESTEP 0.05f

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