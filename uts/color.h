#ifndef _COLOR_H
#define _COLOR_H
#include <stdint.h>

#define COLOR_BLACK         0x000000
#define COLOR_WHITE         0xFFFFFF
#define COLOR_GREY          0x808080
#define COLOR_RED           0xFF0000   
#define COLOR_ORANGE        0xFFA500
#define COLOR_YELLOW        0xFFFF00
#define COLOR_GREEN         0x008000
#define COLOR_BLUE          0x0000FF
#define COLOR_PURPLE        0x800080
#define COLOR_BROWN         0xA52A2A
#define COLOR_MAGENTA       0xFF00FF
#define COLOR_TAN           0xD2B48C
#define COLOR_CYAN          0x00FFFF
#define COLOR_OLIVE         0x808000
#define COLOR_MAROON        0x800000
#define COLOR_NAVY          0x000080
#define COLOR_AQUAMARINE    0x7FFFD4
#define COLOR_TURQUOISE     0x40E0D0
#define COLOR_SILVER        0xC0C0C0
#define COLOR_LIME          0x00FF00
#define COLOR_TEAL          0x008080
#define COLOR_INDIGO        0x4B0082
#define COLOR_VIOLET        0x8A2BE2
#define COLOR_PINK          0xFF69B4

uint32_t convertColorFromCode(int code) {
    switch (code) {
        case 1: return COLOR_BLACK;
        case 2: return COLOR_WHITE;
        case 3: return COLOR_GREY;
        case 4: return COLOR_RED;
        case 5: return COLOR_ORANGE;
        case 6: return COLOR_YELLOW;
        case 7: return COLOR_GREEN;
        case 8: return COLOR_BLUE;
        case 9: return COLOR_PURPLE;
        case 10: return COLOR_BROWN;
        case 11: return COLOR_MAGENTA;
        case 12: return COLOR_TAN;
        case 13: return COLOR_CYAN;
        case 14: return COLOR_OLIVE;
        case 15: return COLOR_MAROON;
        case 16: return COLOR_NAVY;
        case 17: return COLOR_AQUAMARINE;
        case 18: return COLOR_TURQUOISE;
        case 19: return COLOR_SILVER;
        case 20: return COLOR_LIME;
        case 21: return COLOR_TEAL;
        case 22: return COLOR_INDIGO;
        case 23: return COLOR_VIOLET;
        case 24: return COLOR_PINK;
        default: return COLOR_BLACK;
    }
}

#endif
