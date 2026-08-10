#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "stdio.h"

/* imprecise small delay */
__STATIC_INLINE void DelayUs(volatile uint32_t us)
{
	us *= (SystemCoreClock / 1000000);
	while (us--);
}

//uint16_t dX = 0;
//uint16_t dY = 0;
//uint16_t pX = 0;
//uint16_t pY = 0;
//uint16_t pHeight = 0;
//uint16_t pWidth = 0;

/* For tracking previous string dimension */
static uint16_t prevStrX = 0xFFFF;  /* 0xFFFF = "no previous string yet" */
static uint16_t prevStrY = 0;
static uint16_t prevStrW = 0;
static uint16_t prevStrH = 0;

void ILI9341_DrawHollowCircle(uint16_t X, uint16_t Y, uint16_t radius, uint16_t color)
{
	int x = radius-1;
	int y = 0;
	int dx = 1;
	int dy = 1;
	int err = dx - (radius << 1);

	while (x >= y)
	{
		ILI9341_DrawPixel(X + x, Y + y, color);
		ILI9341_DrawPixel(X + y, Y + x, color);
		ILI9341_DrawPixel(X - y, Y + x, color);
		ILI9341_DrawPixel(X - x, Y + y, color);
		ILI9341_DrawPixel(X - x, Y - y, color);
		ILI9341_DrawPixel(X - y, Y - x, color);
		ILI9341_DrawPixel(X + y, Y - x, color);
		ILI9341_DrawPixel(X + x, Y - y, color);

		if (err <= 0)
		{
			y++;
			err += dy;
			dy += 2;
		}

		if (err > 0)
		{
			x--;
			dx += 2;
			err += (-radius << 1) + dx;
		}
	}
}

void ILI9341_DrawFilledCircle(uint16_t X, uint16_t Y, uint16_t radius, uint16_t color)
{

	int x = radius;
	int y = 0;
	int xChange = 1 - (radius << 1);
	int yChange = 0;
	int radiusError = 0;

	while (x >= y)
	{
		for (int i = X - x; i <= X + x; i++)
		{
			ILI9341_DrawPixel(i, Y + y,color);
			ILI9341_DrawPixel(i, Y - y,color);
		}

		for (int i = X - y; i <= X + y; i++)
		{
			ILI9341_DrawPixel(i, Y + x,color);
			ILI9341_DrawPixel(i, Y - x,color);
		}

		y++;
		radiusError += yChange;
		yChange += 2;

		if (((radiusError << 1) + xChange) > 0)
		{
			x--;
			radiusError += xChange;
			xChange += 2;
		}
	}
}

void ILI9341_DrawHollowRectangleCoord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t color)
{
	uint16_t xLen = 0;
	uint16_t yLen = 0;
	uint8_t negX = 0;
	uint8_t negY = 0;
	float negCalc = 0;

	negCalc = X1 - X0;
	if(negCalc < 0) negX = 1;
	negCalc = 0;

	negCalc = Y1 - Y0;
	if(negCalc < 0) negY = 1;

	//DRAW HORIZONTAL!
	if(!negX)
	{
		xLen = X1 - X0;
	}
	else
	{
		xLen = X0 - X1;
	}
	ILI9341_DrawHLine(X0, Y0, xLen, color);
	ILI9341_DrawHLine(X0, Y1, xLen, color);

	//DRAW VERTICAL!
	if(!negY)
	{
		yLen = Y1 - Y0;
	}
	else
	{
		yLen = Y0 - Y1;
	}

	ILI9341_DrawVLine(X0, Y0, yLen, color);
	ILI9341_DrawVLine(X1, Y0, yLen, color);

	if((xLen > 0)||(yLen > 0))
	{
		ILI9341_DrawPixel(X1, Y1, color);
	}
}

void ILI9341_DrawFilledRectangleCoord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t color)
{
	uint16_t xLen = 0;
	uint16_t yLen = 0;
	uint8_t negX = 0;
	uint8_t negY = 0;
	int32_t negCalc = 0;
	uint16_t X0True = 0;
	uint16_t Y0True = 0;

	negCalc = X1 - X0;
	if(negCalc < 0) negX = 1;
	negCalc = 0;

	negCalc = Y1 - Y0;
	if(negCalc < 0) negY = 1;

	if(!negX)
	{
		xLen = X1 - X0;
		X0True = X0;
	}
	else
	{
		xLen = X0 - X1;
		X0True = X1;
	}

	if(!negY)
	{
		yLen = Y1 - Y0;
		Y0True = Y0;
	}
	else
	{
		yLen = Y0 - Y1;
		Y0True = Y1;
	}

	ILI9341_DrawRectangle(X0True, Y0True, xLen, yLen, color);
}

void ILI9341_DrawChar(char ch, const uint16_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor)
{

	if ((ch < 31) || (ch > 127)) return;

	uint16_t fOffset, fWidth, fHeight, fBPL;
	uint16_t *tempChar;

	fOffset = font[0];
	fWidth  = font[1];
	fHeight = font[2];
	fBPL    = font[3];

	tempChar = (uint16_t*)&font[((ch - 0x20) * fOffset) + 4];

	/* Clear background first */
//	if(pWidth > fWidth || pHeight > fHeight)
//	{
//
//	ILI9341_DrawRectangle(pX, pY, pWidth, pHeight, bgcolor);
//	}
//
//	else

	//ILI9341_DrawRectangle(X, Y, fWidth, fHeight, bgcolor);

	for (int j = 0; j < fHeight; j++)
	{
		for (int i = 0; i < fWidth; i++)
		{
			uint16_t z = tempChar[fBPL * i + ((j & 0xFFF0) >> 4) + 1];
			uint16_t b = 1 << (j & 0x0F);
			if ((z & b) != 0x00)
			{
				ILI9341_DrawPixel(X + i, Y + j, color);
			}
		}
	}

//	 pX = X;
//     pY = Y;
//     pHeight = fHeight;
//     pWidth = fWidth;
}
void ILI9341_DrawText(const char* str, const uint16_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor)
{
	uint16_t charWidth;			/* Width of character */
	uint16_t fOffset = font[0];	/* Offset of character */
	uint16_t fWidth = font[1];	/* Width of font */
	uint16_t fHeight = font[2];
	uint16_t startX  = X;
	uint16_t totalWidth = 0;
    const char *s;

    s=str;

    //measuring the string length
	while (*s)
	{
		 uint16_t *tempChar = (uint16_t*)&font[((*s - 0x20) * fOffset) + 4];
		        uint16_t charWidth = tempChar[0];

		        if (charWidth + 2 < fWidth)
		            totalWidth += (charWidth + 2);
		        else
		            totalWidth += fWidth;

		        s++;
	}
// if both previous str and current str has same starting and end point
	if(prevStrX != 0xFFFF  && X == prevStrX && Y == prevStrY && prevStrW > totalWidth)
	{
		ILI9341_DrawRectangle(X, Y, prevStrW, prevStrH, bgcolor);
	}

	//if prev string has smaller starting point than current one
	else if(prevStrX != 0XFFFF && prevStrX < X && Y == prevStrY && prevStrW > totalWidth)
	{
		ILI9341_DrawRectangle(prevStrX, Y, prevStrW, prevStrH, bgcolor);
	}

	//if prev string has smaller starting point and y is smaller too
	else if(prevStrX != 0XFFFF && prevStrX < X && Y < prevStrY && prevStrW > totalWidth)
	{
		ILI9341_DrawRectangle(prevStrX, prevStrY, prevStrW, prevStrH, bgcolor);
	}
	//if only y is smaller than prevY
	else if(prevStrX != 0XFFFF && prevStrX == X && Y < prevStrY && prevStrW > totalWidth)
	{
		ILI9341_DrawRectangle(X, prevStrY, prevStrW, prevStrH, bgcolor);
	}

	//if nothing has changed
	else
	{
		ILI9341_DrawRectangle(X, Y, totalWidth, fHeight, bgcolor);
	}

	//draw the string
    while (*str)
    {
        ILI9341_DrawChar(*str, font, X, Y, color, bgcolor);

        uint16_t *tempChar = (uint16_t*)&font[((*str - 0x20) * fOffset) + 4];
        uint16_t charWidth = tempChar[0];

        if (charWidth + 2 < fWidth)
            X += (charWidth + 2);
        else
            X += fWidth;

        str++;
    }
    prevStrX = startX;
       prevStrY = Y;
       prevStrW = totalWidth;
       prevStrH = fHeight;
}

void ILI9341_DrawImage(const uint8_t* image, uint8_t orientation)
{
    uint16_t w = ILI9341_SCREEN_WIDTH;
    uint16_t h = ILI9341_SCREEN_HEIGHT;

    if(orientation == SCREEN_HORIZONTAL_1 || orientation == SCREEN_HORIZONTAL_2)
    {
        ILI9341_SetRotation(orientation);
        ILI9341_SetAddress(0, 0, w - 1, h - 1);  // Inclusive coordinates!
    }
    else
    {
        ILI9341_SetRotation(orientation);
        ILI9341_SetAddress(0, 0, h - 1, w - 1);  // Swap w/h for vertical
    }

    uint32_t total_bytes = (uint32_t)w * h * 2;  // RGB565 = 2 bytes/pixel
    uint32_t counter = 0;

    while(counter < total_bytes)
    {
        uint32_t chunk = total_bytes - counter;
        if(chunk > BURST_MAX_SIZE)
            chunk = BURST_MAX_SIZE;

        // Ensure we never split a 16-bit pixel across bursts!
        if((chunk % 2) != 0)
            chunk--;  // Make it even

        ILI9341_WriteBuffer((uint8_t*)(image + counter), chunk);
        counter += chunk;

        DelayUs(1);
    }
}
