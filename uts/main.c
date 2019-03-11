#include "draw.h"
#include "color.h"

int main(int argc, char** argv) {
    char *poly_file_name = "../image/poly.txt";
    char *circle_file_name = "../image/circle.txt";
    
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

    //File saves
    int polygon_save_sides[1000];
    int polygon_save_points[1000][1000];
    int polygon_save_color[1000];
    int circle_save_points[1000][3];
    int circle_save_color[1000];
    int iter_polygon = 0;
    int iter_circle = 0;

    // Render the screen to whole black
    clear_screen(pixel_color(0, 0, 0));
    // Print image from input (argv[1] and argv[2] which contains list of pixel location) all white
    print_file_polygon(poly_file_name, polygon_save_sides, polygon_save_points, polygon_save_color, &iter_polygon);
    print_file_circle(circle_file_name, circle_save_points, circle_save_color, &iter_circle);

    // Wait for input
    char input = getchar();
    while(1) {
        switch(input) {
            case '\033': // if the first value is esc
                getchar(); // skip the [
                switch(getchar()) { // the real value
                    case 'A':
                        // code for arrow up
                        translation(polygon_save_sides, polygon_save_points, circle_save_points, polygon_save_color, circle_save_color, iter_polygon, iter_circle, 0, -10);
                        break;
                    case 'B':
                        // code for arrow down
                        translation(polygon_save_sides, polygon_save_points, circle_save_points, polygon_save_color, circle_save_color, iter_polygon, iter_circle, 0, 10);
                        break;
                    case 'C':
                        // code for arrow right
                        translation(polygon_save_sides, polygon_save_points, circle_save_points, polygon_save_color, circle_save_color, iter_polygon, iter_circle, 10, 0);
                        break;
                    case 'D':
                        // code for arrow left
                        translation(polygon_save_sides, polygon_save_points, circle_save_points, polygon_save_color, circle_save_color, iter_polygon, iter_circle, -10, 0);
                        break;
                }
                break;
            case 'u': //scale up
                scale(polygon_save_sides, polygon_save_points, circle_save_points, polygon_save_color, circle_save_color, iter_polygon, iter_circle, 1.2);
                break;
            case 'i': //scale down
                scale(polygon_save_sides, polygon_save_points, circle_save_points, polygon_save_color, circle_save_color, iter_polygon, iter_circle, 0.8);
                break;
            case 'r': //rotate counter-clockwise
                rotate(polygon_save_sides, polygon_save_points, circle_save_points, polygon_save_color, circle_save_color, iter_polygon, iter_circle, 0.04);
                break;
            case 't':
                rotate(polygon_save_sides, polygon_save_points, circle_save_points, polygon_save_color, circle_save_color, iter_polygon, iter_circle, -0.04);
                break;
            case 'f':
                make_bullet(polygon_save_sides, polygon_save_points, circle_save_points, polygon_save_color, circle_save_color, iter_polygon, &iter_circle, 400, 150, 12, pixel_color(255,0,0));
                while (getchar() != 'g') {
                    animate_bullet(polygon_save_sides, polygon_save_points, circle_save_points, polygon_save_color, circle_save_color, iter_polygon, iter_circle, 400, 550);
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

