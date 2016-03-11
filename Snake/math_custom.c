#include <Windows.h>
#include <math.h>

int Abs(int val)
{
	if (val < 0) return -val;
	return val;
}

int GetNumBetween(int min, int max)
{
	return (rand() % max) + min;
}

int RandomInt10(int min, int max)
{
	return (GetNumBetween(min, max)/10) * 10;
}