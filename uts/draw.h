#ifndef _DRAW_H
#define _DRAW_H

//  LIBRARY DECLARATION
#include <linux/fb.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/kd.h>
#include <string.h>
#include <math.h>

// CONSTANT DECLARATION
#define bitmap_size_x 32
#define bitmap_size_y 32

// GLOBAL VARIABLE DECLARATION 
uint8_t *fbp;
struct fb_fix_screeninfo finfo;
struct fb_var_screeninfo vinfo;

unsigned char char_bitmap[sizeof(char)][bitmap_size_x][bitmap_size_y];

// END GLOBAL VARIABLE DECLARATION

/*
    Returning color from RGB into Unsigned Integer
*/
uint32_t pixel_color(uint8_t r, uint8_t g, uint8_t b);

/*
    Clearing the screen
*/
void clear_screen(uint32_t color);

/*
    Printing point (x,y) with given color
*/
void print_point(int x,int y, uint32_t color);

/*
    Printing line from (x0, y0) to (x1, y1) with given color
*/
void print_line(int x1, int y1, int x2, int y2, uint32_t color);

/*
    Printing line from (x1, y1) to (x2, y2) if the slope is less than 1
*/
void print_line_small(int x1, int y1,int x2, int y2, uint32_t color);

/*
    Printing line from (x1, y1) to (x2, y2) if the slope is greater than 1
*/
void print_line_large(int x1, int y1,int x2, int y2, uint32_t color);

/*
    Printing vertical from (x1, y1) to (x2, y2)
*/
void print_vertical_line(int x1, int y1, int x2, int y2, int color);

/*
    Printing horizontal line from (x1, y1) to (x2, y2)
*/
void print_horizontal_line(int x1, int y1, int x2, int y2, int color);

/*
    Displaying files given line
*/
void print_file_line(char* filename, int posx, int posy, uint32_t color);

/*
    Printing polygon given a list of coordinates
*/
void print_polygon(int n, int* arr_coordinate, uint32_t color);

/*
    Printing circle given its middle point
*/
void print_circle(int x0, int y0, int r, uint32_t color);

/*
    Displaying polygon from given file
*/
void print_file_polygon(char* filename, int* polygon_save_sides, int (*polygon_save_points)[1000], int* polygon_save_color, int* iter);

/*
    Displaying circle from given file
*/
void print_file_circle(char* filename, int (*circle_save_points)[3], int* circle_save_color, int* iter);

/*
    Displaying polygon from previous saved points
*/
void print_polygon_save(int* polygon_save_sides, int (*polygon_save_points)[1000], int* polygon_save_color, int iter);

/*
    Displaying circle from previous saved points
*/
void print_circle_save(int (*circle_save_points)[3], int* circle_save_color, int iter);

/*
    Translate the images
*/
void translation(int* polygon_save_sides, int (*polygon_save_points)[1000], int (*circle_save_points)[3], int* polygon_save_color, int* circle_save_color, int iter_polygon, int iter_circle, int dx, int dy);

/*
    Scale the images
*/
void scale(int* polygon_save_sides, int (*polygon_save_points)[1000], int (*circle_save_points)[3], int* polygon_save_color, int* circle_save_color, int iter_polygon, int iter_circle, float scale_factor);

/*
    Rotates the images
*/
void rotate(int* polygon_save_sides, int (*polygon_save_points)[1000], int (*circle_save_points)[3], int* polygon_save_color, int* circle_save_color, int iter_polygon, int iter_circle, float rot_degree);

/*
    Create a bullet (for animation)
*/
void make_bullet(int* polygon_save_sides, int (*polygon_save_points)[1000], int (*circle_save_points)[3], int* polygon_save_color, int* circle_save_color, int iter_polygon, int *iter_circle, int x, int y, int r, uint32_t color);

/*
    Animate the buller (animation)
*/
void animate_bullet(int* polygon_save_sides, int (*polygon_save_points)[1000], int (*circle_save_points)[3], int* polygon_save_color, int* circle_save_color, int iter_polygon, int iter_circle, int xi, int xf);

/*
    Returning region codes for cohen sutherland algorithm
*/
int compute_region_code(int x, int y);

/*
    Drawing line inside a viewpoint
*/
void cohen_sutherland_clip(int x1, int y1, int x2, int y2, uint32_t color);

/*
    Flood fill color
*/
void flood_fill(int x, int y, uint32_t replacement_color);

/*
    Check color
*/
int is_color_same(int x, int y, uint32_t color);

/*
    Check out of bound
*/
int is_out_of_bound(int x, int y);

/*
    Find centroid
*/
int* compute_centroid(int* vertices, int vertexCount);

#endif