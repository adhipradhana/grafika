#ifndef DRAW_H
#define DRAW_H

#include <stdint.h>
#include "primitive.hpp"

typedef struct Drawing {
    int n;
    Primitive *primitives;
} Drawing;

typedef struct Object {
    Drawing drawing;
    double x, y, rot, scale;
} Object;



int initialize();
int finalize();
uint32_t get_pixel_color(uint8_t r, uint8_t g, uint8_t b);
void clear_screen(uint32_t color);
void draw_point(int x, int y, uint32_t color);
void draw_circle(int x0, int y0, int radius, uint32_t color);
void draw_object(Object obj);
int compute_region_code(double x, double y);
void cohen_sutherland_clip(double x1, double y1, double x2, double y2, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void flood_fill(int x,int y, uint32_t replacement_color, uint32_t border_color);


#endif