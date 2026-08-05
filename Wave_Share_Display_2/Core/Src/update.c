void ILI9341_DrawChar(char ch, const uint16_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor)
{
	if ((ch < 31) || (ch > 127)) return;

	uint16_t fOffset, fWidth, fHeight, fBPL;
	uint16_t *tempChar;

	fOffset = font[0];
	fWidth = font[1];
	fHeight = font[2];
	fBPL = font[3];

	tempChar = (uint16_t*)&font[((ch - 0x20) * fOffset) + 4];

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
