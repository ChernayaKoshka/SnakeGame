#include "queue.h"
#include <stdio.h>

Queue_t queue = { 0 };

BOOL push(POINT p)
{
	if (queue.count == MaxQueueSize)
		return FALSE;
	queue.stackArray[queue.front] = p;
	queue.count++;
	return TRUE;
}

POINT pop()
{
	POINT toReturn = { -1 };
	toReturn = queue.stackArray[queue.front];
	queue.front++;
	if (queue.front > MaxQueueSize - 1)
		queue.front = 0;
	queue.count--;
	return toReturn;
}

int CalculateIndex(int count)
{
	if (queue.front - count + 1 < 0)
		return MaxQueueSize + queue.front - count + 1;
	else
		return queue.front - count + 1;
}