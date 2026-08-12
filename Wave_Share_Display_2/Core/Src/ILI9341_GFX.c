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

}
void ILI9341_DrawText(const char* str, const uint16_t font[], uint16_t X, uint16_t Y,
                      uint16_t color, uint16_t bgcolor)
{
    // ─────────────────────────────────────────────────────────────
    // EXTRACT FONT METADATA from font lookup table
    // ─────────────────────────────────────────────────────────────
    uint16_t fOffset = font[0];   // ASCII offset (e.g., 32 for space)
    uint16_t fWidth  = font[1];   // Default/max character width (fixed-width fallback)
    uint16_t fHeight = font[2];   // Font height in pixels (all chars same height)

    const char *s;                // String pointer for iteration
    uint16_t currentX = X;        // Running X position while drawing chars
    uint16_t newLen   = 0;        // Character count of new string
    uint16_t newWidth = 0; // Total pixel width of new string (static persists across calls)
    uint16_t i = 0;               // Index for char arrays

    // ─────────────────────────────────────────────────────────────
    // PASS 1: Measure the new string (width + length)
    // We need total pixel width BEFORE drawing to handle background clearing
    // ─────────────────────────────────────────────────────────────
    s = str;
    while(*s && newLen < MAX_TRACKED_STR_LEN - 1)
    {
        newWidth += ILI9341_GetCharWidth(*s, font);  // Sum each char's pixel width
        newLen++;                                     // Count characters
        s++;
    }

    // ─────────────────────────────────────────────────────────────
    // CHECK: Are we drawing in the SAME text field as last call?
    // Same field = same X, Y, and font height → we can optimize redraws
    // ─────────────────────────────────────────────────────────────
    uint8_t sameField = (prevStringState.valid &&      // Previous state exists?
                         prevStringState.x == X &&      // Same horizontal position?
                         prevStringState.y == Y &&      // Same vertical position?
                         prevStringState.height == fHeight);  // Same font?

    // ─────────────────────────────────────────────────────────────
    // BACKGROUND CLEARING STRATEGY
    // ─────────────────────────────────────────────────────────────
    if(sameField  && prevStringState.width > newWidth )//&& newLen < prevStringState.len
    {
        // ── SAME FIELD: Smart partial clear ─────────────────────
        // Only clear what changed — avoids flickering entire field


            // New string is SHORTER in character count
            // Check if it's also narrower in pixels (proportional fonts!)

                // Old string stuck out farther to the right.
                // Erase just the "tail" — the pixels from new end to old end.
                // X + newWidth  = where new string ends
                // prev - new    = how many pixels of old string remain visible
                ILI9341_DrawRectangle( X +newWidth , Y,
                                      prevStringState.width - newWidth,
                                      fHeight, bgcolor);

        }
        else if(sameField && newWidth > prevStringState.width )// && newLen > prevStringState.len
        {

//        	   ILI9341_DrawRectangle(X - prevStringState.width, Y,
//        	                             newWidth - prevStringState.width,
//        	                             fHeight, bgcolor);


        }
        // Note: If new string is same length or longer, we don't clear here.
        // Individual char logic below handles per-character clearing.

    else if(!sameField)
    {
        // ── DIFFERENT FIELD: Full clear required ────────────────

        // 1) Clear the PREVIOUS field (if any) — it's now stale
        if(prevStringState.valid)
        {
            ILI9341_DrawRectangle(prevStringState.x, prevStringState.y,
                                  prevStringState.width,
                                  prevStringState.height, bgcolor);
        }

        // 2) Clear the NEW field before drawing — we're starting fresh
        ILI9341_DrawRectangle(X, Y, newWidth, fHeight, bgcolor);
    }

    // ─────────────────────────────────────────────────────────────
    // PASS 2: Draw the string character by character
    // ─────────────────────────────────────────────────────────────
    s = str;          // Reset pointer to start of string
    i = 0;            // Reset index

    while(*s && i < MAX_TRACKED_STR_LEN - 1)
    {
        uint16_t charW = ILI9341_GetCharWidth(*s, font);  // This char's pixel width
        uint8_t  needsRedraw = 1;                          // Assume we need to draw

        // ── Optimization: Skip unchanged characters ─────────────
        // If same field AND this position existed before AND char is identical,
        // don't redraw — prevents flicker and saves SPI bandwidth
        if(sameField && i < prevStringState.len)
        {
            if(prevStringState.text[i] == *s)
            {
                needsRedraw = 0;  // Char unchanged, skip drawing
            }
        }

        if(needsRedraw)
        {
            // ── Determine how much background to clear for this char ─
            // Must cover the LARGER of: new char width or old char width
            // (Old char might have been wider, leaving pixels behind)
            uint16_t clearW = charW;
            if(sameField && i < prevStringState.len)
            {
                if(prevStringState.charWidths[i] > clearW)
                    clearW = prevStringState.charWidths[i];  // Use wider of the two
            }

            // Clear background for this character position
            ILI9341_DrawRectangle(currentX, Y, clearW, fHeight, bgcolor);

            // Draw the actual character bitmap
            ILI9341_DrawChar(*s, font, currentX, Y, color, bgcolor);
        }

        // ── Save state for next frame's comparison ──────────────
        prevStringState.text[i]       = *s;      // Remember what char we drew
        prevStringState.charWidths[i] = charW;   // Remember how wide it was

        // Advance to next character position
        currentX += charW;   // Move X by this char's actual width (proportional font)
        s++;                  // Next character in string
        i++;                  // Next index in state arrays
    }

    // ─────────────────────────────────────────────────────────────
    // SAVE GLOBAL STATE for next call
    // ─────────────────────────────────────────────────────────────
    prevStringState.text[i] = '\0';    // Null-terminate stored string
    prevStringState.x       = X;        // Remember where we drew
    prevStringState.y       = Y;
    prevStringState.height  = fHeight;  // Remember font height
    prevStringState.width   = newWidth; // Remember total pixel width
    prevStringState.len     = i;        // Remember character count
    prevStringState.valid   = 1;        // Mark state as valid for next time
}
void ILI9341_DrawImage(const uint8_t* image, uint8_t orientation)
{
    uint16_t w = ILI9341_SCREEN_WIDTH;
    uint16_t h = ILI9341_SCREEN_HEIGHT;

    if(orientation == SCREEN_HORIZONTAL_1 || orientation == SCREEN_HORIZONTAL_2)
    {
        ILI9341_SetRotation(orientation);
        ILI9341_SetAddress(	0, 0, w - 1, h - 1);  // Inclusive coordinates!
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
            chunk--;  //Make it even

        ILI9341_WriteBuffer((uint8_t*)(image + counter), chunk);
        counter += chunk;

        DelayUs(1);
    }
}
