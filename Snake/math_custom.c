#include <Windows.h>
#include <math.h>

int Greatest(int num, int num2)
{
	if (num > num2) return num;
	return num2;
}

int Least(int num, int num2)
{
	if (num < num2) return num;
	return num2;
}

int Difference(int num, int num2)
{
	return Greatest(num, num2) - Least(num, num2);
}

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