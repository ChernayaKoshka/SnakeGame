#pragma once

#include <Windows.h>
#include "game.h"

#define MaxQueueSize (BUFFER_WIDTH*BUFFER_HEIGHT)/FOOD_WIDTH

typedef struct tagQueue
{
	POINT stackArray[MaxQueueSize];
	int front;
	int count;
}Queue_t;

Queue_t queue;

BOOL push(POINT p);
POINT pop();

int CalculateIndex(int count);