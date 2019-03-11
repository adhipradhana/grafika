#include "color.h"

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