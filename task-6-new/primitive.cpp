#include <stdio.h>
#include <string.h>

#include "primitive.hpp"


Primitive Primitive_new(primitive_type type, primitive_color color, int n, int* values)
{
	Primitive p;
	p.type = type;
	p.color = color;
	p.n = n;
	p.values = values;
	return p;
}


void delete_primitive_values(Primitive* p)
{
	// for (int i = 0; i < p->n; ++i)
	// {
	// 	free(p->values[i]);
	// }
	free(p->values);
}

primitive_type get_primitive_type(char type_code)
{
	primitive_type type;
	switch (type_code) {
		case 't':
			type = point;
			break;
		case 'l':
			type = line;
			break;
		case 'p':
			type = polygon;
			break;
		case 'c':
			type = circle;
			break;
		default:
			break;
	}
	return type;
}

primitive_color get_primitive_color(char color_code)
{
    primitive_color color;
    switch (color_code) {
    	case 'w': // white 
    		color = white;
    		break;
    	case 'r': // Vermillion Red
    		color = vermillion_red;
    		break;
    	case 'g': // Bright Green
    		color = bright_green;
    		break;
    	case 'b': // Blue
    		color = blue;
    		break;
    	case 'P': // Champions Purple
    		color = champions_purple;
    		break;
    	case 'y': // Yellow
    		color = yellow;
    		break;
    	case 'n': // Brown
    		color = brown;
    		break;
    	case 'o': // Orange
    		color = orange;
    		break;
    	case 'p': // Plushy Shake / Pink
    		color = plushy_shake;
    		break;
    	case 't': // Teal
    		color = teal;
    		break;
    	case 'G': // Midas Gold
    		color = midas_gold;
    		break;
    	case 'S': // Pristine Platinum / Silver
    		color = pristine_platinum;
    		break;
    	case 'R': // Rubiline / Dark Pink
    		color = rubiline;
    		break;
    	case 'B': // Glacial Flow / Bright Blue
    		color = glacial_flow;
    		break;
    	default: // Black
    		color = black;
    		break;
    }
    return color;
}

// TODO(@izzan): find why xpos ypos are needed
Primitive read_primitive(FILE *fin)
{
	Primitive p;

    char type_code, color_code;

    fscanf(fin, " %c %c", &type_code, &color_code);
    
    p.type = get_primitive_type(type_code);
    p.color = get_primitive_color(color_code);

    if (p.type == point)
    {
    	p.n = 2;
    } else if (p.type == line)
    {
    	p.n = 4;
    } else if (p.type == polygon)
    {
        int x;
    	fscanf(fin, "%d", &x);
        if (x == CUSTOM_CENTER_POLYGON_CODE) {
            p.custom_center = true;
            fscanf(fin, "%d %d", &p.custom_center_x, &p.custom_center_y);
            fscanf(fin, "%d", &p.n);
        } else {
            p.custom_center = false;
            p.n = x;
        }
    	p.n *= 2;
    } else if (p.type == circle)
    {
    	p.n = 3;
    }

    p.values = (int *)malloc(p.n * sizeof(int));
	for (int i = 0; i < p.n; ++i)
	{
		fscanf(fin, "%d", &p.values[i]);
	}

	return p;
}

void print_primitive(Primitive p)
{
	printf("PRIMITIVE - type = ");

	if (p.type == point)
	{
		printf("point");
	} else if (p.type == line)
	{
		printf("line");
	} else if (p.type == polygon)
	{
		printf("polygon");
	} else if (p.type == circle)
	{
		printf("circle");
	}
	printf("\n");
}