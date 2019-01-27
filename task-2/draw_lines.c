#include <linux/fb.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/kd.h>

#define bitmap_size_x 32
#define bitmap_size_y 32

uint8_t *fbp;
struct fb_fix_screeninfo finfo;
struct fb_var_screeninfo vinfo;

unsigned char char_bitmap[sizeof(char)][bitmap_size_x][bitmap_size_y];

uint32_t pixel_color(uint8_t r, uint8_t g, uint8_t b);

void clear_screen(uint32_t color);

int print_point(int x,int y, uint32_t color);

void print_line(int x1, int y1,int x2, int y2, uint32_t color);

void print_file(char* filename, int posx, int posy, uint32_t color);

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

int print_point(int x,int y, uint32_t color) {
    if (!(x >= vinfo.xres || y >= vinfo.yres || x < 0 || y < 0)) {
        // Set a pixel in a specific location to specific color
        long location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) + (y + vinfo.yoffset) * finfo.line_length;
        // Render the pixel in the screen
        *((uint32_t*)(fbp + location)) = color;
    }
}


uint32_t pixel_color(uint8_t r, uint8_t g, uint8_t b) {
    return (r<<vinfo.red.offset) | (g<<vinfo.green.offset) | (b<<vinfo.blue.offset);
}


void clear_screen(uint32_t color) {
    for (int x = 0; x < vinfo.xres; x++) {
        for (int y = 0; y < vinfo.yres; y++) {
            // Set a pixel in a specific location to specific color
            long location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) + (y + vinfo.yoffset) * finfo.line_length;
            // Render the pixel in the screen
            *((uint32_t*)(fbp + location)) = color;
        }
    }
}


void print_file(char* filename, int posx, int posy, uint32_t color) {
    FILE *file = fopen(filename, "r");
    
    int x1, y1, x2, y2;
    fscanf(file, "%d,%d,%d,%d", &x1, &y1, &x2, &y2);

    while (!feof(file)) {
        print_line(x1, y1, x2, y2, color);
        
        fscanf(file, "%d,%d,%d,%d", &x1, &y1, &x2, &y2);
    } 
}

void print_line_small_slope(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int y = y1;
    int eps = 0;

    for (int x = x1; x <= x2; x++) {
        print_point(x, y, color);
        eps += dy;

        if ((eps << 1) >= dx) {
            y++;  eps -= dx;
        }
    }
}

void print_line_large_slope(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int x = x1;
    int eps = 0;

    for (int y = y1; y <= y2; y++) {
        print_point(x, y, color);
        eps += dx;

        if ((eps << 1) >= dy) {
            x++;  eps -= dy;
        }
    }
}

void print_line(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int y = y1;
    int eps = 0;

    if (dy > dx) {
        print_line_large_slope(x1, y1, x2, y2, color);
    } else {
        print_line_small_slope(x1, y1, x2, y2, color);
    }
}