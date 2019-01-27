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

void print_file(char* filename, int posx, int posy, uint32_t color);

void print_vertical_line(int x1, int y1, int x2, int y2, int color);

void print_horizontal_line(int x1, int y1, int x2, int y2, int color);

int main(int argc, char** argv) {
    // Get input (argv[1]) which containst list of pixel
    if (argc != 2) {
        printf("params: <filename>\n");
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


void print_file(char* filename, int posx, int posy, uint32_t color) {
    FILE *file = fopen(filename, "r");
    
    int x1, y1, x2, y2;
    fscanf(file, "%d,%d,%d,%d", &x1, &y1, &x2, &y2);

    while (!feof(file)) {
        if (x1 >= vinfo.xres || y1 >= vinfo.yres || x1 < 0 || y1 < 0) continue;
        if (x2 >= vinfo.xres || y2 >= vinfo.yres || x2 < 0 || y2 < 0) continue;

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
        
        fscanf(file, "%d,%d,%d,%d", &x1, &y1, &x2, &y2);
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