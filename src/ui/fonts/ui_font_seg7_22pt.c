/*******************************************************************************
 * Size: 22 px
 * Bpp: 1
 * Opts: --bpp 1 --size 22 --font /home/osboxes/SquareLine/assets/Seven Segment.ttf -o /home/osboxes/SquareLine/assets/ui_font_seg7_22pt.c --format lvgl -r 0x20-0x7f --no-compress --no-prefilter
 ******************************************************************************/

#include "../ui.h"

#ifndef UI_FONT_SEG7_22PT
#define UI_FONT_SEG7_22PT 1
#endif

#if UI_FONT_SEG7_22PT

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xfb,

    /* U+0022 "\"" */
    0xb4,

    /* U+0023 "#" */
    0x5, 0x83, 0x40, 0x93, 0xff, 0x1a, 0x5, 0x8f,
    0xfc, 0x90, 0x2c, 0x1a, 0x0,

    /* U+0024 "$" */
    0x8, 0x4, 0x1f, 0xd0, 0x8, 0x4, 0x2, 0x1,
    0x0, 0x80, 0x40, 0x1f, 0xc0, 0x10, 0x8, 0x4,
    0x2, 0x1, 0x0, 0xbf, 0x82, 0x1, 0x0,

    /* U+0025 "%" */
    0x0, 0x20, 0x3, 0x1c, 0x31, 0x11, 0x8, 0x98,
    0x44, 0x81, 0xcc, 0x0, 0x40, 0x6, 0x70, 0x24,
    0x43, 0x22, 0x11, 0x11, 0x87, 0x18, 0x0, 0x80,
    0x0,

    /* U+0026 "&" */
    0x7e, 0x0, 0x0, 0x0, 0x0, 0x0, 0x40, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x20, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x10, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x4, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0xfc, 0x3f, 0x87, 0xe1, 0xf8,
    0x3f, 0x80, 0x20, 0x4, 0x1, 0x2, 0x20, 0x40,
    0x10, 0x2, 0x0, 0x81, 0x10, 0x20, 0x8, 0x1,
    0x0, 0x40, 0x88, 0x10, 0x4, 0x0, 0x80, 0x20,
    0x44, 0x8, 0x2, 0x0, 0x40, 0x10, 0x22, 0x4,
    0x1, 0x0, 0x20, 0x8, 0x11, 0x2, 0x0, 0x80,
    0x10, 0x4, 0x8, 0x80, 0xfc, 0x40, 0x8, 0x1,
    0xf8, 0x40,

    /* U+0028 "(" */
    0x7c, 0x21, 0x8, 0x42, 0x10, 0x84, 0x21, 0x8,
    0x42, 0xf,

    /* U+0029 ")" */
    0xf0, 0x42, 0x10, 0x84, 0x21, 0x8, 0x42, 0x10,
    0x84, 0x3e,

    /* U+002A "*" */
    0x10, 0x21, 0xf1, 0xc7, 0xca, 0x84, 0x8,

    /* U+002B "+" */
    0x4, 0x0, 0x80, 0x10, 0x2, 0x0, 0x41, 0xf7,
    0xc1, 0x0, 0x20, 0x4, 0x0, 0x80, 0x10, 0x0,

    /* U+002C "," */
    0x7f, 0x0,

    /* U+002D "-" */
    0xfe,

    /* U+002E "." */
    0xc0,

    /* U+002F "/" */
    0x0, 0x80, 0x20, 0x18, 0x4, 0x3, 0x0, 0x80,
    0x60, 0x10, 0x0, 0x6, 0x1, 0x0, 0xc0, 0x20,
    0x18, 0x4, 0x3, 0x0,

    /* U+0030 "0" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,

    /* U+0031 "1" */
    0xff, 0xff,

    /* U+0032 "2" */
    0x7e, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x7e,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x7e,

    /* U+0033 "3" */
    0xfc, 0x4, 0x8, 0x10, 0x20, 0x40, 0xfe, 0x2,
    0x4, 0x8, 0x10, 0x20, 0x40, 0xfe,

    /* U+0034 "4" */
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,

    /* U+0035 "5" */
    0x7e, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x7e,
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x7e,

    /* U+0036 "6" */
    0x7e, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x7e,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,

    /* U+0037 "7" */
    0xfc, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x2,
    0x4, 0x8, 0x10, 0x20, 0x40, 0x81,

    /* U+0038 "8" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,

    /* U+0039 "9" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x7e,

    /* U+003A ":" */
    0xe6,

    /* U+003C "<" */
    0x13, 0x22, 0x64, 0xc0, 0xc, 0x46, 0x22, 0x31,

    /* U+003D "=" */
    0xff, 0xc0, 0x0, 0x0, 0x0, 0xff, 0xc0,

    /* U+003E ">" */
    0xc4, 0x46, 0x23, 0x31, 0x13, 0x32, 0x64, 0x44,

    /* U+003F "?" */
    0x7e, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x7e,
    0x80, 0x80, 0x80, 0x80, 0x0, 0x80, 0x80,

    /* U+0040 "@" */
    0x7e, 0x81, 0x81, 0x81, 0x8f, 0x91, 0x91, 0x91,
    0x91, 0x91, 0x91, 0x8e, 0x80, 0x80, 0x80, 0x7e,

    /* U+0041 "A" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,

    /* U+0042 "B" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,

    /* U+0043 "C" */
    0x7f, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x81,
    0x2, 0x4, 0x8, 0x10, 0x20, 0x3f,

    /* U+0044 "D" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,

    /* U+0045 "E" */
    0x7f, 0x2, 0x4, 0x8, 0x10, 0x20, 0x3f, 0x81,
    0x2, 0x4, 0x8, 0x10, 0x20, 0x3f,

    /* U+0046 "F" */
    0x7f, 0x2, 0x4, 0x8, 0x10, 0x20, 0x3f, 0x81,
    0x2, 0x4, 0x8, 0x10, 0x20, 0x0,

    /* U+0047 "G" */
    0x7e, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x3e,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,

    /* U+0048 "H" */
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,

    /* U+0049 "I" */
    0xff, 0xff,

    /* U+004A "J" */
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
    0x0, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,

    /* U+004B "K" */
    0x81, 0x83, 0x86, 0x8c, 0x98, 0xb0, 0x80, 0x7e,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,

    /* U+004C "L" */
    0x81, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x1,
    0x2, 0x4, 0x8, 0x10, 0x20, 0x3f,

    /* U+004D "M" */
    0x7d, 0xf4, 0x10, 0x60, 0x83, 0x4, 0x18, 0x20,
    0xc1, 0x6, 0x8, 0x20, 0x0, 0x80, 0xc, 0x0,
    0x60, 0x3, 0x0, 0x18, 0x0, 0xc0, 0x6, 0x0,
    0x30, 0x1,

    /* U+004E "N" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x0,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,

    /* U+004F "O" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,

    /* U+0050 "P" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,

    /* U+0051 "Q" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x83, 0x83, 0x7d,

    /* U+0052 "R" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x7e, 0x80, 0xb0, 0x98, 0x8c, 0x86, 0x83, 0x81,

    /* U+0053 "S" */
    0x7e, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x7e,
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x7e,

    /* U+0054 "T" */
    0xf7, 0x84, 0x2, 0x1, 0x0, 0x80, 0x40, 0x20,
    0x0, 0x8, 0x4, 0x2, 0x1, 0x0, 0x80, 0x40,
    0x20, 0x10,

    /* U+0055 "U" */
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x0, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,

    /* U+0056 "V" */
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x81, 0xc3, 0x42, 0x66, 0x66, 0x24, 0x24,

    /* U+0057 "W" */
    0x80, 0xc, 0x0, 0x60, 0x3, 0x0, 0x18, 0x0,
    0xc0, 0x6, 0x0, 0x30, 0x1, 0x0, 0x4, 0x10,
    0x60, 0x83, 0x4, 0x18, 0x20, 0xc1, 0x6, 0x8,
    0x2f, 0xbe,

    /* U+0058 "X" */
    0xc1, 0xa0, 0xd0, 0x4c, 0x62, 0x21, 0xb0, 0xd8,
    0x28, 0x14, 0x1b, 0xd, 0x84, 0x46, 0x32, 0x9,
    0x6, 0x83,

    /* U+0059 "Y" */
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x7e,

    /* U+005A "Z" */
    0xfe, 0x0, 0x80, 0xc0, 0x60, 0x60, 0x30, 0x30,
    0x0, 0x0, 0x8, 0xc, 0x4, 0x6, 0x2, 0x1,
    0x0, 0x7f,

    /* U+005B "[" */
    0x7c, 0x21, 0x8, 0x42, 0x10, 0x84, 0x21, 0x8,
    0x42, 0xf,

    /* U+005C "\\" */
    0x40, 0x30, 0x6, 0x0, 0x80, 0x30, 0x4, 0x1,
    0x80, 0x20, 0x0, 0x1, 0x80, 0x60, 0xc, 0x1,
    0x0, 0x60, 0x8, 0x2,

    /* U+005D "]" */
    0xf0, 0x42, 0x10, 0x84, 0x21, 0x8, 0x42, 0x10,
    0x84, 0x3e,

    /* U+005E "^" */
    0x2, 0x40, 0xe, 0x70, 0x78, 0x1e, 0xe0, 0x7,

    /* U+005F "_" */
    0xff, 0xe0,

    /* U+0061 "a" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,

    /* U+0062 "b" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,

    /* U+0063 "c" */
    0x7f, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x81,
    0x2, 0x4, 0x8, 0x10, 0x20, 0x3f,

    /* U+0064 "d" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,

    /* U+0065 "e" */
    0x7f, 0x2, 0x4, 0x8, 0x10, 0x20, 0x3f, 0x81,
    0x2, 0x4, 0x8, 0x10, 0x20, 0x3f,

    /* U+0066 "f" */
    0x7f, 0x2, 0x4, 0x8, 0x10, 0x20, 0x3f, 0x81,
    0x2, 0x4, 0x8, 0x10, 0x20, 0x0,

    /* U+0067 "g" */
    0x7e, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x3e,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,

    /* U+0068 "h" */
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,

    /* U+0069 "i" */
    0xff, 0xff,

    /* U+006A "j" */
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
    0x0, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,

    /* U+006B "k" */
    0x81, 0x83, 0x86, 0x8c, 0x98, 0xb0, 0x80, 0x7e,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,

    /* U+006C "l" */
    0x81, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x1,
    0x2, 0x4, 0x8, 0x10, 0x20, 0x3f,

    /* U+006D "m" */
    0x7d, 0xf4, 0x10, 0x60, 0x83, 0x4, 0x18, 0x20,
    0xc1, 0x6, 0x8, 0x20, 0x0, 0x80, 0xc, 0x0,
    0x60, 0x3, 0x0, 0x18, 0x0, 0xc0, 0x6, 0x0,
    0x30, 0x1,

    /* U+006E "n" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x0,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,

    /* U+006F "o" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,

    /* U+0070 "p" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,

    /* U+0071 "q" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x83, 0x83, 0x7d,

    /* U+0072 "r" */
    0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x7e, 0x80, 0xb0, 0x98, 0x8c, 0x86, 0x83, 0x81,

    /* U+0073 "s" */
    0x7e, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x7e,
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x7e,

    /* U+0074 "t" */
    0xf7, 0x84, 0x2, 0x1, 0x0, 0x80, 0x40, 0x20,
    0x0, 0x8, 0x4, 0x2, 0x1, 0x0, 0x80, 0x40,
    0x20, 0x10,

    /* U+0075 "u" */
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x0, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,

    /* U+0076 "v" */
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x81, 0xc3, 0x42, 0x66, 0x66, 0x24, 0x24,

    /* U+0077 "w" */
    0x80, 0xc, 0x0, 0x60, 0x3, 0x0, 0x18, 0x0,
    0xc0, 0x6, 0x0, 0x30, 0x1, 0x0, 0x4, 0x10,
    0x60, 0x83, 0x4, 0x18, 0x20, 0xc1, 0x6, 0x8,
    0x2f, 0xbe,

    /* U+0078 "x" */
    0xc1, 0xa0, 0xd0, 0x4c, 0x62, 0x21, 0xb0, 0xd8,
    0x28, 0x14, 0x1b, 0xd, 0x84, 0x46, 0x32, 0x9,
    0x6, 0x83,

    /* U+0079 "y" */
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e,
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x7e,

    /* U+007A "z" */
    0xfe, 0x0, 0x80, 0xc0, 0x60, 0x60, 0x30, 0x30,
    0x0, 0x0, 0x8, 0xc, 0x4, 0x6, 0x2, 0x1,
    0x0, 0x7f,

    /* U+007B "{" */
    0x1e, 0x40, 0x81, 0x2, 0x4, 0x8, 0x60, 0x20,
    0x40, 0x81, 0x2, 0x4, 0x8, 0xf,

    /* U+007C "|" */
    0xff, 0xff, 0xc0,

    /* U+007D "}" */
    0xf0, 0x10, 0x20, 0x40, 0x81, 0x2, 0x3, 0x8,
    0x10, 0x20, 0x40, 0x81, 0x2, 0x78
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 106, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 65, .box_w = 1, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 3, .adv_w = 99, .box_w = 3, .box_h = 2, .ofs_x = 1, .ofs_y = 14},
    {.bitmap_index = 4, .adv_w = 192, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 17, .adv_w = 177, .box_w = 9, .box_h = 20, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 40, .adv_w = 247, .box_w = 13, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 65, .adv_w = 832, .box_w = 49, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 163, .adv_w = 134, .box_w = 5, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 173, .adv_w = 134, .box_w = 5, .box_h = 16, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 183, .adv_w = 145, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 11},
    {.bitmap_index = 190, .adv_w = 233, .box_w = 11, .box_h = 11, .ofs_x = 2, .ofs_y = 2},
    {.bitmap_index = 206, .adv_w = 81, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 208, .adv_w = 143, .box_w = 7, .box_h = 1, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 209, .adv_w = 65, .box_w = 1, .box_h = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 210, .adv_w = 191, .box_w = 10, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 230, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 246, .adv_w = 65, .box_w = 1, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 248, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 264, .adv_w = 160, .box_w = 7, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 278, .adv_w = 177, .box_w = 8, .box_h = 15, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 293, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 309, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 325, .adv_w = 160, .box_w = 7, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 339, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 355, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 371, .adv_w = 65, .box_w = 1, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 372, .adv_w = 102, .box_w = 4, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 380, .adv_w = 194, .box_w = 10, .box_h = 5, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 387, .adv_w = 102, .box_w = 4, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 395, .adv_w = 177, .box_w = 8, .box_h = 15, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 410, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 426, .adv_w = 177, .box_w = 8, .box_h = 15, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 441, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 457, .adv_w = 160, .box_w = 7, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 471, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 487, .adv_w = 160, .box_w = 7, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 501, .adv_w = 160, .box_w = 7, .box_h = 15, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 515, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 531, .adv_w = 177, .box_w = 8, .box_h = 15, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 546, .adv_w = 65, .box_w = 1, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 548, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 564, .adv_w = 177, .box_w = 8, .box_h = 15, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 579, .adv_w = 160, .box_w = 7, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 593, .adv_w = 241, .box_w = 13, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 619, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 635, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 651, .adv_w = 177, .box_w = 8, .box_h = 15, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 666, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 682, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 698, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 714, .adv_w = 206, .box_w = 9, .box_h = 16, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 732, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 748, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 764, .adv_w = 241, .box_w = 13, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 790, .adv_w = 177, .box_w = 9, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 808, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 824, .adv_w = 177, .box_w = 9, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 842, .adv_w = 134, .box_w = 5, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 852, .adv_w = 191, .box_w = 10, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 872, .adv_w = 134, .box_w = 5, .box_h = 16, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 882, .adv_w = 289, .box_w = 16, .box_h = 4, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 890, .adv_w = 219, .box_w = 11, .box_h = 1, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 892, .adv_w = 177, .box_w = 8, .box_h = 15, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 907, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 923, .adv_w = 160, .box_w = 7, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 937, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 953, .adv_w = 160, .box_w = 7, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 967, .adv_w = 160, .box_w = 7, .box_h = 15, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 981, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 997, .adv_w = 177, .box_w = 8, .box_h = 15, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 1012, .adv_w = 65, .box_w = 1, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1014, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1030, .adv_w = 177, .box_w = 8, .box_h = 15, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 1045, .adv_w = 160, .box_w = 7, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1059, .adv_w = 241, .box_w = 13, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1085, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1101, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1117, .adv_w = 177, .box_w = 8, .box_h = 15, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 1132, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1148, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1164, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1180, .adv_w = 206, .box_w = 9, .box_h = 16, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1198, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1214, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1230, .adv_w = 241, .box_w = 13, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1256, .adv_w = 177, .box_w = 9, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1274, .adv_w = 177, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1290, .adv_w = 177, .box_w = 9, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1308, .adv_w = 169, .box_w = 7, .box_h = 16, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1322, .adv_w = 65, .box_w = 1, .box_h = 18, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1325, .adv_w = 169, .box_w = 7, .box_h = 16, .ofs_x = 2, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 7, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 40, .range_length = 19, .glyph_id_start = 8,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 60, .range_length = 36, .glyph_id_start = 27,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 97, .range_length = 29, .glyph_id_start = 63,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 4,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t ui_font_seg7_22pt = {
#else
lv_font_t ui_font_seg7_22pt = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 21,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -7,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if UI_FONT_SEG7_22PT*/
