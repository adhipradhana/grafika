#include "draw.h"
#include "color.h"

//File saves
int polygon_save_sides[1000];
int polygon_save_points[1000][1000];
int circle_save_points[1000][3];
int iter_polygon = 0;
int iter_circle = 0;

int tty_fd;

// menu
void handle_menu();

// transformation
void handle_transformation();

int main(int argc, char** argv) {
    char *poly_file_name = "war_poly.txt";
    char *circle_file_name = "war_circle.txt";
    char *bitmap_file_name = "names.txt";
    char *lines_file_name = "lines.txt";
    
    // Lock the screen from being re-rendered
    tty_fd = open("/dev/tty0", O_RDWR);
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
    print_line(0, 0, 100, 100, pixel_color(255, 0, 0));
    
    // Print menu screen
    handle_menu();
    
	return 0;
}

void handle_menu() {
    // Wait for input
    char input = getchar();
    while(1) {
        switch(input) {
            case '1':
                // print point
                break;
            case '2':
                // print line
                break;
            case '3':
                // print polygon and circle 
                break;
            case '4':
                // print transformation
                break;
            case '5':
                // print viewpoint
                break;
            case '6':
                // print flood fill
                break;
            case '7':
                // print uts
                break;
            case 'q':
                // exit
                ioctl(tty_fd,KDSETMODE,KD_TEXT);
                return;
        }
        input = getchar();
    }
}

void handle_transformation() {
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
                break;
        }
        if (input == 'e') {
            break;
        }
        input = getchar();
    }

}

