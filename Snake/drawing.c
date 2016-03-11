#include "math_custom.h"
#include "drawing.h"

void Plot(int x, int y, int color, int* buffer, int bufferWidth, int bufferHeight)
{
	if (x > bufferWidth) x = x - bufferWidth;
	if (x < 0) return;

	if (y > bufferHeight) y = y - bufferHeight;
	if (y < 0) return;

	if (((y*bufferWidth) + x)>(bufferWidth*bufferHeight)) return;

	buffer[(y*bufferWidth) + x] = color;
}

void DrawRect(int x, int y, int width, int height, int color, int* buffer, int bufferWidth, int bufferHeight)
{
	for (int i = 0; i < height; i++)
	{
		for (int i2 = 0; i2 < width; i2++)
		{
			Plot(x + i2, y + i, color, buffer, bufferWidth, bufferHeight);
		}
	}
}