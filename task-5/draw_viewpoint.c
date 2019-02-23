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

#define bitmap_size_x 32
#define bitmap_size_y 32

uint8_t *fbp;
struct fb_fix_screeninfo finfo;
struct fb_var_screeninfo vinfo;

unsigned char char_bitmap[sizeof(char)][bitmap_size_x][bitmap_size_y];

// Defining region codes 
int INSIDE = 0; // 0000 
int LEFT = 1;   // 0001 
int RIGHT = 2;  // 0010 
int BOTTOM = 4; // 0100 
int TOP = 8;    // 1000 

// Defining x_max, y_max and x_min, y_min for 
// clipping rectangle. Since diagonal points are 
// enough to define a rectangle 
const int x_max = 200; 
const int y_max = 200; 
const int x_min = 0; 
const int y_min = 0; 

int compute_region_code(int x, int y);

uint32_t pixel_color(uint8_t r, uint8_t g, uint8_t b);

void clear_screen(uint32_t color);

void print_point(int x,int y, uint32_t color);

void print_file(char* filename, int posx, int posy, uint32_t color);

void print_line(int x0, int y0, int x1, int y1, uint32_t color);

void cohen_sutherland_clip(int x1, int y1, int x2, int y2, uint32_t color);

void print_line_small(int x1, int y1,int x2, int y2, uint32_t color);

void print_line_large(int x1, int y1,int x2, int y2, uint32_t color);

void print_file_polygon(char* filename, uint32_t color, int* polygon_save_sides, int (*polygon_save_points)[1000], int* iter);

void print_file_circle(char* filename, uint32_t color, int (*circle_save_points)[3], int* iter);

void print_polygon(int n, int* arr_coordinate, uint32_t color);

void print_circle(int x0, int y0, int r, uint32_t color);

void print_vertical_line(int x1, int y1, int x2, int y2, int color);

void print_horizontal_line(int x1, int y1, int x2, int y2, int color);

void translation(int* polygon_save_sides, int (*polygon_save_points)[1000], int (*circle_save_points)[3], int iter_polygon, int iter_circle, int dx, int dy, uint32_t color);

void scale(int* polygon_save_sides, int (*polygon_save_points)[1000], int (*circle_save_points)[3], int iter_polygon, int iter_circle, float scale_factor, uint32_t color);

void rotate(int* polygon_save_sides, int (*polygon_save_points)[1000], int (*circle_save_points)[3], int iter_polygon, int iter_circle, float rot_degree, uint32_t color);

void print_polygon_save(int* polygon_save_sides, int (*polygon_save_points)[1000], int iter, uint32_t color);

void print_circle_save(int (*circle_save_points)[3], int iter, uint32_t color);

void make_bullet(int* polygon_save_sides, int (*polygon_save_points)[1000], int (*circle_save_points)[3], int iter_polygon, int *iter_circle, int x, int y, int r, uint32_t color);

void animate_bullet(int* polygon_save_sides, int (*polygon_save_points)[1000], int (*circle_save_points)[3], int iter_polygon, int iter_circle, int xi, int xf, uint32_t color);

int main(int argc, char** argv) {
    // Get input (argv[1]) which containst list of pixel
    if (argc != 4) {
        printf("params: <filename x 3>\n");
        return 1;
    }
    
    // Lock the screen from being re-rendered
    int tty_fd = open("/dev/tty0", O_RDWR);
	ioctl(tty_fd,KDSETMODE,KD_GRAPHICS);

	int fb_fd = open("/dev/fb0",O_RDWR);

	//Get variable screen information
	ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);
	vinfo.grayscale = 0;
	vinfo.bits_per_pixel = 32;
	ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vinfo);
	ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);

    // Get fixed screen information
	ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);

    /* 
     * Calculate total size of the screen (in bytes) using got variable (vinfo and finfo)
     * We need to map exactly the right amount of memory and draw into that memory.
     * Failure to draw inside the memory may cause segfault on the machine.
     */
	long screensize = vinfo.yres_virtual * finfo.line_length;

    /* 
     * Map the buffer (according to screensize) to memory. 
     * fbp will contain a pointer to the beginning of the memory.
     */
	fbp = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, (off_t)0);

    // Render the screen to whole black
    clear_screen(pixel_color(0, 0, 0));
    // Print image from input (argv[1] which contains list of pixel location) all white
    print_file(argv[1], 0, 0, pixel_color(255, 0, 0));

    // Wait for input
    getchar();
    // Unlock the screen from being re-rendered
    ioctl(tty_fd,KDSETMODE,KD_TEXT);

    //File saves
    int polygon_save_sides[1000];
    int polygon_save_points[1000][1000];
    int circle_save_points[1000][3];
    int iter_polygon = 0;
    int iter_circle = 0;

    // Render the screen to whole black
    clear_screen(pixel_color(0, 0, 0));
    // Print image from input (argv[1] and argv[2] which contains list of pixel location) all white
    print_file_polygon(argv[2], pixel_color(255, 0, 0), polygon_save_sides, polygon_save_points, &iter_polygon);
    print_file_circle(argv[3], pixel_color(255, 0, 0), circle_save_points, &iter_circle);

    // Wait for input
    char input = getchar();
    while(1) {
        switch(input) {
            case '\033': // if the first value is esc
                getchar(); // skip the [
                switch(getchar()) { // the real value
                    case 'A':
                        // code for arrow up
                        translation(polygon_save_sides, polygon_save_points, circle_save_points, iter_polygon, iter_circle, 0, -10, pixel_color(255, 0, 0));
                        break;
                    case 'B':
                        // code for arrow down
                        translation(polygon_save_sides, polygon_save_points, circle_save_points, iter_polygon, iter_circle, 0, 10, pixel_color(255, 0, 0));
                        break;
                    case 'C':
                        // code for arrow right
                        translation(polygon_save_sides, polygon_save_points, circle_save_points, iter_polygon, iter_circle, 10, 0, pixel_color(255, 0, 0));
                        break;
                    case 'D':
                        // code for arrow left
                        translation(polygon_save_sides, polygon_save_points, circle_save_points, iter_polygon, iter_circle, -10, 0, pixel_color(255, 0, 0));
                        break;
                }
                break;
            case 'u': //scale up
                scale(polygon_save_sides, polygon_save_points, circle_save_points, iter_polygon, iter_circle, 1.2, pixel_color(255, 0, 0));
                break;
            case 'i': //scale down
                scale(polygon_save_sides, polygon_save_points, circle_save_points, iter_polygon, iter_circle, 0.8, pixel_color(255, 0, 0));
                break;
            case 'r': //rotate counter-clockwise
                rotate(polygon_save_sides, polygon_save_points, circle_save_points, iter_polygon, iter_circle, 0.04, pixel_color(255, 0, 0));
                break;
            case 't':
                rotate(polygon_save_sides, polygon_save_points, circle_save_points, iter_polygon, iter_circle, -0.04, pixel_color(255, 0, 0));
                break;
            case 'f':
                make_bullet(polygon_save_sides, polygon_save_points, circle_save_points, iter_polygon, &iter_circle, 400, 150, 12, pixel_color(255,0,0));
                while (getchar() != 'g') {
                    animate_bullet(polygon_save_sides, polygon_save_points, circle_save_points, iter_polygon, iter_circle, 400, 550, pixel_color(255,0,0));
                }
                iter_circle -= 1;
                break;
            case 'e':
                // Unlock the screen from being re-rendered
                ioctl(tty_fd,KDSETMODE,KD_TEXT);
                return 0;
        }
        input = getchar();
    }
	return 0;
}

void print_point(int x,int y, uint32_t color) {
    if (!(x >= vinfo.xres || y >= vinfo.yres || x < 0 || y < 0)) {
        long location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) + (y + vinfo.yoffset) * finfo.line_length;
        *((uint32_t*)(fbp + location)) = color;
    }
}


uint32_t pixel_color(uint8_t r, uint8_t g, uint8_t b) {
    return (r<<vinfo.red.offset) | (g<<vinfo.green.offset) | (b<<vinfo.blue.offset);
}


void clear_screen(uint32_t color) {
    for (int x = 0; x < vinfo.xres; x++) {
        for (int y = 0; y < vinfo.yres; y++) {
            long location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) + (y + vinfo.yoffset) * finfo.line_length;
            *((uint32_t*)(fbp + location)) = color;
        }
    }
}

void print_file_polygon(char* filename, uint32_t color, int* polygon_save_sides, int (*polygon_save_points)[1000], int* iter) {
    FILE *file = fopen(filename, "r");
    
    int n, x, y;
    fscanf(file, "%d", &n);

    while (!feof(file)) {
        polygon_save_sides[*iter] = n;
        for (int i = 0; (i < n) && !feof(file); i++) {
            fscanf(file, "%d,%d", &x, &y);
            polygon_save_points[*iter][i*2] = x;
            polygon_save_points[*iter][i*2+1] = y;
        }
        print_polygon(polygon_save_sides[*iter], polygon_save_points[*iter], color);
        fscanf(file, "%d", &n);
        *iter += 1;
    }
}

void print_file_circle(char* filename, uint32_t color, int (*circle_save_points)[3], int* iter) {
    FILE *file = fopen(filename, "r");
    
    int x0, y0, r;
    fscanf(file, "%d,%d,%d", &x0, &y0, &r);

    while (!feof(file)) {
        circle_save_points[*iter][0] = x0;
        circle_save_points[*iter][1] = y0;
        circle_save_points[*iter][2] = r;
        print_circle(x0, y0, r, color);
        fscanf(file, "%d,%d,%d", &x0, &y0, &r);
        *iter += 1;
    } 
}

void print_polygon(int n, int* arr_coordinate, uint32_t color) {
    int i = 0;
    while (i < (n*2)-2) {
        cohen_sutherland_clip(arr_coordinate[i], arr_coordinate[i+1], arr_coordinate[i+2], arr_coordinate[i+3], color);
        i += 2;
    }
    if (n > 1) {
        cohen_sutherland_clip(arr_coordinate[i], arr_coordinate[i+1], arr_coordinate[0], arr_coordinate[1], color);
    }
}

/*
    Bresenham's Circle Algorithm
    Referenced from: https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
*/
void print_circle(int x0, int y0, int r, uint32_t color) {
    int x = r-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (r << 1);

    while (x >= y)
    {
        print_point(x0 + x, y0 + y, color);
        print_point(x0 + y, y0 + x, color);

        print_point(x0 - y, y0 + x, color);
        print_point(x0 - x, y0 + y, color);

        print_point(x0 - x, y0 - y, color);
        print_point(x0 - y, y0 - x, color);

        print_point(x0 + y, y0 - x, color);
        print_point(x0 + x, y0 - y, color);

        if (err <= 0) {
            y++;
            err += dy;
            dy += 2;
        }
        
        if (err > 0) {
            x--;
            dx += 2;
            err += dx - (r << 1);
        }
    }
}

void cohen_sutherland_clip(int x1, int y1, int x2, int y2, uint32_t color) {
    if (x1 >= vinfo.xres || y1 >= vinfo.yres || x1 < 0 || y1 < 0) return;
    if (x2 >= vinfo.xres || y2 >= vinfo.yres || x2 < 0 || y2 < 0) return;

    // Compute region codes for P1, P2 
    int code1 = compute_region_code(x1, y1); 
    int code2 = compute_region_code(x2, y2); 
  
    // Initialize line as outside the rectangular window 
    int accept = 0; 
  
    while (1) { 
        if ((code1 == 0) && (code2 == 0)) { 
            // If both endpoints lie within rectangle 
            accept = 1; 
            break; 
        } 
        else if (code1 & code2) { 
            // If both endpoints are outside rectangle, 
            // in same region 
            break; 
        } 
        else { 
            // Some segment of line lies within the 
            // rectangle 
            int code_out; 
            double x, y; 
  
            // At least one endpoint is outside the  
            // rectangle, pick it. 
            if (code1 != 0) 
                code_out = code1; 
            else
                code_out = code2; 
  
            // Find intersection point; 
            // using formulas y = y1 + slope * (x - x1), 
            // x = x1 + (1 / slope) * (y - y1) 
            if (code_out & TOP) 
            { 
                // point is above the clip rectangle 
                x = x1 + (x2 - x1) * (y_max - y1) / (y2 - y1); 
                y = y_max; 
            } 
            else if (code_out & BOTTOM) 
            { 
                // point is below the rectangle 
                x = x1 + (x2 - x1) * (y_min - y1) / (y2 - y1); 
                y = y_min; 
            } 
            else if (code_out & RIGHT) 
            { 
                // point is to the right of rectangle 
                y = y1 + (y2 - y1) * (x_max - x1) / (x2 - x1); 
                x = x_max; 
            } 
            else if (code_out & LEFT) 
            { 
                // point is to the left of rectangle 
                y = y1 + (y2 - y1) * (x_min - x1) / (x2 - x1); 
                x = x_min; 
            } 
  
            // Now intersection point x,y is found 
            // We replace point outside rectangle 
            // by intersection point 
            if (code_out == code1) 
            { 
                x1 = x; 
                y1 = y; 
                code1 = compute_region_code(x1, y1); 
            } 
            else
            { 
                x2 = x; 
                y2 = y; 
                code2 = compute_region_code(x2, y2); 
            } 
        } 
    } 
    if (accept) { 
        print_line(x1, y1, x2, y2, color);
    }
}

void print_line_small(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int yi = 1;
    if (dy < 0){
        yi = -1;
        dy = -dy;
    }
        
    int D = 2*dy - dx;
    int y = y1;

    for (int x = x1; x <= x2; x++) {
        print_point(x, y, color);

        if (D > 0){
            y = y + yi;
            D = D - 2 * dx;
        }
        D = D + 2 * dy;
    }
}

void print_line_large(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int xi = 1;
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }
        
    int D = 2*dx - dy;
    int x = x1;

    for (int y = y1; y <= y2; y++) {
        print_point(x, y, color);

        if (D > 0) {
            x = x + xi;
            D = D - 2 * dy;
        }
        D = D + 2 * dx;
    }
}

void print_vertical_line(int x1, int y1, int x2, int y2, int color) {
    for (int i = y1; i <= y2; ++i) {
        print_point(x1, i, color);
    }
}

void print_horizontal_line(int x1, int y1, int x2, int y2, int color){
    for (int i = x1; i <= x2; ++i) {
        print_point(i, y1, color);
    }
}

void translation(int* polygon_save_sides, int (*polygon_save_points)[1000], int (*circle_save_points)[3], int iter_polygon, int iter_circle, int dx, int dy, uint32_t color) {
    for (int i = 0; i < iter_polygon; i++) {
        for (int j = 0; j < polygon_save_sides[i]; j++) {
            polygon_save_points[i][j*2] += dx;
            polygon_save_points[i][j*2+1] += dy;
        }
    }

    for (int i = 0; i < iter_circle; i++) {
        circle_save_points[i][0] += dx;
        circle_save_points[i][1] += dy;
    }

    clear_screen(pixel_color(0, 0, 0));
    print_polygon_save(polygon_save_sides, polygon_save_points, iter_polygon, color);
    print_circle_save(circle_save_points, iter_circle, color);
}

void scale(int* polygon_save_sides, int (*polygon_save_points)[1000], int (*circle_save_points)[3], int iter_polygon, int iter_circle, float scale_factor, uint32_t color) {
    for (int i = 0; i < iter_polygon; i++) {
        for (int j = 0; j < polygon_save_sides[i]; j++) {
            polygon_save_points[i][j*2] = (int) polygon_save_points[i][j*2] * scale_factor;
            polygon_save_points[i][j*2+1] = (int) polygon_save_points[i][j*2+1] * scale_factor;
        }
    }

    for (int i = 0; i < iter_circle; i++) {
        circle_save_points[i][2] = (int) circle_save_points[i][2] * scale_factor;
    }

    clear_screen(pixel_color(0, 0, 0));
    print_polygon_save(polygon_save_sides, polygon_save_points, iter_polygon, color);
    print_circle_save(circle_save_points, iter_circle, color);
}

void rotate(int* polygon_save_sides, int (*polygon_save_points)[1000], int (*circle_save_points)[3], int iter_polygon, int iter_circle, float rot_degree, uint32_t color) {
    double s = sin(rot_degree);
    double c = cos(rot_degree);

    for (int i = 0; i < iter_polygon; i++) {
        for (int j = 0; j < polygon_save_sides[i]; j++) {
            int tmp_x = (int) polygon_save_points[i][j*2] * c - polygon_save_points[i][j*2+1] * s;
            int tmp_y = (int) polygon_save_points[i][j*2] * s + polygon_save_points[i][j*2+1] * c;
            polygon_save_points[i][j*2] = tmp_x;
            polygon_save_points[i][j*2+1] = tmp_y;
        }
    }

    for (int i = 0; i < iter_circle; i++) {
        int tmp_x = (int) circle_save_points[i][0] * c - circle_save_points[i][1] * s;
        int tmp_y = (int) circle_save_points[i][0] * s + circle_save_points[i][1] * c;
        circle_save_points[i][0] = (int) tmp_x;
        circle_save_points[i][1] = (int) tmp_y;
    }

    clear_screen(pixel_color(0, 0, 0));
    print_polygon_save(polygon_save_sides, polygon_save_points, iter_polygon, color);
    print_circle_save(circle_save_points, iter_circle, color);
}

void print_polygon_save(int* polygon_save_sides, int (*polygon_save_points)[1000], int iter, uint32_t color) {
    for (int i = 0; i < iter; i++) {
        print_polygon(polygon_save_sides[i], polygon_save_points[i], color);
    }
}

void print_circle_save(int (*circle_save_points)[3], int iter, uint32_t color) {
    for (int i = 0; i < iter; i++) {
        print_circle(circle_save_points[i][0], circle_save_points[i][1], circle_save_points[i][2], color);
    }
}

void make_bullet(int* polygon_save_sides, int (*polygon_save_points)[1000], int (*circle_save_points)[3], int iter_polygon, int *iter_circle, int x, int y, int r, uint32_t color) {
    circle_save_points[*iter_circle][0] = x;
    circle_save_points[*iter_circle][1] = y;
    circle_save_points[*iter_circle][2] = r;
    *iter_circle += 1;

    clear_screen(pixel_color(0, 0, 0));
    print_polygon_save(polygon_save_sides, polygon_save_points, iter_polygon, color);
    print_circle_save(circle_save_points, *iter_circle, color);
}

void animate_bullet(int* polygon_save_sides, int (*polygon_save_points)[1000], int (*circle_save_points)[3], int iter_polygon, int iter_circle, int xi, int xf, uint32_t color) {
    int iteration = 1;
    for (int i = xi; i <= xf; i += iteration) {
        circle_save_points[iter_circle-1][0] += iteration;
        clear_screen(pixel_color(0, 0, 0));
        print_polygon_save(polygon_save_sides, polygon_save_points, iter_polygon, color);
        print_circle_save(circle_save_points, iter_circle, color);
    }

    for (int i = xf; i >= xi; i -= iteration) {
        circle_save_points[iter_circle-1][0] -= iteration;
        clear_screen(pixel_color(0, 0, 0));
        print_polygon_save(polygon_save_sides, polygon_save_points, iter_polygon, color);
        print_circle_save(circle_save_points, iter_circle, color);
    }

}

void print_line(int x0, int y0, int x1, int y1, uint32_t color) {
    if (x0 == x1) {
        if (y0 > y1) {
            print_vertical_line(x1, y1, x0, y0, color);
        } else {
            print_vertical_line(x0, y0, x1, y1, color);
        }
    } else if (y0 == y1) {
        if (x0 > x1) {
            print_horizontal_line(x1, y1, x0, y0, color);
        } else {
            print_horizontal_line(x0, y0, x1, y1, color);
        }
    } else {
        if (abs(y1 - y0) < abs(x1 - x0)) {
            if (x0 > x1) {
                print_line_small(x1, y1, x0, y0, color);
            } else {
                print_line_small(x0, y0, x1, y1, color);
            }
        } else {
            if (y0 > y1) {
                print_line_large(x1, y1, x0, y0, color);
            } else {
                print_line_large(x0, y0, x1, y1, color);
            }
        }
    }
}

void print_file(char* filename, int posx, int posy, uint32_t color) {
    FILE *file = fopen(filename, "r");
    
    int x1, y1, x2, y2;
    fscanf(file, "%d,%d,%d,%d", &x1, &y1, &x2, &y2);

    while (!feof(file)) {
        cohen_sutherland_clip(x1, y1, x2, y2, color);
        
        fscanf(file, "%d,%d,%d,%d", &x1, &y1, &x2, &y2);
    } 
}

int compute_region_code(int x, int y) { 
    // initialized as being inside  
    int code = INSIDE; 
  
    if (x < x_min)       // to the left of rectangle 
        code |= LEFT; 
    else if (x > x_max)  // to the right of rectangle 
        code |= RIGHT; 
    if (y < y_min)       // below the rectangle 
        code |= BOTTOM; 
    else if (y > y_max)  // above the rectangle 
        code |= TOP; 
  
    return code; 
} 

