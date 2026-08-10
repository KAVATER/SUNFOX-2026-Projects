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
