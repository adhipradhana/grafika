#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "draw.hpp"
#include "primitive.hpp"

#define POINT 't'
#define LINE 'l'
#define POLYGON 'p'
#define CIRCLE 'c'

Drawing *drawings;

int n_objects = 0;
Object *objects;

Object viewport;
int x_max;
int y_max;
int x_min;
int y_min;

int controlled_object = 0;

void draw_example() {
    Primitive p = Primitive_new(polygon, white, 8, (int *) malloc(8 * sizeof(int)));
    p.values[0] = 0;
    p.values[1] = 0;
    p.values[2] = 0;
    p.values[3] = 200;
    p.values[4] = 200;
    p.values[5] = 200;
    p.values[6] = 200;
    p.values[7] = 0;

    Drawing d;
    d.n = 1;
    d.primitives = (Primitive *) malloc(sizeof(Primitive));
    d.primitives[0] = p;
    
    Object o;
    o.drawing = d;
    o.rot = 0;
    o.scale = 1;
    o.x = 300;
    o.y = 400;
    
    draw_object(o);
}

void initialize_viewport(){
    Primitive p = Primitive_new(polygon, vermillion_red, 8, (int *) malloc(8 * sizeof(int)));
    p.values[0] = 0;
    p.values[1] = 0;
    p.values[2] = 0;
    p.values[3] = 1000;
    p.values[4] = 1000;
    p.values[5] = 1000;
    p.values[6] = 1000;
    p.values[7] = 0;

    Drawing d;
    d.n = 1;
    d.primitives = (Primitive *) malloc(sizeof(Primitive));
    d.primitives[0] = p;

    viewport.drawing = d;
    viewport.x =  300;
    viewport.y =  300;
    viewport.rot = 0;
    viewport.scale = 1; 

    // Add view port to objects
    objects[n_objects] = viewport;
    n_objects++; 

    x_max = viewport.x + viewport.drawing.primitives->values[5];
    x_min = viewport.x;
    y_max = viewport.y + viewport.drawing.primitives->values[4] - 200;
    y_min = viewport.y - 200;
}

void read_configuration(FILE *conf)
{
    int n_drawings;
    fscanf(conf, "%d", &n_drawings);
    drawings = (Drawing *) malloc(n_drawings * sizeof(Drawing));

    for (int i = 0; i < n_drawings; i++) {
        char filename[20];
        fscanf(conf, "%s", filename);
        FILE *f_drawing = fopen(filename, "r");

        int n_primitives;
        fscanf(f_drawing, "%d", &n_primitives);
        drawings[i].n = n_primitives;

        drawings[i].primitives = (Primitive *) malloc(n_primitives * sizeof(Primitive));
        for (int j = 0; j < n_primitives; j++) {
            drawings[i].primitives[j] = read_primitive(f_drawing);
        }

        fclose(f_drawing);
    }

    fscanf(conf, "%d", &n_objects);
    objects = (Object *) malloc( (n_objects+1) * sizeof(Object));

    for (int i = 0; i < n_objects; i++) {
        int drawing_index;
        fscanf(conf, "%d", &drawing_index);
        objects[i].drawing = drawings[drawing_index];

        fscanf(conf, "%lf %lf %lf %lf", &objects[i].x, &objects[i].y, &objects[i].rot, &objects[i].scale);
    }

    // Add viewport as an object
    // initialize_viewport();

}

void translate(int i, double x, double y)
{
    objects[i].x += x;
    objects[i].y += y;

    if (i == (n_objects-1))
    {
        x_max += x;
        x_min += x;
        y_max += y;
        y_min += y;
    }
}

void rotate(int i, double deg)
{
    objects[i].rot += deg;
}

void scale(int i, double factor)
{
    objects[i].scale += factor;
}

int main(int argc, char **argv)
{
    // Get input (argv[1]) which containst list of pixel
    if (argc != 2)
    {
        printf("params: <filename>\n");
        return 1;
    }

    char *filename = argv[1];

    // Open configuration file
    FILE *fin = fopen(filename, "r");

    initialize();

    // Read configuration file
    read_configuration(fin);

    // Close configuration file
    fclose(fin);

    // Set terminal settings so keystrokes are sent directly to stdin
    system("stty cbreak");

    do
    {
        // Clear screen
        clear_screen(get_pixel_color(0, 0, 0));
        // draw_example();
        // Draw all objects
        for (int i = 0; i < n_objects; i++) {
            printf("Drawing object %d\n", i);
            draw_object(objects[i]);
            printf("\n");
        }

        switch (getchar())
        {
            case 'w':
                translate(controlled_object, 0, -1);
                break;

            case 'a':
                translate(controlled_object, -1, 0);
                break;

            case 's':
                translate(controlled_object, 0, 1);
                break;

            case 'd':
                translate(controlled_object, 1, 0);
                break;

            case 'e':
                rotate(controlled_object, 0.1);
                break;

            case 'q':
                rotate(controlled_object, -0.1);
                break;

            case 'W':
                scale(controlled_object, 1);
                break;
            
            case 'S':
                scale(controlled_object, -1);
                break;

            case ' ':
                controlled_object = (controlled_object + 1) % n_objects;
                break;

            case '.':
                goto END;

            default:
                break;
        }
    } while (1);
    
    END:
    // Set terminal settings to normal behaviour
    system("stty -cbreak");
    
    finalize();

    return 0;
}
