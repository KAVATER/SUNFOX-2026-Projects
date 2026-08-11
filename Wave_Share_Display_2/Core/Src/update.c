void ILI9341_DrawChar(char ch, const uint16_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor);
void ILI9341_DrawText(const char* str, const uint16_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor);

void ILI9341_DrawChar(char ch, const uint16_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor)
{
	if ((ch < 31) || (ch > 127)) return;

	uint16_t fOffset, fWidth, fHeight, fBPL;
	uint16_t *tempChar;

	fOffset = font[0];
	fWidth = font[1];
	fHeight = font[2];
	fBPL = font[3];

	tempChar = (uint16_t*)&font[((ch - 0x20) * fOffset) + 4]; /* Current Character = Meta + (Character Index * Offset) */

	/* Clear background first */
	ILI9341_DrawRectangle(X, Y, fWidth, fHeight, bgcolor);

	for (int j = 0; j < fHeight; j++)
	{
		for (int i = 0; i < fWidth; i++)
		{
			uint16_t z = tempChar[fBPL * i + ((j & 0xFFF0) >> 4) + 1]; /* j / 16 selects the word */
			uint16_t b = 1 << (j & 0x0F);                              /* j % 16 selects the bit */
			if ((z & b) != 0x00)
			{
				ILI9341_DrawPixel(X + i, Y + j, color);
			}
		}
	}
}

void ILI9341_DrawText(const char* str, const uint16_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor)
{
	uint16_t charWidth;			/* Width of character */
	uint16_t fOffset = font[0];	/* Offset of character */
	uint16_t fWidth = font[1];	/* Width of font */

	while (*str)
	{
		ILI9341_DrawChar(*str, font, X, Y, color, bgcolor);

		/* Check character width and calculate proper position */
		uint16_t *tempChar = (uint16_t*)&font[((*str - 0x20) * fOffset) + 4];
		charWidth = tempChar[0];

		if (charWidth + 2 < fWidth)
		{
			X += (charWidth + 2);
		}
		else
		{
			X += fWidth;
		}

		str++;
	}
}
void ILI9341_DrawText(const char* str, const uint16_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor)
{
	uint16_t charWidth;
	uint16_t fOffset = font[0];
	uint16_t fWidth  = font[1];

	while (*str)
	{
		ILI9341_DrawChar(*str, font, X, Y, color, bgcolor);

		uint16_t *tempChar = (uint16_t*)&font[((*str - 0x20) * fOffset) + 4];
		charWidth = tempChar[0];

		if (charWidth + 2 < fWidth)
			X += (charWidth + 2);
		else
			X += fWidth;

		str++;
	}
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
//--------------------------------------------------------------//

/* =====================================================================
   String-tracking state for smart redraw (single text field)
   ===================================================================== */
#define MAX_TRACKED_STR_LEN  24

static struct {
    uint16_t x, y;              /* Position of the text field */
    uint16_t width;             /* Total pixel width of last drawn string */
    uint16_t height;            /* Font height */
    char     text[MAX_TRACKED_STR_LEN];      /* Previous string content */
    uint16_t charWidths[MAX_TRACKED_STR_LEN]; /* Previous per-char widths */
    uint8_t  len;               /* Length of previous string */
    uint8_t  valid;             /* 0 = no previous draw yet */
} prevStringState = {0};

/* =====================================================================
   Helper: width of one character in pixels (matches your spacing logic)
   ===================================================================== */
static uint16_t ILI9341_GetCharWidth(char ch, const uint16_t font[])
{
    uint16_t fOffset = font[0];
    uint16_t fWidth  = font[1];

    if (ch < 32 || ch > 127) return fWidth;

    uint16_t *tempChar = (uint16_t*)&font[((ch - 0x20) * fOffset) + 4];
    uint16_t charWidth = tempChar[0];

    return (charWidth + 2 < fWidth) ? (charWidth + 2) : fWidth;
}

/* =====================================================================
   DrawChar - draws ONLY the glyph pixels. No clearing, no globals.
   ===================================================================== */
void ILI9341_DrawChar(char ch, const uint16_t font[], uint16_t X, uint16_t Y,
                      uint16_t color, uint16_t bgcolor)
{
    (void)bgcolor;  /* Background clearing is handled by DrawText */

    if ((ch < 31) || (ch > 127)) return;

    uint16_t fOffset = font[0];
    uint16_t fWidth  = font[1];
    uint16_t fHeight = font[2];
    uint16_t fBPL    = font[3];
    uint16_t *tempChar;

    tempChar = (uint16_t*)&font[((ch - 0x20) * fOffset) + 4];

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
}

/* =====================================================================
   DrawText - smart redraw: only changed digits are cleared + redrawn
   ===================================================================== */
void ILI9341_DrawText(const char* str, const uint16_t font[], uint16_t X, uint16_t Y,
                      uint16_t color, uint16_t bgcolor)
{
    uint16_t fOffset = font[0];
    uint16_t fWidth  = font[1];
    uint16_t fHeight = font[2];

    uint16_t currentX = X;
    uint16_t newLen   = 0;
    uint16_t newWidth = 0;
    const char *s;

    /* ---- Measure new string ---- */
    s = str;
    while (*s && newLen < MAX_TRACKED_STR_LEN - 1)
    {
        newWidth += ILI9341_GetCharWidth(*s, font);
        newLen++;
        s++;
    }

    /* ---- Are we updating the same text field? ---- */
    uint8_t sameField = (prevStringState.valid &&
                         prevStringState.x == X &&
                         prevStringState.y == Y &&
                         prevStringState.height == fHeight);

    if (sameField)
    {
        /* New string is shorter: clear the trailing old digits */
        if (newLen < prevStringState.len)
        {
            if (prevStringState.width > newWidth)
            {
                ILI9341_DrawRectangle(X + newWidth, Y,
                                      prevStringState.width - newWidth,
                                      fHeight, bgcolor);
            }
        }
    }
    else
    {
        /* Moved to a new position or first draw: erase old text if any */
        if (prevStringState.valid)
        {
            ILI9341_DrawRectangle(prevStringState.x, prevStringState.y,
                                  prevStringState.width,
                                  prevStringState.height, bgcolor);
        }
        /* Clear the new location before drawing */
        ILI9341_DrawRectangle(X, Y, newWidth, fHeight, bgcolor);
    }

    /* ---- Draw only changed characters ---- */
    s = str;
    uint16_t i = 0;
    while (*s && i < MAX_TRACKED_STR_LEN - 1)
    {
        uint16_t charW = ILI9341_GetCharWidth(*s, font);
        uint8_t  needsRedraw = 1;

        /* If same field and this position existed before, compare */
        if (sameField && i < prevStringState.len)
        {
            if (prevStringState.text[i] == *s)
            {
                needsRedraw = 0;
            }
        }

        if (needsRedraw)
        {
            /* Clear the MAX of old vs new width so no stray pixels remain
               (important for proportional fonts) */
            uint16_t clearW = charW;
            if (sameField && i < prevStringState.len)
            {
                if (prevStringState.charWidths[i] > clearW)
                    clearW = prevStringState.charWidths[i];
            }

            ILI9341_DrawRectangle(currentX, Y, clearW, fHeight, bgcolor);
            ILI9341_DrawChar(*s, font, currentX, Y, color, bgcolor);
        }

        /* Save this character into tracking buffer */
        prevStringState.text[i]       = *s;
        prevStringState.charWidths[i] = charW;

        currentX += charW;
        s++;
        i++;
    }

    /* Null-terminate tracked string */
    prevStringState.text[i] = '\0';

    /* Save state for next call */
    prevStringState.x      = X;
    prevStringState.y      = Y;
    prevStringState.height = fHeight;
    prevStringState.width  = newWidth;
    prevStringState.len    = i;
    prevStringState.valid  = 1;
}
//////////////////////////////////////////////////////////////////////////////////////////////
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
//static uint16_t prevStrX = 0xFFFF;  /* 0xFFFF = "no previous string yet" */
//static uint16_t prevStrY = 0;
//static uint16_t prevStrW = 0;
//static uint16_t prevStrH = 0;
char prevCh;
uint8_t newStr;
uint8_t flag =0;
#define MAX_TRACKED_STR_LEN 24

static struct {
	uint16_t x,y;
	uint16_t width;
	uint16_t height;
	char text[MAX_TRACKED_STR_LEN];
	uint16_t charWidths[MAX_TRACKED_STR_LEN];
	uint8_t len;
	uint8_t valid;
}prevStringState = {0};

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
//width of one character in pixels
static uint16_t ILI9341_GetCharWidth(char ch , const uint16_t font[])
{
    uint16_t fOffset = font[0];
    uint16_t fWidth  = font[1];

  if(ch < 32 || ch > 127)return fWidth;
  uint16_t *tempChar = (uint16_t*)&font[((ch - 0x20) * fOffset) + 4];
  uint16_t charWidth = tempChar[0];
  return (charWidth + 2 < fWidth)? (charWidth + 2) : fWidth;
}
//uint8_t firstCh = 1;
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

//if(prevCh == ch && newStr == 1)
//	{flag =1;
//	return ;
//	}

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


//if(newStr == 1)
//{
//	prevCh =  ch;
//}

//	 pX = X;
//     pY = Y;
//     pHeight = fHeight;
//     pWidth = fWidth;


}

//redrawing only changed characters are cleared and drawn
void ILI9341_DrawText(const char* str, const uint16_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor)
{
	//uint16_t charWidth;			/* Width of character */
	uint16_t fOffset = font[0];	/* Offset of character */
	uint16_t fWidth = font[1];	/* Width of font */
	uint16_t fHeight = font[2];
	//uint16_t startX  = X;
	//uint16_t totalWidth = 0;
    const char *s;
    uint16_t currentX = X;
    uint16_t newLen = 0;
    uint16_t newWidth = 0;

    s=str;
    uint16_t i = 0;

    //measuring the string length
//	while (*s)
//	{
//		 uint16_t *tempChar = (uint16_t*)&font[((*s - 0x20) * fOffset) + 4];
//		        uint16_t charWidth = tempChar[0];
//
//		        if (charWidth + 2 < fWidth)
//		            totalWidth += (charWidth + 2);
//		        else
//		            totalWidth += fWidth;
//
//		        s++;
//	}
while(*s && newLen < MAX_TRACKED_STR_LEN -1)
{
	newWidth += ILI9341_GetCharWidth(*s,font);
	newLen++;
	s++;
}

//checking if we are updating same text or not
uint8_t sameField = (prevStringState.valid &&
		prevStringState.x == X &&
		prevStringState.y == Y &&
		prevStringState.height == fHeight);

if(sameField)
{
	if(newLen < prevStringState.len)
	{
		if(prevStringState.width > newWidth)
		{
			ILI9341_DrawRectangle(X + newWidth, Y, prevStringState.width - newWidth,  fHeight, bgcolor);
		}
	}

}
else
{
	if(prevStringState.valid)
	{
		ILI9341_DrawRectangle(prevStringState.x, prevStringState.y,
		                                  prevStringState.width,
		                                  prevStringState.height, bgcolor);
	}
    ILI9341_DrawRectangle(X, Y, newWidth, fHeight, bgcolor);

}

//	if( flag == 0  )
//	{
    // if both previous str and current str has same starting and end point
//	if(prevStrX != 0xFFFF  && X == prevStrX && Y == prevStrY && prevStrW > totalWidth)
//	{
//		ILI9341_DrawRectangle(X, Y, prevStrW, prevStrH, bgcolor);
//	}
//
//	//if prev string has smaller starting point than current one
//	else if(prevStrX != 0XFFFF && prevStrX < X && Y == prevStrY && prevStrW > totalWidth)
//	{
//		ILI9341_DrawRectangle(prevStrX, Y, prevStrW, prevStrH, bgcolor);
//	}
//
//	//if prev string has smaller starting point and y is smaller too
//	else if(prevStrX != 0XFFFF && prevStrX < X && Y < prevStrY && prevStrW > totalWidth)
//	{
//		ILI9341_DrawRectangle(prevStrX, prevStrY, prevStrW, prevStrH, bgcolor);
//	}
//	//if only y is smaller than prevY
//	else if(prevStrX != 0XFFFF && prevStrX == X && Y < prevStrY && prevStrW > totalWidth)
//	{
//		ILI9341_DrawRectangle(X, prevStrY, prevStrW, prevStrH, bgcolor);
//	}
//
//	//if nothing has changed
//	else
//	{
//		ILI9341_DrawRectangle(X, Y, totalWidth, fHeight, bgcolor);
//	}
//}
//	else
//	flag = 0;

	//draw the string
    while (*str && i<MAX_TRACKED_STR_LEN -1)
    {
        uint16_t charW = ILI9341_GetCharWidth(*s, font);
        uint8_t needsRedraw = 1;
//        ILI9341_DrawChar(*str, font, X, Y, color, bgcolor);
//
//        uint16_t *tempChar = (uint16_t*)&font[((*str - 0x20) * fOffset) + 4];
//        uint16_t charWidth = tempChar[0];
//
//        if (charWidth + 2 < fWidth)
//            X += (charWidth + 2);
//        else
//            X += fWidth;
//
//        str++;
//        newStr  = 0;
        /* If same field and this position existed before, compare */
             if (sameField && i < prevStringState.len)
             {
                 if (prevStringState.text[i] == *s)
                 {
                     needsRedraw = 0;
                 }
             }

             if (needsRedraw)
             {
                 /* Clear the MAX of old vs new width so no stray pixels remain
                    (important for proportional fonts) */
                 uint16_t clearW = charW;
                 if (sameField && i < prevStringState.len)
                 {
                     if (prevStringState.charWidths[i] > clearW)
                         clearW = prevStringState.charWidths[i];
                 }

                 ILI9341_DrawRectangle(currentX, Y, clearW, fHeight, bgcolor);
                 ILI9341_DrawChar(*s, font, currentX, Y, color, bgcolor);
             }

             /* Save this character into tracking buffer */
             prevStringState.text[i]       = *s;
             prevStringState.charWidths[i] = charW;

             currentX += charW;
             s++;
             i++;
    }
/* Null-terminate tracked string */
  prevStringState.text[i] = '\0';

  /* Save state for next call */
  prevStringState.x      = X;
  prevStringState.y      = Y;
  prevStringState.height = fHeight;
  prevStringState.width  = newWidth;
  prevStringState.len    = i;
  prevStringState.valid  = 1;
//    prevStrX = startX;
//       prevStrY = Y;
//       prevStrW = totalWidth;
//       prevStrH = fHeight;
//       newStr  = 1;
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
