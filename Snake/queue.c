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
	#if DEBUG
		TestCalculateIndex2(count);
	#endif
	if (queue.front - count + 1 < 0)
		return MaxQueueSize + queue.front - count + 1;
	else
		return queue.front - count + 1;
}

char* fmt0 = "v\r[0][1][2][3][4]\r\n";
char* fmt1 = "    v\r[0][1][2][3][4]\r\n";
char* fmt2 = "       v\r[0][1][2][3][4]\r\n";
char* fmt3 = "          v\r[0][1][2][3][4]\r\n";
char* fmt4 = "             v\r[0][1][2][3][4]\r\n";
static FILE* fp;
void OpenLog()
{
	fopen_s(&fp, "log.txt", "w");
}

void TestCalculateIndex2(int count)
{
	char* fmt = "Front: %d\r\nCount: %d\r\nIndex: %d\r\n";

	fprintf_s(fp, "0-----------------------0\r\n");
		int num = 0;
		if (queue.front - count +1 < 0)
			num =  MaxQueueSize + queue.front - count + 1;
		else
			num =  queue.front - count + 1;

		fprintf_s(fp, "1-----------------------1\r\n");
		fprintf_s(fp, fmt, queue.front, count, num);
		switch (num)
		{
		case 0:
			fprintf_s(fp, fmt0);
			break;
		case 1:
			fprintf_s(fp, fmt1);
			break;
		case 2:
			fprintf_s(fp, fmt2);
			break;
		case 3:
			fprintf_s(fp, fmt3);
			break;
		case 4:
			fprintf_s(fp, fmt4);
			break;
		}
		fprintf_s(fp, "1-----------------------1\r\n");
	
	fprintf_s(fp, "0-----------------------0\r\n");
}