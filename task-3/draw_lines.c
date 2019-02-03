#include <linux/fb.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/kd.h>
#include <string.h>

#define bitmap_size_x 32
#define bitmap_size_y 32

uint8_t *fbp;
struct fb_fix_screeninfo finfo;
struct fb_var_screeninfo vinfo;

unsigned char char_bitmap[sizeof(char)][bitmap_size_x][bitmap_size_y];

uint32_t pixel_color(uint8_t r, uint8_t g, uint8_t b);

void clear_screen(uint32_t color);

void print_point(int x,int y, uint32_t color);

void print_line_small(int x1, int y1,int x2, int y2, uint32_t color);

void print_line_large(int x1, int y1,int x2, int y2, uint32_t color);

// void print_file(char* filename, int posx, int posy, uint32_t color);

void print_file_polygon(char* filename, uint32_t color);

void print_file_circle(char* filename, uint32_t color);

void print_polygon(int n, int* arr_coordinate, uint32_t color);

void print_circle(int x0, int y0, int r, uint32_t color);

void print_vertical_line(int x1, int y1, int x2, int y2, int color);

void print_horizontal_line(int x1, int y1, int x2, int y2, int color);

int main(int argc, char** argv) {
    // Get input (argv[1]) which containst list of pixel
    if (argc != 3) {
        printf("params: <filename_poly> <filename_circle>\n");
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
    // Print image from input (argv[1] and argv[2] which contains list of pixel location) all white
    print_file_polygon(argv[1], pixel_color(255, 0, 0));
    print_file_circle(argv[2], pixel_color(255, 0, 0));

    // Wait for input
    getchar();
    // Unlock the screen from being re-rendered
    ioctl(tty_fd,KDSETMODE,KD_TEXT);
    
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


// void print_file(char* filename, int posx, int posy, uint32_t color) {
//     FILE *file = fopen(filename, "r");
    
//     int x1, y1, x2, y2;
//     fscanf(file, "%d,%d,%d,%d", &x1, &y1, &x2, &y2);

//     while (!feof(file)) {
//         print_line(x1, y1, x2, y2, color);
//         fscanf(file, "%d,%d,%d,%d", &x1, &y1, &x2, &y2);
//     } 
// }

void print_file_polygon(char* filename, uint32_t color) {
    FILE *file = fopen(filename, "r");
    
    int n, x, y;
    fscanf(file, "%d", &n);

    while (!feof(file)) {
        int arr_coordinate[n*2];
        for (int i = 0; (i < n) && !feof(file); i++) {
            fscanf(file, "%d,%d", &x, &y);
            arr_coordinate[i*2] = x;
            arr_coordinate[i*2+1] = y;
        }
        print_polygon(n, arr_coordinate, color);
        fscanf(file, "%d", &n);
    }
}

void print_file_circle(char* filename, uint32_t color) {
    FILE *file = fopen(filename, "r");
    
    int x0, y0, r;
    fscanf(file, "%d,%d,%d", &x0, &y0, &r);

    while (!feof(file)) {
        print_circle(x0, y0, r, color);
        fscanf(file, "%d,%d,%d", &x0, &y0, &r);
    } 
}

void print_polygon(int n, int* arr_coordinate, uint32_t color) {
    int i = 0;
    while (i < (n*2)-2) {
        print_line(arr_coordinate[i], arr_coordinate[i+1], arr_coordinate[i+2], arr_coordinate[i+3], color);
        i += 2;
    }
    if (n > 1) {
        print_line(arr_coordinate[i], arr_coordinate[i+1], arr_coordinate[0], arr_coordinate[1], color);
    }
}

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

void print_line(int x1, int y1, int x2, int y2, uint32_t color) {
    if (x1 >= vinfo.xres || y1 >= vinfo.yres || x1 < 0 || y1 < 0) return;
    if (x2 >= vinfo.xres || y2 >= vinfo.yres || x2 < 0 || y2 < 0) return;

    if (x1 == x2) {
        if (y1 < y2) {
            print_vertical_line(x1, y1, x2, y2, color);
        } else {
            print_vertical_line(x1, y2, x2, y1, color);
        }
    } else if (y1 == y2){
        if (x1 < x2) {
            print_horizontal_line(x1, y1, x2, y2, color);
        } else {
            print_horizontal_line(x2, y1, x1, y2, color);
        }
    } else {
        if (abs(y2 - y1) < abs(x2 - x1)) {
            if (x1 > x2) {
                print_line_small(x2, y2, x1, y1, color);
            } else {
                print_line_small(x1, y1, x2, y2, color);
            }
        } else {
            if (y1 > y2) {
                print_line_large(x2, y2, x1, y1, color);
            } else {
                print_line_large(x1, y1, x2, y2, color);
            }
        }
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