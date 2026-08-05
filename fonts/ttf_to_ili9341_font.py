"""
TTF -> ILI9341_GFX custom font converter
Produces a C array matching this exact DrawChar layout:
  font[0] = fOffset  (words per character block, incl. width word)
  font[1] = fWidth
  font[2] = fHeight
  font[3] = fBPL     (words per column = ceil(fHeight/16))
  Then for each char (0x20..0x7F), a block of fOffset words:
    word 0            = charWidth (actual glyph width, <= fWidth)
    word 1..fWidth*fBPL = column-major bitmap:
        index = fBPL*i + w + 1   (i = column 0..fWidth-1, w = word 0..fBPL-1)
        bit (j % 16) of that word = pixel at row j = w*16 + (j%16)
Requires: pip install pillow
Usage:
  python ttf_to_ili9341_font.py MyFont.ttf 22 FontName22 > fontdata.c
  (2nd arg = target pixel HEIGHT; width per char is derived automatically)
"""
import sys
from PIL import Image, ImageDraw, ImageFont


def render_glyph(font, ch, target_h):
    """
    Render a glyph so every character shares the SAME baseline.

    Key idea: for a given font+size, PIL's default text anchor ("la") places
    xy at the top of the font's ascender box, which is identical for every
    character. So we draw every glyph at the *same* fixed origin (only the
    ink's horizontal extent differs per character) and never move the y
    origin per-glyph -- doing so (as the old code did, using each glyph's
    own ink bottom) breaks the shared baseline and scatters characters
    vertically depending on whether they have descenders.

    We only crop tightly in X (to get the real glyph width); the full
    [0, target_h) vertical window is kept as-is for every character.
    """
    pad_w = target_h * 3  # generous horizontal padding for wide/negative-bearing glyphs
    img = Image.new("L", (pad_w, target_h), 0)
    draw = ImageDraw.Draw(img)
    origin_x = pad_w // 2
    draw.text((origin_x, 0), ch, fill=255, font=font)

    bbox = img.getbbox()
    if bbox is None:
        # No ink (e.g. space) -- fall back to the font's advance width.
        w = max(1, round(font.getlength(ch)))
        return Image.new("L", (w, target_h), 0), w

    left, _top, right, _bottom = bbox
    # Crop tight in X only; keep the FULL height range so the baseline
    # (established by origin_x, 0 above) is identical across all glyphs.
    glyph = img.crop((left, 0, right, target_h))
    return glyph, max(right - left, 1)


def pack_column_major(glyph_img, fWidth, fHeight, fBPL, threshold=128):
    px = glyph_img.load()
    gw, gh = glyph_img.size
    words = [0] * (fWidth * fBPL)
    for i in range(fWidth):
        for j in range(fHeight):
            if i < gw and j < gh:
                lit = px[i, j] >= threshold
            else:
                lit = False
            if lit:
                word_idx = i * fBPL + (j // 16)
                bit = j % 16
                words[word_idx] |= (1 << bit)
    return words


def main():
    if len(sys.argv) < 4:
        print("Usage: python ttf_to_ili9341_font.py <font.ttf> <pixel_height> <FontName>", file=sys.stderr)
        sys.exit(1)

    ttf_path = sys.argv[1]
    target_h = int(sys.argv[2])
    name = sys.argv[3]

    font = ImageFont.truetype(ttf_path, target_h)
    chars = [chr(c) for c in range(0x20, 0x80)]

    glyphs = {}
    max_w = 0
    for ch in chars:
        glyph_img, w = render_glyph(font, ch, target_h)
        glyphs[ch] = (glyph_img, w)
        max_w = max(max_w, w)

    fWidth = max_w
    fHeight = target_h
    fBPL = (fHeight + 15) // 16
    fOffset = 1 + fWidth * fBPL

    print(f"const uint16_t {name}[] = {{")
    print(f"\t{fOffset},{fWidth},{fHeight},{fBPL},")

    for ch in chars:
        glyph_img, charWidth = glyphs[ch]
        words = pack_column_major(glyph_img, fWidth, fHeight, fBPL)
        hex_words = ", ".join(f"0x{w:04X}" for w in words)
        label = ch if ch not in ('\\', "'") else ('backslash' if ch == '\\' else "quote")
        print(f"\t{charWidth}, {hex_words},  // {label}")

    print("};")


if __name__ == "__main__":
    main()
