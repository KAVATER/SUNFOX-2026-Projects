"""
Convert any image into a 16-bit RGB565 C header for ILI9341_DrawImage().
RGB565 format: RRRRRGGG GGGBBBBB (2 bytes per pixel)

Usage:
    python convert_to_rgb565.py <input> <output.h> <width> <height> [var_name] [--bgr] [--swap]

Options:
    --bgr    Swap Red/Blue for BGR displays (common on cheap modules)
    --swap   Swap high/low bytes per pixel (if your SPI library expects little-endian)
"""

import sys
import argparse
from PIL import Image


def convert_to_rgb565(input_path, output_header, width, height, var_name="myImage", bgr=False, swap_bytes=False):
    img = Image.open(input_path).convert("RGB")
    img = img.resize((width, height), Image.LANCZOS)

    pixels = list(img.getdata())
    data = []
    for (r, g, b) in pixels:
        if bgr:
            r, b = b, r  # Swap red and blue channels

        # Pack into RGB565: RRRRRGGG GGGBBBBB
        rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

        high = (rgb565 >> 8) & 0xFF   # RRRRRGGG
        low  = rgb565 & 0xFF          # GGGBBBBB

        if swap_bytes:
            data.append(low)
            data.append(high)
        else:
            data.append(high)
            data.append(low)

    with open(output_header, "w") as f:
        f.write("#include <stdint.h>\n\n")
        f.write(f"#define {var_name.upper()}_WIDTH  {width}\n")
        f.write(f"#define {var_name.upper()}_HEIGHT {height}\n\n")
        f.write(f"const uint8_t {var_name}[{len(data)}] = {{\n")
        for i in range(0, len(data), 20):
            line = data[i:i + 20]
            f.write(",".join(f"0x{v:02X}" for v in line))
            f.write(",\n" if i + 20 < len(data) else "\n")
        f.write("};\n")

    print(f"Done: {width}x{height} = {len(data)} bytes ({len(data)//2} pixels) -> {output_header}")
    if bgr:
        print("Note: BGR mode enabled (red/blue swapped)")
    if swap_bytes:
        print("Note: Byte swap enabled (low byte first)")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Convert image to RGB565 C array")
    parser.add_argument("input", help="Input image path")
    parser.add_argument("output", help="Output header file")
    parser.add_argument("width", type=int, help="Target width")
    parser.add_argument("height", type=int, help="Target height")
    parser.add_argument("var_name", nargs="?", default="myImage", help="C variable name")
    parser.add_argument("--bgr", action="store_true", help="Use BGR instead of RGB")
    parser.add_argument("--swap", action="store_true", help="Swap high/low bytes per pixel")
    args = parser.parse_args()

    convert_to_rgb565(args.input, args.output, args.width, args.height, args.var_name, args.bgr, args.swap)
