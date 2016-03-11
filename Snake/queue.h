#pragma once

#include <Windows.h>
#include "game.h"

#define MaxQueueSize (BufferWidth*BufferHeight)/10

//#define MaxQueueSize 5

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
void TestCalculateIndex2(int count);
void OpenLog();