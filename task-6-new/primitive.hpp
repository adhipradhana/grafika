#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#define CUSTOM_CENTER_POLYGON_CODE -1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum primitive_type
{
	point,
	line,
	polygon,
	circle
} primitive_type;

typedef enum primitive_color
{
	white,
	vermillion_red,
	bright_green,
	blue,
	champions_purple,
	yellow,
	brown,
	orange,
	plushy_shake,
	teal,
	midas_gold,
	pristine_platinum,
	rubiline,
	glacial_flow,
	black,
	border_color // color used in floodfill's border
} primitive_color;

typedef struct Primitive 
{
	primitive_type type;
	primitive_color color;
	int n;
	int* values;
	bool custom_center;
	int custom_center_x;
	int custom_center_y;
} Primitive;

Primitive Primitive_new(primitive_type type, primitive_color color, int n, int* values);

void delete_primitive_values(Primitive* p);

primitive_type get_primitive_type(char type_code);

primitive_color get_primitive_color(char color_code);

Primitive read_primitive(FILE *fin);

void print_primitive(Primitive p);

#endif